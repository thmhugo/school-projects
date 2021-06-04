#ifndef TEST_CATTAR_AUX_H
#define TEST_CATTAR_AUX_H

#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../src/inc/commons.h"
#include "../../src/inc/cattar.h"
#include "commons_test.h"

int test_cattar_aux_is_returning_as_expected(
	const char *path_to_src,
	const char *file_path,
	char 	   *buffer
);

#endif
