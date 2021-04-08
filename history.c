/** @file history.c
 *  @brief  shell history data structures and retrieval functions.
 *
 * This file contains data structe and functions that support listing history.
 * 
 * - data structure:
 * History command are stored in a circular list data stucture, limited by the @param limit;
 * Once the command count reaches the limit, it would override the previous commands.
 * 
 * - supportive method included:
 *  include initializing, adding, searching by number or prefix, displaying, and destorying the history list.
 */

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "history.h"
#include "logger.h"

struct hist_clist *list = NULL;

/**
 * @brief @struct hist_clist are used to store the command history 
 * 
 */
struct hist_clist {
    size_t insertions;  /**< number of insertion of history*/
    size_t capacity;    /**< capacity of the circular list used to store history command */
    void *cmd_storage;  /**< the storage space for history command*/
};

/**
 * @brief the iterator of history list
 * 
 */
struct iterator {
    unsigned int initialized : 1;   /**< initialized or not*/
    size_t idx;                     /**< index of iterator*/
};

void hist_init(unsigned int limit)
{
    // set up history data structures, with 'limit' being the maximum
    // number of entries maintained.
    list =  malloc(sizeof(struct hist_clist));
    if (list == NULL) {
        perror("Could not malloc");
    }

    list->capacity = limit;
    list->insertions = 0;

    list->cmd_storage = malloc (list->capacity * sizeof(char *));
    if (list->cmd_storage == NULL) {
        perror("malloc error");
	    free(list);
    }
}

void hist_add(void *cmd)
{
    if (list == NULL) {
        perror("histroy list does not exits");
    }
    size_t idx = list->insertions % list->capacity;
    char **ptr = (char **)((char *)list->cmd_storage + idx * (sizeof(char *)));
    if (list->insertions >= list->capacity) {
        // override the previous storage
        // since we are storing malloc char array, 
        // need to free before override
        free(*ptr);
    }
    memcpy(ptr, cmd, sizeof(char *));
    list->insertions++;
}

struct iterator create_iter(void)
{
    struct iterator it = { 0 };
    return it;
}


void *get_idx(int idx)
{
    // Retrieves a particular command number. Return NULL if no match found.
    if (hist_idx_isValid(idx) != 0)
    {
        return NULL;
    } 

    size_t real_idx = idx % list->capacity;

    return (void *)((char *)list->cmd_storage + real_idx * sizeof(char *));
}

int hist_idx_isValid(int idx) 
{
    if (list == NULL || idx >= list->insertions || ((list->insertions > list->capacity) && (idx < list->insertions - list->capacity))) {
        return 1;
    } else {
        return 0;
    }
}

void *iterate(struct iterator *iter) {
    void *ptr = get_idx(list->insertions - iter->idx - 1);
    iter->idx++;
    return ptr;
}

void *iterate_rev(struct iterator *iter) {
    if (list->insertions < list->capacity) {
        return get_idx(iter->idx++);
    } else {
        void *ptr = get_idx(iter->idx + list->insertions - list->capacity);
        iter->idx++;
        return ptr;
    }
}

void hist_print(void)
{
    // print history
    void *elem;
    struct iterator iter = create_iter();
    while ((elem = iterate_rev(&iter)) != NULL) {
        if (list->insertions < list->capacity) {
            printf("%*zu %s\n", 3, iter.idx, *(char **) elem);
            fflush(stdout);
        } else {
            printf("%*zu %s\n", 3, iter.idx - list->capacity + list->insertions, *(char **)elem);
            fflush(stdout);
        }
    }
}

const char *hist_search_cnum(int command_num) {
    return get_idx(command_num - 1) == NULL ? NULL : (const char *) *(char **) get_idx(command_num - 1);
}

const char *hist_search_prefix(char *prefix)
{
    // Retrieves the most recent command starting with 'prefix', or NULL
    // if no match found.
    struct iterator iter = create_iter();
    void *elem;
    while ((elem = iterate(&iter)) != NULL)
    {
        char * currentStr = *(char **)elem;
        if (strncmp(prefix, currentStr, strlen(prefix)) == 0) {
            return currentStr;
        }
    }
    
    return NULL;
}

unsigned int hist_last_cnum(void)
{
    return list->insertions;
}


void hist_destroy(void)
{   
    if (list != NULL) {
        struct iterator iter = create_iter();
        char ** elem;
        while ((elem = iterate_rev(&iter)) != NULL) {
            free(*elem);
        }
        free(list->cmd_storage);
        free(list);
    }
}
