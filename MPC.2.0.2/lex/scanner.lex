/******************************************************************************
* Lexical rules for Mobile Pascal.                                            *                 
*                                                                             *
* Author: Niksa Orlic (norlic@fly.srk.fer.hr)                                 *
* Date: 15.04.2004.                                                           *
* Editor: gvim                                                                *
*                                                                             *
* :set ts=5                                                                   *
******************************************************************************/


%{

#include "../util/strings.h"
#include "../util/message.h"
#include "../util/error.h"
#include "tokens.h"

#include <stdlib.h>
#include <string.h>

#define MAX_PREPROCESSOR_STACK_SIZE 256

#define PREPROCESS_RETURN(x) 	if (preprocessor_stack_size == 0) \
							return (x); \
						else \
						{ \
						  int i; \
						  int bReturn = 1; \
						  for(i = 0; i < preprocessor_stack_size; i++) \
						  { \
						    if ((preprocessor_stack[i] == _false) \
						       || (preprocessor_stack[i] == _else_false)) \
							  { \
							    bReturn = 0; \
							    break; \
							  } \
						  } \
                                \
						  if (bReturn) \
						    return (x); \
						}
							
/* Stores the current line number */
long int linenum = 1;

long int integer_constant;
float real_constant;
char boolean_constant;
char char_constant;
string *string_constant = NULL;
int warning_101_count = 0;

/* preprocessor variables and enumerations */
enum preprocessor_token_type {_none, _ifdef, _ifndef, _else, _endif};
enum preprocessor_token_type preprocessor_token = _none;

enum preprocessor_stack_element {_true, _false, _else_true, _else_false};
enum preprocessor_stack_element preprocessor_stack[MAX_PREPROCESSOR_STACK_SIZE];
int preprocessor_stack_size = 0;

string *preprocessor_define_name = NULL;

string* build_configuration_define = NULL; // externally defined with the name of the current build configuration; must be lowercase!!!

%}


 /* Start conditions used by lexical scanner (for more info see flex manual) */
%s INITAL
%s STRING
%s MULTI_LINE_COMMENT_CURLY
%s MULTI_LINE_COMMENT_STAR
%s ONE_LINE_COMMENT

%s PREPROCESSOR
%s PREPROCESSOR_TOKEN
%option case-insensitive


%%
 /**************************** RULES SECTION *********************************/

 /**************************** Constants *************************************/
<INITIAL>[0-9]+						{
										integer_constant = atol(yytext);
										PREPROCESS_RETURN (CST_INTEGER);
									}

<INITIAL>[0-9]+("."[0-9]+)?(E[+-]?[0-9]+)?	{
										real_constant = (float) atof(yytext);
										PREPROCESS_RETURN (CST_REAL);
									}

<INITIAL>"$"[0-9A-F]+					{
										char *stop_string;
										integer_constant = strtol(yytext+1, &stop_string, 16);
										PREPROCESS_RETURN (CST_INTEGER);
									}

<INITIAL>false							{
										boolean_constant = 0;
										PREPROCESS_RETURN (CST_BOOLEAN);
									}

<INITIAL>true						{
										boolean_constant = 1;
										PREPROCESS_RETURN (CST_BOOLEAN);
									}
									
<INITIAL>\'							{
										/* initialize the string */
										BEGIN(STRING);
										if (string_constant == NULL)
											string_constant = string_create();

										string_empty(string_constant);
									}

<STRING>[^\n\r\']+						{
										/* add characters to the string */
										string_append_cstr(string_constant, yytext);
									}

<STRING>[\r]						{
										/* ignore */
									}

<STRING>[\n]						{
										if (warning_101_count == 0)
										{
											add_warning_message(MSG_101, linenum, "", "");
											warning_101_count ++;
										}

										/* add the newline character to the string */
										string_append_cstr(string_constant, "\n");
										linenum ++;
									}

<STRING>\'							{
										BEGIN(INITIAL);
										
										/* check if string length is 1, then return CST_CHAR */
										if ((string_constant != NULL)
										 && (string_length(string_constant) == 1))
										{
											char_constant = *(string_get_cstr(string_constant));
											PREPROCESS_RETURN (CST_CHAR);
										}
										
										PREPROCESS_RETURN (CST_STRING);
									}

<STRING>\'\'							{
										/* add the ' into the string */
										string_append_cstr(string_constant, "\'");
									}


 /**************************** Keywords & operators **************************/
<INITIAL>mod							PREPROCESS_RETURN (OP_MOD);
<INITIAL>div							PREPROCESS_RETURN (OP_DIV);
<INITIAL>"+"							PREPROCESS_RETURN (OP_PLUS);
<INITIAL>"-"							PREPROCESS_RETURN (OP_MINUS);
<INITIAL>"*"							PREPROCESS_RETURN (OP_MULT);
<INITIAL>"/"							PREPROCESS_RETURN (OP_SLASH);
<INITIAL>=							PREPROCESS_RETURN (OP_EQUAL);
<INITIAL>begin							PREPROCESS_RETURN (KWD_BEGIN);
<INITIAL>end							PREPROCESS_RETURN (KWD_END);
<INITIAL>and							PREPROCESS_RETURN (OP_AND);
<INITIAL>program						PREPROCESS_RETURN (KWD_PROGRAM);
<INITIAL>procedure						PREPROCESS_RETURN (KWD_PROCEDURE);
<INITIAL>function						PREPROCESS_RETURN (KWD_FUNCTION);
<INITIAL>;							PREPROCESS_RETURN (SEMI_COLON);
<INITIAL>:							PREPROCESS_RETURN (COLON);
<INITIAL>:=							PREPROCESS_RETURN (OP_ASSIGN);
<INITIAL>"<"							PREPROCESS_RETURN (OP_LESS);
<INITIAL>">"							PREPROCESS_RETURN (OP_GREATER);
<INITIAL>"<="							PREPROCESS_RETURN (OP_LESS_EQUAL);
<INITIAL>">="							PREPROCESS_RETURN (OP_GREATER_EQUAL);
<INITIAL>"<>"							PREPROCESS_RETURN (OP_NOT_EQUAL);
<INITIAL>"."							PREPROCESS_RETURN (DOT);
<INITIAL>".."							PREPROCESS_RETURN (DOTDOT);
<INITIAL>"["							PREPROCESS_RETURN (OPEN_SQ_BR);
<INITIAL>"]"							PREPROCESS_RETURN (CLOSE_SQ_BR);
<INITIAL>var							PREPROCESS_RETURN (KWD_VAR);
<INITIAL>for							PREPROCESS_RETURN (KWD_FOR);
<INITIAL>to							PREPROCESS_RETURN (KWD_TO);
<INITIAL>downto						PREPROCESS_RETURN (KWD_DOWNTO);
<INITIAL>do							PREPROCESS_RETURN (KWD_DO);
<INITIAL>"("							PREPROCESS_RETURN (OPEN_BR);
<INITIAL>")"							PREPROCESS_RETURN (CLOSE_BR);
<INITIAL>const							PREPROCESS_RETURN (KWD_CONST);
<INITIAL>type							PREPROCESS_RETURN (KWD_TYPE);
<INITIAL>if							PREPROCESS_RETURN (KWD_IF);
<INITIAL>then							PREPROCESS_RETURN (KWD_THEN);
<INITIAL>else							PREPROCESS_RETURN (KWD_ELSE);
<INITIAL>case							PREPROCESS_RETURN (KWD_CASE);
<INITIAL>of							PREPROCESS_RETURN (KWD_OF);
<INITIAL>while							PREPROCESS_RETURN (KWD_WHILE);
<INITIAL>repeat						PREPROCESS_RETURN (KWD_REPEAT);
<INITIAL>until							PREPROCESS_RETURN (KWD_UNTIL);
<INITIAL>with							PREPROCESS_RETURN (KWD_WITH);
<INITIAL>packed						PREPROCESS_RETURN (KWD_PACKED);
<INITIAL>array							PREPROCESS_RETURN (KWD_ARRAY);
<INITIAL>or							PREPROCESS_RETURN (OP_OR);
<INITIAL>file							PREPROCESS_RETURN (KWD_FILE);
<INITIAL>set							PREPROCESS_RETURN (KWD_SET);
<INITIAL>record						PREPROCESS_RETURN (KWD_RECORD);
<INITIAL>in							PREPROCESS_RETURN (OP_IN);
<INITIAL>not							PREPROCESS_RETURN (OP_NOT);
<INITIAL>xor							PREPROCESS_RETURN (OP_XOR);
<INITIAL>forward						PREPROCESS_RETURN (KWD_FORWARD);
<INITIAL>break							PREPROCESS_RETURN (KWD_BREAK);
<INITIAL>","							PREPROCESS_RETURN (COMMA);
<INITIAL>uses							PREPROCESS_RETURN (KWD_USES);
<INITIAL>unit							PREPROCESS_RETURN (KWD_UNIT);
<INITIAL>interface						PREPROCESS_RETURN (KWD_INTERFACE);
<INITIAL>implementation						PREPROCESS_RETURN (KWD_IMPLEMENTATION);
<INITIAL>initialization						PREPROCESS_RETURN (KWD_INITIALIZATION);
<INITIAL>finalization						PREPROCESS_RETURN (KWD_FINALIZATION);

 
 /**************************** Identifiers ************************************/
<INITIAL>[_[:alpha:]][_[:alnum:]]*			PREPROCESS_RETURN (IDENTIFIER);

 /**************************** Comments **************************************/
<INITIAL>"{$"						{
									preprocessor_token = _none;
									BEGIN(PREPROCESSOR);
									if(preprocessor_define_name != NULL)
									{
										preprocessor_define_name = NULL;
									}
								}

<INITIAL>"{"							BEGIN(MULTI_LINE_COMMENT_CURLY);

<MULTI_LINE_COMMENT_CURLY>[^\n\}]+			/* ignore */

<MULTI_LINE_COMMENT_CURLY>[\n]			linenum ++;

<MULTI_LINE_COMMENT_CURLY>"}"				BEGIN(INITIAL);



<INITIAL>"(*"							BEGIN(MULTI_LINE_COMMENT_STAR);

<MULTI_LINE_COMMENT_STAR>[^\n\*]+			/* ignore */

<MULTI_LINE_COMMENT_STAR>[\n]				linenum ++;

<MULTI_LINE_COMMENT_STAR>\*				/* ignore */

<MULTI_LINE_COMMENT_STAR>"*)"				BEGIN(INITIAL);


<INITIAL>"//"							BEGIN(ONE_LINE_COMMENT);

<ONE_LINE_COMMENT>[^\n]+					/* ignore */

<ONE_LINE_COMMENT>[\n]					BEGIN(INITIAL); linenum ++;


 /**************************** Preprocessor ***********************************/
<PREPROCESSOR>ifdef					preprocessor_token = _ifdef; BEGIN(PREPROCESSOR_TOKEN);

<PREPROCESSOR>ifndef				preprocessor_token = _ifndef; BEGIN(PREPROCESSOR_TOKEN);

<PREPROCESSOR>else					preprocessor_token = _else; BEGIN(PREPROCESSOR_TOKEN);

<PREPROCESSOR>endif					preprocessor_token = _endif; BEGIN(PREPROCESSOR_TOKEN);

<PREPROCESSOR_TOKEN>[:blank:]		/* do nothing */

<PREPROCESSOR_TOKEN>[^}[:blank:]]*					{
									if ((preprocessor_token != _ifdef) && (preprocessor_token != _ifndef))
										add_error_message(MSG_102, linenum, "", "");
									else
									{
										if (preprocessor_define_name != NULL)
											add_error_message(MSG_102, linenum, "", "");
										else
											preprocessor_define_name = string_from_cstr(yytext);
										lowercase(string_get_cstr(preprocessor_define_name));
									}
								}

<PREPROCESSOR>"}"					add_error_message(MSG_102, linenum, "", "");	BEGIN(INITIAL);

<PREPROCESSOR>.					/* do nothing */

<PREPROCESSOR_TOKEN>"}"					{
									switch (preprocessor_token)
									{
									case _none: add_error_message(MSG_102, linenum, "", "");
									break;

									case _ifdef:
										if (preprocessor_define_name == NULL)
											add_error_message(MSG_102, linenum, "", "");
										else
										{
											if (preprocessor_stack_size >= MAX_PREPROCESSOR_STACK_SIZE)
												add_error_message(MSG_102, linenum, "", "");
											else
											  if (strcmp(string_get_cstr(preprocessor_define_name), 
													string_get_cstr(build_configuration_define)) == 0)
												preprocessor_stack[preprocessor_stack_size] = _true;
											  else
												preprocessor_stack[preprocessor_stack_size] = _false;
										
											preprocessor_stack_size ++;
										}
									break;

									case _ifndef:
										if (preprocessor_define_name == NULL)
											add_error_message(MSG_102, linenum, "", "");
										else
										{
											if (preprocessor_stack_size >= MAX_PREPROCESSOR_STACK_SIZE)
												add_error_message(MSG_102, linenum, "", "");
											else
											  if (strcmp(string_get_cstr(preprocessor_define_name), 
													string_get_cstr(build_configuration_define)) != 0)
												preprocessor_stack[preprocessor_stack_size] = _true;
											  else
												preprocessor_stack[preprocessor_stack_size] = _false;

											preprocessor_stack_size ++;
										}
									break;
									
									case _else:
										if ((preprocessor_stack_size <= 0) 
										|| (preprocessor_stack[preprocessor_stack_size-1] == _else_true)
										|| (preprocessor_stack[preprocessor_stack_size-1] == _else_false))
										{
											add_error_message(MSG_102, linenum, "", "");
										}
										else
										{
											if(preprocessor_stack[preprocessor_stack_size-1] == _true)
												preprocessor_stack[preprocessor_stack_size-1]  = _else_false;
											else
												preprocessor_stack[preprocessor_stack_size-1]  = _else_true;
										}
									break;

									case _endif:
										if (preprocessor_stack_size <= 0)
											add_error_message(MSG_102, linenum, "", "");
										else
											preprocessor_stack_size --;
									break;
									}
									BEGIN(INITIAL);
								}



 /**************************** Special **************************************/
<INITIAL>[\r[:blank:]]					/* ignore */

<INITIAL>[\n]							linenum ++;

<INITIAL>.							{
									if (yytext[0] != '\0')
										add_warning_message(MSG_100, linenum, yytext, "");
									}

<<EOF>>							return END_OF_INPUT;

%%

/* My implementation of yywrap function. See flex documentation for more info. */
int yywrap()
{
	return 1;
}
