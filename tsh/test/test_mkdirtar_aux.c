#include "inc/test_mkdirtar_aux.h"

void test_mkdirtar_init_local_var(char *uid, char *gid, char *uname, char *gname) {
	struct passwd *user_data;
	struct group  *group_data;

	sprintf(uid, INT_TO_STRING_FORMAT, getuid());
	sprintf(gid, INT_TO_STRING_FORMAT, getgid());

	user_data = getpwuid(getuid());
	group_data = getgrgid(getgid());

	if (NULL == user_data || NULL == group_data);
	else {
		strcpy(uname, user_data->pw_name);
		strcpy(gname, group_data->gr_name);
	}
}
