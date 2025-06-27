#ifndef SSI_H
#define SSI_H

#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "helpers/shell_commands.h"
#include "helpers/pipe.h"

#define MAX_LOG_NAME 2048
#define MAX_HOSTNAME_LEN 2048
#define MAX_PATH 2048

#define MAX_BG_PROCESSES 100

#define RL_START "\001"
#define RL_END   "\002"

#define KNRM     "\x1b[0m"    // normal
#define KBLK     "\033[30m"   //black
#define KRED     "\x1b[31m"   // red
#define KGRN     "\x1b[32m"   // green
#define KYEL     "\x1b[33m"   // yellow
#define KBLU     "\x1b[34m"   // blue
#define KMAG     "\x1b[35m"   // magenta
#define KCYN     "\x1b[36m"   // cyan
#define BOLD     "\033[1m"    // bold

typedef struct
{
    pid_t pid;
    char cmd[MAX_INPUT_SIZE];
    char dirc[MAX_PATH];
} process;




#endif