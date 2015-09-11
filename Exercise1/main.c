/*
* Exercise 1
* Justin Anderson
* 14136222
* 11 Sep 15
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#include <readline/readline.h>

#include "command.h"
#include "matrix.h"

void run_commands (Commands_t* cmd, Matrix_t** mats, unsigned int num_mats);
unsigned int find_matrix_given_name (Matrix_t** mats, unsigned int num_mats, 
			const char* target);
void destroy_remaining_heap_allocations (Matrix_t **mats, unsigned int num_mats);

/*
* Purpose: The main function is meant to create a block of memory reserved for the
*          matrix. It also tests a temporary matrix to make sure it does not fail
* 		   before the user creates a matrix. And finally, main check the validity
* 		   of the user command. 		   
* Input: This function takes in a string entered by the user. This input is supposed
* 		 to be a valid command to perform an operation on the matrix. 
* Return: This function will exit the program if the temporary matrix fails to
* 		  initialize. Upon a valid input by the user, main will send the command along
* 		  with its respective arguments to the run_commands function
*/
int main (int argc, char **argv) {
	srand(time(NULL));		
	char *line = NULL;
	Commands_t* cmd;
	unsigned int pos = 0;

	Matrix_t *mats[10];
	memset(&mats,0, sizeof(Matrix_t*) * 10); // IMPORTANT C FUNCTION TO LEARN

	Matrix_t *temp = NULL;
	if (create_matrix (&temp,"temp_mat", 5, 5) == false) {
		perror("Could not create a temporary matrix");
		return -2;
	}

	pos = add_matrix_to_array(mats,temp, 10);
	if (!(mats[pos])) {
		perror("Could not add temporary matrix to array.");
		return -3;
	}
	
	int mat_idx = find_matrix_given_name(mats,10,"temp_mat");

	if (mat_idx < 0) {
		perror("PROGRAM FAILED TO INIT\n");
		return -1;
	}
	random_matrix(mats[mat_idx], 10, 15);
	if (write_matrix("temp_mat", mats[mat_idx]) == false) {
		perror("Failed to write matrix.");
		return -4;
	} 

	line = readline("> ");
	while (strncmp(line,"exit", strlen("exit")  + 1) != 0) {
		
		if (!parse_user_input(line,&cmd)) {
			printf("Failed at parsing command\n\n");
		}
		
		if (cmd->num_cmds > 1) {	
			run_commands(cmd,mats,10);
		}
		if (line) {
			free(line);
		}
		destroy_commands(&cmd);
		line = readline("> ");
	}
	free(line);
	destroy_remaining_heap_allocations(mats,10);
	return 0;	
}

/*
* Purpose: This function processes a valid command from the user and manipulates
* 		   the matrix accordingly by calling the function associated with the command
* 		   of the user. It also checks to make sure the matrix exists along with
*          other error checking of incoming parameters.
* Input: The pointer of the command, the matrix pointer, and the max number of
* 		 matrices (10), are taken in by this function
* Return: run_commands does not return anything but just returns to the main program
* 		  when the matrix operations are complete
*/
void run_commands (Commands_t* cmd, Matrix_t** mats, unsigned int num_mats) {

	if(!cmd || !(*mats) || (num_mats<0)) {
		return;
	}
	
	unsigned int pos = 0;

	/*Parsing and calling of commands*/
	if (strncmp(cmd->cmds[0],"display",strlen("display") + 1) == 0
		&& cmd->num_cmds == 2) {
			/*find the requested matrix*/
			int idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
			if (idx >= 0) {
				display_matrix (mats[idx]);
			}
			else {
				printf("Matrix (%s) doesn't exist\n", cmd->cmds[1]);
				return;
			}
	}
	else if (strncmp(cmd->cmds[0],"add",strlen("add") + 1) == 0
		&& cmd->num_cmds == 4) {
			int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
			int mat2_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[2]);
			if (mat1_idx >= 0 && mat2_idx >= 0) {
				Matrix_t* c = NULL;
				if( !create_matrix (&c,cmd->cmds[3], mats[mat1_idx]->rows, 
						mats[mat1_idx]->cols)) {
					printf("Failure to create the result Matrix (%s)\n", cmd->cmds[3]);
					return;
				}
			
				pos = add_matrix_to_array(mats,c, num_mats);
				if (!(mats[pos])) {
					perror("Could not add matrix to array.");
					return;
				}

				if (! add_matrices(mats[mat1_idx], mats[mat2_idx],c) ) {
					printf("Failure to add %s with %s into %s\n", mats[mat1_idx]->name, mats[mat2_idx]->name,c->name);
					return;	
				}
			}
	}
	else if (strncmp(cmd->cmds[0],"duplicate",strlen("duplicate") + 1) == 0
		&& cmd->num_cmds == 3 && strlen(cmd->cmds[1]) + 1 <= MATRIX_NAME_LEN) {
		int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
		if (mat1_idx >= 0 ) {
				Matrix_t* dup_mat = NULL;
				if( !create_matrix (&dup_mat,cmd->cmds[2], mats[mat1_idx]->rows, 
						mats[mat1_idx]->cols)) {
					return;
				}
				if (duplicate_matrix (mats[mat1_idx], dup_mat) == false) { 
						perror("Could not duplicate matrix.");
						return;
				}
				pos = add_matrix_to_array(mats,dup_mat,num_mats);
				if (!(mats[pos])) {
					perror("Could not add matrix to array.");
					return;
				}
				printf ("Duplication of %s into %s finished\n", mats[mat1_idx]->name, cmd->cmds[2]);
		}
		else {
			printf("Duplication Failed\n");
			return;
		}
	}
	else if (strncmp(cmd->cmds[0],"equal",strlen("equal") + 1) == 0
		&& cmd->num_cmds == 3) {
			int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
			int mat2_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[2]);
			if (mat1_idx >= 0 && mat2_idx >= 0) {
				if ( equal_matrices(mats[mat1_idx],mats[mat2_idx]) ) {
					printf("SAME DATA IN BOTH\n");
				}
				else {
					printf("DIFFERENT DATA IN BOTH\n");
				}
			}
			else {
				printf("Equal Failed\n");
				return;
			}
	}
	else if (strncmp(cmd->cmds[0],"shift",strlen("shift") + 1) == 0
		&& cmd->num_cmds == 4) {
		int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
		const int shift_value = atoi(cmd->cmds[3]);
		if (mat1_idx >= 0 ) {
			if (bitwise_shift_matrix(mats[mat1_idx],cmd->cmds[2][0], shift_value) == false) {
					perror("Failed to perform bitwise operation shift");
					return;
			}
			printf("Matrix (%s) has been shifted by %d\n", mats[mat1_idx]->name, shift_value);
		}
		else {
			printf("Matrix shift failed\n");
			return;
		}

	}
	else if (strncmp(cmd->cmds[0],"read",strlen("read") + 1) == 0
		&& cmd->num_cmds == 2) {
		Matrix_t* new_matrix = NULL;
		if(! read_matrix(cmd->cmds[1],&new_matrix)) {
			printf("Read Failed\n");
			return;
		}	
		
		pos = add_matrix_to_array(mats,new_matrix, num_mats);
		if (!(mats[pos])) {
			perror("Could not add matrix to array.");
			return;
		}
		printf("Matrix (%s) is read from the filesystem\n", cmd->cmds[1]);	
	}
	else if (strncmp(cmd->cmds[0],"write",strlen("write") + 1) == 0
		&& cmd->num_cmds == 2) {
		int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
		if(! write_matrix(mats[mat1_idx]->name,mats[mat1_idx])) {
			printf("Write Failed\n");
			return;
		}
		else {
			printf("Matrix (%s) is wrote out to the filesystem\n", mats[mat1_idx]->name);
		}
	}
	else if (strncmp(cmd->cmds[0], "create", strlen("create") + 1) == 0
		&& strlen(cmd->cmds[1]) + 1 <= MATRIX_NAME_LEN && cmd->num_cmds == 4) {
		Matrix_t* new_mat = NULL;
		const unsigned int rows = atoi(cmd->cmds[2]);
		const unsigned int cols = atoi(cmd->cmds[3]);

		if (create_matrix(&new_mat,cmd->cmds[1],rows, cols) == false) {
			perror("Failed to create a matrix.");
			return;
		}
		pos = add_matrix_to_array(mats,new_mat,num_mats);
		if (!(mats[pos])) {
			perror("Could not add matrix to array.");
			return;
		}
		printf("Created Matrix (%s,%u,%u)\n", new_mat->name, new_mat->rows, new_mat->cols);
	}
	else if (strncmp(cmd->cmds[0], "random", strlen("random") + 1) == 0
		&& cmd->num_cmds == 4) {
		int mat1_idx = find_matrix_given_name(mats,num_mats,cmd->cmds[1]);
		const unsigned int start_range = atoi(cmd->cmds[2]);
		const unsigned int end_range = atoi(cmd->cmds[3]);
		if (random_matrix(mats[mat1_idx],start_range, end_range) == false) {
			perror("Failed to randomize matrix");
			return;
		}

		printf("Matrix (%s) is randomized between %u %u\n", mats[mat1_idx]->name, start_range, end_range);
	}
	else {
		printf("Not a command in this application\n");
	}

}

/*
* Purpose: This function searches through the list of matrices in order to find
* 		   a name that matches (or doesn't match) the matrix the user is
* 		   wanting to use.
* Input: The pointer to the matrices, the number of matrices, and the potential
* 		 name of the matrix are sent to this function
* Return: This function returns the index of the matrix if one is found, otherwise
* 		  it returns -1 if the matrix is not found
*/
unsigned int find_matrix_given_name (Matrix_t** mats, unsigned int num_mats, const char* target) {

	if(!target || !(*mats) || (num_mats<0)) {
		return -1;
	}

	for (int i = 0; i < num_mats; ++i) {
		if (strncmp(mats[i]->name,target,strlen(mats[i]->name)) == 0) {
			return i;
		}
	}
	return -1;
}

/*
* Purpose: This function destroys the remaining heap allocations
* Input: The pointer to the matrices and the number of matrices are sent to this
* 		 function
* Return: This function returns to main right before the program ends once
* 		  deallocation is complete
*/
void destroy_remaining_heap_allocations(Matrix_t **mats, unsigned int num_mats) {

	if(!(*mats) || (num_mats<0)) {
		return;
	}

	// Deallocate heap while checking for NULL pointer
	for (int i = 0; i < num_mats; ++i) {
		//check for NULL pointer
		if (mats[i] != NULL) {
			destroy_matrix(&mats[i]);
		}
		else {
				break;
		}
	}
	free(*mats);
}
