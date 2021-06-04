#include "inc/commons_tar.h"

static void fill_two_zero_block(const int fd);
static void set_posix_header_typeflag(s_posix_header *pheader, const struct stat buffer_stat);
static void set_posix_header_size(s_posix_header *pheader, const struct stat buffer_stat);
static void set_posix_header_mtime(s_posix_header *pheader, const time_t *rawtime);
static int get_offset_for_formatting_name(const char *src_name, const int depth);
static int set_posix_header_uname(s_posix_header *pheader, const uid_t uid);
static int set_posix_header_gname(s_posix_header *pheader, const gid_t gid);
static int set_posix_header_name_from_file(s_posix_header *pheader, const char* path_to_file);
static int set_posix_header_name(s_posix_header *pheader, const char *path_from_tar_root);
static int set_posix_header_linkname(s_posix_header *pheader, const char *path_to_file);
static int read_and_write(const int out_fd, const int in_fd, const s_posix_header pheader);
static return_handler commons_tar_erase_file(
	const int fd_in,
	const s_tsh_path *file_path,
	s_posix_header *pheader_to_append,
	const char *dest_name,
	const int tar_fd);

int commons_tar_find_file_from(
		const int tar_fd,
		const char *path_from_tar_root,
		s_posix_header *header
) {
	s_posix_header *pheader;

	if (NULL != header) {
		pheader = header;
	}
	else {
		assert(NULL != (pheader = malloc(sizeof(s_posix_header))));
	}

	lseek(tar_fd, 0, SEEK_SET);
	while (0 < read(tar_fd, pheader, TAR_BLOCKSIZE)) {
		if (OK == strcmp(pheader->name, path_from_tar_root)) {
			return TRUE;
		}
		commons_tar_lseek_size_of_file(tar_fd, pheader);
	}

	return FALSE;
}

int commons_tar_find_dir_from(
		const int tar_fd,
		const char *path_from_tar_root,
		s_posix_header *header
) {
	lseek(tar_fd, 0, SEEK_SET);

	while (0 < read(tar_fd, header, TAR_BLOCKSIZE)) {
		/* Looks if the tarball contains a subfile/subfolder of the given file
		 * to speed up the search. */
		if (0 == strcmp(commons_tar_get_parent_path(header->name), path_from_tar_root)) {
			return TRUE;
		}
		if (0 == strcmp(header->name, path_from_tar_root) && commons_tar_is_dir(*header)) {
			return TRUE;
		}
		commons_tar_lseek_size_of_file(tar_fd, header);
	}

	return FALSE;
}

int commons_tar_is_valid(const int tar_fd) {
	s_posix_header header;
	ssize_t read_size;
	ssize_t current_offset;

	if (ERR == tar_fd) {
		return FALSE;
	}

	current_offset = lseek(tar_fd, 0, SEEK_CUR);
	lseek(tar_fd, 0, SEEK_SET);

	while (ERR != (read_size = read(tar_fd, &header, TAR_BLOCKSIZE))
			&& NONECHAR != header.name[0]
	) {
		if (CHECKSUM_ERR == tar_check_checksum(&header)) {
			return FALSE;
		}
		commons_tar_lseek_size_of_file(tar_fd, &header);
	}

	if (ERR == read_size) {
		return FALSE;
	}

	lseek(tar_fd, current_offset, SEEK_SET);

	return TRUE;
}

int commons_tar_get_depth(const char *path_from_tar_root) {
	ssize_t str_len = strlen(path_from_tar_root);
	int depth = 0;

	if (0 < str_len && 0 != strcmp(path_from_tar_root, ".")) {
		++depth;
		for (int i = 0; i < str_len - 1; ++i) {
			if (PATH_DELIMITER_CHR == path_from_tar_root[i] ) {
				++depth;
			}
		}
	}

	return depth;
}

static void fill_two_zero_block(const int fd) {
	char buffer[2 * TAR_BLOCKSIZE];

	memset(buffer, NONECHAR, 2 * TAR_BLOCKSIZE);
	write(fd, buffer, 2 * TAR_BLOCKSIZE);
}

static int read_and_write(
		const int out_fd,
		const int in_fd,
		const s_posix_header pheader)
{
	char buffer[TAR_BLOCKSIZE];
	int content_nb_blocks;

	lseek(out_fd, 0, SEEK_SET);
	commons_tar_lseek_to_the_end(out_fd);

	/* Appends the header. */
	write(out_fd, &pheader, TAR_BLOCKSIZE);

	/* Writes the file content to the end of the tarball. */
	content_nb_blocks = commons_tar_get_content_nb_blocks(pheader);
	while (0 < content_nb_blocks--) {
		memset(buffer, NONECHAR, TAR_BLOCKSIZE);
		if (0 > read(in_fd, buffer, TAR_BLOCKSIZE)) {
			return ERR;
		}
		write(out_fd, buffer, TAR_BLOCKSIZE);
	}

	fill_two_zero_block(out_fd);

	return OK;
}

return_handler commons_tar_append_extern_file_to(
	const char *extern_file_path,
	const int tarball_fd,
	const char *new_file_name
) {
	s_tsh_path path;

	commons_safe_str_cpy(path.outside_path, extern_file_path);
	path.tar_fd = FD_NOTDEF;

	return commons_tar_append_file(tarball_fd, path, new_file_name);
}

return_handler commons_tar_append_file(const int tar_fd, const s_tsh_path file_path, const char *dest_name) {
	s_posix_header pheader_to_append;
	return_handler rh;
	char filename[NAME_MAX];
	int fd_in;

	if (FD_NOTDEF == file_path.tar_fd) { /* From outside file. */
		/* Tries to open the file. */
		if (ERR == (fd_in = open(file_path.outside_path, O_RDONLY))) {
			/* Tries to open the symbolic link. */
			if (ERR == readlink(file_path.outside_path, filename, NAME_MAX)
				|| ERR == (fd_in = open(filename, O_RDONLY)))
			{
				return create_error_return_handler(ERR_APPEND, ERR_NO_FILE, file_path.outside_path);
			}
		}
		/* Tries to create an associated posix header. */
		if (ERR == commons_tar_create_posix_header_from_file(
					file_path.outside_path, &pheader_to_append))
		{
			close(fd_in);
			return create_error_return_handler(ERR_APPEND, ERR_NO_FILE, file_path.outside_path);
		}

		rh = commons_tar_erase_file(fd_in, &file_path, &pheader_to_append, dest_name, tar_fd);
		close(fd_in);
		if (ERR == rh.verdict) {
			return rh;
		}
	}
	else { /* From inside file. */
		/* Tries to find the file posix header. */
		if (!commons_tar_find_file_from(
					file_path.tar_fd,
					file_path.inside_path,
					&pheader_to_append))
		{
			return create_error_return_handler(ERR_APPEND, ERR_NO_FILE, file_path.inside_path);
		}

		rh = commons_tar_erase_file(file_path.tar_fd, &file_path, &pheader_to_append, dest_name, tar_fd);
		if (ERR == rh.verdict) {
			return rh;
		}
	}
	return create_successful_return_handler();
}

static return_handler commons_tar_erase_file(
	const int fd_in,
	const s_tsh_path *file_path,
	s_posix_header *pheader_to_append,
	const char *dest_name,
	const int tar_fd)
{
	return_handler rh;
	s_posix_header tmp_pheader;

	commons_tar_change_pheader_name(pheader_to_append, dest_name);
	if (commons_tar_find_file_from(tar_fd, pheader_to_append->name, &tmp_pheader)
		&& TAR_DIR_F != tmp_pheader.typeflag)
	{
		/* If the file already exist, removes it. */
		rh = commons_tar_remove_file_at_current_offset(tar_fd, &tmp_pheader);
		if (ERR == rh.verdict) {
			perror(rh.message);
			return rh;
		}
	}
	/* Copies the file content to the destination. */
	if (ERR == read_and_write(tar_fd, fd_in, *pheader_to_append)) {
		return create_error_return_handler(ERR_APPEND, ERR_READING, file_path->outside_path);
	}
	return create_successful_return_handler();
}

void commons_tar_change_pheader_name(s_posix_header *pheader, const char *new_name) {
	if (NULL != new_name) {
		commons_safe_str_cpy(pheader->name, new_name);
		tar_set_checksum(pheader);
	}
}

return_handler commons_tar_blockmove(
		const int tar_fd,
		const ssize_t dest_offset,
		const ssize_t src_offset
) {
	char buffer[TAR_BLOCKSIZE];
	ssize_t cur_src_offset = src_offset;
	ssize_t cur_dest_offset = dest_offset;

	if (0 > dest_offset || 0 > src_offset || dest_offset >= src_offset) {
		return create_error_return_handler(ERR_BLCKMV, ERR_REMOVING, "");
	}

	do {
		/* Fills the buffer with NONECHARs. */
		memset(buffer, NONECHAR, TAR_BLOCKSIZE);

		/* Puts the offset to the next block to copy. */
		lseek(tar_fd, cur_src_offset, SEEK_SET);
		if (0 >= read(tar_fd, buffer, TAR_BLOCKSIZE)) {
			return create_error_return_handler(ERR_BLCKMV, ERR_READING, "");
		}

		/* Puts the offset to the next block to overwrite. */
		lseek(tar_fd, cur_dest_offset, SEEK_SET);
		if (0 >= write(tar_fd, buffer, TAR_BLOCKSIZE)) {
			return create_error_return_handler(ERR_BLCKMV, ERR_WRITING, "");
		}
		cur_src_offset += TAR_BLOCKSIZE;
		cur_dest_offset += TAR_BLOCKSIZE;
	} while (NONECHAR != buffer[0]);

	commons_tar_fill_with_zero_block_from(tar_fd, lseek(tar_fd, 0, SEEK_CUR));

	return create_successful_return_handler();
}

int commons_tar_get_content_nb_blocks(const s_posix_header pheader) {
	int size_int = 0;

	sscanf(pheader.size, "%o", &size_int);

	return (size_int + TAR_BLOCKSIZE - 1) / TAR_BLOCKSIZE;
}

void commons_tar_fill_with_zero_block_from(const int tar_fd, const ssize_t offset) {
	char buffer[TAR_BLOCKSIZE];

	lseek(tar_fd, offset, SEEK_SET);
	while (0 < read(tar_fd, buffer, TAR_BLOCKSIZE)) {
		if (NONECHAR != buffer[0]) {
			lseek(tar_fd, -TAR_BLOCKSIZE, SEEK_CUR);
			memset(buffer, NONECHAR, TAR_BLOCKSIZE);
			write(tar_fd, buffer, TAR_BLOCKSIZE);
		}
	}
}

static int get_offset_for_formatting_name(const char *src_name, const int depth) {
	ssize_t src_len = strlen(src_name);
	int current_depth = 0;
	int offset = 0;

	while (current_depth < depth && offset < src_len) {
		if (PATH_DELIMITER_CHR == src_name[offset]) {
			++current_depth;
		}
		++offset;
	}

	return offset;
}

char *commons_tar_format_name_according_max_depth(
		const char *src_name,
		const int max_depth
) {
	char *formatted_name = "";
	const int offset = get_offset_for_formatting_name(src_name, max_depth);

	if (0 != offset) {
		assert(NULL != (formatted_name = malloc((offset + 1) * sizeof(char))));
		assert(NULL != strncpy(formatted_name, src_name, offset));
		formatted_name[offset] = NONECHAR;
	}

	return formatted_name;
}

char *commons_tar_format_name_according_min_depth(
		const char *src_name,
		const int min_depth
) {
	char *formatted_name = "";
	const long src_name_len = strlen(src_name);
	const int offset = get_offset_for_formatting_name(src_name, min_depth);

	if (strlen(src_name) != offset) {
		assert(
			NULL != (formatted_name = malloc((src_name_len - offset + 1) * sizeof(char)))
		);
		assert(
			NULL != strncpy(formatted_name, src_name + offset, src_name_len - offset)
		);
		formatted_name[src_name_len - offset] = NONECHAR;
	}

	return formatted_name;
}

char *commons_tar_get_parent_path(const char *path) {
	int path_depth = commons_tar_get_depth(path);
	return commons_tar_format_name_according_max_depth(path, path_depth-1);
}

void commons_tar_lseek_size_of_file(const int tar_fd, const s_posix_header *pheader) {
	const int content_nb_blocks = commons_tar_get_content_nb_blocks(*pheader);

	lseek(tar_fd, content_nb_blocks * TAR_BLOCKSIZE, SEEK_CUR);
}

void commons_tar_lseek_to_the_end(const int tar_fd) {
	char buffer[TAR_BLOCKSIZE];

	do {
		memset(buffer, NONECHAR, TAR_BLOCKSIZE);
	} while (0 < read(tar_fd, buffer, TAR_BLOCKSIZE) && NONECHAR != buffer[0]);

	lseek(tar_fd, -TAR_BLOCKSIZE, SEEK_CUR);
}

int commons_tar_is_dir_empty(const int tar_fd, const char *path_from_tar_root) {
    s_posix_header pheader;
    const ssize_t stored_curr_offset = lseek(tar_fd, 0, SEEK_CUR);

	while (0 < read(tar_fd, &pheader, TAR_BLOCKSIZE)) {
		if (0 == strcmp(commons_tar_get_parent_path(pheader.name), path_from_tar_root)) {
			lseek(tar_fd, stored_curr_offset, SEEK_SET);
			return FALSE;
		}
		commons_tar_lseek_size_of_file(tar_fd, &pheader);
	}

	lseek(tar_fd, stored_curr_offset, SEEK_SET);
    return TRUE;
}

int commons_tar_create_posix_header_from_file(const char *path_to_file, s_posix_header *pheader) {
	struct stat buffer_stat;
	int permissions;

	if (ERR == lstat(path_to_file, &buffer_stat)) {
		return ERR;
	}

	permissions = buffer_stat.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
	sprintf(pheader->mode, OCTAL_PERMISSION_FORMAT, permissions);
	sprintf(pheader->uid, INT_TO_STRING_FORMAT, buffer_stat.st_uid);
	sprintf(pheader->gid, INT_TO_STRING_FORMAT, buffer_stat.st_gid);

	strcpy(pheader->magic, TMAGIC);
	strcpy(pheader->version, TVERSION);

	set_posix_header_mtime(pheader, &buffer_stat.st_mtime);
	set_posix_header_typeflag(pheader, buffer_stat);
	set_posix_header_size(pheader, buffer_stat);

	if (
		ERR == set_posix_header_uname(pheader, buffer_stat.st_uid) ||
		ERR == set_posix_header_gname(pheader, buffer_stat.st_gid) ||
		ERR == set_posix_header_name_from_file(pheader, path_to_file) ||
		ERR == set_posix_header_linkname(pheader, path_to_file)
	) {
		return ERR;
	}

	tar_set_checksum(pheader);

	return OK;
}

return_handler commons_tar_append_dir(const int tar_fd, const char *path_from_tar_root) {
	s_posix_header pheader;
	time_t rawtime;

	memset(&pheader, NONECHAR, TAR_BLOCKSIZE);

	sprintf(pheader.mode, INT_TO_STRING_FORMAT, TAR_DEFAULT_DIR_OCTAL_MODE);
	sprintf(pheader.uid, INT_TO_STRING_FORMAT, getuid());
	sprintf(pheader.gid, INT_TO_STRING_FORMAT, getgid());

	strcpy(pheader.magic, TMAGIC);
	strcpy(pheader.version, TVERSION);

	time(&rawtime);
	set_posix_header_mtime(&pheader, &rawtime);
	pheader.typeflag = TAR_DIR_F;
	strcpy(pheader.size, TAR_DEFAULT_DIR_SIZE);

	if (
		ERR == set_posix_header_uname(&pheader, getuid()) ||
		ERR == set_posix_header_gname(&pheader, getgid()) ||
		ERR == set_posix_header_name(&pheader, path_from_tar_root)
	) {
		return create_error_return_handler(ERR_MKDIRTAR, ERR_FILE_TOO_LONG, path_from_tar_root);
	}

	tar_set_checksum(&pheader);

	if (!tar_check_checksum(&pheader)) {
		return create_error_return_handler(ERR_APPEND_DIR, ERR_CHKSUM, pheader.name);
	}

	lseek(tar_fd, 0, SEEK_SET);
	commons_tar_lseek_to_the_end(tar_fd);

	if (0 >= write(tar_fd, &pheader, TAR_BLOCKSIZE)) {
		return create_error_return_handler(ERR_APPEND_DIR, ERR_WRITING, path_from_tar_root);
	}

	fill_two_zero_block(tar_fd);

	return create_successful_return_handler();
}

static void set_posix_header_typeflag(s_posix_header *pheader, const struct stat buffer_stat) {
	const mode_t mode = buffer_stat.st_mode;

	if (S_ISREG(mode)) {
		pheader->typeflag = TAR_NORMALFILE_F;
	}
	else if (S_ISDIR(mode)) {
		pheader->typeflag = TAR_DIR_F;
	}
	else if (S_ISCHR(mode)) {
		pheader->typeflag = TAR_SPECIALCHAR_F;
	}
	else if (S_ISBLK(mode)) {
		pheader->typeflag = TAR_SPECIALBLOC_F;
	}
	else if (S_ISFIFO(mode)) {
		pheader->typeflag = TAR_FIFO_F ;
	}
	else if (S_ISLNK(mode)) {
		pheader->typeflag = TAR_SLINK_F;
	}
}

static void set_posix_header_size(s_posix_header *pheader, const struct stat buffer_stat){
	if (TAR_DIR_F == pheader->typeflag) {
		strcpy(pheader->size, TAR_DEFAULT_DIR_SIZE);
	}
	else {
		sprintf(pheader->size, OCTAL_FILESIZE_FORMAT, buffer_stat.st_size);
	}
}

static void set_posix_header_mtime(s_posix_header *pheader, const time_t *rawtime){
	struct tm *time;

	time = localtime(rawtime);
	sprintf(pheader->mtime, OCTAL_MTIME_FORMAT, mktime(time));
}

static int set_posix_header_uname(s_posix_header *pheader, const uid_t uid){
	struct passwd *user_data;

	user_data = getpwuid(uid);

	if (NULL == user_data || TAR_U_G_NAME_SIZE < strlen(user_data->pw_name)) {
		return ERR;
	}
	else {
		strcpy(pheader->uname, user_data->pw_name);
		return OK;
	}
}

static int set_posix_header_gname(s_posix_header *pheader, const gid_t gid){
	struct group  *group_data;

	group_data = getgrgid(gid);

	if (NULL == group_data || TAR_U_G_NAME_SIZE < strlen(group_data->gr_name)) {
		return ERR;
	}
	else {
		strcpy(pheader->gname, group_data->gr_name);
		return OK;
	}
}

static int set_posix_header_name_from_file(s_posix_header *pheader, const char *path_to_file){
	char *filename = commons_tar_format_name_according_min_depth(
		path_to_file, commons_tar_get_depth(path_to_file)-1
	);

	return set_posix_header_name(pheader, filename);
}

static int set_posix_header_name(s_posix_header *pheader, const char *path_from_tar_root){
	if(TAR_NAME_SIZE < strlen(path_from_tar_root)) {
		return ERR;
	}

	commons_safe_str_cpy(pheader->name, path_from_tar_root);

	return OK;
}

static int set_posix_header_linkname(s_posix_header *pheader, const char *path_to_file){
	char filename[TAR_NAME_SIZE];

	memset(filename, NONECHAR, TAR_NAME_SIZE);

	if (TAR_SLINK_F == pheader->typeflag) {
		if (ERR == readlink(path_to_file, filename, TAR_NAME_SIZE) || TAR_NAME_SIZE < strlen(filename)) {
			return ERR;
		}
		else {
			strcpy(pheader->linkname, filename);
		}
	}

	return OK;
}

int commons_tar_is_dir(s_posix_header pheader) {
	return TAR_DIR_F == pheader.typeflag;
}

int commons_tar_is_same_ancestor(const int tar_fd, const char *path_src, const char *path_target) {
    const char* filename = commons_tar_format_name_according_max_depth(
								path_target, commons_tar_get_depth(path_src)
							);
    return OK == strcmp(path_src, filename);
}

void commons_tar_init_offsets(
        const int tar_fd,
        ssize_t *src_offset,
        ssize_t *dest_offset,
        const s_posix_header *pheader
) {
    *dest_offset = lseek(tar_fd, -TAR_BLOCKSIZE, SEEK_CUR);
    lseek(tar_fd, TAR_BLOCKSIZE, SEEK_CUR);
    commons_tar_lseek_size_of_file(tar_fd, pheader);
    *src_offset = lseek(tar_fd, 0, SEEK_CUR);
}

return_handler commons_tar_remove_file_at_current_offset(
	const int tar_fd,
	const s_posix_header *pheader_to_remove)
{
	ssize_t dest_offset;
	ssize_t src_offset;

	commons_tar_init_offsets(tar_fd, &src_offset, &dest_offset, pheader_to_remove);
	return commons_tar_blockmove(tar_fd, dest_offset, src_offset);
}
