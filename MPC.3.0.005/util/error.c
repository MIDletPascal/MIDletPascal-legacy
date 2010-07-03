/********************************************************************
	
	error.c - implementation of error-handling routines

  Niksa Orlic, 2004-04-19

********************************************************************/

#include "error.h"
#include "message.h"
//#include "../main/static_entry.h"
#include "memory.h"

#include <stdio.h>
#include <stdlib.h>

short int error_count = 0;
short int warning_count = 0;

long int last_error_linenum = -1;

extern char *source_file_name;

extern FILE *yyin;


/*
	Terminate the program and output the error message
*/
void die(char *message)
{
	compile_write3("Fatal error: %s\n", message, "");

	mem_close();

	fclose(yyin);

	compile_terminate();
}


/*
	Add an error message, if the total number of errors exceeds 100,
	die
*/
void add_error_message(char *message, long int linenum, 
					   char *additional_data1,
					   char *additional_data2)
{
	char temp_str[256];

	if (linenum == last_error_linenum)
		return;

	last_error_linenum = linenum;
	
	sprintf(temp_str, "%s:%d: error, %s\n", source_file_name, linenum, message);
	
	compile_write3(temp_str, additional_data1, additional_data2);

	error_count ++;

	if (error_count > 100)
		die(MSG_002);
}


/*
	Add warning message, if the total number of errors exceeds 100,
	die
*/
void add_warning_message(char *message, long int linenum, 
					   char *additional_data1,
					   char *additional_data2)
{
	char temp_str[256];
	
	sprintf(temp_str, "%s:%d: warning, %s\n", source_file_name, linenum, message);
	
	compile_write3(temp_str, additional_data1, additional_data2);

	warning_count ++;

	if (warning_count > 100)
		die(MSG_002);
}