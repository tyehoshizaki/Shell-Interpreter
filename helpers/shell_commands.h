#ifndef SHELLCOMMANDS_H
#define SHELLCOMMANDS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <limits.h>
#include <errno.h>

void ctrlChandler(int signo);

void cd_func(char *str);

void check_curr_dirc(char *dirc);

void print_history();

#endif