#ifndef TEST_PATH_CHECKER_AUX_H
#define TEST_PATH_CHECKER_AUX_H

#include <assert.h>
#include <linux/limits.h>
#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../../src/inc/commons_tsh.h"
#include "../../src/inc/cd.h"
#include "../../src/inc/lstar.h"
#include "../../src/inc/cptar.h"
#include "test_parser_aux.h"

void test_commons_tsh_init(char *cwd, s_tsh_state *tsh_state);
void test_commons_tsh_init2(char *cwd, s_tsh_state *tsh_state);
void test_commons_tsh_clean(s_tsh_state *tsh_state);
s_tsh_state *test_commons_tsh_create_state_from_cwd(void);

#endif
