#ifndef SHELL_H
#define SHELL_H

#include <sys/wait.h>
#include <unistd.h>

#include "cattar.h"
#include "cd.h"
#include "commons.h"
#include "commons_tsh.h"
#include "cptar.h"
#include "lstar.h"
#include "mvtar.h"
#include "parser.h"
#include "pwd.h"
#include "rmdirtar.h"
#include "rmtar.h"
#include "tsh.h"

#define WELCOME_MSG "--- Running \033[34mtsh\033[0m ---\n"

#define PROMPT_LENGTH  	3
#define PROMPT 			" > "

#define CD_ERR_MSG "cd: string not in pwd: "
#define FORK_ERR_MSG "tsh: an error occured while forking\n"

#define EXIT  1

#define NB_INTERNALS_CMDS 7

#define READER 0
#define WRITER 1

#define TMP_NAME_TEMPLATE ".tempfile-XXXXXX"

#endif
