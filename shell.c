#include <fcntl.h>
#include <pwd.h>
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

int handle_cd(const char *path) 
{
    if (chdir(path) != 0) {
        perror("cd error: ");
        return 1;
    } else {
        return 0;
    }
}

int handle_history(void) {
    hist_print();
    return 0;
}

void handle_hist_excution(void) {

}

void handle_jobs(void) {

}

void handle_exit(void) {

}

char *next_token(char **str_ptr, const char *delim)
{
    if (*str_ptr == NULL) {
        return NULL;
    }

    size_t tok_start = strspn(*str_ptr, delim);
    size_t tok_end = strcspn(*str_ptr + tok_start, delim);

    if (tok_end  == 0) {
        *str_ptr = NULL;
        return NULL;
    }

    char *current_ptr = *str_ptr + tok_start;

    *str_ptr += tok_start + tok_end;

    if (**str_ptr == '\0') {
        *str_ptr = NULL;
    } else {
        **str_ptr = '\0';
        (*str_ptr)++;
    }

    return current_ptr;
}

int main(void)
{
    init_ui();
    hist_init(10);

    char *command;


    while (true) {
        command = read_command();
        if (command == NULL) {
            break;
        }

        LOG("Input command: %s\n", command);
        hist_add(command);

        struct elist *tokens = elist_create(10, 100);

        // tolkenized command and add command to a list
        char *next_tok = command;
        char *cur_tok;
        while ((cur_tok = next_token(&next_tok, " ")) != NULL) 
        {   
            if (elist_add(tokens, &cur_tok) == -1) {
                perror("cannot add token");
                free(command);
                elist_destroy(tokens);
                break;
            }
        }

        if (elist_size(tokens) == 0) {
            perror("no argument\n");
            free(command);
            elist_destroy(tokens);
            continue;
        }
        
        // TODO: check for built-in functions
        char * first_cmd = *(char **)elist_get(tokens, 0);
        LOG("first: %s\n", first_cmd);

        if (strcmp("cd", first_cmd) == 0) {
            if (elist_size(tokens) == 1) {
                struct passwd *pw = getpwuid(getuid());
                const char *homedir = pw->pw_dir;
                LOG("home dir: %s\n", homedir);

                handle_cd(homedir);
            } else if (elist_size(tokens) == 2){
                handle_cd(*(char**) elist_get(tokens, 1));
            } else {
                perror("receiving too many argument");
                free(command);
                elist_destroy(tokens);
                continue;
            }
        } else if (strcmp("history", first_cmd) == 0) {
            LOGP("handle history");
            if (elist_size(tokens) == 1) {
                handle_history();
            } else {
                perror("receiving too many argument\n");
                free(command);
                elist_destroy(tokens);
                continue;
            }
        } else if (strcmp("exit", first_cmd) == 0) {
            elist_destroy(tokens);
            free(command);
            break;
        } 

        // TODO: preprocess command (before redirection "<>>", background "&")
        // TODO: fork a child process
        // TODO: excute whatever command the user asked for

        /* We are done with command; free it */
        elist_destroy(tokens);
        free(command);
    }
    hist_destroy();
    destroy_ui();

    return 0;
}
