/********************************************************************
	strings.h - declaration of a string-handling routines
  Niksa Orlic, 2004-04-19
********************************************************************/
struct string_struct
{
	char *cstr;
	short int length;
	short int allocated;
};

typedef struct string_struct string;

/*
	Create an empty string
*/
string* string_create();

/*
	Create a copy of the string
*/
string* string_duplicate(string*);

/*
	Create a string from a C-style string
*/
string* string_from_cstr(char *cstr);

/*
	Free all memory used by the string
*/
void string_destroy(string*);

/*
	Empties the given string	
*/
void string_empty(string*);

/*
	Append the second string to the first string
*/
void string_append(string*, string*);

/*
	Append the C-style string to the string
*/
void string_append_cstr(string*, char*);

/*
	Get the C-style string from a given string
*/
char* string_get_cstr(string*);

/*
	Get the string length
*/
short int string_length(string*);

/*
	Translates a string into lower case
*/
void lowercase(char *);
