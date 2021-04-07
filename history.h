/**
 * @file history.h
 *
 * @brief Header file for history.c. Contains shell history data structures and retrieval functions.
 */

#ifndef _HISTORY_H_
#define _HISTORY_H_

/**
 * @brief Initialize the history list
 * 
 * @param limit the capacity of the history list
 */
void hist_init(unsigned int limit);

/**
 * @brief clean up history list
 * 
 */
void hist_destroy(void);

/**
 * @brief add to the history list
 * 
 * @param cmd the command to add
 */
void hist_add(void *cmd);

/**
 * @brief print history list
 * 
 */
void hist_print(void);

/**
 * @brief search prefix in the history list
 * 
 * @param prefix the prefix to search
 * @return const char* pointer to the hisrtory item with input prefix
 */
const char *hist_search_prefix(char *prefix);

/**
 * @brief search history list by history number
 * 
 * @param command_num 
 * @return const char* pointer to the history item with the input number
 */
const char *hist_search_cnum(int command_num);

/**
 * @brief retrive the number of the last added history item
 * 
 * @return unsigned int the number of last added history item
 */
unsigned int hist_last_cnum(void);


/**
 * @brief check if given index is valid 
 * (note index is not the history number)
 * 
 * @param idx the index to check
 * @return int zero on valid index
 */
int hist_idx_isValid(int idx);

#endif
