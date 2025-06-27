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

char **builtins = NULL;
size_t builtins_count = 0;

void init_builtins(int flag) {

    if(flag){
        char *source = "/bin";
        DIR *d = opendir(source);
        if (!d) return;
        struct dirent *entry;
        while ((entry = readdir(d))) {
            if (entry->d_name[0] == '.') continue;
            char path[PATH_MAX];
            snprintf(path, sizeof path, "%s/%s", source, entry->d_name);
            struct stat st2;
            if (stat(path, &st2) == 0
             && S_ISREG(st2.st_mode)
             && (st2.st_mode & S_IXUSR)) 
            {
                builtins = realloc(builtins, (builtins_count + 2) * sizeof *builtins);
                builtins[builtins_count++] = strdup(entry->d_name);
            }
        }
        closedir(d);
    }else{
        FILE* file = fopen(FILENAME, "r");
        if (!file){
            perror("fopen");
            return;
        } 

        char *line = NULL;
        size_t n = 0;
        ssize_t nread;

        while((nread = getline(&line, &n, file)) != -1){
            line[strcspn(line,"\r\n")] = '\0';
            if(*line == '\0') continue;
            builtins = realloc(builtins, (builtins_count + 2) * sizeof(builtins));
            builtins[builtins_count++] = strdup(line);
        }
        free(line);
        fclose(file);
    }

    if (builtins)
        builtins[builtins_count] = NULL;
}

void free_builtins(void) {
    for (size_t i = 0; i < builtins_count; i++)
        free(builtins[i]);
    free(builtins);
    builtins = NULL;
    builtins_count = 0;
}


char *commandgenerator(const char *text, int state){

    int idx, len;
    const char *name;


    if(state == 0){
        idx = 0;
        len = strlen(text);
    }

    while((idx < builtins_count)){
        name = builtins[idx++];
        if (strncmp(name, text, len) == 0)
        return strdup(name);
    }

    return rl_filename_completion_function(text, state);
}

char **ssi_completion_func(const char *word, int start, int end){
    rl_attempted_completion_over = 1;
    return rl_completion_matches(word, commandgenerator);
}