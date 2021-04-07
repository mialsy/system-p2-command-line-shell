/** @file history.c
 *  @brief  shell history data structures and retrieval functions.
 *
 *  These empty function definitions are provided
 *  so that stdio will build without complaining.
 *  You will need to fill these functions in. This
 *  is the implementation of the console driver.
 *  Important details about its implementation
 *  should go in these comments.
 */

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"
#include "history.h"

#define MAX_CMD_LEN 10

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


void hist_destroy(void)
{   
    if (list != NULL) {
        free(list->cmd_storage);
        free(list);
    }
}

void hist_add(void *cmd)
{
    // LOG("hist add: %s",* (char **) cmd);
    if (list == NULL) {
        perror("histroy list does not exits");
    }
    size_t idx = list->insertions % list->capacity;
    void *ptr = (void *)((char *)list->cmd_storage + idx * (sizeof(char *)));
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

void test_hist(void) {

    // test 
    hist_init(5);
    char * str = "ls";
    char * str2 = "pwd";
    hist_add(&str);
    hist_add(&str2);


    hist_print();
    printf("last: %x\n", hist_last_cnum());
    // LOGP("I am here\n");
    char *pre = "!ls";
    printf("is alpha: %d\n", isalpha('0'));
    printf("last ls: %s\n", hist_search_prefix(pre + 1));
    printf("cmd 9: %s\n", hist_search_cnum(9));
}

// void main(void) {
//     test_hist();
// }

