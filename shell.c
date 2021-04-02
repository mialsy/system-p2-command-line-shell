#include <fcntl.h>
#include <pwd.h>
#include <readline/readline.h>
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
    if (elist_size(tokens) == 1) {
        struct passwd *pw = getpwuid(getuid());
        char *homedir = pw->pw_dir;
        LOG("home dir: %s\n", homedir);
        path = homedir;
    } else if (elist_size(tokens) == 2){
        path = *(char **) elist_get(tokens, 1);
    } else {
        perror("receiving too many argument");
        return 2;
    }
    LOG("path: %s\n", path);

    if (chdir(path) != 0) {
        perror("cd error: ");
        return 1;
    } else {
        return 0;
    }
}

int handle_history(struct elist *tokens) {

    if (elist_size(tokens) == 1) {
        hist_print();
        return 0;
    } else {
        perror("receiving too many argument\n");
        return 1;
    }
}

void handle_jobs(void) {

}

char *next_token(char **str_ptr, const char *delim)
{
    if (*str_ptr == NULL || **str_ptr == '\0') {
        LOGP("return null\n");
        return NULL;
    }
    LOG("str: %s\n", *str_ptr);

    size_t tok_start = strspn(*str_ptr, delim);
    size_t tok_end = strcspn(*str_ptr + tok_start, delim);

    LOG("start: %zu, end %zu\n", tok_start, tok_end);
    if (tok_end  == 0) {
        return NULL;
    }

    char *current_ptr = *str_ptr + tok_start;

    *str_ptr += tok_start + tok_end;

    if (**str_ptr != '\0') {
        **str_ptr = '\0';
        (*str_ptr)++;
    }

    LOG("current tok: %s\n", current_ptr);
    
    return current_ptr;
}

int main(void)
{
    init_ui();
    hist_init(10);

    char *command;

    while (true) {
        command = read_command();
        label:
        if (command == NULL) {
            break;
        }

        LOG("Input command: %s\n", command);
        hist_add(command);

        struct elist *tokens = elist_create(10, sizeof(char **));

        // tolkenized command and add command to a list
        char *next_tok = command;
        char *cur_tok;
        while ((cur_tok = next_token(&next_tok, " ")) != NULL) 
        {   
            if (elist_add(tokens, &cur_tok) == -1) {
                perror("cannot add token");
                break;
            }
        }

        if (elist_size(tokens) == 0) {
            perror("no argument\n");
            free(command);
            elist_destroy(tokens);
            continue;
        }

        size_t idx = 0;
        char **current_cmd;
        while ((current_cmd = (char **)elist_get(tokens, idx++)) != NULL) {
            LOG("cmd: %s\n", *current_cmd);
        }

        // TODO: check for built-in functions
        char * first_cmd = *(char **)elist_get(tokens, 0);
        LOG("first: %s\n", first_cmd);

        // check built-ins
        if (strcmp("cd", first_cmd) == 0) {
            // cd
            set_status(handle_cd(tokens));
        } else if (strcmp("history", first_cmd) == 0) {
            // history
            set_status(handle_history(tokens));            
        } else if (strncmp("!", first_cmd, 1) == 0) {
            // hist go to
            if (elist_size(tokens) == 1 && strcmp(first_cmd, "!!") == 0) {
                strcpy(command, hist_search_cnum(hist_last_cnum() - 1));
                set_status(0);
                elist_destroy(tokens);
                goto label;
            } else if (elist_size(tokens) == 2 && strcmp(first_cmd, "!") == 0) {
                int hist_num = atoi(*(char **)elist_get(tokens, 1));
                LOG("hist num: %d\n", hist_num);
                const char *hist_item = hist_search_cnum(hist_num);
                if (hist_item == NULL) {
                    perror("history item not found");
                    set_status(1);
                } else {
                    strcpy(command, hist_item);
                    set_status(0);
                    elist_destroy(tokens);
                    goto label;
                }
            } else {
                perror("too many argument");
                set_status(1);
            }
        } else if (strcmp("exit", first_cmd) == 0) {
            // exit
            elist_destroy(tokens);
            free(command);
            break;
        } else if (strcmp("jobs", first_cmd) == 0) {
            handle_jobs();
        } else {
            // TODO: preprocess command (before redirection "<>>", background "&")


            // TODO: fork a child process
            

            // TODO: excute whatever command the user asked for
            
        }

        

        /* We are done with command; free it */
        elist_destroy(tokens);
        free(command);
    }
    hist_destroy();
    destroy_ui();

    return 0;
}
