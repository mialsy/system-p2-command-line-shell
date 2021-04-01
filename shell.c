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

#include "history.h"
#include "logger.h"
#include "ui.h"

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

        // TODO: tolkenized command

        // TODO: add each token to a list
        // TODO: check for built-in functions
        // TODO: preprocess command (before redirection "<>>", background "&")
        // TODO: fork a child process
        // TODO: excute whatever command the user asked for

        /* We are done with command; free it */
        free(command);
    }

    return 0;
}
