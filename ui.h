/**
 * @file
 *
 * Text-based UI functionality. These functions are primarily concerned with
 * interacting with the readline library.
 */

#ifndef _UI_H_
#define _UI_H_

/**
 * Get the pointer of the elist
 * @param list The list of operation
*/
void set_status(int num);

/**
 * Get the pointer of the elist
 * @param list The list of operation
*/
void init_ui(void);

/**
 * Get the pointer of the elist
 * @param list The list of operation
*/
void destroy_ui(void);


/**
 * Get the pointer of the elist
 * @param list The list of operation
*/
char *prompt_line(void);

/**
 * Get the pointer of the elist
 * @param list The list of operation
*/
char *prompt_username(void);

/**
 * Get the pointer of the elist
 * @param list The list of operation
*/
char *prompt_hostname(void);

/**
 * Get the pointer of the elist
 * @param list The list of operation
*/
char *prompt_cwd(void);

/**
 * Get the pointer of the elist
 * @param list The list of operation
*/
int prompt_status(void);

/**
 * Get the pointer of the elist
 * @param list The list of operation
*/
unsigned int prompt_cmd_num(void);


/**
 * Get the pointer of the elist
 * @param list The list of operation
*/
char *read_command(void);

/**
 * Get the pointer of the elist
 * @param list The list of operation
*/
int key_up(int count, int key);

/**
 * Get the pointer of the elist
 * @param list The list of operation
*/
int key_down(int count, int key);

#endif
