/********************************************************************
	error.c - implementation of error-handling routines
  Niksa Orlic, 2004-04-19
********************************************************************/
#pragma warning (disable:4996)
#include <stdio.h>


extern char msgstr[1024];
extern char *source_file_name;
extern int linenum;
char locbuf[1024];
short int error_count = 0;
short int warning_count = 0;
long int last_error_linenum = -1;
int new_linenum = 0;

char *msg(int i){
	switch (i)
	{
	// General messages
	case 1: return "Out of memory";
	case 2: return "Too many errors";
	case 3: return "Cannot create record file";
	case 4: return "Cannot create class file";
	case 5: return "Cannot open source file";
	case 6: return "Serious error occured, stopping compilation";
	case 7: return "Cannot create binary symbol file";
	case 10: return "Internal error #010";
	case 11: return "Internal error #011";
	case 12: return "Internal error #012";
	case 13: return "Internal error #013";
	case 14: return "Internal error #014";
	case 15: return "Internal error #015";
	case 16: return "Internal error #016";
	case 17: return "Internal error #017";
	case 18: return "Internal error #018";
	case 19: return "Internal error #019";
	case 20: return "Internal error #020";
	case 21: return "Internal error #021";
	case 22: return "Internal error #022";
	case 23: return "Internal error #023";
	case 24: return "Internal error #024";
	case 25: return "Internal error #025";
	case 26: return "Internal error #026 memory";
	// Scanner errors
	case 100: return "unknown character \'%s\', ignored";
	case 101: return "newline in string constant";
	case 102: return "error in preprocessor statement";
	// Parser errors
	case 200: return "character \'%s\' expected, \'%s\' found";
	case 201: return "unexpected text \'%s\' after program end";
	case 202: return "identifier (name) \'%s\' expected";
	case 203: return "keyword \'%s\' expected, \'%s\' found";
	case 204: return "unexpected token \'%s\'";
	case 205: return "operator \'=\' expected, \'%s\' found";
	case 206: return "constant expected";
	case 207: return "unexpected end of file found";
	case 208: return "error in parameter list on token \'%s\'";
	case 209: return "operator \':=\' expected, \'%s\' found";
	case 210: return "\'packed\' arrays not supported; ordinary array will be used";
	case 211: return "sets are not supported in this version";
	case 212: return "enumerated types are not supported in this version";
	case 213: return "operator \'..\' expected, \'%s\' found";
	case 214: return "keyword \'end\' expected, \'%s\' found";
	case 215: return "\'with\' is not supported in this version";
	case 216: return "keyword \'then\' expected, \'%s\' found";
	case 217: return "constant of type integer or char expected";
	case 218: return "operator \':=\' expected";
	case 219: return "\'break\' statemnt found outside of do-while, repeat-until or for loop";
	case 220: return "compiler does ot support the \'finalization\' part of the unit";
	case 221: return "procedures and functions may not contain body inside unit interface part";
	// Semantic errors
	case 400: return "identifier \'%s\' already defined";
	case 401: return "method was already declared as forward";
	case 402: return "a %s with the same name was declared as forward";
	case 403: return "parameter list is shorter than the list in previous forward declaration";
	case 404: return "parameter list differs on element %s from the list in previous forward declaration";
	case 405: return "return type is different than the return type declared in the previous forward declaration";
	case 406: return "unknown type \'%s\'";
	case 407: return "%s type expected";
	case 408: return "integer, char or string type expected";
	case 409: return "constant of type %s expected, %s found";
	case 410: return "\'%s\' is not a constant";
	case 411: return "integer or char type expected";
	case 412: return "%s type expected, %s type found";
	case 413: return "wrong type in interval definition";
	case 414: return "\'%s\' is not a constant";
	case 415: return "type name or integer/char constant expected, \'%s\' found";
	case 416: return "error in interval definition";
	case 417: return "operand is wrong type";
	case 418: return "name \'%s\' already exists in a record declaration";
	case 419: return "procedure/function takes 0 arguments, brackets must be left out";
	case 420: return "procedure/function takes more than 0 arguments, '(' expected";
	case 421: return "parameter list is too short";
	case 422: return "error on parameter %s";
	case 423: return "left and right operands to \':=\' must have the same type";
	case 424: return "\'.\' operator can only be used on record types";
	case 425: return "the record does not contain element named \'%s\'";
	case 426: return "\'[\' operator can only be used on array types";
	case 427: return "error in array dimensions (array has %s dimensions, %s are specified)";
	case 428: return "identifier \'%s\' is not procedure, function, variable or unit name";
	case 429: return "identifier \'%s\' is not constant, function or variable name";
	case 430: return "only interval types can be used for array indexes";
	case 431: return "nested functions or procedures are not supported";
	case 432: return "function \'%s\' cannot be called from here; procedure call or assignement expected";
	case 433: return "\'run\' is reserved name and cannot be used as a function or procedure name";
	case 434: return "only string, character, integer or boolean can be appended to string";
	case 435: return "files are not supported in this version";
	case 436: return "variable parameters are not supported and will be ignored";
	case 437: return "invalid interval";
	case 438: return "interval too large";
	case 439: return "only integer type can be used as string index";
	case 440: return "variables of interval type are not supported";
	case 441: return "only forward declaration supplied; the implementation is missing";
	case 442: return "case statement is not supported in this version";
	case 443: return "a value cannot be asigned to array. This will be fixed in future versions of MIDletPascal";
	case 444: return "unknown identifier \'%s\'";
	case 445: return "name of the procedure/function has already been taken by another identifier";
	case 446: return "arrays in records (\'%s\') are not yet implemented";
	case 447: return "unknown record element \'%s\'";
	case 448: return "failed to load \'%s\' library/unit";
	case 449: return "error reading external library file";
	case 450: return "could not load library/unit \'%s\' because identifier of the same name already exists";
	case 451: return "serious internal error in library";
	case 452: return "library/unit contains a method with an incorrect argument type";
	case 453: return "library/unit does not contain function \'%s\'";
	case 454: return "unit name must contain at least 2 characters";
	case 455: return "library/unit does not contain type \'%s\'";
	case 456: return "iterator used in for-loop must not be defined inside a library/unit";
	case 457: return "identifier \'%s\' is not a procedure or variable name";
	case 458: return "identifier \'%s\' is not a function, variable or constant name";
	case 459: return "identifier \'%s\' found in more units; use syntax 'unit_name'.'identifier' to resolve ambiguity";
	}
	return "???\'%s\'???\'%s\'???";
}

/*
	Terminate the program and output the error message
*/
void die(int i)
{
	sprintf(msgstr,"Fatal error (line:%d): %s\n", linenum, msg(i));
	compileMSG();
	compile_terminate();
}

/*
	Add an error message, if the total number of errors exceeds 100,
	die
*/
void add_error_message(int num, char *additional_data1, char *additional_data2)
{
	if (new_linenum==0) new_linenum=linenum;
	if (new_linenum != last_error_linenum)
	{
		sprintf(locbuf, "%s:%d: error, %s\n", source_file_name, new_linenum, msg(num));
		sprintf(msgstr, locbuf, additional_data1, additional_data2);
		compileMSG();
		error_count ++;
		if (error_count > 100) die(2);
		last_error_linenum = new_linenum;
	}
	new_linenum=0;
}


/*
	Add warning message, if the total number of errors exceeds 100,
	die
*/
void add_warning_message(int num, char *additional_data1, char *additional_data2)
{
	if (new_linenum==0) new_linenum=linenum;
	sprintf(locbuf, "%s:%d: warning, %s\n", source_file_name, new_linenum, msg(num));
	sprintf(msgstr, locbuf, additional_data1, additional_data2);
	compileMSG();
	warning_count ++;
	if (warning_count > 100) die(2);
	new_linenum=0;
}