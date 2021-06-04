#ifndef TEST_LSTAR_AUX_H
#define TEST_LSTAR_AUX_H

#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../src/inc/commons.h"
#include "../../src/inc/lstar.h"

#include "commons_test.h"

void test_lstar_aux_init(const char *path_to_src, int *fd_src);
void test_lstar_aux_clean(int *fd_src);

int test_lstar_aux_is_returning_as_expected(
        const char *path_to_src,
        const char *path_from_tar_root,
        const int   with_option,
        char       *buffer
);

#endif
