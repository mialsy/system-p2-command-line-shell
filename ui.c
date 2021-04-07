/** @file ui.c
 *  @brief The c file for UI functionality for crash program.
 *
 *  This c file contains elements to construct the ui 
 * and functions to help building the ui, including:
 * 1. functions getting command number, username, hostname,
 * the current working directy and process exit status;
 * 2. functions to set the process status
 * 3. key up and key down function to handle rolling back or moving forward
 * in command history.
 */

#include <limits.h>
#include <locale.h>
#include <malloc.h>
#include <pwd.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>

#include "history.h"
#include "logger.h"
#include "ui.h"

static const char *good_str = "😌";         /**< emoji to display when process status is good*/
static const char *bad_str  = "🤯";         /**< emoji to display when process status is bad*/
static unsigned int command_count = 0;      /**< current command number*/
static int history_offset = 0;              /**< offset of history index, calculated by key up and key down */
static int status_code = 0;                 /**< previous process status code*/

/** 
 * @brief Helper method to set up readline
 * 
 * @return int value - zero on succcess
 */
static int readline_init(void);

void set_status(int num) {
    status_code = num;
}

void init_ui(void)
{
    LOGP("Initializing UI...\n");

    char *locale = setlocale(LC_ALL, "en_US.UTF-8");
    LOG("Setting locale: %s\n",
            (locale != NULL) ? locale : "could not set locale!");

    rl_startup_hook = readline_init;
}

void destroy_ui(void)
{
}

char *prompt_line(void)
{
    const char *status = prompt_status() ? bad_str : good_str;

    char cmd_num[25];
    snprintf(cmd_num, 25, "%u", prompt_cmd_num());

    char *user = prompt_username();
    char *host = prompt_hostname();
    char *cwd = prompt_cwd();

    char *format_str = ">>-[%s]-[%s]-[%s@%s:%s]-> ";

    size_t prompt_sz
        = strlen(format_str)
        + strlen(status)
        + strlen(cmd_num)
        + strlen(user)
        + strlen(host)
        + strlen(cwd)
        + 1;

    char *prompt_str =  malloc(sizeof(char) * prompt_sz);

    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    
    size_t homelen = strlen(homedir);

    char *cwd_short = cwd;

    if (memcmp(homedir, cwd_short, homelen) == 0) {
        cwd_short = cwd + homelen - 1;
        *cwd_short = '~';
    } else {
        cwd_short = cwd;
    }

    snprintf(prompt_str, prompt_sz, format_str,
            status,
            cmd_num,
            user,
            host,
            cwd_short);

    free(cwd);
    free(host);
    return prompt_str;
}

char *prompt_username(void)
{
    char *username = getlogin();
    return username == NULL ? "unknown_user" : username;
}

char *prompt_hostname(void)
{   
    char *hostname = malloc(sizeof(char) * (HOST_NAME_MAX + 1));
    if (gethostname(hostname, HOST_NAME_MAX + 1) != 0) {
        strcpy(hostname, "unkown_host");
    }
    return hostname;
}

char *prompt_cwd(void)
{
    char *cwd = malloc(sizeof(char) * PATH_MAX);
    getcwd(cwd, PATH_MAX);
    
    return cwd;
}

int prompt_status(void)
{
    return status_code;
}

unsigned int prompt_cmd_num(void)
{
    return ++command_count;
}

char *read_command(void)
{
    char *prompt = prompt_line();
    char *command = readline(prompt);
    free(prompt);
    return command;
}

int readline_init(void)
{
    rl_bind_keyseq("\\e[A", key_up);
    rl_bind_keyseq("\\e[B", key_down);
    rl_variable_bind("show-all-if-ambiguous", "on");
    rl_variable_bind("colored-completion-prefix", "on");
    history_offset = 0;
    return 0;
}

int key_up(int count, int key)
{
    /* Modify the command entry text: */
    rl_replace_line(hist_idx_isValid(hist_last_cnum() + history_offset - 1) == 0 ? hist_search_cnum(hist_last_cnum() + history_offset) : "" , 1);

    /* Move the cursor to the end of the line: */
    rl_point = rl_end;

    // step back through the history until no more history entries are
    // left. Once the end of the history is reached, stop updating the command
    // line.
    if (hist_idx_isValid(hist_last_cnum() + history_offset - count - 1) == 0) {
        history_offset -= count;
    }

    return 0;
}

int key_down(int count, int key)
{
    /* Modify the command entry text: */
    rl_replace_line(hist_idx_isValid(hist_last_cnum() + history_offset - 1) == 0 ? hist_search_cnum(hist_last_cnum() + history_offset) : "" , 1);

    /* Move the cursor to the end of the line: */
    rl_point = rl_end;

    // step forward through the history (assuming we have stepped back
    // previously). Going past the most recent history command blanks out the
    // command line to allow the user to type a new command.

    if (hist_idx_isValid(hist_last_cnum() + history_offset + count - 1) == 0) {
        history_offset += count;
    }

    return 0;
}
