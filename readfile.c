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
#include "logger.h"


void main(void)
{
    char *command = NULL;

    signal(SIGINT, SIG_IGN);
    size_t len = 0;

    int count = 0;
    printf("%d \n", isatty(STDIN_FILENO));
    while ((isatty(STDIN_FILENO) || getline(&command, &len, stdin) != -1) && count < 50)
    {
        printf("line %d: %s\n", count++, command);
        pid_t child = fork();

            if (child == -1)
            {
                perror("fork");
            }
            else if (child == 0)
            {
                // handle excution
                LOG("child: %s\n", command);
                _exit(0);
            } else {
                 waitpid(child, NULL, 0);
                
            }
    }
    free(command);
    LOGP("hi\n");
}