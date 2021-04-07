
/** @file shell.c
 *  @brief The driver for command line shell - crash.
 *
 *  This file include the functions to drive the command line shell - crash.
 * Including implementation of crash build in functions, and implementation of 
 * handling signal, I/O redirection, background jobs, comment, and executing 
 * external executable programs.
 *
 */

#include <ctype.h>
#include <fcntl.h>
#include <sys/prctl.h>
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

static struct elist * jobs_list; /**< elist structure to hold back ground jobs*/

/**
 * @brief built-in cd function for crash
 * 
 * @param tokens tokenzied command
 * @return int value, zero on success, non-zero on failure
 */
int handle_cd(struct elist *tokens); 

/**
 * @brief built-in history function for crash
 * 
 * @param tokens tokenzied command
 * @return int value, zero on success, non-zero on failure
 */
int handle_history(struct elist *tokens);

/**
 * @brief handle executing external executable programs in child process 
 * 
 * @param tokens tokenzied command
 * @return int value, zero on success, non-zero on failure
 */
int handle_child_execution(struct elist *tokens);

/**
 * @brief built-in list jobs function for crash
 * 
 */
void handle_jobs(void);

/**
 * @brief handler for SIGINT signal
 * 
 * @param signo the signal number
 */
void sigint_handler(int signo);

/**
 * @brief Helper method for tokenziation of the command input
 * 
 * @param str_ptr pointer to the command string
 * @param delim delimators to check
 * @return char* pointer to the start of next token
 */
char *next_token(char **str_ptr, const char *delim);

/**
 * @brief helper method for redirect io
 * 
 * @param fname the redirect direction
 * @param open_flages open flags
 * @param open_perms open perms
 * @param redir_Out zero if is redirecting output, one if redirecting input
 * @return int value, zero on success, non-zero on failure
 */
int handle_redirect_helper(char *fname, int open_flages, int open_perms, size_t redir_Out);

/**
 * @brief redirect io 
 * 
 * @param tokens tokenzied command
 * @return int value, zero on success, non-zero on failure
 */
int handle_redirect(struct elist *tokens);

/**
 * @brief handler for SIGCHLD signal
 * 
 * @param signo the signal number
 */
void sigchild_handler(int signo);

/**
 * @brief Print usage for interactive mode.
 */
void print_usage(void) {
    printf("============== welcome to crash ==============\n");
    printf("crash is a command line shell that supports: \n ");
    printf("1. build in functions: \n");
    printf("\t 1.1 cd - for entering directory\n;");
    printf("\t 1.2 history - for review command history\n;");
    printf("\t 1.3 !<command number> or !<command prefix> or !! - for executing history command\n;");
    printf("\t 1.4 jobs - for listing currently-running background jobs\n;");
    printf("\t 1.5 exit - for exiting the crash\n;");
    printf("2. & - background job\n");
    printf("3. # - comment\n");
}

int main(void)
{   
    if (isatty(STDIN_FILENO)) {
        print_usage();
        LOGP("stdin is a TTY; entering interactive mode\n");
    } else {
        set_scripting(1);
        LOGP("data piped in on stdin; entering script mode\n");
    }
    
    init_ui();
    hist_init(100);
    jobs_list = elist_create(10, sizeof(pid_t));

    int childProcessRes = 0;

    char *command;

    signal(SIGINT, SIG_IGN);
    signal(SIGCHLD, sigchild_handler);

    while (true)
    {
        command = read_command();   
    label:
        if (command == NULL)
        {
            break;
        }

        LOG("Input command: %s\n", command);
        if (strlen(command) == 0) {
            continue;
        }
        
        if (strncmp(command, "!", 1) != 0)
        {
            hist_add(&command);
        }

        struct elist *tokens = elist_create(10, sizeof(char **));

        // tolkenized command and add command to a list
        char *next_tok = command;
        char *cur_tok;
        while ((cur_tok = next_token(&next_tok, " \n\t")) != NULL)
        {
            if (strncmp(cur_tok, "#", 1) == 0) {
                break;
            }
            if (elist_add(tokens, &cur_tok) == -1)
            {
                perror("cannot add token");
                break;
            }
        }

        if (elist_size(tokens) == 0)
        {
            perror("no argument\n");
            LOGP("fre1\n");
            elist_destroy(tokens);
            continue;
        }

        // size_t idx = 0;
        // char **current_cmd;
        // while ((current_cmd = (char **)elist_get(tokens, idx++)) != NULL)
        // {
        //     LOG("cmd: %s\n", *current_cmd);
        // }

        char *first_cmd = *(char **)elist_get(tokens, 0);
        // LOG("first: %s\n", first_cmd);

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
            if (elist_size(tokens) != 1 || strlen(first_cmd) <= 1)
            {
                perror("argument issue");
                set_status(1);
            }
            else
            {
                int hist_num;
                if (strcmp(first_cmd, "!!") == 0)
                {
                    hist_num = hist_last_cnum();
                } else {
                    hist_num = atoi((const char *)(first_cmd + 1));
                }
                // as hist num range start from 1
                // error in atoi would be handled in search num
                // LOG("cnum: %d\n", hist_num);
                const char *hist_item = isalpha(*(const char *)(first_cmd + 1)) == 0 ? 
                hist_search_cnum(hist_num) 
                : hist_search_prefix(first_cmd + 1);
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
            LOGP("fre2\n");
            break;
        }
        else if (strcmp("jobs", first_cmd) == 0)
        {
            handle_jobs();
        }
        else
        {
            // process background "&"
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
                prctl(PR_SET_PDEATHSIG, SIGTERM);

                // process redirect
                if (handle_redirect(tokens) != 0) {
                    elist_destroy(tokens);
                    
                    _exit(1);
                }
            
                signal(SIGINT, sigint_handler);
                childProcessRes = handle_child_execution(tokens);
                elist_destroy(tokens);
                hist_destroy();
                elist_destroy(jobs_list);
                destroy_ui();
                _exit(childProcessRes);
            } else {
                int status = 0;
                // LOG("first cmd in parent: %s\n", first_cmd);
                if (isBackground != 0) {
                    set_status(0);
                    elist_add(jobs_list, &child);
                    LOG("back: %zu\n", isBackground);
                    set_status(status);
                } else {
                    waitpid(child, &status, 0);
                    set_status(status);
                }
                LOG("child status %d\n", status);
            }
        }

        /* We are done with command; free it */
        elist_destroy(tokens);
        
    }
    LOGP("fre4\n");
    free(command);
    destroy_ui();
    hist_destroy();
    elist_destroy(jobs_list);

    return childProcessRes;
}

int handle_cd(struct elist *tokens)
{
    char *path;
    if (elist_size(tokens) == 1)
    {
        struct passwd *pw = getpwuid(getuid());
        char *homedir = pw->pw_dir;
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

int handle_child_execution(struct elist *tokens) {
    char **arguments = (char **)elist_get_list(tokens);
    // LOG("child process, excueting: %s\n", *arguments);

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
    LOG("jobs of len: %zu\n", elist_size(jobs_list));
    size_t idx = 0;
    while (idx < elist_size(jobs_list)) {
        printf("[%zu] %d\n", idx, *(pid_t *) elist_get(jobs_list, idx));
        idx++;
    }
}

void sigint_handler(int signo) {
    perror("child interupted");
    _exit(EINTR);
}

char *next_token(char **str_ptr, const char *delim)
{
    if (*str_ptr == NULL || **str_ptr == '\0')
    {
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

int handle_redirect_helper(char *fname, int open_flages, int open_perms, size_t redir_Out) {
    int fd = open(fname, open_flages, open_perms);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    if (dup2(fd, fileno(redir_Out == 0 ? stdout : stdin)) == -1) {
        perror("dup2");
        close(fd);
        return 1;
    }
   
    close(fd);
    return 0;
}

int handle_redirect(struct elist *tokens) {
    size_t idx = 0;
    int open_perms = 0666;
    int create_flags = O_RDWR | O_CREAT | O_TRUNC;
    int append_flags = O_RDWR | O_CREAT | O_APPEND;

    int newSize = elist_size(tokens);
    int redirectRes = 0;
    while (idx < elist_size(tokens))
    {   
        char *current_cmd = *(char **) elist_get(tokens, idx++);
        if (strcmp(current_cmd, ">") == 0) {
            newSize = newSize == elist_size(tokens) ? idx : newSize;
            char * fname = *(char **) elist_get(tokens, idx++);
            redirectRes = handle_redirect_helper(fname, create_flags, open_perms, 0);
        } else if (strcmp(current_cmd, ">>") == 0) {
            newSize = newSize == elist_size(tokens) ? idx : newSize;
            char * fname = *(char **) elist_get(tokens, idx++);
            redirectRes = handle_redirect_helper(fname, append_flags, open_perms, 0);
        } else if (strcmp(current_cmd, "<") == 0) {
            newSize = newSize == elist_size(tokens) ? idx : newSize;
            char * fname = *(char **) elist_get(tokens, idx++);
            redirectRes = handle_redirect_helper(fname, O_RDONLY, open_perms, 1);
        }
        // LOG("idx %zu\n", idx);
    }
    // LOGP("traverse done\n");

    if (newSize != elist_size(tokens)) {
        elist_set_capacity(tokens, newSize);
            elist_remove(tokens, newSize - 1);
    }
    
    return redirectRes;
}

void sigchild_handler(int signo) {
    pid_t pid;
    int status;
    set_status(0);
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        int idx = elist_index_of(jobs_list, &pid);
        elist_remove(jobs_list, idx);
        // LOG("\npid: %d\n", pid);
    }
}
