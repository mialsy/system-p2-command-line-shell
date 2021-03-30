#include <stdio.h>
#include <readline/readline.h>
#include <limits.h>
#include <locale.h>
#include <malloc.h>
#include <pwd.h>
#include <stdlib.h>

#include "history.h"
#include "logger.h"
#include "ui.h"

static const char *good_str = "😌";
static const char *bad_str  = "🤯";
unsigned int command_count = 0;
char *cwd = NULL;
char *hostname = NULL;

static int readline_init(void);

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
    // TODO cleanup code, if necessary
    free(cwd);
    free(hostname);
}

// do not need to change
char *prompt_line(void)
{
    const char *status = prompt_status() ? bad_str : good_str;

    char cmd_num[25];
    snprintf(cmd_num, 25, "%d", prompt_cmd_num());

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

    snprintf(prompt_str, prompt_sz, format_str,
            status,
            cmd_num,
            user,
            host,
            cwd);

    return prompt_str;
}

char *prompt_username(void)
{
    char *username = getlogin();
    return username == NULL ? "unknown_user" : username;
}

char *prompt_hostname(void)
{   
    hostname = malloc(HOST_NAME_MAX + 1);
    gethostname(hostname, HOST_NAME_MAX + 1);
    return strlen(hostname) == 0 ? "unknown_host" : hostname;
}

char *prompt_cwd(void)
{
    cwd = malloc(PATH_MAX);
    getcwd(cwd, PATH_MAX);

    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    
    size_t homelen = strlen(homedir);
    size_t cwdlen = strlen(cwd);

    if (homelen <= cwdlen && memcmp(homedir, cwd, homelen) == 0) {
        const char *homeShort = "~";
        strncpy(cwd, homeShort, strlen(homeShort));
    }

    return strlen(cwd) == 0 ? "/unknown/path": cwd;
}

int prompt_status(void)
{
    return -1;
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
    return 0;
}

int key_up(int count, int key)
{
    /* Modify the command entry text: */
    rl_replace_line("User pressed 'up' key", 1);

    /* Move the cursor to the end of the line: */
    rl_point = rl_end;

    // TODO: step back through the history until no more history entries are
    // left. Once the end of the history is reached, stop updating the command
    // line.

    return 0;
}

int key_down(int count, int key)
{
    /* Modify the command entry text: */
    rl_replace_line("User pressed 'down' key", 1);

    /* Move the cursor to the end of the line: */
    rl_point = rl_end;

    // TODO: step forward through the history (assuming we have stepped back
    // previously). Going past the most recent history command blanks out the
    // command line to allow the user to type a new command.

    return 0;
}