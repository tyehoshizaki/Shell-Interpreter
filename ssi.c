#include "helpers/shell_commands.h"
#include "helpers/pipe.h"

#define MAXLOGNAME 516
#define MAXHOSTNAMELEN 516
#define MAXPATH 516

int main()
{

    char username[MAXLOGNAME], hostname[MAXHOSTNAMELEN], curr_dirc[MAXPATH]; // varaibles

    // handles crtl + c
    if (signal(SIGINT, ctrlChandler) == SIG_ERR)
    {
        perror("signal");
        return 1;
    }

    // creates a history file
    char histfile[MAXPATH];
    char *home = getenv("HOME");
    if (home)
    {
        snprintf(histfile, sizeof(histfile), "%s/.ssi_history", home);
        using_history();
        read_history(histfile);
        stifle_history(1000);
        rl_readline_name = "ssi";
    }

    while (1)
    {
        // geting info
        if (getcwd(curr_dirc, sizeof(curr_dirc)) == NULL)
        {
            perror("getcwd");
            break;
        }
        if (gethostname(hostname, sizeof(hostname)) == -1)
        {
            perror("gethostname");
            break;
        }
        if (getlogin_r(username, sizeof(username)) != 0)
        {
            perror("getlogin");
            break;
        }

        // promt
        char prompt[MAXLOGNAME + MAXHOSTNAMELEN + MAXPATH + 7];
        check_curr_dirc(curr_dirc);

        snprintf(prompt, sizeof(prompt), "%s@%s: %s -> ", username, hostname, curr_dirc);

        // read input
        char *line = readline(prompt);

        // add to history
        if (*line)
            add_history(line);

        // exits
        if (strcmp(line, "exit") == 0)
        {
            free(line);
            break;

        } // empty input
        else if (line[0] == '\0')
        {
            free(line);
            continue;
        }
        else
        {
            char input[MAX_INPUT_SIZE];
            strcpy(input, line);

            char *tok = strtok(line, " ");

            // command to change directory
            if (strcmp(tok, "cd") == 0)
            {
                cd_func(input);
            }
            // history
            else if (strcmp(tok, "history") == 0)
            {
                print_history();
            }
            else if (strcmp(tok, "clear_history") == 0)
            {
                clear_history();
            }
            // foreground executions
            else
            {
                bin_func(input);
            }

            free(line);
        }

        // write to history file
        if (home)
            write_history(histfile);
    }

    return 0;
}