#include "ssi.h"

int main(int argc, char *argv[])
{
    int builtin_flag = 0;
    if (argc > 1){
        builtin_flag = 1;
    }

    char username[MAX_LOG_NAME], hostname[MAX_HOSTNAME_LEN], curr_dirc[MAX_PATH]; // varaibles
    process *pids[MAX_BG_PROCESSES];
    int pid_count = 0;

    // handles crtl + c
    if (signal(SIGINT, ctrlChandler) == SIG_ERR)
    {
        perror("signal");
        return 1;
    }

    // creates a history file
    char histfile[MAX_PATH];
    char *home = getenv("HOME");
    if (home)
    {
        snprintf(histfile, sizeof(histfile), "%s/.ssi_history", home);
        using_history();
        read_history(histfile);
        stifle_history(1000);

        init_builtins(builtin_flag);

        rl_readline_name = "ssi";
        rl_attempted_completion_function = ssi_completion_func;
        rl_bind_key('\t', rl_complete);
    }

    pid_t done;
    int status;

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
        char prompt[MAX_LOG_NAME + MAX_HOSTNAME_LEN + MAX_PATH + 100];
        check_curr_dirc(curr_dirc);

        snprintf(prompt, sizeof(prompt),
        RL_START BOLD RL_END
        RL_START KGRN RL_END   // start green
        "%s"                     // username
        RL_START KBLK RL_END
        "@"
        RL_START KCYN RL_END    // start cyan
        "%s"                     // hostname
        RL_START KBLK RL_END
        ": "
        RL_START KMAG RL_END    // start blue
        "%s"                     // cwd (with ~ if home)
        RL_START KBLK RL_END
        " -> "
        RL_START KNRM RL_END,
        username, hostname, curr_dirc
        );

        char bold_prompt[sizeof(prompt) + 10];

        snprintf(bold_prompt, sizeof(bold_prompt), "\033[1m%s", prompt);

        // read input
        char *line = readline(prompt);

        while ((done = waitpid(-1, &status, WNOHANG)) > 0) {
            for (int i = 0; i < MAX_BG_PROCESSES; i++){
                if(pids[i] != NULL && pids[i]->pid == done){
                    printf("%d: %s %s has terminated\n", pids[i]->pid, pids[i]->dirc, pids[i]->cmd);
                    free(pids[i]);
                    pids[i] = NULL;
                }
            }
        }

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
            int status;

            char *tok = strtok(line, " ");

            // command to change directory
            if (strcmp(tok, "cd") == 0)
            {
                cd_func(input);
            }
            // command to make background process
            else if (strcmp(tok, "bg") == 0)
            {

                char *input = strtok(NULL,"\n");

                pid_t cur_process = fork();
                if (cur_process < 0)
                {
                    exit(EXIT_FAILURE);
                }

                if(cur_process == 0){
                    char file[MAX_PATH];
                    snprintf(file, sizeof(file), "%s/Code/shell_interpreter/bg/ProcessesOutput.txt", home);
                    int outfile = open(file, O_WRONLY | O_APPEND | O_CREAT, 0644);
                    if (outfile == -1) 
                    {
                        perror("open ProcessesOutput");
                        continue;
                    }
                    dup2(outfile, STDOUT_FILENO);
                    close(outfile);

                    setpgid(0, 0);

                    bin_func(input);
                    exit(1);
                }else{
                    process *cur = malloc(sizeof(process));
                    strcpy(cur->cmd, input);
                    strcpy(cur->dirc, curr_dirc);
                    cur->pid = cur_process;
                    
                    pids[pid_count%MAX_BG_PROCESSES] = cur;
                    pid_count++;

                    continue;
                }

            }
            // command to list background processes
            else if (strcmp(tok, "bglist") == 0)
            {
                for(int i = 0; i < MAX_BG_PROCESSES; i++){
                    if(pids[i] != NULL){
                        printf("%d: %s %s is running\n", pids[i]->pid, pids[i]->dirc, pids[i]->cmd);
                    }
                }
            }
            else if(strcmp(tok, "bgkill") == 0)
            {
                char *arg = strtok(NULL, "\n");
                pid_t pidc = (pid_t)atoi(arg);

                for(int i = 0; i < MAX_BG_PROCESSES; i++)
                {
                    if(pids[i] != NULL && pids[i]->pid == pidc)
                    {
                        if(kill(-pidc, SIGINT) == -1)
                        {
                            if(kill(-pidc, SIGTERM) == -1)
                            {
                                kill(-pidc, SIGKILL);
                            }
                            perror("kill");
                            continue;
                        }
                        free(pids[i]);
                        pids[i] = NULL;
                    }
                }
            }
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

    free_builtins();
    return 0;
}