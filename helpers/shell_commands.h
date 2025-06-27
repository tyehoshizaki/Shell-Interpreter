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
#include <bits/local_lim.h>
#include <errno.h>

#include <dirent.h>
#include <sys/stat.h>

#define FILENAME "./helpers/commands.txt"

// dynamic built-ins populated from filename
extern char **builtins;
extern size_t builtins_count;

// initialize and tear down
void init_builtins(int flag);

void free_builtins(void);

void ctrlChandler(int signo);

void cd_func(char *str);

void check_curr_dirc(char *dirc);

void print_history();

char *commandgenerator(const char *text, int state);

char **ssi_completion_func(const char *word, int start, int end);

#endif