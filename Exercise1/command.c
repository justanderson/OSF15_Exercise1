/*
* Exercise 1
* Justin Anderson
* 14136222
* 11 Sep 15
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "command.h"

#define MAX_CMD_COUNT 50
#define MAX_CMD_LEN 25

/*
* Purpose: This function is designed to allocate memory for a command entered by
* 		   the user. It parses the user command and makes sure it is under the
* 		   max number of commands which is 50 and less than 25 chars in length.
* 		   If the command is valid, it is entered into the cmd struct
* Input: parse_user_input takes in the user command as a string as well as the
* 		 cmd struct pointer
* Return: It returns true or false depending on if the command was successfully
* 		  allocated or not. On success, the command is stored in the cmd struct
*/
bool parse_user_input (const char* input, Commands_t** cmd) {

	if(!input) {
		return false;
	}

	char *string = strdup(input);
	
	*cmd = calloc (1,sizeof(Commands_t));
	(*cmd)->cmds = calloc(MAX_CMD_COUNT,sizeof(char*));

	unsigned int i = 0;
	char *token;
	token = strtok(string, " \n");
	for (; token != NULL && i < MAX_CMD_COUNT; ++i) {
		(*cmd)->cmds[i] = calloc(MAX_CMD_LEN,sizeof(char));
		if (!(*cmd)->cmds[i]) {
			perror("Allocation Error\n");
			return false;
		}	
		strncpy((*cmd)->cmds[i],token, strlen(token) + 1);
		(*cmd)->num_cmds++;
		token = strtok(NULL, " \n");
	}
	free(string);
	return true;
}

/*
* Purpose: This function frees the memory that was allocated for the user commands
* Input: destroy_commands takes in the cmd struct pointer
* Return: The result of this function is freed memory, nothing is returned.
*/
void destroy_commands(Commands_t** cmd) {

	if(!(*cmd)) {
		return;
	}	

	for (int i = 0; i < (*cmd)->num_cmds; ++i) {
		free((*cmd)->cmds[i]);
	}
	free((*cmd)->cmds);
	free((*cmd));
	*cmd = NULL;
}

