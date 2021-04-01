#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "history.h"

#define MAX_CMD_LEN 1000

// circlar list or linked list

struct hist_clist *list = NULL;

struct hist_clist {
    size_t insertions;
    size_t capacity;
    void *cmd_storage;
};

struct iterator {
    unsigned int initialized : 1;
    size_t idx;
};

void hist_init(unsigned int limit)
{
    // : set up history data structures, with 'limit' being the maximum
    // number of entries maintained.
    list =  malloc(sizeof(struct hist_clist));
    if (list == NULL) {
        free(list);
        perror("Could not malloc");
    }

    list->capacity = limit;
    list->insertions = 0;

    list->cmd_storage = malloc (list->capacity * MAX_CMD_LEN);
    if (list->cmd_storage == NULL) {
        perror("malloc error");
	    free(list);
    }
}

void hist_destroy(void)
{   
    if (list != NULL) {
        free(list->cmd_storage);
        free(list);
    }
}

void hist_add(const char *cmd)
{
    if (list == NULL) {
        perror("histroy list does not exits");
    }
    size_t idx = list->insertions % list->capacity;
    void *ptr = list->cmd_storage + idx * MAX_CMD_LEN;
    strcpy(ptr, cmd);
    list->insertions++;
}

struct iterator create_iter(void)
{
    struct iterator it = { 0 };
    return it;
}

const char *iterate(struct iterator *iter) {
    const char *ptr = hist_search_cnum(list->insertions - iter->idx - 1);
    iter->idx++;
    return ptr;
}

const char *iterate_rev(struct iterator *iter) {
    if (list->insertions < list->capacity) {
        return hist_search_cnum(iter->idx++);
    } else {
        const char *ptr = hist_search_cnum(iter->idx + list->insertions - list->capacity);
        iter->idx++;
        return ptr;
    }
}

void hist_print(void)
{
    // print history
    const char *elem;
    struct iterator iter = create_iter();
    while ((elem = iterate_rev(&iter)) != NULL) {
        printf("%*zu %s", 3, iter.idx, elem);
    }
}

const char *hist_search_prefix(char *prefix)
{
    // Retrieves the most recent command starting with 'prefix', or NULL
    // if no match found.

    struct iterator iter = create_iter();
    const char *elem;
    while ((elem = iterate(&iter)) != NULL)
    {
        if (strncmp(prefix, elem, strlen(prefix)) == 0) {
            return elem;
        }
    }
    
    return NULL;
}

const char *hist_search_cnum(int command_number)
{
    // Retrieves a particular command number. Return NULL if no match found.
    if (list == NULL || command_number >= list->insertions || ((list->insertions > list->capacity) && (command_number < list->insertions - list->capacity)))
    {
        return NULL;
    } 

    size_t idx = command_number % list->capacity;

    return list->cmd_storage + idx * MAX_CMD_LEN;
}

unsigned int hist_last_cnum(void)
{
    return list->insertions;
}
