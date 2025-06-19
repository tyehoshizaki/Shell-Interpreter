#include "shell_commands.h"

// handles ctrl+c
void ctrlChandler(int signo)
{ 
    (void)signo;

    write(STDOUT_FILENO, "\n", 1);

    // tells readline to redisplay
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

// changes directory
void cd_func(char *str)
{ // changes current directory

    char *tok = strtok(str, " ");
    if (strcmp(tok, "cd") == 0)
    {
        tok = strtok(NULL, "");
        if (tok == NULL)
        {
            return;
        }
        if (tok[0] == '~')
        {
            char *home = getenv("HOME"); // gets users home

            char path[1024];
            snprintf(path, sizeof(path), "%s%s", home, &tok[1]); // cats the users home with where they want to go

            if (chdir(path) == -1)
            { // changes directory with error checking
                printf("<%s>: No such file or directory\n", path);
            }
        }
        else
        {

            if (chdir(tok) == -1)
            { // changes directory with error checking
                printf("<%s>: No such file or directory\n", tok);
            }
        }
    }
}

// shortens pwd if it could
void check_curr_dirc(char *dirc)
{
    char *home = getenv("HOME");

    if (!home)
        return;

    // length of the home directory
    size_t home_len = strlen(home);

    // compare the first home_len of dirc with home directory
    if (strncmp(home, dirc, home_len) == 0)
    {
        size_t tail_len = strlen(dirc) + home_len;

        // shifts dirc by home_len
        memmove(dirc + 1, dirc + home_len, tail_len + 1);
        dirc[0] = '~';
    }
}

// prints shells history
void print_history()
{
    // make list of history
    HIST_ENTRY **list = history_list();
    if (list)
    {
        for(int i = 0; list[i]; i++){
            printf("%d: %s\n", i + history_base, list[i]->line);
        }
    }
}