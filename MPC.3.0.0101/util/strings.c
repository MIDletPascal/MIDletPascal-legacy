/********************************************************************
	strings.c - implementation of a string-handling routines
  Niksa Orlic, 2004-04-19
********************************************************************/
#include "strings.h"
#include <string.h>

/*
	Create an empty string
*/
string* string_create()
{
	string *new_string;
	new_string = (string*) mem_alloc(sizeof(string));
	if (new_string == NULL) die(1);
	new_string->cstr = (char*) mem_alloc(sizeof(char) * 16);
	if (new_string->cstr == NULL) die(1);
	new_string->cstr[0] = '\0';
	new_string->length = 0;
	new_string->allocated = 16;
	return new_string;
}

/*
	Create a copy of the string
*/
string* string_duplicate(string *str)
{
	string *new_string;
	new_string = (string*) mem_alloc(sizeof(string));
	if (new_string == NULL) die(1);
	new_string->cstr = (char*) mem_alloc(sizeof(char) * (str->allocated));
	if (new_string->cstr == NULL) die(1);
	memcpy(new_string->cstr, str->cstr, str->length + 1); /* also copy the terminating NULL character */
	new_string->allocated = str->allocated;
	new_string->length = str->length;
	return new_string;
}

/*
	Create a string from a C-style string
*/
string* string_from_cstr(char *cstr)
{
	string *new_string;
	new_string = (string*) mem_alloc(sizeof(string));
	if (new_string == NULL) die(1);
	new_string->cstr = (char*) mem_alloc((strlen(cstr)+5) * sizeof(char));
	if (new_string->cstr == NULL) die(1);
	strcpy(new_string->cstr, cstr);
	new_string->allocated = strlen(cstr) + 5;
	new_string->length = strlen(cstr);
	return new_string;
}

/*
	Free all memory used by the string
*/
void string_destroy(string *str)
{
	mem_free(str->cstr);
	mem_free(str);
}


/*
	Empties the given string	
*/
void string_empty(string *str)
{
	str->length = 0;
	str->cstr[0] = '\0';
}


/*
	Append the second string to the first string
*/
void string_append(string *str1, string *str2)
{
	if (str1->length + str2->length + 1 >= str1->allocated)
	{
		str1->cstr = (char*) mem_realloc(str1->cstr, sizeof(char)*(str1->length + str2->length + 1));
		if (str1->cstr == NULL) die(1);
		str1->allocated = str1->length + str2->length + 1;
	}
	memcpy(str1->cstr + str1->length, str2->cstr, str2->length);
	str1->length = str1->length + str2->length;
	str1->cstr[str1->length] = '\0';
}

/*
	Append the C-style string to the string
*/
void string_append_cstr(string *str1, char *cstr2)
{
	short int cstr2_length;
	if (str1 == NULL) str1 = string_create();
	cstr2_length = strlen(cstr2);
	if (str1->length + cstr2_length + 1 >= str1->allocated)
	{
		str1->cstr = (char*) mem_realloc(str1->cstr, sizeof(char)*(str1->length + cstr2_length + 1));
		if (str1->cstr == NULL) die(1);
		str1->allocated = str1->length + cstr2_length + 1;
	}
	memcpy(str1->cstr + str1->length, cstr2, cstr2_length);
	str1->length = str1->length + cstr2_length;
	str1->cstr[str1->length] = '\0';
}

/*
	Get the C-style string from a given string
*/
char *string_get_cstr(string *str)
{
	return str->cstr;
}


/*
	Get the string length
*/
short int string_length(string *str)
{
	return str->length;
}

/*
	Translate the string into lowercase
*/
void lowercase(char *cstr)
{
	int i = 0;
	while (cstr[i] != 0)
	{
		if ((cstr[i] >= 'A') && (cstr[i] <= 'Z')) cstr[i] = cstr[i] | (char) 0x20;
		i++;
	}
}