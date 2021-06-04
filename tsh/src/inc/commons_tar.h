#ifndef COMMONS_TAR_H
#define COMMONS_TAR_H

/** Contains all commons functions managing tarballs. */

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "commons.h"
#include "tar.h"
#include "tsh.h"
#include "error.h"

/**
 * Reads by block of TAR_BLOCKSIZE bytes a tarball from SEEK_SET until the searched file is found.
 *
 * @note If the file is found the next call to read() will read the file content,
 *  else lseek() at the beggining.
 *
 * @param tar_fd is the file descriptor associated with the opening of a valid tarball.
 * @param path_from_tar_root is the path from the tarball's root of the searched file.
 * @param pheader is a posix_header pointer corresponding to the header of the wanted
 * file if path_from_tar_root (could be NULL if not interrested to store it).
 *
 * @return FALSE if the file is not found else TRUE.
 */
int commons_tar_find_file_from(
		const int tar_fd,
		const char *path_from_tar_root,
		s_posix_header *pheader
);

/**
 * Reads by block of TAR_BLOCKSIZE bytes a tarball from SEEK_SET until the searched dir
 * or a subdir/file is found.
 *
 * @note If the file is found the next call to read() will read the file content,
 *  else lseek() at the beggining.
 *
 * @param tar_fd is the file descriptor associated with the opening of a valid tarball.
 * @param path_from_tar_root is the path from the tarball's root of the searched dir.
 * @param pheader is a posix_header pointer.
 *
 * @return FALSE if the dir is not found else TRUE.
 */
int commons_tar_find_dir_from(
		const int tar_fd,
		const char *path_from_tar_root,
		s_posix_header *header
);

/**
 * Verifies if the given file is a valid tarball.
 *
 * @param tar_fd is the file descriptor.
 *
 * @return TRUE if the file is a valid tarball else FALSE.
 */
int commons_tar_is_valid(const int tar_fd);


/**
 * Returns the depth of a file inside a tarball.
 *
 * Ex:@code{.c}
 * - commons_tar_get_depth("")      returns 0
 * - commons_tar_get_depth("dir1/") returns 1
 * @endcode
 *
 * @param path_from_tar_root is the file path from the tarball's root.
 *
 * @return the depth.
 */
int commons_tar_get_depth(const char *path_from_tar_root);

/**
 * Adds a new header and all the content blocks from a given file
 * at the end of a supposed valid tar.
 *
 * @note If the file path leads to a physical link, a normal file header and the content
 *   of the file linked to, will be added to the end of the tarball.
 *
 * @param file_path is path to the file to append.
 * @param tar_fd    is the file descriptor associated to the tarball.
 * @param dest_name is the new name of the file in the tarball.
 *
 * @return a return_handler with the following verdict value:
 * 			ERR in these cases:
 *				the file does not exist.
 * 				reading error in the file descriptor.
 *         	else OK.
 */
return_handler commons_tar_append_file(const int tar_fd, const s_tsh_path file_path, const char *dest_name);

/**
 * Creates a s_tsh_path before calling commons_tar_append_file.
 *
 * @param extern_file_path is the path the to the external source file.
 * @param tarball_fd is the file descriptor associated to the destination tarball.
 * @param new_file_name is the new name of the file in the tarball.
 *
 * @return a return_handler with the following verdict value:
 * 			ERR in these cases:
 *				the file does not exist.
 * 				reading error in the file descriptor.
 *         	else OK.
 */
return_handler commons_tar_append_extern_file_to(
	const char *extern_file_path,
	const int tarball_fd,
	const char *new_file_name
);

/**
 * Change the file name of a s_posix_header.
 *
 * @param pheader is the s_posix_header to which we want to change the name.
 * @param new_name is the new name.
 */
void commons_tar_change_pheader_name(s_posix_header *pheader, const char *new_name);

/**
 * Moves (to the left) to the destination offset all blocks starting at the source offset.
 *
 * @note The destination offset should be strictly lower than the source offset.
 *
 * @param tar_fd is the file descriptor associated to the tarball.
 * @param dest_offset is the destination offset.
 * @param src_offset is the source offset.
 *
 * @return a return_handler with the following verdict value:
 * 			ERR in these cases:
 *                 	the destination or the source offset is/are illegal.
 * 					reading/writing error(s) in the file descriptor.
 *         	else OK.
 */
return_handler commons_tar_blockmove(
		const int tar_fd,
		const ssize_t dest_offset,
		const ssize_t src_offset
);

/**
 * Calculates the number of TAR_BLOCKSIZE sized blocks needed to store all the file content.
 *
 * @param pheader is the posix header corresponding to the file.
 *
 * @return (filesize + TAR_BLOCKSIZE - 1) / TAR_BLOCKSIZE;
 */
int commons_tar_get_content_nb_blocks(const s_posix_header pheader);

/**
 * Formats the name according a maximum depth.
 *
 * Ex:@code{.c}
 * - commons_tar_format_name_according_max_depth("dir1/dir2/file", 0) returns ""
 * - commons_tar_format_name_according_max_depth("dir1/dir2/file", 2) returns "dir1/dir2/"
 * - commons_tar_format_name_according_max_depth("file", 1) 		  returns "file"
 * @endcode
 *
 * @param src_name is the name to format.
 * @param max_depth is the maximum depth.
 *
 * @return the formatted name.
 */
char *commons_tar_format_name_according_max_depth(
		const char *src_name,
		const int max_depth
);

/**
 * Formats the name according a minimum depth.
 *
 * Ex:@code{.c}
 * - commons_tar_format_name_according_min_depth("dir1/dir2/file", 0) returns "dir1/dir2/file"
 * - commons_tar_format_name_according_min_depth("dir1/dir2/file", 2) returns "file"
 * - commons_tar_format_name_according_min_depth("dir1/dir2/file", 3) returns ""
 * @endcode
 *
 * @param src_name is the name to format.
 * @param min_depth is the minimum depth.
 *
 * @return the formatted name.
 */
char *commons_tar_format_name_according_min_depth(
		const char *src_name,
		const int min_depth
);

/**
 * Get the path of the parent dir.
 * (commons_tar_format_name_according_max_depth(path, path_depth-1))
 *
 * @param path is the path to the wanted dir/file.
 *
 * @return the path to the parent dir.
 */
char *commons_tar_get_parent_path(const char *path);

/**
 * Lseek the size of the file from SEEK_CUR inside a tarball.
 *
 * @param tar_fd is the file descriptor associated to the tarball.
 * @param pheader is the posix_header of the file.
 */
void commons_tar_lseek_size_of_file(const int tar_fd, const s_posix_header *pheader);

/**
 * Lseek until the first zero block.
 *
 * @param tar_fd is the file descriptor associated to the tarball.
 */
void commons_tar_lseek_to_the_end(const int tar_fd);

/**
 * Fills non empty blocks with NONECHARs blocks from offset to the end of supposed valid tarball.
 *
 * @param tar_fd is the file descriptor associated to the tarball.
 * @param offset is the offset to start from.
 */
void commons_tar_fill_with_zero_block_from(const int tar_fd, const ssize_t offset);

/**
 * Calculates the total tarball size without changing the current offset.
 *
 * @param tar_fd is the file descriptor associated to the tarball.
 *
 * @return the total tarball size.
 */
ssize_t commons_tar_get_total_tar_size(const int tar_fd);

/**
 * Verifies if the folder given in the tarball is empty or not.
 *
 * @param tar_fd is the file descriptor associated to the tarball.
 * @param path_from_tar_root is the path of the file in the tarball.
 *
 * @return TRUE if it is empty, else FALSE.
 */
int commons_tar_is_dir_empty(const int tar_fd, const char *path_from_tar_root);

/**
 * Creates a posix_header from a file outside the tarball.
 *
 * @note The name in the posix_header is only the filename and not the correct path in the tarball.
 * If needed, the name must be correctly changed by the calling method.
 *
 * @param path_to_file is the path to the file outside the tarball.
 * @param pheader will be the posix_header of the file.
 *
 * @return ERR (-1) in these cases:
 *                      the file is not found.
 *                      the filename or the linkname is exceeding the TAR_NAME_SIZE (100).
 *                      the gname or the uname of the file is exceeding the TAR_U_G_NAME_SIZE (32).
 *                  else OK (0).
 */
int commons_tar_create_posix_header_from_file(const char *path_to_file, s_posix_header *pheader);

/**
 * Creates a folder in the tarball and add it to the end.
 *
 * @param tar_fd is the file descriptor associated to the tarball.
 * @param path_from_tar_root is the the path of the created file in the tarball.
 *
 * @return a return_handler with the following verdict value:
 * 			ERR in these cases:
 *                 	the file's name is too long.
 * 					the checksum of a header is invalid.
 * 					writing error in the file descriptor.
 *         	else OK.
 */
return_handler commons_tar_append_dir(const int tar_fd, const char *path_from_tar_root);

/**
 * Verifies if a given s_posix_header corresponds to a directory.
 *
 * @param pheader is the tested s_posix_header.
 *
 * @return TRUE if pheader corresponds to a directory, FALSE otherwise.
 */
int commons_tar_is_dir(s_posix_header pheader);

/**
 * Returns true if path_target contains path_src
 * (that means that path_src is an ancestor of path_target in the file tree).
 *
 * @param tar_fd is the file descriptor of the current tarball.
 * @param path_src is the supposed ancestor.
 * @param path_target is the path we want to verify.
 *
 * @return TRUE if path_target contains path_src.
 */
int commons_tar_is_same_ancestor(const int tar_fd, const char *path_src, const char *path_target);

/**
 * Removes the header block and all content blocks of the file at the current offset.
 *
 * @param tar_fd is the file descriptor of the associated to the tarball.
 * @param pheader_to_remove is the posix header of the file to remove.
 *
 * @return a return_handler.
 */
return_handler commons_tar_remove_file_at_current_offset(
	const int tar_fd,
	const s_posix_header *pheader_to_remove
);

/**
 * Initializes the destination offset corresponding to the file to be erased and
 * the source offset associated to the next file to be copied.
 * The current offset should be at the beggining of the first content block to remove.
 *
 * @param tar_fd is the file descriptor of the associated to the tarball.
 * @param src_offset is the offset that will be contains the source offset.
 * @param dest_offset is the offset that will be contains the destination offset.
 * @param pheader is the posix header of the file to remove.
 */
void commons_tar_init_offsets(
        const int tar_fd,
        ssize_t *src_ofset,
        ssize_t *dest_offset,
        const s_posix_header *pheader
);

#endif
