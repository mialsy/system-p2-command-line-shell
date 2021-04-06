/**
 * @file
 *
 * Text-based UI functionality. These functions are primarily concerned with
 * interacting with the readline library.
 */

#ifndef _UI_H_
#define _UI_H_

/**
 * Set the status of the previous execution
 * @param num The status code
*/
void set_status(int num);

/**
 * Initiate the ui
*/
void init_ui(void);

/**
 * Clean up the ui
*/
void destroy_ui(void);


/**
 * Construct the prompt line to desplay in ui 
 * @return pointer to the constructed prompt line
*/
char *prompt_line(void);

/**
 * Get the username to display at prompt line
 * @return pointer to the username string
*/
char *prompt_username(void);

/**
 * Get the hostname to display at prompt line
 * @return pointer to the hostname string
*/
char *prompt_hostname(void);

/**
 * Get the current working directory to display at prompt line
 * @return pointer to the current working directory string
*/
char *prompt_cwd(void);

/**
 * Get the status of the previous execution result
 * @return the status code
*/
int prompt_status(void);

/**
 * Get the command number of the current prompt
 * @return the command number
*/
unsigned int prompt_cmd_num(void);


/**
 * Read command from user Input
 * @return a pointer to the read in result
*/
char *read_command(void);

/**
 * Handle user press key up, history roll back by one 
 * @return 0 on success
*/
int key_up(int count, int key);

/**
 * Handle user press key down, history roll forward by one 
 * @return 0 on success
*/
int key_down(int count, int key);

#endif
