#ifndef PIPE_H
#define PIPE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 4096
#define MAX_NUM_CMDS 16

void bin_func(char str[]);

#endif