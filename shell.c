#include <fcntl.h>
#include <errno.h>
#include <pwd.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pwd.h>

#include "history.h"
#include "logger.h"
#include "ui.h"
#include "elist.h"

int handle_cd(struct elist *tokens)
{
    char *path;
    if (elist_size(tokens) == 1)
    {
        struct passwd *pw = getpwuid(getuid());
        char *homedir = pw->pw_dir;
        LOG("home dir: %s\n", homedir);
        path = homedir;
    }
    else if (elist_size(tokens) == 2)
    {
        path = *(char **)elist_get(tokens, 1);
    }
    else
    {
        perror("receiving too many argument");
        return 2;
    }
    LOG("path: %s\n", path);

    if (chdir(path) != 0)
    {
        perror("cd error: ");
        return 1;
    }
    else
    {
        return 0;
    }
}

int handle_history(struct elist *tokens)
{

    if (elist_size(tokens) == 1)
    {
        hist_print();
        return 0;
    }
    else
    {
        perror("receiving too many argument\n");
        return 1;
    }
}

int handle_child_excution(struct elist *tokens) {
    char **arguments = (char **)elist_get_list(tokens);
    LOG("child process, excueting: %s\n", *arguments);

    // add null terminator if size is larger than 1
    if (elist_add_new(tokens) == NULL) {
        perror("error elist_add_new");
        return -1;
    }
    
    arguments[elist_size(tokens) - 1] = NULL;
    return execvp(*(char **)elist_get(tokens, 0), arguments);
} 

void handle_jobs(void)
{
}

void handle_signint(int num) {
    perror("child interupted");
    exit(EINTR);
}

char *next_token(char **str_ptr, const char *delim)
{
    if (*str_ptr == NULL || **str_ptr == '\0')
    {
        LOGP("return null\n");
        return NULL;
    }

    size_t tok_start = strspn(*str_ptr, delim);
    size_t tok_end = strcspn(*str_ptr + tok_start, delim);

    if (tok_end == 0)
    {
        return NULL;
    }

    char *current_ptr = *str_ptr + tok_start;

    *str_ptr += tok_start + tok_end;

    if (**str_ptr != '\0')
    {
        **str_ptr = '\0';
        (*str_ptr)++;
    }

    return current_ptr;
}

int main(void)
{   

    if (isatty(STDIN_FILENO)) {
        printf("stdin is a TTY; entering interactive mode\n");
    } else {
        printf("data piped in on stdin; entering script mode\n");
    }

    init_ui();
    hist_init(10);
    int childProcessRes = 0;

    char *command = NULL;
    size_t len = 0;
    ssize_t lineSize = 0;

    signal(SIGINT, SIG_IGN);

    while (true)
    {
        if (isatty(STDIN_FILENO)) {
            command = read_command();
        } else {
            lineSize = getline(&command, &len, stdin);
            LOG("linesize: %zd\n", lineSize);
            
            if (lineSize == -1) {
                break;
            }
            LOG("line: %s\n", command);
        }
        
    label:
        if (command == NULL)
        {
            break;
        }

        LOG("Input command: %s\n", command);
        if (strncmp(command, "!", 1) != 0)
        {
            hist_add(command);
        }

        struct elist *tokens = elist_create(10, sizeof(char **));

        // tolkenized command and add command to a list
        char *next_tok = command;
        char *cur_tok;
        while ((cur_tok = next_token(&next_tok, " ")) != NULL)
        {
            if (elist_add(tokens, &cur_tok) == -1)
            {
                perror("cannot add token");
                break;
            }
        }

        if (elist_size(tokens) == 0)
        {
            perror("no argument\n");
            free(command);
            elist_destroy(tokens);
            continue;
        }

        size_t idx = 0;
        char **current_cmd;
        while ((current_cmd = (char **)elist_get(tokens, idx++)) != NULL)
        {
            LOG("cmd: %s\n", *current_cmd);
        }

        // TODO: check for built-in functions
        char *first_cmd = *(char **)elist_get(tokens, 0);
        LOG("first: %s\n", first_cmd);

        // check built-ins
        if (strcmp("cd", first_cmd) == 0)
        {
            // cd
            set_status(handle_cd(tokens));
        }
        else if (strcmp("history", first_cmd) == 0)
        {
            // history
            set_status(handle_history(tokens));
        }
        else if (strncmp("!", first_cmd, 1) == 0)
        {
            // hist excute, need to set up go to and command
            if (elist_size(tokens) != 1)
            {
                perror("too many argument");
                set_status(1);
            }
            else
            {
                int hist_num;
                if (strcmp(first_cmd, "!!") == 0)
                {
                    hist_num = hist_last_cnum();
                }
                else
                {
                    hist_num = atoi((const char *)(first_cmd + 1));
                }
                // as hist num range start from 1
                // error in atoi would be handled in search num
                const char *hist_item = hist_search_cnum(hist_num);
                if (hist_item == NULL)
                {
                    perror("history item not found");
                    set_status(1);
                }
                else
                {
                    strcpy(command, hist_item);
                    set_status(0);
                    elist_destroy(tokens);
                    goto label;
                }
            }
        }
        else if (strcmp("exit", first_cmd) == 0)
        {
            // exit
            elist_destroy(tokens);
            free(command);
            break;
        }
        else if (strcmp("jobs", first_cmd) == 0)
        {
            handle_jobs();
        }
        else
        {
            // TODO: preprocess command (before redirection "<>>", background "&")

            size_t isBackground = 0;
            if (strncmp("&", first_cmd, 1) == 0) {
                isBackground = 1;
                first_cmd++;
                elist_set(tokens, 0, &first_cmd);
            }

            // fork a child process
            pid_t child = fork();

            if (child == -1)
            {
                perror("fork");
            }
            else if (child == 0)
            {
                // handle excution
                signal(SIGINT, handle_signint);
                childProcessRes = handle_child_excution(tokens);
                elist_destroy(tokens);
                free(command);
                hist_destroy();
                destroy_ui();
                return childProcessRes;
            } else {
                int status;
                LOG("first cmd in parent: %s\n", first_cmd);
                if (isBackground != 0) {
                    set_status(0);
                    LOG("back: %zu\n", isBackground);
                    set_status(status);
                } else {
                    wait(&status);
                    set_status(status);
                }
                LOG("child status %d\n", status);
            }
        }

        /* We are done with command; free it */
        elist_destroy(tokens);
        free(command);
    }
    hist_destroy();
    destroy_ui();

    return childProcessRes;
}
