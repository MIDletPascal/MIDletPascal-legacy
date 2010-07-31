/********************************************************************
	error.h - declaration of error-handling routines
  Niksa Orlic, 2004-04-19
********************************************************************/

/*
	Terminate the program and output the error message
*/
void die(int i/*char *message*/);

/*
	Adds an error message
*/
char *msg(int i);
void add_error_message(/*char *message, long int linenum,*/ int num, char *additional_data1, char *additional_data2);
void add_warning_message(/*char *message, long int linenum,*/ int num, char *additional_data1, char *additional_data2);
