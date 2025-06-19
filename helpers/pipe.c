#include "pipe.h"

void bin_func(char str[MAX_INPUT_SIZE])
{

    // getting args
    char lines[MAX_NUM_CMDS][MAX_INPUT_SIZE];
    int count = 0;
    int n;
    char *tok = strtok(str, "|");

    while (tok != NULL)
    {
        strcpy(lines[count], tok);
        count++;
        tok = strtok(NULL, "|\n");
    }

    char *cmds[count][MAX_INPUT_SIZE]; // will store the commands and args for each command

    // takes the inputs and splits each command into separate arguments (also cleans up the input)
    for (int i = 0; i < count; i++)
    {
        char *tok = strtok(lines[i], " \n");
        int num_of_args = 0;
        while (tok != NULL && num_of_args < 8)
        {
            cmds[i][num_of_args] = tok;

            num_of_args++;
            tok = strtok(NULL, " \n");
        }
        cmds[i][num_of_args] = NULL;
    }

    int pfd[3] = {-1, -1, -1}; // file descriptors pfd[0] = read, pfd[1] = write, pfd[2] = read from previous pipe
    pid_t pids[count];         // hold process ids

    // this loops for each command
    for (int i = 0; i < count; i++)
    {
        pid_t cur_pid;
        // creating pipe if not the last iteration
        if (i < count - 1)
        {
            if (pipe(pfd) == -1)
            {
                exit(EXIT_FAILURE);
            }
        }

        // forking process
        if ((cur_pid = fork()) < 0)
        {
            exit(EXIT_FAILURE);
        }

        // child process
        if (cur_pid == 0)
        {
            // makes the stdin be the prev pipe
            if (pfd[2] != -1)
            {
                dup2(pfd[2], STDIN_FILENO);
            }
            // makes the stdout be the new pipe
            if (i < count - 1)
            {
                dup2(pfd[1], STDOUT_FILENO);
            }

            // closes all the unused file descritors
            if (pfd[0] != -1)
                close(pfd[0]);
            if (pfd[1] != -1)
                close(pfd[1]);
            if (pfd[2] != -1)
                close(pfd[2]);

            // executes the command and writes through the stdout, fd[1]
            if (execvp(cmds[i][0], cmds[i]) == -1)
            {
                printf("<%s>: No such file or directory\n", cmds[i][0]);
                exit(EXIT_FAILURE);
            }
        }
        // closes the old prev pipe
        if (pfd[2] != -1)
            close(pfd[2]);

        // closes the write fd
        if (pfd[1] != -1)
            close(pfd[1]);

        pfd[2] = pfd[0];   // puts the new pipe into the prev pipe place for the next interation
        pids[i] = cur_pid; // stores the child pid for later
    }

    // closes all file descritors
    close(pfd[0]);
    close(pfd[1]);
    close(pfd[2]);

    // waits for all child processes to be done
    for (int i = 0; i < count; i++)
    {
        waitpid(pids[i], NULL, 0);
    }
}