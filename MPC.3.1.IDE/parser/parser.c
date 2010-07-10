/********************************************************************

	parser.c - recursive-descent parser

  Niksa Orlic, 2004-04-19

********************************************************************/


//#include "../util/message.h"
#include "../util/error.h"
#include "../util/strings.h"
#include "../structures/type_list.h"
#include "../structures/string_list.h"
#include "../structures/type.h"
#include "../structures/identifier.h"
#include "../structures/name_table.h"
#include "../classgen/bytecode.h"
#include "../structures/block.h"
#include "../lex/tokens.h"
#include "../util/memory.h"
#include "../structures/unit.h"

#include "../classgen/constant_pool.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../classgen/classgen.h"

#include "parser.h"
#include "stdpas.h"

#pragma warning (disable: 4305)
#pragma warning (disable: 4761)

extern int procedure_linenum;

//extern void (*compile_terminate)();

extern FILE* yyin;


int current_token;
block *root_block;

int next_record_ID;

/* Scanner variables and functions */
extern long int linenum;
extern int new_linenum;
extern long int integer_constant;
extern int error_count;
extern float real_constant;
extern char boolean_constant;
extern char char_constant;
extern string *string_constant;
extern char *yytext;
extern int yylex();
extern FILE *yyin;
extern void backchar(int c);
extern int goverify;

extern void VerifyFile(char*);

extern int usesFloat;

extern char *output_path;

extern int detect_units_only;

/* Small macro for outputting the yytext value */
#define YYTEXT_STRING	(current_token == END_OF_INPUT ? "<EOF>" : yytext)


/* A macro to explicitly declare we didn't forget break inside
   the case statement */
//#define no_break		/* do nothing */;


int inside_loop = 0; /* the counter checks if 'break' is located inside a loop */

int compiling_unit = 0; /* set to 1 if this is an unit */
int inside_interface_part = 0; /* if the compiler is inside the interface part of the unit */
int inside_implementation_part = 0; /* if the compiler is inside the implementation part of the unit */

string *str_program_name;

extern int mathType;

FILE *symbols_file;

/*
	Starts parsing the input file. The grammar rule
	used is:

	<parser_start> -> <RD_program_header> <RD_uses_list> <RD_block> . <<EOF>>
	               -> <RD_program_header> <RD_unit_interface> <RD_unit_implementation> <RD_unit_initialization> <RD_unit_finalization> . <<EOF>>
*/
void parser_start()
{
	block *program_block;
	int random_field_index;
	int random_class_index;
	int random_method_index;

	root_block = initialize_root_block();

	current_token = yylex();

	compiling_unit = 0;

	str_program_name = RD_program_header();

	if (compiling_unit == 0)
	{
		RD_uses_list();

		if (str_program_name == NULL)
			str_program_name = string_from_cstr("");

		program_block = block_create(root_block, str_program_name);

        // j-a-s-d
        if (detect_units_only)
            goto end_parsing;

		/* initialize the random number generator */
		random_field_index = cp_add_fieldref("M", "RNG", "Ljava/util/Random;");
		random_class_index = cp_add_class("java/util/Random");
		random_method_index = cp_add_methodref("java/util/Random", "<init>", "()V");

		bytecode_append(program_block->code, new$);
		bytecode_append_short_int(program_block->code, random_class_index);

		bytecode_append(program_block->code, dup$);

		bytecode_append(program_block->code, invokespecial$);
		bytecode_append_short_int(program_block->code, random_method_index);

		bytecode_append(program_block->code, putstatic$);
		bytecode_append_short_int(program_block->code, random_field_index);
		/* END initialize the random number generator */

		/* initialize the index counter field */
		bytecode_append(program_block->code, bipush$);
		bytecode_append(program_block->code, 25);
		bytecode_append(program_block->code, newarray$);
		bytecode_append(program_block->code, 10);
		bytecode_append(program_block->code, putstatic$);
		bytecode_append_short_int(program_block->code, cp_add_fieldref("M", "IC", "[I"));
		/* END initialize the IC field */

		/* reset the keyboard buffers */
		bytecode_append(program_block->code, iconst_0$);
		bytecode_append(program_block->code, putstatic$);
		bytecode_append_short_int(program_block->code, cp_add_fieldref("M", "KC", "I"));
		bytecode_append(program_block->code, iconst_0$);
		bytecode_append(program_block->code, putstatic$);
		bytecode_append_short_int(program_block->code, cp_add_fieldref("M", "KP", "I"));
		/* END reset the keyboard buffers */

		RD_block(program_block);

		/* do the 'halt' at the end of program execution */
		bytecode_append(program_block->code, getstatic$);
		bytecode_append_short_int(program_block->code, cp_add_fieldref("FW", "fw", "LFW;"));
		bytecode_append(program_block->code, iconst_1$);
		bytecode_append(program_block->code, invokevirtual$);
		bytecode_append_short_int(program_block->code, cp_add_methodref("FW", "destroyApp", "(Z)V"));

		/* bytecode verififer wants this unnecessary return */
		bytecode_append(program_block->code, return$);
	}
	else
	{
		char *symbols_filename;
		int filepos;

		symbols_filename = (char*) mem_alloc(strlen(output_path) + string_length(str_program_name) + 10);
		sprintf(symbols_filename, "%s\\%s.bsf", output_path, string_get_cstr(str_program_name));
		symbols_file = fopen(symbols_filename, "wb");
		mem_free(symbols_filename);

		if (symbols_file == NULL)
			die(7);

		program_block = block_create(root_block, str_program_name);

		lowercase(string_get_cstr(str_program_name));

		RD_unit_interface(program_block);
		RD_unit_implementation(program_block);
		RD_unit_initialization(program_block);
		RD_unit_finalization(program_block);

        // j-a-s-d
        if (detect_units_only) {
            fclose(symbols_file);
            goto end_parsing;
        }

		if (current_token != KWD_END)
			add_error_message(203, "end", YYTEXT_STRING);
		else
		{
			current_token = yylex();
		}

		fclose(symbols_file);

		bytecode_append(program_block->code, return$);
	}

	/* check if there are forward declarations without the body */
	check_unmatched_forward_declarations(program_block, program_block->names);

	if (current_token != DOT)
	{
		add_error_message(200, ".", YYTEXT_STRING);

		/* Error-recovery: find the end of input */
		while (current_token != END_OF_INPUT)
			current_token = yylex();
	}
	else
		current_token = yylex();

	if (current_token != END_OF_INPUT)
	{
		add_error_message(201, YYTEXT_STRING, "");
	}

	if (error_count == 0)
	{
		FILE *fp;
		char *output_file_name;
		char *short_file_name;

		if (compiling_unit == 0)
		{
			output_file_name = (char*) mem_alloc(strlen(output_path) + 10);
			short_file_name = (char*) mem_alloc(10);
		}
		else
		{
			output_file_name = (char*) mem_alloc(strlen(output_path) + string_length(str_program_name) + 10);
			short_file_name = (char*) mem_alloc(string_length(str_program_name) + 10);
		}

		if (output_file_name == NULL)
			die (1);

		if (compiling_unit == 0)
		{
			sprintf(output_file_name, "%s\\M.class", output_path);
			sprintf(short_file_name, "M");
		}
		else
		{
			sprintf(output_file_name, "%s\\%s.class", output_path, string_get_cstr(str_program_name));
			sprintf(short_file_name, "%s", string_get_cstr(str_program_name));
		}

		fp = fopen(output_file_name, "wb");
		mem_free(output_file_name);
		if (fp == NULL)
		{
			die (4);
		}
		create_class_file(program_block, fp);
		fclose(fp);

		if (goverify!=0) VerifyFile(short_file_name);
	}
// j-a-s-d
end_parsing:
	string_destroy(str_program_name);

	block_destroy(program_block);
	block_destroy(root_block);
}


/*
	Program header, uses the grammar rule:

	<RD_program_header> -> [program IDENTIFIER ;]
*/
string* RD_program_header()
{
	string *program_name = NULL;

	if ((current_token == KWD_PROGRAM)
		|| (current_token == KWD_UNIT))
	{
		if (current_token == KWD_UNIT)
			compiling_unit = 1;

		current_token = yylex();

		if (current_token != IDENTIFIER)
		{
			add_error_message(202, "", "");
		}
		else
			program_name = string_from_cstr(YYTEXT_STRING);

		if ((compiling_unit) && (string_length(program_name) < 3))
			add_error_message(454, "", "");

		current_token = yylex();

		if (current_token != SEMI_COLON)
		{

			add_error_message(200, ";", YYTEXT_STRING);

			/* Error-recovery: find any of the following:
			   CONST, TYPE, VAR, PROCEDURE, FUNCTION, BEGIN, ., EOF
			*/
			while ( (current_token != KWD_CONST)
				&& (current_token != KWD_TYPE)
				&& (current_token != KWD_VAR)
				&& (current_token != KWD_PROCEDURE)
				&& (current_token != KWD_FUNCTION)
				&& (current_token != KWD_BEGIN)
				&& (current_token != DOT)
				&& (current_token != END_OF_INPUT))
			{
				current_token = yylex();
			}
		}
		else
			current_token = yylex();
	}

	return program_name;
}

/*
	the interface part of the unit source

    <RD_unit_interface> -> [interface [;] <RD_block> ]

    special rules apply to declarations inside the interface; global variable inside_interface_part is used to denote the special cases
*/
void RD_unit_interface(block *current_block)
{
	if (current_token != KWD_INTERFACE)
	{
		if ((current_token == KWD_IMPLEMENTATION)
			|| (current_token == KWD_INITIALIZATION))
			return;

		add_error_message(203, "interface", YYTEXT_STRING);

		do
		{
			current_token = yylex();
		}
		while ((current_token != KWD_IMPLEMENTATION) && (current_token != END_OF_INPUT));

		return;
	}

	current_token = yylex();

	if (current_token == SEMI_COLON)
		current_token = yylex();

	inside_interface_part = 1;
	RD_block(current_block);
	inside_interface_part = 0;
}

/*
  the implementation part of the unit source

  <RD_unit_implementation> -> implementation [;] <RD_uses_list> <RD_block>
*/
void RD_unit_implementation(block *current_block)
{
	if (current_token == END_OF_INPUT)
		return;

	if (current_token != KWD_IMPLEMENTATION)
	{
		add_error_message(203, "implementation", YYTEXT_STRING);

		do
		{
			current_token = yylex();
		}
		while (current_token != END_OF_INPUT);

		return;
	}

	current_token = yylex();

	if (current_token == SEMI_COLON)
		current_token = yylex();

	RD_uses_list();

    // j-a-s-d
    if (detect_units_only)
      return;

	inside_implementation_part = 1;
	RD_block(current_block);
	inside_implementation_part = 0;
}

/*
	the initialization part of the unit

    <RD_unit_initialization> -> [initialization [;] <RD_block_body> ]
*/
void RD_unit_initialization(block* current_block)
{
	if (current_token == KWD_INITIALIZATION)
	{
		current_token = yylex();

		if (current_token == SEMI_COLON)
			current_token = yylex();

		RD_block_body(current_block);
	}
}

/*
	the finalization part of the unit is not supported by the compiler
*/
void RD_unit_finalization(block *current_block)
{
	if (current_token == KWD_FINALIZATION)
	{
		add_error_message(220, "", "");
		current_token = yylex();
		return;
	}
}

/*
	The list of 'uses' directives.
	<RD_uses_list> -> ( uses NAME; )*
*/
void RD_uses_list()
{
	while (current_token == KWD_USES)
	{
following_unit:
		current_token = yylex();

		if (current_token != IDENTIFIER)
		{
			add_error_message(202, "", "");

			/* do error recovery */
			while ((current_token != END_OF_INPUT)
				&& (current_token != SEMI_COLON))
			{
				current_token = yylex();
				return;
			}
		}

		/* process the library */
		load_extern_library(string_from_cstr(YYTEXT_STRING));

		current_token = yylex();

		if (current_token == COMMA)
			goto following_unit;

		if (current_token != SEMI_COLON)
		{
			add_error_message(200, ";", YYTEXT_STRING);

			/* do error recovery */
			while ((current_token != END_OF_INPUT)
				&& (current_token != SEMI_COLON))
			{
				current_token = yylex();
				return;
			}
		}
		else
			current_token = yylex();
	}

	if (current_token == SEMI_COLON)
		current_token = yylex();

	if (detect_units_only)
	{
		mem_close();

		fclose(yyin);

        // j-a-s-d
		//compile_terminate();
        exit(0);
	}
}


/*
	Program block with declarations and code.
	The grammar rule used:

	<RD_block> -> const <RD_const_declaration>
	            | type <RD_type_declaration>
				| var <RD_var_declaration>
				| procedure <RD_procedure_declaration>
				| function <RD_function_declaration>
				| begin <RD_block_body> end
*/
block* RD_block(block *current_block)
{
	switch (current_token)
	{
	case KWD_CONST:
		{
			current_token = yylex();
			RD_const_declaration(current_block);
			break;
		}

	case KWD_TYPE:
		{
			current_token = yylex();
			RD_type_declaration(current_block);
			break;
		}

	case KWD_VAR:
		{
			current_token = yylex();
			RD_var_declaration(current_block);
			break;
		}

	case KWD_PROCEDURE:
		{
			if (current_block->parent_block != root_block)
				add_error_message(431, "", "");

			current_token = yylex();
			RD_procedure_declaration(current_block);
			break;
		}

	case KWD_FUNCTION:
		{
			if (current_block->parent_block != root_block)
				add_error_message(431, "", "");

			current_token = yylex();
			RD_function_declaration(current_block);
			break;
		}

	case KWD_BEGIN:
		{
			if ((inside_interface_part) || (inside_implementation_part))
			{
				add_error_message(204, YYTEXT_STRING, "");

				do
				{
					current_token = yylex();
				} while (current_token != END_OF_INPUT);

				break;
			}

			current_token = yylex();

			RD_block_body(current_block);

			if (current_token != KWD_END)
			{
				add_error_message(203, "end", YYTEXT_STRING);
			}
			else
				current_token = yylex();

			break;
		}

	default:
		{
			if ((inside_interface_part)
				&& (   (current_token == KWD_IMPLEMENTATION)
				    || (current_token == KWD_INITIALIZATION)
				    || (current_token == KWD_FINALIZATION)
					|| (current_token == KWD_END)
				))
			{
				return current_block;
			}

			if ((inside_implementation_part)
				&& (   (current_token == KWD_INITIALIZATION)
				    || (current_token == KWD_FINALIZATION)
					|| (current_token == KWD_END)
				))
			{
				return current_block;
			}


			add_error_message(204, YYTEXT_STRING, "");

			/* Error-recovery: find any of the following:
			   CONST, TYPE, VAR, PROCEDURE, FUNCTION, BEGIN, ., EOF
			*/
			while ( (current_token != KWD_CONST)
				&& (current_token != KWD_TYPE)
				&& (current_token != KWD_VAR)
				&& (current_token != KWD_PROCEDURE)
				&& (current_token != KWD_FUNCTION)
				&& (current_token != KWD_BEGIN)
				&& (current_token != DOT)
				&& (current_token != END_OF_INPUT))
			{
				current_token = yylex();
			}

			break;
		}
	}

	return current_block;
}


/*
	Constant declarations, the grammar rule used is:

	  <RD_constant_declaration> -> ( IDN = CONST ; )+ <RD_block>
*/
void RD_const_declaration(block *current_block)
{
	int first_pass = 1;
	string *constant_name;

	do
	{
		if (first_pass)
			first_pass = 0;
		else
			current_token = yylex();

		if (current_token != IDENTIFIER)
		{
			break;
		}

		if (!block_check_name(current_block, YYTEXT_STRING))
		{
			add_error_message(400, YYTEXT_STRING, "");
		}

		constant_name = string_from_cstr(YYTEXT_STRING);

		current_token = yylex();

		if (current_token != OP_EQUAL)
		{
			add_error_message(200, '=', YYTEXT_STRING);

			/* Error-recovery: find the first SEMI_COLON */
			while ((current_token != SEMI_COLON)
				&& (current_token != END_OF_INPUT))
			{
				current_token = yylex();
			}

			if (current_token == END_OF_INPUT)
			{
				add_error_message(207, "", "");
				string_destroy(constant_name);
				return;
			}

			continue;
		}


		current_token = yylex();

		switch (current_token)
		{
		case CST_INTEGER:
			{
				add_integer_constant(current_block,
					integer_constant, string_get_cstr(constant_name));

				if (inside_interface_part)
					bsf_write_integer_constant(integer_constant, string_get_cstr(constant_name));

				current_token = yylex();
				break;
			}

		case CST_REAL:
			{
				add_real_constant(current_block,
					real_constant, string_get_cstr(constant_name));

				if (inside_interface_part)
					bsf_write_real_constant(real_constant, string_get_cstr(constant_name));

				current_token = yylex();
				break;
			}

		case CST_BOOLEAN:
			{
				add_boolean_constant(current_block,
					boolean_constant, string_get_cstr(constant_name));

				if (inside_interface_part)
					bsf_write_boolean_constant(boolean_constant, string_get_cstr(constant_name));

				current_token = yylex();
				break;
			}

		case CST_CHAR:
			{
				add_char_constant(current_block,
					char_constant, string_get_cstr(constant_name));

				if (inside_interface_part)
					bsf_write_char_constant(char_constant, string_get_cstr(constant_name));

				current_token = yylex();
				break;
			}

		case CST_STRING:
			{
				add_string_constant(current_block,
					string_constant, string_get_cstr(constant_name));

				if (inside_interface_part)
					bsf_write_string_constant(string_constant, string_get_cstr(constant_name));

				current_token = yylex();
				break;
			}

		default:
			{
				/* TODO:: ovo nije dobro, ovdje bi mogao pisati bilo kakav
				   konstantni izraz */
				add_error_message(206, "", "");

				/* Error-recovery: find the first semi-colon */
				while ( (current_token != SEMI_COLON)
					&& (current_token != END_OF_INPUT))
				{
					current_token = yylex();
				}

				if (current_token == END_OF_INPUT)
				{
					add_error_message(207, "", "");
					return;
				}

				break;
			}
		}

		string_destroy(constant_name);

	} while (current_token == SEMI_COLON);

	RD_block(current_block);
}


/*
	Handles variable declarations field. The grammar rule used is:

	<RD_var_declaration> -> (<RD_identifier_list> : <RD_type> ;)+ <RD_block>
*/
void RD_var_declaration(block *current_block)
{
	string_list *identifier_list;
	type *var_type;

	do
	{
		identifier_list = RD_identifier_list(current_block, 1);

		if (current_token != COLON)
		{
			add_error_message(200, ":", YYTEXT_STRING);

			/* Error-recovery: find any of the following:
			   CONST, TYPE, VAR, PROCEDURE, FUNCTION, BEGIN, ., EOF
			*/
			while ( (current_token != KWD_CONST)
				&& (current_token != KWD_TYPE)
				&& (current_token != KWD_VAR)
				&& (current_token != KWD_PROCEDURE)
				&& (current_token != KWD_FUNCTION)
				&& (current_token != KWD_BEGIN)
				&& (current_token != DOT)
				&& (current_token != END_OF_INPUT))
			{
				current_token = yylex();
			}

			if ((current_token == END_OF_INPUT)
				|| (current_token == DOT))
			{
				add_error_message(207, "", "");
				return;
			}

			break;
		}

		current_token = yylex();

		var_type = RD_type(current_block);

		if (var_type->type_class == interval_type)
		{
			add_error_message(440, "", "");
			var_type->type_class = integer_type;
		}

		add_variables(current_block, identifier_list, var_type);

		if (inside_interface_part)
			bsf_write_variables(identifier_list, var_type);

		type_destroy(var_type);
		string_list_destroy(identifier_list);

		if (current_token != SEMI_COLON)
		{
			add_error_message(200, ";", YYTEXT_STRING);

			/* Error-recovery: find any of the following:
			   CONST, TYPE, VAR, PROCEDURE, FUNCTION, BEGIN, ., EOF
			*/
			while ( (current_token != KWD_CONST)
				&& (current_token != KWD_TYPE)
				&& (current_token != KWD_VAR)
				&& (current_token != KWD_PROCEDURE)
				&& (current_token != KWD_FUNCTION)
				&& (current_token != KWD_BEGIN)
				&& (current_token != DOT)
				&& (current_token != END_OF_INPUT))
			{
				current_token = yylex();
			}

			if ((current_token == END_OF_INPUT)
				|| (current_token == DOT))
			{
				add_error_message(207, "", "");
				return;
			}

			break;
		}

		current_token = yylex();

		if (((inside_implementation_part) || (inside_interface_part))
				&& (   (current_token == KWD_INITIALIZATION)
				    || (current_token == KWD_IMPLEMENTATION)
					|| (current_token == KWD_INTERFACE)
					|| (current_token == KWD_END)
				))
			{
				break;
			}

	} while ( (current_token != KWD_CONST)
			&& (current_token != KWD_TYPE)
			&& (current_token != KWD_VAR)
			&& (current_token != KWD_PROCEDURE)
			&& (current_token != KWD_FUNCTION)
			&& (current_token != KWD_BEGIN)
			&& (current_token != DOT)
			&& (current_token != END_OF_INPUT));

	if ((current_token == END_OF_INPUT)
		|| (current_token == DOT))
	{
		add_error_message(207, "", "");
		return;
	}

	RD_block(current_block);
}


/*
	The list of identifiers, the rule used is:

	<RD_identifier_list> -> IDN (, IDN)*

	param check_valid_names should be nonzero if the function
	should call block_check_name() for each identifier
*/
string_list* RD_identifier_list(block *current_block,
								int check_valid_names)
{
	string_list *return_list;
	string *identifier;

	return_list = string_list_create();

	if (current_token != IDENTIFIER)
	{
		add_error_message(444, "", "");

		/* Error-recovery: find the first : or EOF */
		while ((current_token != COLON)
			&& (current_token != DOT)
			&& (current_token != END_OF_INPUT))
		{
			current_token = yylex();
		}

		return return_list;
	}

	if ((check_valid_names)
		&& (!block_check_name(current_block, YYTEXT_STRING)))
	{
		add_error_message(400, YYTEXT_STRING, "");
	}

	identifier = string_from_cstr(YYTEXT_STRING);
	string_list_append(return_list, identifier);
	string_destroy(identifier);

	current_token = yylex();

	while (current_token == COMMA)
	{
		current_token = yylex();

		if (current_token != IDENTIFIER)
		{
			add_error_message(202, "", "");

			/* Error-recovery: find the first : or EOF */
			while ((current_token != COLON)
				&& (current_token != DOT)
				&& (current_token != END_OF_INPUT))
			{
				current_token = yylex();
			}

			return return_list;
		}

		if ((check_valid_names)
			&& (!block_check_name(current_block, YYTEXT_STRING)))
		{
			add_error_message(400, YYTEXT_STRING, "");
		}

		identifier = string_from_cstr(YYTEXT_STRING);
		string_list_append(return_list, identifier);
		string_destroy(identifier);

		current_token = yylex();
	}

	return return_list;
}


/*
	Handles the declaration of a new type. The grammar
	rule used is:

	<RD_type_declaration> -> ( IDN = <RD_type> ; )+ <RD_block>
*/
void RD_type_declaration(block *current_block)
{
	string *type_name;
	type *type_type;

	do
	{
		if (current_token != IDENTIFIER)
		{
			if (((inside_implementation_part) || (inside_interface_part))
				&& (   (current_token == KWD_INITIALIZATION)
				    || (current_token == KWD_IMPLEMENTATION)
					|| (current_token == KWD_INTERFACE)
					|| (current_token == KWD_END)
				))
			{
				return;
			}

			add_error_message(202, "", "");

			/* Error-recovery: find the first ; or EOF */
			while ((current_token != SEMI_COLON)
				&& (current_token != DOT)
				&& (current_token != END_OF_INPUT))
			{
				current_token = yylex();
			}

			if (current_token == SEMI_COLON)
				RD_block(current_block);

			return;
		}

		type_name = string_from_cstr(YYTEXT_STRING);
		if (!block_check_name(current_block, YYTEXT_STRING))
			add_error_message(400, YYTEXT_STRING, "");

		current_token = yylex();

		if (current_token != OP_EQUAL)
		{
			add_error_message(205, YYTEXT_STRING, "");

			/* Error-recovery: find the first ; or EOF */
			while ((current_token != SEMI_COLON)
				&& (current_token != DOT)
				&& (current_token != END_OF_INPUT))
			{
				current_token = yylex();
			}

			if (current_token == SEMI_COLON)
				RD_block(current_block);

			string_destroy(type_name);

			return;
		}

		current_token = yylex();

		type_type = RD_type(current_block);

		add_type(current_block, type_name, type_type);

		if (inside_interface_part)
			bsf_write_type(type_name, type_type);

		if (current_token != SEMI_COLON)
		{
			add_error_message(200, ";", YYTEXT_STRING);

			/* Error-recovery: find the first ; or EOF */
			while ((current_token != SEMI_COLON)
				&& (current_token != DOT)
				&& (current_token != END_OF_INPUT))
			{
				current_token = yylex();
			}

			if (current_token == SEMI_COLON)
				RD_block(current_block);

			return;
		}

		current_token = yylex();
	} while ( (current_token != KWD_CONST)
			&& (current_token != KWD_TYPE)
			&& (current_token != KWD_VAR)
			&& (current_token != KWD_PROCEDURE)
			&& (current_token != KWD_FUNCTION)
			&& (current_token != KWD_BEGIN)
			&& (current_token != DOT)
			&& (current_token != END_OF_INPUT));

	if ((current_token == END_OF_INPUT)
		|| (current_token == DOT))
	{
		add_error_message(207, "", "");
		return;
	}

	RD_block(current_block);

}


/*
	Handles the procedure declaration. The grammar
	rule used is:

	<RD_procedure_declaration> -> IDN <RD_param_list> ;
	                               <RD_proc_block> ; <RD_block>
*/
void RD_procedure_declaration(block *current_block)
{
	string *procedure_name;
	block *procedure_block;
	type_list *parameters;
	int forward_declaration;
	type *void_return_type;
	int procedure_linenum = linenum;

	int old_inside_implementation = inside_implementation_part;

	inside_implementation_part = 0;

	if (!block_check_name(current_block, YYTEXT_STRING))
	{
		add_error_message(400, YYTEXT_STRING, "");
	}

	procedure_name = string_from_cstr(YYTEXT_STRING);

	if (current_token != IDENTIFIER)
	{
		add_error_message(202, "", "");

		/* Error-recovery: find the first ; */
		while ((current_token != SEMI_COLON)
			&& (current_token != END_OF_INPUT))
		{
			current_token = yylex();
		}

		if (current_token == END_OF_INPUT)
			return;
	}
	else
	{
		current_token = yylex();
	}

	if (STRING_COMPARE(procedure_name->cstr, "run") == 0)
		add_error_message(433, "", "");

	/* create a new block for the procedure */
	procedure_block = block_create(current_block, string_duplicate(procedure_name));

	parameters = RD_param_list(procedure_block);

	if (current_token != SEMI_COLON)
	{
		add_error_message(200, ";", YYTEXT_STRING);

		/* Error-recovery: do nothing */
	}
	else
		current_token = yylex();

	void_return_type = type_create();
	void_return_type->type_class = void_type;

	add_procedure(current_block, procedure_name, parameters, -1, procedure_linenum);

	if (inside_interface_part)
		bsf_write_procedure(procedure_name, parameters);

	forward_declaration = RD_proc_block(procedure_block, void_return_type, parameters);
	type_destroy(void_return_type);

	add_procedure(current_block, procedure_name, parameters, forward_declaration, procedure_linenum);

	if (current_token != SEMI_COLON)
	{
		if (inside_interface_part == 0)
			add_error_message(200, ";", YYTEXT_STRING);

		/* Error-recovery: do nothing */
	}
	else
		current_token = yylex();

	inside_implementation_part = old_inside_implementation;

	RD_block(current_block);
}


/*
	Handles the function declaration. The grammar
	rule used is:

	<RD_function_declaration> -> IDN <RD_param_list> : IDN ;
	                               <RD_proc_block> ; <RD_block>
*/
void RD_function_declaration(block *current_block)
{
	string *function_name;
	block *function_block;
	type_list *parameters;
	int forward_declaration;
	type *return_type;
	identifier *return_value;
	int function_linenum = linenum;

	int old_inside_implementation = inside_implementation_part;

	inside_implementation_part = 0;

	if (!block_check_name(current_block, YYTEXT_STRING))
	{
		add_error_message(400, YYTEXT_STRING, "");
	}

	function_name = string_from_cstr(YYTEXT_STRING);

	if (current_token != IDENTIFIER)
	{
		add_error_message(202, "", "");

		/* Error-recovery: find the first ; */
		while ((current_token != SEMI_COLON)
			&& (current_token != END_OF_INPUT))
		{
			current_token = yylex();
		}

		if (current_token == END_OF_INPUT)
			return;
	}
	else
		current_token = yylex();

	if (STRING_COMPARE(function_name->cstr, "run") == 0)
		add_error_message(433, "", "");

	/* create a new block for the function */
	function_block = block_create(current_block, string_duplicate(function_name));

	/* parameters start at index 0, variables follow after the return value */
	function_block->next_parameter_index = 0;
	function_block->next_variable_index = 0;

	parameters = RD_param_list(function_block);

	function_block->next_variable_index ++;

	if (current_token != COLON)
	{
		add_error_message(200, ":", YYTEXT_STRING);
	}
	else
		current_token = yylex();

	return_type = type_from_name(current_block, YYTEXT_STRING);

	if (return_type->type_class == error_type)
		add_error_message(406, YYTEXT_STRING, "");

	if (return_type->type_class == interval_type)
	{
		add_error_message(440, "", "");
		return_type->type_class = integer_type;
	}

	if (current_token != IDENTIFIER)
	{
		add_error_message(202, "", "");
	}
	else
		current_token = yylex();

	if (current_token != SEMI_COLON)
	{
		add_error_message(200, ";", YYTEXT_STRING);

		/* Error-recovery: do nothing */
	}
	else
		current_token = yylex();

	/* initialize the return value */
	return_value = identifier_create();
	return_value->identifier_class = variable_name;
	return_value->variable_index = type_list_length(parameters);
	return_value->variable_type = type_duplicate(return_type);
	return_value->belongs_to_program_block = 0;
	if (compiling_unit == 0)
		initialize_variable(function_block, return_value, NULL, 1, "M");
	else
		initialize_variable(function_block, return_value, NULL, 1, string_get_cstr(str_program_name));

	identifier_destroy(return_value);

	add_function(current_block, function_name, parameters,
			return_type, -1, function_linenum);

	forward_declaration = RD_proc_block(function_block, return_type, parameters);

	add_function(current_block, function_name, parameters,
			return_type, forward_declaration, function_linenum);

	if (inside_interface_part)
		bsf_write_function(function_name, parameters, return_type);

	if (current_token != SEMI_COLON)
	{
		if (inside_interface_part == 0)
			add_error_message(200, ";", YYTEXT_STRING);

		/* Error-recovery: do nothing */
	}
	else
		current_token = yylex();

	inside_implementation_part = old_inside_implementation;

	RD_block(current_block);
}


/*
	Allows forward declarations. The rule used is:

	<RD_proc_block> -> forward | <RD_block>

	Return nonzero value if the declaration is forward
	declaration.
*/
int RD_proc_block(block *current_block, type *return_type, type_list *parameters)
{
	int forward_declaration = 0;

	if ((current_token == KWD_FORWARD)
		|| (inside_interface_part))
	{
		forward_declaration = 1;

		if (inside_interface_part == 0)
			current_token = yylex();

		block_destroy(current_block);
	}
	else
	{
		if (inside_interface_part)
			add_error_message(221, "", "");

		RD_block(current_block);

		/* insert current_block into it's parents block list */
		if (current_block->parent_block->children_count == 0)
		{
			current_block->parent_block->children = (block**) mem_alloc(sizeof(block*));
		}
		else
		{
			current_block->parent_block->children = (block**) mem_realloc(current_block->parent_block->children,
				(current_block->parent_block->children_count + 1) * sizeof(block*));
		}

		current_block->parent_block->children_count ++;

		if (current_block->parent_block->children == NULL)
			die(20);

		switch(return_type->type_class)
		{
		case void_type:
			bytecode_append(current_block->code, return$);
			break;

		case real_type:
			usesFloat=1;
			if (mathType == 1)
			{
				bytecode_append(current_block->code, iload$);
				bytecode_append(current_block->code, type_list_length(parameters));
				bytecode_append(current_block->code, ireturn$);
			}
			else
			{
				switch(type_list_length(parameters))
				{
				case 0:
					bytecode_append(current_block->code, aload_0$);
					break;
				case 1:
					bytecode_append(current_block->code, aload_1$);
					break;
				case 2:
					bytecode_append(current_block->code, aload_2$);
					break;
				case 3:
					bytecode_append(current_block->code, aload_3$);
					break;
				default:
					bytecode_append(current_block->code, aload$);
					bytecode_append(current_block->code, type_list_length(parameters));
				}

				bytecode_append(current_block->code, areturn$);
			}
			break;

		case integer_type:
		case boolean_type:
		case char_type:
			bytecode_append(current_block->code, iload$);
			bytecode_append(current_block->code, type_list_length(parameters));
			bytecode_append(current_block->code, ireturn$);
			break;

		case string_type:
		case array_type:
		case record_type:
		case image_type:
		case command_type:
		case stream_type:
		case record_store_type:
		case http_type:
			switch(type_list_length(parameters))
			{
			case 0:
				bytecode_append(current_block->code, aload_0$);
				break;
			case 1:
				bytecode_append(current_block->code, aload_1$);
				break;
			case 2:
				bytecode_append(current_block->code, aload_2$);
				break;
			case 3:
				bytecode_append(current_block->code, aload_3$);
				break;
			default:
				bytecode_append(current_block->code, aload$);
				bytecode_append(current_block->code, type_list_length(parameters));
			}

			bytecode_append(current_block->code, areturn$);

			break;
		}

		current_block->parent_block->children[current_block->parent_block->children_count-1] = current_block;
	}

	return forward_declaration;
}


/*
	Handles the declarations of parameters to a
	procedure/function. The grammar rule used is:

	<RD_param_list> -> empty
	                   | "(" [var] <RD_identifier_list> : [IDN.]IDN (; [var] <RD_identifier_list> : [IDN.]IDN )+ ")"
*/
type_list* RD_param_list(block *current_block)
{
	type_list *parameter_list;
	string_list *identifier_list;
	type *parameters_type;
	int is_parameter_variable;

	int i, len;

	parameter_list = type_list_create();

	if (current_token == OPEN_BR)
	{
		do
		{
			current_token = yylex();

			if (current_token == END_OF_INPUT)
			{
				add_error_message(207, "", "");
				return parameter_list;
			}

			if (current_token == CLOSE_BR)
				break;

			is_parameter_variable = 0;

			if (current_token == KWD_VAR)
			{
				is_parameter_variable = 1;
				current_token = yylex();
				add_warning_message(436, "", "");
			}

			identifier_list = RD_identifier_list(current_block, 0);

			if (current_token != COLON)
			{
				add_error_message(200, ":", YYTEXT_STRING);

				/* Error-recovery, find the ")" */
				while (current_token != CLOSE_BR)
				{
					current_token = yylex();
				}

				if (current_token == END_OF_INPUT)
					return parameter_list;

				break;

			}
			else
				current_token = yylex();

			parameters_type = type_from_name(current_block, YYTEXT_STRING);

			if (parameters_type->type_class == error_type)
			{
				/* check if the name is an unit name */
				identifier *unit;

				lowercase(YYTEXT_STRING);

				unit = get_identifier(current_block, YYTEXT_STRING);

				if (unit->identifier_class == unit_name)
				{
					identifier *type_identifier;

					current_token = yylex();

					if (current_token != DOT)
						add_error_message(200, ".", YYTEXT_STRING);
					else
						current_token = yylex();

					type_identifier = name_table_find(unit->unit_block->names, string_from_cstr(YYTEXT_STRING));

					if (type_identifier == NULL)
						add_error_message(455, YYTEXT_STRING, "");
					else
						parameters_type = type_identifier->defined_type;
				}

				identifier_destroy(unit);
			}

			if (parameters_type->type_class == interval_type)
			{
				add_error_message(440, "", "");
				parameters_type->type_class = integer_type;
			}

			/* add the types into parameter_list */
			len = string_list_length(identifier_list);
			for(i=0; i<len; i++)
				type_list_append(parameter_list, type_duplicate(parameters_type));

			add_parameters(current_block, identifier_list, parameters_type,
					is_parameter_variable);

			if (current_token != IDENTIFIER)
			{
				add_error_message(202, "", "");

				/* Error-recovery */
				if ((current_token != COMMA)
					&& (current_token != CLOSE_BR))
					current_token = yylex();
			}
			else
				current_token = yylex();

		} while (current_token == SEMI_COLON);

		if (current_token != CLOSE_BR)
		{
			add_error_message(208, YYTEXT_STRING, "");

			/* Error-recovery: find the first ; */
			while ((current_token != CLOSE_BR)
				&& (current_token != END_OF_INPUT))
			{
				current_token = yylex();
			}
		}

		current_token = yylex();
	}

	return parameter_list;
}

/*
	Handles the block of code. The grammar rule used is:

	<RD_block_body> -> (<RD_statement> ; )+
*/
void RD_block_body(block *current_block)
{
	short int first_pass = 1;

	do
	{
		if (first_pass)
			first_pass = 0;
		else
			current_token = yylex();

		RD_statement(current_block);

	} while (current_token == SEMI_COLON);
}


/*
	The statements rule:

	<RD_statement> -> begin <RD_block_body> end
	                | empty (if current_token == end || current_token == until)
					| if <RD_if_statement>
					| case <RD_case_statement>
					| while <RD_while_statement>
					| repeat <RD_repeat_statement>
					| for <RD_for_statement>
					| with <RD_with_statement>
					| <RD_assignment_or_procedure_call>
					| break
*/
void RD_statement(block *current_block)
{
	switch (current_token)
	{
	case KWD_BEGIN:
		{
			current_token = yylex();
			RD_block_body(current_block);

			if (current_token != KWD_END)
			{
				add_error_message(203, "end", YYTEXT_STRING);
			}

			current_token = yylex();

			break;
		}

	case KWD_END: case KWD_UNTIL: case KWD_FOREVER:
		{
			return;
		}
	case KWD_IF:
		{
			current_token = yylex();
			RD_if_statement(current_block);
			break;
		}

	case KWD_CASE:
		{
			current_token = yylex();
			RD_case_statement(current_block);
			break;
		}

	case KWD_WHILE:
		{
			current_token = yylex();
			RD_while_statement(current_block);
			break;
		}

	case KWD_REPEAT:
		{
			current_token = yylex();
			RD_repeat_statement(current_block);
			break;
		}

	case KWD_FOR:
		{
			current_token = yylex();
			RD_for_statement(current_block);
			break;
		}

	case KWD_WITH:
		{
			add_error_message(215, "", "");
			current_token = yylex();
			RD_with_statement(current_block);
			break;
		}

	case IDENTIFIER:
		{
			RD_assignment_or_procedure_call(current_block);
			break;
		}

	case KWD_BREAK:
		{
			current_token = yylex();

			if (inside_loop <= 0)
				add_error_message(219, "", "");
			else
			{
				bytecode_append(current_block->code, break_stmt$);
				bytecode_append_short_int(current_block->code, 0);
			}
			break;
		}

	case KWD_INLINE:
		{
			current_token = yylex();
			if (current_token == OPEN_BR) {
				RD_inline_body(current_block);
				if (current_token != CLOSE_BR) {
					add_error_message(200, "<inline>) ", YYTEXT_STRING);
					while ((current_token != SEMI_COLON) && (current_token != END_OF_INPUT))
						current_token = yylex();
				} else current_token = yylex();
			} else {
				add_error_message(200, "<inline>(", YYTEXT_STRING);
				while ((current_token != SEMI_COLON) && (current_token != END_OF_INPUT))
					current_token = yylex();
			}
			break;
		}

	default:
		{
			add_error_message(204, YYTEXT_STRING, "");
			break;
		}
	}
}




///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// JAVA - ASM /////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Handles the inline statement. The grammar rule
	is:
	<inline (byte)>
	<inline (opcode)>
	<inline (opcode int)>
	<inline (opcode [[cp_type] 'param1', 'param2', 'param3'])>

*/

int RD_opcodes() {
	if (strcmp(yytext, "nop") == 0)              return 0x00;	// ничего не делает
	if (strcmp(yytext, "aconst_null") == 0)      return 0x01;	// загрузка в стек null ( пустой ссылки на объект)
	if (strcmp(yytext, "iconst_m1") == 0)        return 0x02;	// загрузка целочисленной константы -1
	if (strcmp(yytext, "iconst_0") == 0)         return 0x03;	// загрузка целочисленной константы 0}
	if (strcmp(yytext, "iconst_1") == 0)         return 0x04;	// загрузка целочисленной константы 1}
	if (strcmp(yytext, "iconst_2") == 0)         return 0x05;	// загрузка целочисленной константы 2}
	if (strcmp(yytext, "iconst_3") == 0)         return 0x06;	// загрузка целочисленной константы 3}
	if (strcmp(yytext, "iconst_4") == 0)         return 0x07;	// загрузка целочисленной константы 4}
	if (strcmp(yytext, "iconst_5") == 0)         return 0x08;	// загрузка целочисленной константы 5}
	if (strcmp(yytext, "lconst_0") == 0)         return 0x09;	// загрузка длинной целочисленной константы 0}
	if (strcmp(yytext, "lconst_1") == 0)         return 0x0A;	// загрузка длинной целочисленной константы 1}
	if (strcmp(yytext, "fconst_0") == 0)         return 0x0B;	// загрузка вещественного числа одинарной точности 0}
	if (strcmp(yytext, "fconst_1") == 0)         return 0x0C;	// загрузка вещественного числа одинарной точности 1}
	if (strcmp(yytext, "fconst_2") == 0)         return 0x0D;	// загрузка вещественного числа одинарной точности 2}
	if (strcmp(yytext, "dconst_0") == 0)         return 0x0E;	// загрузка вещественного числа двойной точности 0}
	if (strcmp(yytext, "dconst_1") == 0)         return 0x0F;	// загрузка вещественного числа двойной точности 1}
	if (strcmp(yytext, "bipush") == 0)           return 0x10;	// [1]загрузка в стек однобайтового целого со знаком}
	if (strcmp(yytext, "sipush") == 0)           return 0x11;	// [2+-]загрузка в стек двухбайтового целого со знаком}
	if (strcmp(yytext, "ldc1") == 0)             return 0x12;	// [1]загрузка в стек элемента из константного пула}
	if (strcmp(yytext, "ldc2") == 0)             return 0x13;	// [2]загрузка в стек элемента из константного пула}
	if (strcmp(yytext, "ldc2w") == 0)            return 0x14;	// [2]загрузка в стек длинного целого или двойного вещественного значения из константного пула}
	if (strcmp(yytext, "iload") == 0)            return 0x15;	// [1]загрузка целого из локальной переменной}
	if (strcmp(yytext, "lload") == 0)            return 0x16;	// [1]загрузка длинного целого из локальной переменной}
	if (strcmp(yytext, "fload") == 0)            return 0x17;	// [1]загрузка вещественного одинарной точности из локальной переменной}
	if (strcmp(yytext, "dload") == 0)            return 0x18;	// [1]загрузка вещественного двойной точности из локальной переменной}
	if (strcmp(yytext, "aload") == 0)            return 0x19;	// [1]загрузка объектной ссылки из локальной переменной}
	if (strcmp(yytext, "iload_0") == 0)          return 0x1A;	// загрузка целого из локальной переменной 0}
	if (strcmp(yytext, "iload_1") == 0)          return 0x1B;	// загрузка целого из локальной переменной 1}
	if (strcmp(yytext, "iload_2") == 0)          return 0x1C;	// загрузка целого из локальной переменной 2}
	if (strcmp(yytext, "iload_3") == 0)          return 0x1D;	// загрузка целого из локальной переменной 3}
	if (strcmp(yytext, "lload_0") == 0)          return 0x1E;	// загрузка длинного целого из локальной переменной 0}
	if (strcmp(yytext, "lload_1") == 0)          return 0x1F;	// загрузка длинного целого из локальной переменной 1}
	if (strcmp(yytext, "lload_2") == 0)          return 0x20;	// загрузка длинного целого из локальной переменной 2}
	if (strcmp(yytext, "lload_3") == 0)          return 0x21;	// загрузка длинного целого из локальной переменной 3}
	if (strcmp(yytext, "fload_0") == 0)          return 0x22;	// загрузка вещественного одинарной точности из локальной переменной 0}
	if (strcmp(yytext, "fload_1") == 0)          return 0x23;	// загрузка вещественного одинарной точности из локальной переменной 1}
	if (strcmp(yytext, "fload_2") == 0)          return 0x24;	// загрузка вещественного одинарной точности из локальной переменной 2}
	if (strcmp(yytext, "fload_3") == 0)          return 0x25;	// загрузка вещественного одинарной точности из локальной переменной 3}
	if (strcmp(yytext, "dload_0") == 0)          return 0x26;	// загрузка вещественного двойной точности из локальной переменной 0}
	if (strcmp(yytext, "dload_1") == 0)          return 0x27;	// загрузка вещественного двойной точности из локальной переменной 1}
	if (strcmp(yytext, "dload_2") == 0)          return 0x28;	// загрузка вещественного двойной точности из локальной переменной 2}
	if (strcmp(yytext, "dload_3") == 0)          return 0x29;	// загрузка вещественного двойной точности из локальной переменной 3}
	if (strcmp(yytext, "aload_0") == 0)          return 0x2A;	// загрузка объектной ссылки из локальной переменной 0}
	if (strcmp(yytext, "aload_1") == 0)          return 0x2B;	// загрузка объектной ссылки из локальной переменной 1}
	if (strcmp(yytext, "aload_2") == 0)          return 0x2C;	// загрузка объектной ссылки из локальной переменной 2}
	if (strcmp(yytext, "aload_3") == 0)          return 0x2D;	// загрузка объектной ссылки из локальной переменной 3}
	if (strcmp(yytext, "iaload") == 0)           return 0x2E;	// загрузка целого из массива}
	if (strcmp(yytext, "laload") == 0)           return 0x2F;	// загрузка длинного целого из массива}
	if (strcmp(yytext, "faload") == 0)           return 0x30;	// загрузка вещественного из массива}
	if (strcmp(yytext, "daload") == 0)           return 0x31;	// загрузка двойного вещественного из массива}
	if (strcmp(yytext, "aaload") == 0)           return 0x32;	// загрузка объектной ссылки из массива}
	if (strcmp(yytext, "baload") == 0)           return 0x33;	// загрузка байта со знаком из массива}
	if (strcmp(yytext, "caload") == 0)           return 0x34;	// загрузка символа из массива}
	if (strcmp(yytext, "saload") == 0)           return 0x35;	// загрузка короткого из массива}
	if (strcmp(yytext, "istore") == 0)           return 0x36;	// [1]сохранение целого значения в локальной переменной}
	if (strcmp(yytext, "lstore") == 0)           return 0x37;	// [1]сохранение длинного целого в локальной переменной}
	if (strcmp(yytext, "fstore") == 0)           return 0x38;	// [1]сохранение вещественного одинарной точности в локальной переменной}
	if (strcmp(yytext, "dstore") == 0)           return 0x39;	// [1]сохранение двойного вещественного в локальной переменной}
	if (strcmp(yytext, "astore") == 0)           return 0x3A;	// [1]сохранение объектной ссылки в локальной переменной}
	if (strcmp(yytext, "istore_0") == 0)         return 0x3B;	// сохранение целого в локальной переменной 0}
	if (strcmp(yytext, "istore_1") == 0)         return 0x3C;	// сохранение целого в локальной переменной 1}
	if (strcmp(yytext, "istore_2") == 0)         return 0x3D;	// сохранение целого в локальной переменной 2}
	if (strcmp(yytext, "istore_3") == 0)         return 0x3E;	// сохранение целого в локальной переменной 3}
	if (strcmp(yytext, "lstore_0") == 0)         return 0x3F;	// сохранение длинного целого в локальной переменной 0}
	if (strcmp(yytext, "lstore_1") == 0)         return 0x40;	// сохранение длинного целого в локальной переменной 1}
	if (strcmp(yytext, "lstore_2") == 0)         return 0x41;	// сохранение длинного целого в локальной переменной 2}
	if (strcmp(yytext, "lstore_3") == 0)         return 0x42;	// сохранение длинного целого в локальной переменной 3}
	if (strcmp(yytext, "fstore_0") == 0)         return 0x43;	// сохранение вещественного одинарной точности в локальной переменной 0}
	if (strcmp(yytext, "fstore_1") == 0)         return 0x44;	// сохранение вещественного одинарной точности в локальной переменной 1}
	if (strcmp(yytext, "fstore_2") == 0)         return 0x45;	// сохранение вещественного одинарной точности в локальной переменной 2}
	if (strcmp(yytext, "fstore_3") == 0)         return 0x46;	// сохранение вещественного одинарной точности в локальной переменной 3}
	if (strcmp(yytext, "dstore_0") == 0)         return 0x47;	// сохранение двойного вещественного в локальной переменной 0}
	if (strcmp(yytext, "dstore_1") == 0)         return 0x48;	// сохранение двойного вещественного в локальной переменной 1}
	if (strcmp(yytext, "dstore_2") == 0)         return 0x49;	// сохранение двойного вещественного в локальной переменной 2}
	if (strcmp(yytext, "dstore_3") == 0)         return 0x4A;	// сохранение двойного вещественного в локальной переменной 3}
	if (strcmp(yytext, "astore_0") == 0)         return 0x4B;	// сохранение объектной ссылки в локальной переменной 0}
	if (strcmp(yytext, "astore_1") == 0)         return 0x4C;	// сохранение объектной ссылки в локальной переменной 1}
	if (strcmp(yytext, "astore_2") == 0)         return 0x4D;	// сохранение объектной ссылки в локальной переменной 2}
	if (strcmp(yytext, "astore_3") == 0)         return 0x4E;	// сохранение объектной ссылки в локальной переменной 3}
	if (strcmp(yytext, "iastore") == 0)          return 0x4F;	// сохранение в целочисленном массиве}
	if (strcmp(yytext, "lastore") == 0)          return 0x50;	// сохранение в массиве из длинных целых}
	if (strcmp(yytext, "fastore") == 0)          return 0x51;	// сохранение в массиве из одинарных вещественных}
	if (strcmp(yytext, "dastore") == 0)          return 0x52;	// сохранение в массиве из двойных вещественных}
	if (strcmp(yytext, "aastore") == 0)          return 0x53;	// сохранение в массиве из объектных ссылок}
	if (strcmp(yytext, "bastore") == 0)          return 0x54;	// сохранение в массиве байтов со знаком}
	if (strcmp(yytext, "castore") == 0)          return 0x55;	// сохранение в символьном массиве}
	if (strcmp(yytext, "sastore") == 0)          return 0x56;	// сохранение в массиве из коротких целых}
	if (strcmp(yytext, "pop") == 0)              return 0x57;	// извлечение слова с вершины стека}
	if (strcmp(yytext, "pop2") == 0)             return 0x58;	// извлечение двух слов с вершины стека}
	if (strcmp(yytext, "dup") == 0)              return 0x59;	// дублирование слова на вершине стека}
	if (strcmp(yytext, "dup_x1") == 0)           return 0x5A;	// дублирование слово на вершине стека и помещение копии в стек на два слова ниже}
	if (strcmp(yytext, "dup_x2") == 0)           return 0x5B;	// дублирование вершины стека и помещение копии на три слова ниже}
	if (strcmp(yytext, "dup2") == 0)             return 0x5C;	// дублирование двух слов на вершине стека}
	if (strcmp(yytext, "dup2_x1") == 0)          return 0x5D;	// дублирование двух слов на вершине стека и помещение копий на два слова ниже}
	if (strcmp(yytext, "dup2_x2") == 0)          return 0x5E;	// дублирование двух слов на вершине стека и помещение копий на три слова ниже}
	if (strcmp(yytext, "swap") == 0)             return 0x5F;	// обмен двух слов на вершине стека}
	if (strcmp(yytext, "iadd") == 0)             return 0x60;	// сложение целых}
	if (strcmp(yytext, "ladd") == 0)             return 0x61;	// сложение длинных целых}
	if (strcmp(yytext, "fadd") == 0)             return 0x62;	// сложение одинарных вещественных}
	if (strcmp(yytext, "dadd") == 0)             return 0x63;	// сложение двойных вещественных}
	if (strcmp(yytext, "isub") == 0)             return 0x64;	// вычитание целых}
	if (strcmp(yytext, "lsub") == 0)             return 0x65;	// вычитание длинных целых}
	if (strcmp(yytext, "fsub") == 0)             return 0x66;	// вычитание одинарных вещественных}
	if (strcmp(yytext, "dsub") == 0)             return 0x67;	// вычитание двойных вещественных}
	if (strcmp(yytext, "imul") == 0)             return 0x68;	// умножение целых}
	if (strcmp(yytext, "lmul") == 0)             return 0x69;	// умножение длинных целых}
	if (strcmp(yytext, "fmul") == 0)             return 0x6A;	// умножение одинарных вещественных}
	if (strcmp(yytext, "dmul") == 0)             return 0x6B;	// умножение двойных вещественных}
	if (strcmp(yytext, "idiv") == 0)             return 0x6C;	// деление целых}
	if (strcmp(yytext, "ldiv") == 0)             return 0x6D;	// деление длинных целых}
	if (strcmp(yytext, "fdiv") == 0)             return 0x6E;	// деление одинарных вещественных}
	if (strcmp(yytext, "ddiv") == 0)             return 0x6F;	// деление двойных вещественных}
	if (strcmp(yytext, "irem") == 0)             return 0x70;	// остаток от деления целых}
	if (strcmp(yytext, "lrem") == 0)             return 0x71;	// остаток от деления длинных целых}
	if (strcmp(yytext, "frem") == 0)             return 0x72;	// остаток от деления одинарных вещественных}
	if (strcmp(yytext, "drem") == 0)             return 0x73;	// остаток от деления двойных вещественных}
	if (strcmp(yytext, "ineg") == 0)             return 0x74;	// отрицание целого}
	if (strcmp(yytext, "leg") == 0)              return 0x75;	// отрицание длинного целого}
	if (strcmp(yytext, "fneg") == 0)             return 0x76;	// отрицание одинарного вещественного}
	if (strcmp(yytext, "dneg") == 0)             return 0x77;	// отрицание двойного вещественного числа}
	if (strcmp(yytext, "ishl") == 0)             return 0x78;	// сдвиг целого влево}
	if (strcmp(yytext, "lshl") == 0)             return 0x79;	// сдвиг длинного целого влево}
	if (strcmp(yytext, "ishr") == 0)             return 0x7A;	// арифметический сдвиг целого вправо}
	if (strcmp(yytext, "lshr") == 0)             return 0x7B;	// арифметический сдвиг длинного целого вправо}
	if (strcmp(yytext, "iushr") == 0)            return 0x7C;	// логический сдвиг целого вправо}
	if (strcmp(yytext, "lushr") == 0)            return 0x7D;	// логический сдвиг длинного целого вправо}
	if (strcmp(yytext, "iand") == 0)             return 0x7E;	// логическое И с операндами целого типа}
	if (strcmp(yytext, "land") == 0)             return 0x7F;	// логическое И с операндами длинного целого типа}
	if (strcmp(yytext, "ior") == 0)              return 0x80;	// логическое ИЛИ с целочисленными операндами}
	if (strcmp(yytext, "lor") == 0)              return 0x81;	// логическое ИЛИ с операндами длинного целого типа}
	if (strcmp(yytext, "ixor") == 0)             return 0x82;	// исключающее ИЛИ с целочисленными операндами}
	if (strcmp(yytext, "lxor") == 0)             return 0x83;	// исключающее ИЛИ с операндами длинного целого типа}
	if (strcmp(yytext, "iinc") == 0)             return 0x84;	// [1,1+-]увеличение локальной переменной на константу}
	if (strcmp(yytext, "i2l") == 0)              return 0x85;	// преобразование целого в длинное целое}
	if (strcmp(yytext, "i2f") == 0)              return 0x86;	// целое в вещественное}
	if (strcmp(yytext, "i2d") == 0)              return 0x87;	// целое в двойное вещественное}
	if (strcmp(yytext, "l2i") == 0)              return 0x88;	// длинное целое в целое}
	if (strcmp(yytext, "l2f") == 0)              return 0x89;	// длинное целое в вещественное}
	if (strcmp(yytext, "l2d") == 0)              return 0x8A;	// длинное целое в двойное вещественное}
	if (strcmp(yytext, "f2i") == 0)              return 0x8B;	// вещественное в целое}
	if (strcmp(yytext, "f2l") == 0)              return 0x8C;	// вещественное в длинное целое}
	if (strcmp(yytext, "f2d") == 0)              return 0x8D;	// вещественное в двойное вещественное}
	if (strcmp(yytext, "d2i") == 0)              return 0x8E;	// двойное вещественное в целое}
	if (strcmp(yytext, "d2l") == 0)              return 0x8F;	// двойное вещественное в длинное целое}
	if (strcmp(yytext, "d2f") == 0)              return 0x90;	// двойное вещественное в вещественное}
	if (strcmp(yytext, "int2byte") == 0)         return 0x91;	// целое в знаковый байт}
	if (strcmp(yytext, "int2char") == 0)         return 0x92;	// целое в символ}
	if (strcmp(yytext, "int2short") == 0)        return 0x93;	// целое в короткое}
	if (strcmp(yytext, "lcmp") == 0)             return 0x94;	// сравнение длинных целых}
	if (strcmp(yytext, "fcmpl") == 0)            return 0x95;	// сравнение вещественных одинарной точности (-1 при NaN)}
	if (strcmp(yytext, "fcmpg") == 0)            return 0x96;	// сравнение вещественных одинарной точности (1 при NaN)}
	if (strcmp(yytext, "dcmpl") == 0)            return 0x97;	// сравнение вещественных двойной точности(-1 при NaN)}
	if (strcmp(yytext, "dcmpg") == 0)            return 0x98;	// сравнение вещественных двойной точности(1 при NaN)}
	if (strcmp(yytext, "ifeq") == 0)             return 0x99;	// [2]переход, если равно 0}
	if (strcmp(yytext, "ifne") == 0)             return 0x9A;	// [2]переход, если не равно 0}
	if (strcmp(yytext, "iflt") == 0)             return 0x9B;	// [2]переход, если меньше 0}
	if (strcmp(yytext, "ifge") == 0)             return 0x9C;	// [2]переход, если больше или равно 0}
	if (strcmp(yytext, "ifgt") == 0)             return 0x9D;	// [2]переход, если больше 0}
	if (strcmp(yytext, "ifle") == 0)             return 0x9E;	// [2]переход, если меньше или равно 0}
	if (strcmp(yytext, "if_icmpeq") == 0)        return 0x9F;	// [2]переход, если целые равны}
	if (strcmp(yytext, "if_icmpne") == 0)        return 0xA0;	// [2]переход, если целые не равны}
	if (strcmp(yytext, "if_icmplt") == 0)        return 0xA1;	// [2]переход, если целое меньше 0}
	if (strcmp(yytext, "if_icmpge") == 0)        return 0xA2;	// [2]переход, если целое больше или равно}
	if (strcmp(yytext, "if_icmpgt") == 0)        return 0xA3;	// [2]переход, если целое больше 0}
	if (strcmp(yytext, "if_icmple") == 0)        return 0xA4;	// [2]переход, если целое меньше или равно}
	if (strcmp(yytext, "if_acmpeq") == 0)        return 0xA5;	// [2]переход, если ссылки на объект равны}
	if (strcmp(yytext, "if_acmpne") == 0)        return 0xA6;	// [2]переход, если ссылки на объект не равны}
	if (strcmp(yytext, "goto") == 0)             return 0xA7;	// [2]переход на}
	if (strcmp(yytext, "jsr") == 0)              return 0xA8;	// [2]переход на подпрограмму}
	if (strcmp(yytext, "ret") == 0)              return 0xA9;	// [1]возврат из подпрограммы}
	if (strcmp(yytext, "tableswitch") == 0)      return 0xAA;	// [tbs] доступ к таблице перехода по индексу и переход}
	if (strcmp(yytext, "lookupswitch") == 0)     return 0xAB;	// [lks] доступ к таблице перехода по сравнению с ключом и переход}
	if (strcmp(yytext, "ireturn") == 0)          return 0xAC;	// возврат целого значения функции}
	if (strcmp(yytext, "lreturn") == 0)          return 0xAD;	// возврат длинного целого значения функции}
	if (strcmp(yytext, "freturn") == 0)          return 0xAE;	// возврат одинарного вещественного значения функции}
	if (strcmp(yytext, "dreturn") == 0)          return 0xAF;	// возврат двойного вещественного значения функции}
	if (strcmp(yytext, "areturn") == 0)          return 0xB0;	// возврат объектной ссылки из функции}
	if (strcmp(yytext, "return") == 0)           return 0xB1;	// возврат(опустошающий) из процедуры}
	if (strcmp(yytext, "getstatic") == 0)        return 0xB2;	// [2fld]получение статического поля класса}
	if (strcmp(yytext, "putstatic") == 0)        return 0xB3;	// [2fld]установка статического поля в классе}
	if (strcmp(yytext, "getfield") == 0)         return 0xB4;	// [2fld]перенос поля из объекта}
	if (strcmp(yytext, "putfield") == 0)         return 0xB5;	// [2fld]установка поля в объекте}
	if (strcmp(yytext, "invokevirtual") == 0)    return 0xB6;	// [2mtd],вызывает метод экземпляра, основываясь на типе времени выполнения}
	if (strcmp(yytext, "invokenonvirtual") == 0) return 0xB7;	// [2mtd],вызывает метод экземпляра, основываясь на не виртуальном типе}
	if (strcmp(yytext, "invokestatic") == 0)     return 0xB8;	// [2mtd]вызов метода класса (статического метода)}
	if (strcmp(yytext, "invokeinterface") == 0)  return 0xB9;	// [2,1,1]вызывает метод интерфейса}
	if (strcmp(yytext, "new") == 0)              return 0xBB;	// [2]создает новый объект}
	if (strcmp(yytext, "newarray") == 0)         return 0xBC;	// [1]atype> T_BOOLEAN=4,T_CHAR=5,T_FLOAT=6,T_DOUBLE=7,T_BYTE=8, T_SHORT=9,T_INT=9,T_LONG=11}
	if (strcmp(yytext, "anewarray") == 0)        return 0xBD;	// [2class]объявление нового массива из ссылок на объекты}
	if (strcmp(yytext, "arraylength") == 0)      return 0xBE;	// возвращает длину массива}
	if (strcmp(yytext, "athrow") == 0)           return 0xBF;	// генерация обработки или ошибки}
	if (strcmp(yytext, "checkcast") == 0)        return 0xC0;	// cs 2,проверяет, что объект имеет данный тип}
	if (strcmp(yytext, "instanceof") == 0)       return 0xC1;	// [2class]определяет, имеет ли объект данный тип}
	if (strcmp(yytext, "monitorenter") == 0)     return 0xC2;	// вход в контролируемую область кода}
	if (strcmp(yytext, "monitorexit") == 0)      return 0xC3;	// выход из контролируемой области кода}
	if (strcmp(yytext, "wide") == 0)             return 0xC4;	// расширенный индекс для доступа к локальным переменным для команд загрузки, сохранения и приращени}
	if (strcmp(yytext, "multianewarray") == 0)   return 0xC5;	// [2cp-index,1b]размещение нового многомерного массива}
	if (strcmp(yytext, "ifnull") == 0)           return 0xC6;	// [2ofs]переход, если пустой указатель}
	if (strcmp(yytext, "ifnonnull") == 0)        return 0xC7;	// [2ofs]переход, если не пустой указатель}
	if (strcmp(yytext, "goto_w") == 0)           return 0xC8;	// [4ofs]переход на (расширенный индекс)}
	if (strcmp(yytext, "jsr_w") == 0)            return 0xC9;	// [4ofs]переход на подпрограмму (расширенный индекс)}
	if (strcmp(yytext, "breakpoint") == 0)       return 0xCA;	// остановка и передача контроля обработчику прерываний}
	if (strcmp(yytext, "ret_w") == 0)            return 0xD1;	// [2]возврат из подпрограммы (расширенный индекс)}
	return 0xFF;
	//0x10,0xBC					1b;
	//0x11						2b+-;
	//0x12						1b constpool-index;
	//0x13						2b constpool-index(2byte-const);
	//0x14						2b constpool-index(4byte-const);
	//0xB2,0xB3,0xB4,0xB5		2b constpool-field;
	//0xB6,0xB7,0xB8			2b constpool-method;
	//0xB9						2b constpool-method; 1b; 1b;
	//0xBB						2b constpool-index;
	//0xC5						2b constpool-index; 1b;
	//0xBD,0xC1					2b constpool-class;
	//0xC0						2b constpool-string;
	//0x15,0x16,0x17,0x18,0x19	1b locvar-index;
	//0x36,0x37,0x38,0x39,0x3A	1b locvar-index;
	//0x84						1b; 1b+-(locvar-add-const);
	//0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xC6,0xC7	2b offs;
	//0xC8						4b offs;
	//0xC9						4b call;
	//0xA8						2b call;
	//0xA9						1b ret;
	//0xD1						2b ret;
	//0xC4						wide...;
	//0xAA						tableswitch...;
	//0xAB						loookupswitch...;
	//остальные коды параметров не имеют...
}


int cp_index_body(){ //разбор модификаций ссылок на параметры константного пула
	char p1[512];
	char p2[512];
	if (current_token == IDENTIFIER) {
		if (strcmp(yytext, "int") == 0) {
			current_token = yylex();
			if (current_token == CST_INTEGER) return cp_add_integer(integer_constant);
		} else if (strcmp(yytext, "utf8") == 0) {
			current_token = yylex();
			if ((current_token == CST_STRING)||(current_token == CST_CHAR)) return cp_add_utf8(yytext);
		} else if (strcmp(yytext, "str") == 0) {
			current_token = yylex();
			if ((current_token == CST_STRING)||(current_token == CST_CHAR)) return cp_add_string(yytext);
		} else if (strcmp(yytext, "class") == 0) {
			current_token = yylex();
			if ((current_token == CST_STRING)||(current_token == CST_CHAR)) return cp_add_class(yytext);
		} else if (strcmp(yytext, "nametype") == 0) {
			current_token = yylex();
			if ((current_token == CST_STRING)||(current_token == CST_CHAR)) {
				strcpy(&p1,yytext);
				current_token = yylex();
				if (current_token == COMMA) {
					current_token = yylex();
					if ((current_token == CST_STRING)||(current_token == CST_CHAR)) return cp_add_nameandtype(p1, yytext);
				}
			}
		} else if (strcmp(yytext, "field") == 0) {
			current_token = yylex();
			if ((current_token == CST_STRING)||(current_token == CST_CHAR)) {
				strcpy(&p1,yytext);
				current_token = yylex();
				if (current_token == COMMA) {
					current_token = yylex();
					if ((current_token == CST_STRING)||(current_token == CST_CHAR)) {
						strcpy(&p2,yytext);
						current_token = yylex();
						if (current_token == COMMA) {
							current_token = yylex();
							if ((current_token == CST_STRING)||(current_token == CST_CHAR)) return cp_add_fieldref(p1,p2,yytext);
						}
					}
				}
			}
		} else if (strcmp(yytext, "method") == 0) {
			current_token = yylex();
			if ((current_token == CST_STRING)||(current_token == CST_CHAR)) {
				strcpy(&p1,yytext);
				current_token = yylex();
				if (current_token == COMMA) {
					current_token = yylex();
					if ((current_token == CST_STRING)||(current_token == CST_CHAR)) {
						strcpy(&p2,yytext);
						current_token = yylex();
						if (current_token == COMMA) {
							current_token = yylex();
							if ((current_token == CST_STRING)||(current_token == CST_CHAR)) return cp_add_methodref(p1,p2,yytext);
						}
					}
				}
			}
		}
	}
	return -1;  //ошибка разбора параметров
}


void RD_inline_body(block *current_block)
{
	int cp_index,opcode,err;
	err=0;
	do {
		current_token = yylex();
		if (current_token == IDENTIFIER) {
			opcode=RD_opcodes();
			bytecode_append(current_block->code, opcode);
			///////
			switch (opcode)
			{
			case 0x10:  //1byte;
			case 0xBC:
			case 0x15:  //1b locvar-index;
			case 0x16:
			case 0x17:
			case 0x18:
			case 0x19:
			case 0x36:  //1b locvar-index;
			case 0x37:
			case 0x38:
			case 0x39:
			case 0x3A:
			case 0xA9:  //1b ret;
				err=1;
				current_token = yylex();
				if ((current_token == CST_INTEGER) && (integer_constant>=0) && (integer_constant<=255)) {
					bytecode_append(current_block->code, integer_constant);
					err=0;
				}
				break;
			case 0x84:  //1b; 1b+-(locvar-add-const);
				err=1;
				current_token = yylex();
				if ((current_token == CST_INTEGER) && (integer_constant>=0) && (integer_constant<=255)) {
					bytecode_append(current_block->code, integer_constant);
					current_token = yylex();
					if (current_token == COMMA) {
						current_token = yylex();
						if ((current_token == CST_INTEGER) && (integer_constant>=0)&&(integer_constant<=255)) {
							bytecode_append(current_block->code, integer_constant);
							err=0;
						}
					}
				}
				break;
			case 0x11:  //2b+-;
				err=1;
				current_token = yylex();
				if ((current_token == CST_INTEGER) && (integer_constant>=0) && (integer_constant<=65535)) {
					bytecode_append_short_int(current_block->code, integer_constant);
					err=0;
				}
				break;
			case 0x99:  //2b offs;
			case 0x9A:
			case 0x9B:
			case 0x9C:
			case 0x9D:
			case 0x9E:
			case 0x9F:
			case 0xA0:
			case 0xA1:
			case 0xA2:
			case 0xA3:
			case 0xA4:
			case 0xA5:
			case 0xA6:
			case 0xA7:
			case 0xC6:
			case 0xC7:
			case 0xA8:  //2b call;
			case 0xD1:  //2b ret;
				err=1;
				current_token = yylex();
				if ((current_token == CST_INTEGER) && (integer_constant>=0) && (integer_constant<=65535)) {
					bytecode_append_short_int(current_block->code, integer_constant);
					err=0;
				}
				break;
			case 0xC8:  //4b offs;
			case 0xC9:  //4b call;
				err=1;
				current_token = yylex();
				if (current_token == CST_INTEGER) {
					bytecode_append_long_int(current_block->code, integer_constant);
					err=0;
				}
				break;
			case 0x12:  //1b constpool-index;
				err=1;
				current_token = yylex();
				cp_index=cp_index_body();
				if ((cp_index>=0) && (cp_index<=255)) {
					bytecode_append(current_block->code, cp_index);
					err=0;
				}
				break;
			case 0xBB:  //2b constpool-index;
				err=1;
				current_token = yylex();
				cp_index=cp_index_body();
				if (cp_index>=0) {
					bytecode_append_short_int(current_block->code, cp_index);
					err=0;
				}
				break;
			case 0xC5:  //2b constpool-index; 1b;
				err=1;
				current_token = yylex();
				cp_index=cp_index_body();
				if (cp_index>=0) {
					bytecode_append_short_int(current_block->code, cp_index);
					current_token = yylex();
					if (current_token == COMMA) {
						current_token = yylex();
						if (current_token == CST_INTEGER) {
							if ((integer_constant>=0) && (integer_constant<=255)) {
								bytecode_append(current_block->code, integer_constant);
								err=0;
							}
						}
					}
				}
				break;
			case 0xBD:  //2b constpool-class;
			case 0xC1:
				err=1;
				current_token = yylex();
				if ((current_token == IDENTIFIER) && (strcmp(yytext, "class") == 0)) {
					cp_index=cp_index_body();
					if (cp_index>=0) {
						bytecode_append_short_int(current_block->code, cp_index);
						err=0;
					}
				}
				break;
			case 0xC0:  //2b constpool-string;
				err=1;
				current_token = yylex();
				if ((current_token == IDENTIFIER) && (strcmp(yytext, "str") == 0)) {
					cp_index=cp_index_body();
					if (cp_index>=0) {
						bytecode_append_short_int(current_block->code, cp_index);
						err=0;
					}
				}
				break;
			case 0x13:  //2b constpool-index(2byte-const);
			case 0x14:  //2b constpool-index(4byte-const);
				err=1;
				current_token = yylex();
				if ((current_token == IDENTIFIER) && (strcmp(yytext, "int") == 0)) {
					cp_index=cp_index_body();
					if (cp_index>=0) {
						bytecode_append_short_int(current_block->code, cp_index);
						err=0;
					}
				}
				break;
			case 0xB2:  //2b constpool-field;
			case 0xB3:
			case 0xB4:
			case 0xB5:
				err=1;
				current_token = yylex();
				if ((current_token == IDENTIFIER) && (strcmp(yytext, "field") == 0)) {
					cp_index=cp_index_body();
					if (cp_index>=0) {
						bytecode_append_short_int(current_block->code, cp_index);
						err=0;
					}
				}
				break;
			case 0xB6:  //2b constpool-method;
			case 0xB7:
			case 0xB8:
				err=1;
				current_token = yylex();
				if ((current_token == IDENTIFIER) && (strcmp(yytext, "method") == 0)) {
					cp_index=cp_index_body();
					if (cp_index>=0) {
						bytecode_append_short_int(current_block->code, cp_index);
						err=0;
					}
				}
				break;
			case 0xB9:  //2b constpool-method; 1b; 1b;
				err=1;
				current_token = yylex();
				if ((current_token == IDENTIFIER) && (strcmp(yytext, "method") == 0)) {
					cp_index=cp_index_body();
					if (cp_index>=0) {
						bytecode_append_short_int(current_block->code, cp_index);
						current_token = yylex();
						if (current_token == COMMA) {
							current_token = yylex();
							if (current_token == CST_INTEGER) {
								if ((integer_constant>=0) && (integer_constant<=255)) {
									bytecode_append(current_block->code, integer_constant);
									current_token = yylex();
									if (current_token == COMMA) {
										current_token = yylex();
										if (current_token == CST_INTEGER) {
											if ((integer_constant>=0) && (integer_constant<=255)) {
												bytecode_append(current_block->code, integer_constant);
												err=0;
											}
										}
									}
								}
							}
						}
					}
				}
				break;
			case 0xAA:  //tableswitch...;
			case 0xAB:  //loookupswitch...;
				while (current_block->code->bytecode_pos%4!=0)  //4byte-align
					bytecode_append(current_block->code, 0);
				break;
			/*
			case 0xC4:  //wide;
				break;
			*/
			case 0xFF:
				err=1;
				break;
			}
		} else if (current_token == CST_INTEGER) {
			if (integer_constant<=255) {
				bytecode_append(current_block->code, integer_constant);
			} else if (integer_constant<=65535) {
				bytecode_append_short_int(current_block->code, integer_constant);
			} else {
				bytecode_append_long_int(current_block->code, integer_constant);
			}
			err=0;
		} else err=1;
		/////////
		if (err==0) {
			current_token = yylex();
			if (current_token != SEMI_COLON) {
				err=1;
			}
		}
	} while (err==0);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// END JAVA - ASM ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////




/*
	Handles the if-then-[else] statement. The grammar rule
	is:

	<RD_if_statement> -> <RD_expression> then <RD_statement>
	                      [ else <RD_statement> ]

*/
void RD_if_statement(block *current_block)
{
	type *expression_type;
	long int jump_offset_position;
	signed short int offset;

	expression_type = RD_expression(current_block);

	if ((expression_type->type_class != error_type) /* a simple error-recovery */
		&& (expression_type->type_class != boolean_type))
	{
		add_error_message(407, "boolean", "");
	}

	type_destroy(expression_type);

	if (current_token != KWD_THEN)
	{
		add_error_message(216, YYTEXT_STRING, "");
	}

	current_token = yylex();

	/* generate the code */
	bytecode_append(current_block->code, ifeq$); /* branch if comparison with zero succedd */
	jump_offset_position = current_block->code->bytecode_pos;
	bytecode_append(current_block->code, 0); /* the place for offset of where to jump */
	bytecode_append(current_block->code, 0);

	RD_statement(current_block);

	while (current_token == SEMI_COLON)
		current_token = yylex();

	if (current_token == KWD_ELSE)
	{
		/* set the address of the next instruction */
		offset = (short) (current_block->code->bytecode_pos + 4 - jump_offset_position);
		current_block->code->bytecode[jump_offset_position] = (char) (offset>>8);
		current_block->code->bytecode[jump_offset_position+1] = (char) offset;

		bytecode_append(current_block->code, goto$);
		jump_offset_position = current_block->code->bytecode_pos;
		bytecode_append(current_block->code, 0);
		bytecode_append(current_block->code, 0);

		current_token = yylex();

		RD_statement(current_block);

		/* set the adress of the next instruction */
		offset = (short) (current_block->code->bytecode_pos - jump_offset_position + 1);
		current_block->code->bytecode[jump_offset_position] = (char) (offset>>8);
		current_block->code->bytecode[jump_offset_position+1] = (char) offset;
	}
	else
	{
		char *yycopy;
		int i;

		/* set the address of the next instruction */
		offset = (short) (current_block->code->bytecode_pos - jump_offset_position + 1);
		current_block->code->bytecode[jump_offset_position] = (char) (offset>>8);
		current_block->code->bytecode[jump_offset_position+1] = (char) offset;

		/* put the current token back to stream and preceed it with ';' */
		yycopy = strdup(yytext);

		for(i = strlen(yytext) - 1; i >= 0; i--)
			backchar(yycopy[i]);
		backchar(';');
		free(yycopy);

		current_token = yylex();
	}
}

/*
	The case-of statement. The rule used is:

	<RD_case_statement> -> <RD_expression> OF
	                         <RD_case_list> end
*/
void RD_case_statement(block *current_block)
{
	type *expression_type;
	expression_type = RD_expression(current_block);

	add_error_message(442, "", "");

	if ((expression_type->type_class != error_type) /* a simple error-recovery */
		&& (expression_type->type_class != integer_type)
		&& (expression_type->type_class != char_type)
		&& (expression_type->type_class != string_type))
	{
		add_error_message(408, "", "");
	}

	if (current_token != KWD_OF)
	{
		add_error_message(203, "of", YYTEXT_STRING);
	}

	current_token = yylex();

	RD_case_list(current_block, expression_type);

	if (current_token != KWD_END)
	{
		add_error_message(203, "end", YYTEXT_STRING);
	}

	current_token = yylex();
	type_destroy(expression_type);
}


/*
	The case expression's list.

	<RD_case_list> -> ( CONST (, CONST)* : <RD_statement> )+
*/
void RD_case_list(block *current_block, type *case_type)
{
	short int first_pass = 1;

	do
	{
		if (first_pass)
			first_pass = 0;
		else
			current_token = yylex();

		switch(current_token)
		{
		case CST_INTEGER:
			{
				if (case_type->type_class != integer_type)
				{
					string *expected_type_name;
					expected_type_name = type_get_name(case_type);
					add_error_message(409, string_get_cstr(expected_type_name), "integer");
					string_destroy(expected_type_name);
				}
				current_token = yylex();
				break;
			}

		case CST_REAL:
			{
				string *expected_type_name;
				expected_type_name = type_get_name(case_type);
				add_error_message(409, string_get_cstr(expected_type_name), "real");
				string_destroy(expected_type_name);

				current_token = yylex();
				break;
			}

		case CST_BOOLEAN:
			{

				string *expected_type_name;
				expected_type_name = type_get_name(case_type);
				add_error_message(409, string_get_cstr(expected_type_name), "boolean");
				string_destroy(expected_type_name);

				current_token = yylex();
				break;
			}

		case CST_CHAR:
			{
				if (case_type->type_class != char_type)
				{
					string *expected_type_name;
					expected_type_name = type_get_name(case_type);
					add_error_message(409, string_get_cstr(expected_type_name), "char");
					string_destroy(expected_type_name);
				}
				current_token = yylex();
				break;
			}

		case CST_STRING:
			{
				if (case_type->type_class != string_type)
				{
					string *expected_type_name;
					expected_type_name = type_get_name(case_type);
					add_error_message(409, string_get_cstr(expected_type_name), "string");
					string_destroy(expected_type_name);
				}
				current_token = yylex();
				break;
			}

		case IDENTIFIER:
			{
				/* find the constant type for the given identifier */
				type *constant_type;

				constant_type = get_constant_type(current_block, YYTEXT_STRING);

				if (constant_type->type_class == error_type)
					add_error_message(410, YYTEXT_STRING, "");
				else
					if (!type_equal(constant_type, case_type))
					{
						string *expected_type_name;
						string *constant_type_name;

						expected_type_name = type_get_name(case_type);
						constant_type_name = type_get_name(constant_type);

						add_error_message(409, string_get_cstr(expected_type_name), string_get_cstr(constant_type_name));

						string_destroy(expected_type_name);
						string_destroy(constant_type_name);
					}

					type_destroy(constant_type);

				current_token = yylex();
				break;
			}

		default:
			{
				add_error_message(206, "", "");

				/* Error-recovery: find the first : */
				while ((current_token != COLON)
					&& (current_token != END_OF_INPUT))
				{
					current_token = yylex();
				}

				if (current_token == END_OF_INPUT)
					return;

				break;
			}
		}


	} while (current_token == COMMA);

	current_token = yylex();

	if (current_token != COLON)
	{
		add_error_message(200, ":", YYTEXT_STRING);

		/* Error-recovery: find the first : */
		while ((current_token != COLON)
			&& (current_token != END_OF_INPUT))
		{
			current_token = yylex();
		}

		if (current_token == END_OF_INPUT)
			return;
	}

	current_token = yylex();

	RD_statement(current_block);
}


/*
	The while-do statement.

	<RD_while_statement> -> <RD_expression> do <RD_statement>
*/
void RD_while_statement(block *current_block)
{
	type *expression_type;
	int pos1, pos2;
	int jump_offset_position;
	int jump_offset;
	int break_pos1, break_pos2;

	pos1 = current_block->code->bytecode_pos;

	expression_type = RD_expression(current_block);

	pos2 = current_block->code->bytecode_pos;
	bytecode_append(current_block->code, ifeq$);
	jump_offset_position = current_block->code->bytecode_pos;
	bytecode_append_short_int(current_block->code, 0); /* make place for offset value */

	if ((expression_type->type_class != error_type) /* a simple error-recovery */
		&& (expression_type->type_class != boolean_type))
	{
		add_error_message(407, "boolean", "");
	}

	type_destroy(expression_type);

	if (current_token != KWD_DO)
	{
		add_error_message(203, "do", YYTEXT_STRING);
	}
	else
		current_token = yylex();

	inside_loop++;
	break_pos1 = current_block->code->bytecode_pos;
	RD_statement(current_block);
	break_pos2 = current_block->code->bytecode_pos;
	inside_loop --;

	bytecode_append(current_block->code, goto$);
	bytecode_append_short_int(current_block->code, pos1 - current_block->code->bytecode_pos + 1);

	jump_offset = current_block->code->bytecode_pos - pos2;
	current_block->code->bytecode[jump_offset_position] = (char)(jump_offset >> 8);
	current_block->code->bytecode[jump_offset_position + 1] = (char) jump_offset;

	transform_break_stmts(current_block->code, break_pos1, break_pos2, current_block->code->bytecode_pos);
}


// j-a-s-d: besides
//
//	current_token = yylex();
//
//	bytecode_append(current_block->code, goto$);
//	bytecode_append_short_int(current_block->code, pos1 - current_block->code->bytecode_pos + 1);
//
//	transform_break_stmts(current_block->code, break_pos1, break_pos2, current_block->code->bytecode_pos);
//
// is the exact implementation of forever,
// the lesser impact implementation of repeat/forever to this MP compiler seems to be an "until false"

type* RD_forever(block *current_block)
{
    type *return_type;
	return_type = type_create();
	bytecode_append(current_block->code, iconst_0$);
	current_token = yylex();
	return_type->type_class = boolean_type;
	return return_type;
}

/*
	The repeat-until statement.

	<RD_repeat_statement> -> <RD_block_body> until <RD_expression>
*/

void RD_repeat_statement(block *current_block)
{
	type *expression_type;

	int pos1;
	int break_pos1, break_pos2;

	pos1 = current_block->code->bytecode_pos;
	inside_loop ++;
	break_pos1 = current_block->code->bytecode_pos;
	RD_block_body(current_block);
	break_pos2 = current_block->code->bytecode_pos;
	inside_loop --;

	if (current_token == KWD_FOREVER)
	{
		expression_type = RD_forever(current_block);
	} else {
		if (current_token != KWD_UNTIL)
		{
			add_error_message(203, "until", YYTEXT_STRING);
		}

		current_token = yylex();

		expression_type = RD_expression(current_block);
	}
	/* generate the code */
	bytecode_append(current_block->code, ifeq$);
	bytecode_append_short_int(current_block->code, pos1 - current_block->code->bytecode_pos + 1);

	if ((expression_type->type_class != error_type) /* a simple error-recovery */
		&& (expression_type->type_class != boolean_type))
	{
		add_error_message(407, "boolean", "");
	}

	transform_break_stmts(current_block->code, break_pos1, break_pos2, current_block->code->bytecode_pos);

	type_destroy(expression_type);
}



/*
	The for statement.

	<RD_for_statement> -> IDN := <RD_expression> (to | downto)
	                        <RD_expression> do <RD_statement>
*/
void RD_for_statement(block *current_block)
{
	type *iterator_type;
	type *expression1_type;
	type *expression2_type;
	identifier *iterator;
	string *iterator_name;
	int is_field;
	int check_bytecode_pos;
	int direction;
	int evaluate_pos;
	int break_pos1, break_pos2;

	iterator_type = get_variable_type(current_block, YYTEXT_STRING);
	iterator = get_identifier(current_block, YYTEXT_STRING);
	iterator_name = string_from_cstr(YYTEXT_STRING);

	if (iterator->identifier_class == unit_name)
	{
		add_error_message(456, "", "");
		current_token = yylex();
		current_token = yylex();
	}

	if ((iterator_type->type_class != integer_type)
		&& (iterator_type->type_class != char_type))
	{
		add_error_message(411, "", "");
	}

	if (current_token != IDENTIFIER)
	{
		add_error_message(202, "", "");

		/* Error-recovery: find the first := */
		while ((current_token != OP_ASSIGN)
			&& (current_token != END_OF_INPUT))
		{
			current_token = yylex();
		}

		if (current_token == END_OF_INPUT)
		{
			type_destroy(iterator_type);
			string_destroy(iterator_name);
			return;
		}
	}
	else
		current_token = yylex();

	if (current_token != OP_ASSIGN)
	{
		add_error_message(209, YYTEXT_STRING, "");
	}
	else
		current_token = yylex();

	expression1_type = RD_expression(current_block);

	is_field = iterator->belongs_to_program_block;
	create_put_variable_bytecode(iterator, current_block->code, iterator_name->cstr, is_field);

	if (!type_equal(iterator_type, expression1_type))
	{
		string *name1, *name2;
		name1 = type_get_name(iterator_type);
		name2 = type_get_name(expression1_type);

		add_error_message(412, string_get_cstr(name1), string_get_cstr(name2));

		string_destroy(name1);
		string_destroy(name2);
	}

	switch (current_token)
	{
	case KWD_TO:
		{
			direction = 1;
			current_token = yylex();
			break;
		}

	case KWD_DOWNTO:
		{
			direction = -1;
			current_token = yylex();
			break;
		}

	default:
		{
			add_error_message(204, YYTEXT_STRING, "");
			break;
		}
	}

	evaluate_pos = current_block->code->bytecode_pos;
	expression2_type = RD_expression(current_block);

	create_variable_bytecode(iterator, current_block->code, iterator_name->cstr, iterator->belongs_to_program_block);

	check_bytecode_pos = current_block->code->bytecode_pos;
	if (direction == 1)
		bytecode_append(current_block->code, if_icmplt$);
	else
		bytecode_append(current_block->code, if_icmpgt$);

	bytecode_append_short_int(current_block->code, 0);

	if (!type_equal(iterator_type, expression2_type))
	{
		string *name1, *name2;
		name1 = type_get_name(iterator_type);
		name2 = type_get_name(expression2_type);

		add_error_message(412, string_get_cstr(name1), string_get_cstr(name2));

		string_destroy(name1);
		string_destroy(name2);
	}

	if (current_token != KWD_DO)
	{
		add_error_message(203, "do", YYTEXT_STRING);
	}
	else
		current_token = yylex();

	inside_loop ++;
	break_pos1 = current_block->code->bytecode_pos;
	RD_statement(current_block);
	break_pos2 = current_block->code->bytecode_pos;
	inside_loop --;


	// TODO:: moguca optimizacija za neke slucajeve sa iinc$ naredbom
	create_variable_bytecode(iterator, current_block->code, iterator_name->cstr, iterator->belongs_to_program_block);

	if (direction == 1)
		bytecode_append(current_block->code, iconst_1$);
	else
		bytecode_append(current_block->code, iconst_m1$);

	bytecode_append(current_block->code, iadd$);

	create_put_variable_bytecode(iterator, current_block->code, iterator_name->cstr, iterator->belongs_to_program_block);

	bytecode_append(current_block->code, goto$);
	bytecode_append_short_int(current_block->code, evaluate_pos - current_block->code->bytecode_pos + 1);

	current_block->code->bytecode[check_bytecode_pos + 1] = (char) ((current_block->code->bytecode_pos - check_bytecode_pos)>>8);
	current_block->code->bytecode[check_bytecode_pos + 2] = (char) (current_block->code->bytecode_pos - check_bytecode_pos);


	transform_break_stmts(current_block->code, break_pos1, break_pos2, current_block->code->bytecode_pos);

	type_destroy(iterator_type);
	type_destroy(expression1_type);
	type_destroy(expression2_type);
	string_destroy(iterator_name);
}


/*
	Type declaration:

	<RD_type> -> <RD_basic_type>
	           | [packed] array <RD_array_declaration>
			   | record <RD_record_declaration>
			   | file <RD_file_declaration>
			   | set <RD_set_declaration>  currently unsupported !!!
*/
type* RD_type(block *current_block)
{
	switch (current_token)
	{
	case KWD_PACKED:
		{
			add_warning_message(210, "", "");
			current_token = yylex();
			if (current_token != KWD_ARRAY)
				add_error_message(203, "array", YYTEXT_STRING);
			//no_break;
		}

	case KWD_ARRAY:
		{
			current_token = yylex();
			return RD_array_declaration(current_block);
		}

	case KWD_RECORD:
		{
			current_token = yylex();
			return RD_record_declaration(current_block);
		}

	case KWD_FILE:
		{
			current_token = yylex();
			return RD_file_declaration(current_block);
		}

	case KWD_SET:
		{
			type *error;
			error = type_create();
			error->type_class = error_type;
			add_error_message(211, "", "");
			current_token = yylex();
			RD_set_declaration(current_block);
			return error;
		}

	default:
		{
			return RD_basic_type(current_block);
			break;
		}
	}
}


/*
	Basic type:

	<RD_basic_type> -> [IDN .] IDN
	                 | CONST .. CONST
					 | "(" IDN (, IDN)* ")"    enumerated types are not supported
*/
type* RD_basic_type(block *current_block)
{
	type *return_type;
	return_type = type_create();
	return_type->type_class = error_type;

	switch (current_token)
	{
	case IDENTIFIER:
		{
			identifier* name;
			type *declared_type;
			declared_type = type_from_name(current_block, YYTEXT_STRING);

			lowercase(YYTEXT_STRING);

			name = get_identifier(current_block, YYTEXT_STRING);

			if (name->identifier_class == unit_name)
			{
				identifier* type_identifier;

				current_token = yylex();

				if (current_token != DOT)
					add_error_message(200, ".", YYTEXT_STRING);
				else
					current_token = yylex();

				type_identifier = name_table_find_cstr(name->unit_block->names, YYTEXT_STRING);

				if (type_identifier == NULL)
					add_error_message(455, YYTEXT_STRING, "");
				else
					declared_type = type_duplicate(type_identifier->defined_type);
			}

			identifier_destroy(name);

			if (declared_type->type_class != error_type)
			{
				type_destroy(return_type);
				return_type = type_duplicate(declared_type);
				current_token = yylex();
				type_destroy(declared_type);
			}
			else
			{
				identifier *constant_identifier;
				constant_identifier  = get_constant_identifier(current_block, YYTEXT_STRING);

				type_destroy(declared_type);

				if ((constant_identifier->identifier_class == constant_name)
					&& ((constant_identifier->constant_type->type_class == integer_type)
					    || (constant_identifier->constant_type->type_class == char_type)))
				{
					return_type->type_class = interval_type;
					return_type->interval_base_type = constant_identifier->constant_type->type_class;

					if (constant_identifier->constant_type->type_class == integer_type)
						return_type->first_element = constant_identifier->constant_int_value;
					else
						return_type->first_element = constant_identifier->constant_int_value;

					return_type->last_element = return_type->first_element + 1;

					identifier_destroy(constant_identifier);

					current_token = yylex();

					if (current_token != DOTDOT)
					{
						add_error_message(213, YYTEXT_STRING, "");

						/* Error-recovery */
						while ((current_token != CLOSE_SQ_BR)
							&& (current_token != SEMI_COLON)
							&& (current_token != KWD_END)
							&& (current_token != END_OF_INPUT))
						{
							current_token = yylex();
						}

						break;
					}

					current_token = yylex();

					if (current_token == IDENTIFIER)
					{
						constant_identifier = get_constant_identifier(current_block, YYTEXT_STRING);

						if ((constant_identifier->identifier_class != constant_name)
							|| (constant_identifier->constant_type->type_class != return_type->interval_base_type))
						{
							add_error_message(416, "", "");
						}
						else
						{
							if (return_type->interval_base_type == integer_type)
								return_type->last_element = constant_identifier->constant_int_value;
							else
								return_type->last_element = constant_identifier->constant_int_value;
						}

						identifier_destroy(constant_identifier);
					}
					else if (current_token == CST_INTEGER)
					{
						if (return_type->interval_base_type != integer_type)
							add_error_message(416, "", "");

						return_type->last_element = integer_constant;
					}
					else if (current_token == CST_CHAR)
					{
						if (return_type->interval_base_type != char_type)
							add_error_message(416, "", "");

						return_type->last_element = char_constant;
					}
					else
					{
						add_error_message(416, "", "");
					}

					current_token = yylex();

				}
				else
				{
					add_error_message(415, YYTEXT_STRING, "");
					current_token = yylex();
					identifier_destroy(constant_identifier);
				}
			}



			break;
		}

	case CST_INTEGER:
	case CST_CHAR:
		{
			return_type->type_class = interval_type;

			if (current_token == CST_INTEGER)
			{
				return_type->interval_base_type = integer_type;
				return_type->first_element = integer_constant;
			}
			else
			{
				return_type->interval_base_type = char_type;
				return_type->first_element = (long int) char_constant;
			}

			return_type->last_element = return_type->first_element + 1;

			current_token = yylex();

			if (current_token != DOTDOT)
			{
				add_error_message(213, YYTEXT_STRING, "");
			}

			current_token = yylex();

			if ((current_token != IDENTIFIER)
				&& (current_token != CST_INTEGER)
				&& (current_token != CST_CHAR))
			{
				add_error_message(217, "", "");
			}
			else
			{
				if (current_token == IDENTIFIER)
				{
					type *constant_type;
					identifier *constant_identifier;

					constant_type = get_constant_type(current_block, YYTEXT_STRING);

					if (constant_type->type_class != return_type->interval_base_type)
					{
						add_error_message(413, "", "");
					}

					type_destroy(constant_type);

					constant_identifier = get_constant_identifier(current_block, YYTEXT_STRING);

					if (constant_identifier->identifier_class != constant_name)
					{
						add_error_message(414, YYTEXT_STRING, "");
					}
					else
					{
						if (return_type->interval_base_type == integer_type)
							return_type->last_element = constant_identifier->constant_int_value;
						else
							return_type->last_element = constant_identifier->constant_int_value;
					}

					identifier_destroy(constant_identifier);

				}
				else if (current_token == CST_INTEGER)
				{
					return_type->last_element = integer_constant;
				}
				else
				{
					return_type->last_element = (long int) char_constant;
				}
			}

			current_token = yylex();

			break;
		}

	case OPEN_BR:
		{
			int brack_count = 1;
			add_error_message(212, "", "");

			while (brack_count > 0)
			{
				current_token = yylex();

				if (current_token == OPEN_BR)
					brack_count ++;

				if (current_token == CLOSE_BR)
					brack_count --;

				if (current_token == END_OF_INPUT)
					return return_type;
			}

			current_token = yylex();

			break;
		}

	default:
		{
			add_error_message(204, YYTEXT_STRING, "");
			while ((current_token != CLOSE_SQ_BR)
				&& (current_token != SEMI_COLON)
				&& (current_token != END_OF_INPUT)
				&& (current_token != KWD_END))
			{
				current_token = yylex();
			}

			break;
		}
	}

	return return_type;
}


/*
	The declaration of an array.

	<RD_array_declaration> -> "[" (<RD_basic_type> (, <RD_basic_type>)* )+ "]" of <RD_type>
*/
type* RD_array_declaration(block *current_block)
{
	type *new_type;
	type *dimension;

	new_type = type_create();
	new_type->type_class = array_type;

	new_type->dimensions_list = type_list_create();

	if (current_token != OPEN_SQ_BR)
	{
		add_error_message(200, "[", YYTEXT_STRING);
	}

	do
	{
		current_token = yylex();

		dimension = RD_basic_type(current_block);

		if (dimension->type_class != interval_type)
		{
			add_error_message(430, "", "");
		}

		type_list_append(new_type->dimensions_list, dimension);
		type_destroy(dimension);

		if ((current_token != CLOSE_SQ_BR)
			&& (current_token != COMMA))
		{
			add_error_message(200, ",", YYTEXT_STRING);
		}

		if (current_token == END_OF_INPUT)
		{
			add_error_message(207, "", "");
			type_destroy(new_type);
			new_type = type_create();
			new_type->type_class = error_type;
			return new_type;
		}

	} while (current_token != CLOSE_SQ_BR);

	current_token = yylex();

	if (current_token != KWD_OF)
	{
		add_error_message(203, "of", YYTEXT_STRING);
	}

	current_token = yylex();

	new_type->element_type = RD_type(current_block);

	return new_type;
}


/*
	The declaration of a record.

	<RD_record_declaration> -> <RD_identifier_list> ":" <RD_type>
	                             ( <RD_identifier_list> ":" <RD_type> )* end
*/
type* RD_record_declaration(block *current_block)
{
	type *new_type;
	string_list *identifier_list;
	type *element_type;
	int old_linenum, tmp;

	short int first_pass = 1;

	new_type = type_create();
	new_type->type_class = record_type;
	new_type->elements_name_list = string_list_create();
	new_type->elements_type_list = type_list_create();
	new_type->unique_record_ID = next_record_ID;

	next_record_ID ++;

	do
	{
		if (first_pass)
			first_pass = 0;
		else
			current_token = yylex();

		if ((current_token == KWD_END)
			|| (current_token == END_OF_INPUT))
			break;

		identifier_list = RD_identifier_list(current_block, 0);

		if (current_token != COLON)
		{
			add_error_message(200, ":", YYTEXT_STRING);
		}

		old_linenum = linenum;
		current_token = yylex();

		element_type = RD_type(current_block);

		if (element_type->type_class == interval_type)
		{
			add_error_message(440, "", "");
			element_type->type_class = integer_type;
		}

		tmp = linenum;
		linenum = old_linenum;

		type_add_record(new_type, identifier_list, element_type);

		linenum = tmp;

		string_list_destroy(identifier_list);
		type_destroy(element_type);

	} while (current_token == SEMI_COLON);

	if (current_token != KWD_END)
	{
		add_error_message(214, YYTEXT_STRING, "");
	}

	current_token = yylex();

	create_record_class(new_type);

	return new_type;
}


/*
	The declaration of a file

	<RD_file_declaration> -> of IDN
*/
type* RD_file_declaration(block *current_block)
{
	type *file_type;
	file_type = type_create();
	file_type->type_class = error_type;
	if (current_token != KWD_OF)
	{
		add_error_message(203, "of", YYTEXT_STRING);
	}

	current_token = yylex();
// !!!! TODO:: dodati podrsku za fajlove
	current_token = yylex();
	add_error_message(435, "", "");
	return file_type;
}


/*
	Sets are not supported yet, however, this rule is used
	as an error-recovery rule if someone tries to use sets.

	<RD_set_declaration> -> of <RD_basic_type>
*/
void RD_set_declaration(block *current_block)
{
	current_token = yylex();

	RD_basic_type(current_block);
}


/*
	Handle the expressions, the relational operators.

	<RD_expression> -> <RD_sum> [<rel_operator> <RD_sum>]
*/
type* RD_expression(block *current_block)
{
	int old_linenum;
	int old_token;
	type *type1, *type2;

	type1 = RD_sum(current_block);

	if ((current_token == OP_LESS)
		|| (current_token == OP_LESS_EQUAL)
		|| (current_token == OP_GREATER_EQUAL)
		|| (current_token == OP_GREATER)
		|| (current_token == OP_EQUAL)
		|| (current_token == OP_NOT_EQUAL))
	{
		old_token = current_token;
		current_token = yylex();

		old_linenum = linenum;
		type2 = RD_sum(current_block);

		if ((type2->type_class != integer_type)
			&& (type2->type_class != real_type)
			&& (type2->type_class != char_type)
			&& (type2->type_class != string_type)
			&& (type2->type_class != boolean_type)
			&& (type2->type_class != error_type)
			&& (type2->type_class != command_type))
		{
			new_linenum=old_linenum;
			add_error_message(417, "", "");
		}
		else
		{
			int cast;
			cast = type_equal_cast(type1, type2);

			if (cast == 0){
				new_linenum=old_linenum;
				add_error_message(417, "", "");
			}

			/* if operands need to be casted */
			if ((type1->type_class == integer_type)
				|| (type2->type_class == integer_type))
			{
				if (cast != 1)
				{
					usesFloat=1;

					if (cast == 3)
						bytecode_append(current_block->code, swap$);

					if (mathType == 1)
					{
						bytecode_append(current_block->code, invokestatic$);
						bytecode_append_short_int(current_block->code, cp_add_methodref("F", "fI", "(I)I"));
					}
					else
					{
						bytecode_append(current_block->code, new$);
						bytecode_append_short_int(current_block->code, cp_add_class("Real"));
						bytecode_append(current_block->code, dup_x1$);
						bytecode_append(current_block->code, swap$);
						bytecode_append(current_block->code, invokespecial$);
						bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "<init>", "(I)V"));
					}

					if (cast == 3)
						bytecode_append(current_block->code, swap$);
				}

			}
			else
			{
				if ((cast == 2) || (cast == 3))
				{
					int method_index = cp_add_methodref("java/lang/String", "valueOf", "(C)Ljava/lang/String;");

					if (cast == 3)
						bytecode_append(current_block->code, swap$);

					/* string cast */
					bytecode_append(current_block->code, invokestatic$);
					bytecode_append_short_int(current_block->code, method_index);

					if (cast == 3)
						bytecode_append(current_block->code, swap$);
				}
			}

			if (cast == 3) /* return the real type if cast happened */
			{
				type *tmp;
				tmp = type2;
				type2 = type1;
				type1 = tmp;
			}

			/* create the code */
			if ((type1->type_class == integer_type)
				|| (type1->type_class == boolean_type)
				|| (type1->type_class == char_type)
				|| ((type1->type_class == real_type) && (mathType == 1)))
			{
				if (type1->type_class == real_type)
					usesFloat = 1;

				switch (old_token)
				{
				case OP_LESS:
					bytecode_append(current_block->code, if_icmplt$);
					break;
				case OP_LESS_EQUAL:
					bytecode_append(current_block->code, if_icmple$);
					break;
				case OP_GREATER_EQUAL:
					bytecode_append(current_block->code, if_icmpge$);
					break;
				case OP_GREATER:
					bytecode_append(current_block->code, if_icmpgt$);
					break;
				case OP_EQUAL:
					bytecode_append(current_block->code, if_icmpeq$);
					break;
				case OP_NOT_EQUAL:
					bytecode_append(current_block->code, if_icmpne$);
					break;
				}

				/* create the rest of the jumping and setting the whatever code */
				bytecode_append_short_int(current_block->code, 7);
				bytecode_append(current_block->code, iconst_0$);
				bytecode_append(current_block->code, goto$);
				bytecode_append_short_int(current_block->code, 4);
				bytecode_append(current_block->code, iconst_m1$);
			}

			if ((type1->type_class == real_type) && (mathType != 1))
			{
				usesFloat = 1;
				bytecode_append(current_block->code, invokevirtual$);

				switch (old_token)
				{
				case OP_LESS:
					bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "lessThan", "(LReal;)Z"));
					break;
				case OP_LESS_EQUAL:
					bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "lessEqual", "(LReal;)Z"));
					break;
				case OP_GREATER_EQUAL:
					bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "greaterEqual", "(LReal;)Z"));
					break;
				case OP_GREATER:
					bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "greaterThan", "(LReal;)Z"));
					break;
				case OP_EQUAL:
					bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "equalTo", "(LReal;)Z"));
					break;
				case OP_NOT_EQUAL:
					bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "notEqualTo", "(LReal;)Z"));
					break;
				}

				/* create the rest of the jumping and setting the whatever code */
				bytecode_append(current_block->code, ifne$);
				bytecode_append_short_int(current_block->code, 7);
				bytecode_append(current_block->code, iconst_0$);
				bytecode_append(current_block->code, goto$);
				bytecode_append_short_int(current_block->code, 4);
				bytecode_append(current_block->code, iconst_m1$);
			}

			if (type1->type_class == command_type)
			{
				if (old_token == OP_EQUAL)
				{
					bytecode_append(current_block->code, if_acmpeq$);
				}
				else if (old_token == OP_NOT_EQUAL)
				{
					bytecode_append(current_block->code, if_acmpne$);
				}
				else
				{
					new_linenum=old_linenum;
					add_error_message(417, "", "");
				}

				/* create the rest of the jumping and setting the whatever code */
				bytecode_append_short_int(current_block->code, 7);
				bytecode_append(current_block->code, iconst_0$);
				bytecode_append(current_block->code, goto$);
				bytecode_append_short_int(current_block->code, 4);
				bytecode_append(current_block->code, iconst_m1$);
			}

		}

		if (type1->type_class == string_type)
		{
			int method_index = cp_add_methodref("java/lang/String", "compareTo", "(Ljava/lang/String;)I");
			bytecode_append(current_block->code, invokevirtual$);
			bytecode_append_short_int(current_block->code, method_index);

			switch (old_token)
			{
			case OP_LESS:
				bytecode_append(current_block->code, iflt$);
				break;
			case OP_LESS_EQUAL:
				bytecode_append(current_block->code, ifle$);
				break;
			case OP_GREATER_EQUAL:
				bytecode_append(current_block->code, ifge$);
				break;
			case OP_GREATER:
				bytecode_append(current_block->code, ifgt$);
				break;
			case OP_EQUAL:
				bytecode_append(current_block->code, ifeq$);
				break;
			case OP_NOT_EQUAL:
				bytecode_append(current_block->code, ifne$);
				break;
			}

			/* create the rest of the jumping and setting the whatever code */
			bytecode_append_short_int(current_block->code, 7);
			bytecode_append(current_block->code, iconst_0$);
			bytecode_append(current_block->code, goto$);
			bytecode_append_short_int(current_block->code, 4);
			bytecode_append(current_block->code, iconst_m1$);
		}

		type_destroy(type2);
		type_destroy(type1);
		/* the type of comparison is boolean */
		type1 = type_create();
		type1->type_class = boolean_type;
	}

	return type1;
}


/*
	Handle +, -, or, xor operators.

	<RD_sum> -> <RD_mult> (<sum_operator> <RD_mult>)*
*/
type* RD_sum(block *current_block)
{
	int old_linenum;
	int old_token;

	type *type1, *type2;

	type1 = RD_mult(current_block);

	while ((current_token == OP_PLUS)
		|| (current_token == OP_MINUS)
		|| (current_token == OP_OR)
		|| (current_token == OP_XOR))
	{
		old_token = current_token;

		current_token = yylex();

		old_linenum = linenum;

		type2 = RD_mult(current_block);

		if (((type2->type_class != integer_type)
			&& (type2->type_class != real_type)
			&& (type2->type_class != char_type)
			&& (type2->type_class != string_type)
			&& (type2->type_class != boolean_type)
			&& (type2->type_class != error_type))
		|| ((type1->type_class == string_type)
		    && (old_token != OP_PLUS)))
		{
			new_linenum=old_linenum;
			add_error_message(417, "", "");
		}
		else
		{
			int cast;

			/* if first type is char, cast it to string */
			if (type1->type_class == char_type)
			{
				int method_index = cp_add_methodref("java/lang/String", "valueOf", "(C)Ljava/lang/String;");

				bytecode_append(current_block->code, swap$);
				bytecode_append(current_block->code, invokestatic$);
				bytecode_append_short_int(current_block->code, method_index);
				bytecode_append(current_block->code, swap$);

				type1->type_class = string_type;
			}

			cast = type_equal_cast(type1, type2);

			if ((cast == 0) && (type1->type_class != string_type)){
				new_linenum=old_linenum;
				add_error_message(417, "", "");
			}

			/* if operands need to be casted */
			if ((type1->type_class != string_type)
				&& (cast != 1))
			{
				usesFloat=1;

				if (cast == 3)
					bytecode_append(current_block->code, swap$);

				if (mathType == 1)
				{
					bytecode_append(current_block->code, invokestatic$);
					bytecode_append_short_int(current_block->code, cp_add_methodref("F", "fI", "(I)I"));
				}
				else
				{
					bytecode_append(current_block->code, new$);
					bytecode_append_short_int(current_block->code, cp_add_class("Real"));
					bytecode_append(current_block->code, dup_x1$);
					bytecode_append(current_block->code, swap$);
					bytecode_append(current_block->code, invokespecial$);
					bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "<init>", "(I)V"));
				}


				if (cast == 3)
					bytecode_append(current_block->code, swap$);

				if (cast == 3) /* return the real type if cast happened */
				{
					type *tmp;
					tmp = type2;
					type2 = type1;
					type1 = tmp;
				}

			}
		}


		if ((type1->type_class != integer_type)
			&& (type1->type_class != boolean_type)
			&& (type1->type_class != error_type)
			&& ((old_token == OP_OR) || (old_token == OP_XOR)))
		{
			new_linenum=old_linenum;
			add_error_message(417, "", "");
		}

		/* create the code */
		if ((type1->type_class == integer_type)
			|| (type1->type_class == boolean_type)
			|| ((type1->type_class == real_type) && (mathType == 1)))
		{
			if (type1->type_class == real_type)
				usesFloat = 1;

			switch (old_token)
			{
			case OP_PLUS:
				bytecode_append(current_block->code, iadd$);
				break;
			case OP_MINUS:
				bytecode_append(current_block->code, isub$);
				break;
			case OP_OR:
				bytecode_append(current_block->code, ior$);
				break;
			case OP_XOR:
				bytecode_append(current_block->code, ixor$);
				break;
			}
		}

		if ((type1->type_class == real_type) && (mathType != 1))
		{
			usesFloat = 1;
			if (old_token == OP_PLUS)
			{
				bytecode_append(current_block->code, new$);
				bytecode_append_short_int(current_block->code, cp_add_class("Real"));
				bytecode_append(current_block->code, dup_x1$);
				bytecode_append(current_block->code, swap$);
				bytecode_append(current_block->code, invokespecial$);
				bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "<init>", "(LReal;)V"));

				bytecode_append(current_block->code, swap$);

				bytecode_append(current_block->code, new$);
				bytecode_append_short_int(current_block->code, cp_add_class("Real"));
				bytecode_append(current_block->code, dup_x1$);
				bytecode_append(current_block->code, swap$);
				bytecode_append(current_block->code, invokespecial$);
				bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "<init>", "(LReal;)V"));

				bytecode_append(current_block->code, dup_x1$);
				bytecode_append(current_block->code, swap$);

				bytecode_append(current_block->code, invokevirtual$);
				bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "add", "(LReal;)V"));

			}

			if (old_token == OP_MINUS)
			{
				bytecode_append(current_block->code, new$);
				bytecode_append_short_int(current_block->code, cp_add_class("Real"));
				bytecode_append(current_block->code, dup_x1$);
				bytecode_append(current_block->code, swap$);
				bytecode_append(current_block->code, invokespecial$);
				bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "<init>", "(LReal;)V"));

				bytecode_append(current_block->code, swap$);

				bytecode_append(current_block->code, new$);
				bytecode_append_short_int(current_block->code, cp_add_class("Real"));
				bytecode_append(current_block->code, dup_x1$);
				bytecode_append(current_block->code, swap$);
				bytecode_append(current_block->code, invokespecial$);
				bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "<init>", "(LReal;)V"));

				bytecode_append(current_block->code, dup_x1$);
				bytecode_append(current_block->code, swap$);

				bytecode_append(current_block->code, invokevirtual$);
				bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "sub", "(LReal;)V"));

			}
		}

		if ((type1->type_class == string_type)
			&& (old_token == OP_PLUS))
		{
			int class_index;
			int init_index;
			int append_index;
			int append2_index;
			int toString_index;

			if (type2->type_class == real_type)
			{
				usesFloat = 1;

				if (mathType == 1)
				{
					bytecode_append(current_block->code, invokestatic$);
					bytecode_append_short_int(current_block->code, cp_add_methodref("F", "tS", "(I)Ljava/lang/String;"));
				}
				else
				{
					bytecode_append(current_block->code, invokevirtual$);
					bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "toString", "()Ljava/lang/String;"));
				}
			}

			class_index = cp_add_class("java/lang/StringBuffer");
			init_index = cp_add_methodref("java/lang/StringBuffer", "<init>", "()V");
			append_index = cp_add_methodref("java/lang/StringBuffer", "append", "(Ljava/lang/String;)Ljava/lang/StringBuffer;");
			toString_index = cp_add_methodref("java/lang/StringBuffer", "toString", "()Ljava/lang/String;");

			switch(type2->type_class)
			{
			case string_type:
			case real_type:
				append2_index = cp_add_methodref("java/lang/StringBuffer", "append", "(Ljava/lang/String;)Ljava/lang/StringBuffer;");
				break;
			case integer_type:
				append2_index = cp_add_methodref("java/lang/StringBuffer", "append", "(I)Ljava/lang/StringBuffer;");
				break;
			case char_type:
				bytecode_append(current_block->code, i2c$);
				append2_index = cp_add_methodref("java/lang/StringBuffer", "append", "(C)Ljava/lang/StringBuffer;");
				break;
			case boolean_type:
				bytecode_append(current_block->code, i2b$);
				append2_index = cp_add_methodref("java/lang/StringBuffer", "append", "(Z)Ljava/lang/StringBuffer;");
				break;
			default:
				add_error_message(434, "", "");
			}

			bytecode_append(current_block->code, swap$);

			/* create new string buffer */
			bytecode_append(current_block->code, new$);
			bytecode_append_short_int(current_block->code, class_index);
			bytecode_append(current_block->code, dup$);
			bytecode_append(current_block->code, invokespecial$);
			bytecode_append_short_int(current_block->code, init_index);

			/* append the first argument */
			bytecode_append(current_block->code, swap$);
			bytecode_append(current_block->code, invokevirtual$);
			bytecode_append_short_int(current_block->code, append_index);

			/* append the second argument */
			bytecode_append(current_block->code, swap$);

			bytecode_append(current_block->code, invokevirtual$);
			bytecode_append_short_int(current_block->code, append2_index);

			/* convert StringBuffer into the String */
			bytecode_append(current_block->code, invokevirtual$);
			bytecode_append_short_int(current_block->code, toString_index);



			type_destroy(type2);
		}

	}

	return type1;
}


/*
	Handle * / div mod and operators.

	<RD_mult> -> <RD_not> (<mult_operator> <RD_not>)*
*/
type* RD_mult(block *current_block)
{
	int old_linenum;
	int old_token;
	type *type1, *type2;

	type1 = RD_not(current_block);

	while ((current_token == OP_MULT)
		|| (current_token == OP_SLASH)
		|| (current_token == OP_DIV)
		|| (current_token == OP_MOD)
		|| (current_token == OP_AND)
		|| (current_token == OP_SHR)
		|| (current_token == OP_SHL))
	{
		old_token = current_token;

		current_token = yylex();

		old_linenum = linenum;

		type2 = RD_not(current_block);

		if ((type2->type_class != integer_type)
			&& (type2->type_class != real_type)
			&& (type2->type_class != boolean_type)
			&& (type2->type_class != error_type))
		{
			new_linenum=old_linenum;
			add_error_message(417, "", "");
		}
		else
		{
			int cast;
			cast = type_equal_cast(type1, type2);

			if (cast == 0){
				new_linenum=old_linenum;
				add_error_message(417, "", "");
			}

			if (cast != 1)
			{
				/* if operands need to be casted */
				usesFloat=1;

				if (cast == 3)
					bytecode_append(current_block->code, swap$);

				if (mathType == 1)
				{
					bytecode_append(current_block->code, invokestatic$);
					bytecode_append_short_int(current_block->code, cp_add_methodref("F", "fI", "(I)I"));
				}
				else
				{
					bytecode_append(current_block->code, new$);
					bytecode_append_short_int(current_block->code, cp_add_class("Real"));
					bytecode_append(current_block->code, dup_x1$);
					bytecode_append(current_block->code, swap$);
					bytecode_append(current_block->code, invokespecial$);
					bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "<init>", "(I)V"));
				}

				if (cast == 3)
					bytecode_append(current_block->code, swap$);

				if (cast == 3) /* return the real type if cast happened */
				{
					type *tmp;
					tmp = type2;
					type2 = type1;
					type1 = tmp;
				}
			}
		}

		type_destroy(type2);

		if ((type1->type_class == real_type)
			&& (old_token != OP_MULT)
			&& (old_token != OP_SLASH))
		{
			new_linenum=old_linenum;
			add_error_message(417, "", "");
		}

		if ((type1->type_class == boolean_type)
			&& (old_token != OP_AND))
		{
			new_linenum=old_linenum;
			add_error_message(417, "", "");
		}

		/* generate the code */
		if (type1->type_class == integer_type)
		{
			switch(old_token)
			{
			case OP_MULT:
				bytecode_append(current_block->code, imul$);
				break;
			case OP_SLASH:
			case OP_DIV:
				bytecode_append(current_block->code, idiv$);
				break;
			case OP_MOD:
				bytecode_append(current_block->code, irem$);
				break;
			case OP_AND:
				bytecode_append(current_block->code, iand$);
				break;
			///////////
			case OP_SHR:
				bytecode_append(current_block->code, ishr$);
				break;
			case OP_SHL:
				bytecode_append(current_block->code, ishl$);
				break;
			}
		}

		if (type1->type_class == real_type)
		{
			usesFloat = 1;
			switch(old_token)
			{
			case OP_MULT:
				{
					if (mathType == 1)
					{
						bytecode_append(current_block->code, invokestatic$);
						bytecode_append_short_int(current_block->code,
							cp_add_methodref("F", "M", "(II)I"));
					}
					else
					{
						bytecode_append(current_block->code, new$);
						bytecode_append_short_int(current_block->code, cp_add_class("Real"));
						bytecode_append(current_block->code, dup_x1$);
						bytecode_append(current_block->code, swap$);
						bytecode_append(current_block->code, invokespecial$);
						bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "<init>", "(LReal;)V"));

						bytecode_append(current_block->code, swap$);

						bytecode_append(current_block->code, new$);
						bytecode_append_short_int(current_block->code, cp_add_class("Real"));
						bytecode_append(current_block->code, dup_x1$);
						bytecode_append(current_block->code, swap$);
						bytecode_append(current_block->code, invokespecial$); ///
						bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "<init>", "(LReal;)V"));

						bytecode_append(current_block->code, dup_x1$);
						bytecode_append(current_block->code, swap$);

						bytecode_append(current_block->code, invokevirtual$);
						bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "mul", "(LReal;)V"));
					}
				}
				break;
			case OP_SLASH:
				{
					if (mathType == 1)
					{
						bytecode_append(current_block->code, invokestatic$);
						bytecode_append_short_int(current_block->code,
							cp_add_methodref("F", "D", "(II)I"));
					}
					else
					{
						bytecode_append(current_block->code, new$);
						bytecode_append_short_int(current_block->code, cp_add_class("Real"));
						bytecode_append(current_block->code, dup_x1$);
						bytecode_append(current_block->code, swap$);
						bytecode_append(current_block->code, invokespecial$);
						bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "<init>", "(LReal;)V"));

						bytecode_append(current_block->code, swap$);

						bytecode_append(current_block->code, new$);
						bytecode_append_short_int(current_block->code, cp_add_class("Real"));
						bytecode_append(current_block->code, dup_x1$);
						bytecode_append(current_block->code, swap$);
						bytecode_append(current_block->code, invokespecial$);
						bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "<init>", "(LReal;)V"));

						bytecode_append(current_block->code, dup_x1$);
						bytecode_append(current_block->code, swap$);

						bytecode_append(current_block->code, invokevirtual$);
						bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "div", "(LReal;)V"));
					}
				}
				break;
			}
		}

		if ((type1->type_class == boolean_type)
			&& (old_token == OP_AND))
		{
			bytecode_append(current_block->code, iand$);
		}
	}

	return type1;
}


/*
	The not operator.

	<RD_not> -> [not] <RD_neg>
*/
type* RD_not(block *current_block)
{
	type *type1;
	int is_not = 0;
	int old_linenum;

	if (current_token == OP_NOT)
	{
		current_token = yylex();
		is_not = 1;
	}

	old_linenum = linenum;

	type1 = RD_neg(current_block);

	/* generate the code */
	if (is_not)
	{
		bytecode_append(current_block->code, iconst_m1$);
		bytecode_append(current_block->code, ixor$);
	}

	if ((is_not)
		&& (type1->type_class != integer_type)
		&& (type1->type_class != boolean_type)
		&& (type1->type_class != error_type))
	{
		new_linenum=old_linenum;
		add_error_message(417, "", "");
	}

	return type1;
}


/*
	The - sign operator.

	<RD_neg> -> ["-"] <RD_value>
*/
type* RD_neg(block *current_block)
{
	type *type1;
	int is_neg = 0;
	int old_linenum;

	if (current_token == OP_MINUS)
	{
		current_token = yylex();
		is_neg = 1;
	}

	old_linenum = linenum;

	type1 = RD_value(current_block);

	/* generate the code */
	if (is_neg)
	{
		if ((type1->type_class == integer_type)
			|| ((type1->type_class == real_type) && (mathType == 1)))
			bytecode_append(current_block->code, ineg$);

		if ((type1->type_class == real_type) && (mathType != 1))
		{
			usesFloat = 1;

			bytecode_append(current_block->code, new$);
			bytecode_append_short_int(current_block->code, cp_add_class("Real"));
			bytecode_append(current_block->code, dup_x1$);
			bytecode_append(current_block->code, swap$);
			bytecode_append(current_block->code, invokespecial$);
			bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "<init>", "(LReal;)V"));
			bytecode_append(current_block->code, dup$);
			bytecode_append(current_block->code, invokevirtual$);
			bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "neg", "()V"));
		}
	}

	if ((type1->type_class != real_type)
		&& (type1->type_class != integer_type)
		&& (type1->type_class != error_type)
		&& (is_neg))
	{
		new_linenum=old_linenum;
		add_error_message(417, "", "");
	}

	return type1;
}


/*
	The expression that calls a procedure or
	assigns a value to a variable.

	<RD_assignment_or_procedure_call>

		-> IDN ["[" <RD_expression_list> "]" ] (. IDN ["[" <RD_expression_list> "]" ])* := <RD_expression>
		-> IDN ["(" <RD_expression_list> ")"]

*/
void RD_assignment_or_procedure_call(block *current_block)
{
	identifier *name;
	string *element_name;
	char *identifier_text;

	name = get_identifier(current_block, YYTEXT_STRING);
	element_name = string_from_cstr(YYTEXT_STRING);

	procedure_linenum = linenum;

	if (current_token != IDENTIFIER)
	{
		add_error_message(202, "", "");

		/* Error-recovery: find the first ";", end, "." or <<EOF>> */
		while ((current_token != SEMI_COLON)
			&& (current_token != DOT)
			&& (current_token != KWD_END)
			&& (current_token != END_OF_INPUT))
		{
			current_token = yylex();
		}

		string_destroy(element_name);
		return;
	}


	if ((name->identifier_class == none)
		|| (name->identifier_class == program_name)
		|| (name->identifier_class == constant_name)
		|| (name->identifier_class == type_name))
	{

		add_error_message(428, YYTEXT_STRING, "");
		current_token = yylex();
		identifier_destroy(name);
		string_destroy(element_name);
		return;
	}

	current_token = yylex();

	if (name->identifier_class == unit_name)
	{
		identifier *unit_name;
		type_list *params;
		if (current_token != DOT)
		{
			add_error_message(200, ".", YYTEXT_STRING);
			while (current_token != SEMI_COLON)
				current_token = yylex();
			return;
		}

		current_token = yylex();
		unit_name = name_table_find(name->unit_block->names, string_from_cstr(YYTEXT_STRING));
		identifier_text = malloc(strlen(YYTEXT_STRING) + 1);
		strcpy(identifier_text, YYTEXT_STRING);
		string_destroy(element_name);
		element_name = string_from_cstr(YYTEXT_STRING);

		if (unit_name == NULL)
		{
			add_error_message(453, YYTEXT_STRING, "");
			add_error_message(200, ".", YYTEXT_STRING);
			while (current_token != SEMI_COLON)
				current_token = yylex();
			return;
		}

		name = identifier_duplicate(unit_name);
		current_token = yylex();

		if (name->identifier_class == procedure_name)
			goto procedure_call;
		if (name->identifier_class == variable_name)
			goto variable_lvalue;

		add_error_message(457, YYTEXT_STRING, "");
	}


	/* the name belongs to a procedure */
	if (name->identifier_class == procedure_name)
	{
procedure_call:
		if (name->standard_function)
		{
			create_std_function_prefix(current_block->code, element_name->cstr);
		}

		if (type_list_length(name->parameters) == 0)
		{
			int br_depth = 0;
			if (current_token == OPEN_BR)
			{
				add_error_message(419, "", "");
				br_depth = 1;
				while (br_depth > 0)
				{
					if ((current_token == END_OF_INPUT)
						|| (current_token == KWD_END))
					{
						identifier_destroy(name);
						string_destroy(element_name);
						return;
					}

					current_token = yylex();

					if (current_token == OPEN_BR)
						br_depth ++;

					if (current_token == CLOSE_BR)
						br_depth --;
				}

				current_token = yylex();
			}
		}
		else
		{
			if (current_token != OPEN_BR)
			{
				add_error_message(420, "", "");
			}
			else
			{
				int compare_result;
				int old_linenum;
				type_list *parameter_list;
				current_token = yylex();

				old_linenum = linenum;

				parameter_list = RD_expression_list_cast(current_block, name->parameters);

				compare_result = type_list_different_parameter_cast(parameter_list, name->parameters);

				if (compare_result == -1) {
					new_linenum=old_linenum;
					add_error_message(421, "", "");
				}

				if (compare_result > 0)
				{
					char par_num[4];
					sprintf(par_num, "%d", compare_result);
					new_linenum=old_linenum;
					add_error_message(422, par_num, "");
				}

				type_list_destroy(parameter_list);

				if (current_token != CLOSE_BR)
				{
					add_error_message(200, ")", YYTEXT_STRING);
				}
				else
					current_token = yylex();
			}
		}

		/* create the bytecode */
		if (!name->standard_function)
		{
			/* the procedure is user-defined */
			int methodref_index;
			identifier *block_identifier;
			type_list *it;
			int pos;

			char* descriptor = (char*) mem_alloc(5*1024);
			if (descriptor == NULL)
				die(1);
			descriptor[0] = '(';

			if (!name->unit_function)
			{
				block_identifier = name_table_find(current_block->names, element_name);

				if (block_identifier == NULL)
					block_identifier = name_table_find(current_block->parent_block->names, element_name);
			}
			else
			{
				block_identifier = name;
			}

			it = block_identifier->parameters;
			pos = 1;

			while(it != NULL)
			{
				if (it->data == NULL)
					break;

				get_field_descriptor(it->data, descriptor + pos);
				pos = strlen(descriptor);
				it = it->next;
			}

			descriptor[pos] = ')';
			pos ++;
			descriptor[pos] = '\0';

			if (block_identifier->identifier_class == procedure_name)
				strcat(descriptor, "V");
			else
				get_field_descriptor(block_identifier->return_type, descriptor + pos);


			lowercase(element_name->cstr);
			if (name->unit_function)
			{
				string *full_unit_name;

				if (name->container_unit->is_library)
					full_unit_name = string_from_cstr("Lib_");
				else
					full_unit_name = string_create();
				string_append(full_unit_name, name->container_unit->unit_name);
				methodref_index = cp_add_methodref(full_unit_name->cstr, element_name->cstr, descriptor);
				string_destroy(full_unit_name);
			}
			else
			{
				if (compiling_unit == 0)
					methodref_index = cp_add_methodref("M", element_name->cstr, descriptor);
				else
					methodref_index = cp_add_methodref(string_get_cstr(str_program_name), element_name->cstr, descriptor);
			}
			bytecode_append(current_block->code, invokestatic$); /* call */
			bytecode_append_short_int(current_block->code, methodref_index);

			mem_free(descriptor);
		}
		else
		{
			/* handle standard functions and procedures */
			create_std_function_code(current_block->code, element_name->cstr);
		}
	}

	/* if the identifier is the function name and that is not the name of the current
	block, report an error */
	if (name->identifier_class == function_name)
	{
		if (STRING_COMPARE(element_name->cstr, current_block->block_name->cstr) != 0)
		{
			add_error_message(432, element_name->cstr, "");
		}
	}

	/* if an identifier is a valid l-value */
	if ((name->identifier_class == variable_name)
		|| (name->identifier_class == parameter_name)
		|| (name->identifier_class == function_name)) /* function names are treated as variables
													     for return values */
	{
		type *current_type;
		type *new_type;
		type *expression_type;
		int assign_linenum;
		int is_field = 0;
		bytecode *get_bytecode;
		bytecode *put_bytecode;
variable_lvalue:
		is_field = 0;
		get_bytecode = bytecode_create();
		put_bytecode = bytecode_create();

		if (name->identifier_class == variable_name)
			current_type = type_duplicate(name->variable_type);

		if (name->identifier_class == parameter_name)
			current_type = type_duplicate(name->parameter_type);

		if (name->identifier_class == function_name)
			current_type = type_duplicate(name->return_type);

		/* create the get and put bytecodes */
		is_field = name->belongs_to_program_block;

		if (name->identifier_class == function_name)
			is_field = 0;

		create_variable_bytecode(name, get_bytecode, element_name->cstr, is_field);
		create_put_variable_bytecode(name, put_bytecode, element_name->cstr, is_field);

		identifier_destroy(name);
		name = NULL;
		string_destroy(element_name);
		element_name = NULL;

		while (current_token != OP_ASSIGN)
		{
			if ((current_token == SEMI_COLON)
				|| (current_token == KWD_END)
				|| (current_token == END_OF_INPUT))
			{
				add_error_message(218, "", "");
				type_destroy(current_type);
				bytecode_destroy(get_bytecode);
				bytecode_destroy(put_bytecode);
				return;
			}

			if (current_token == DOT)
			{
				int fieldref_index;
				char record_name[15];
				char field_descriptor[128];

				current_token = yylex();

				/* append the old get bytecode */
				bytecode_append_bytecode(current_block->code, get_bytecode);

				/* reset the bytecodes */
				put_bytecode->bytecode_pos = 0;
				get_bytecode->bytecode_pos = 0;

				if (current_type->type_class != record_type)
				{
					add_error_message(424, "", "");

					/* Error-recovery */
					while ((current_token != SEMI_COLON)
						&& (current_token != KWD_END)
						&& (current_token != END_OF_INPUT))
					{
						current_token = yylex();
					}

					type_destroy(current_type);
					bytecode_destroy(get_bytecode);
					bytecode_destroy(put_bytecode);
					return;
				}

				new_type = type_find_record_element(current_type, YYTEXT_STRING);

				if (new_type == NULL)
				{
					add_error_message(447, YYTEXT_STRING, "");
				}
				else
				{
					/* create the new put and get bytecodes */
					sprintf(record_name, "R_%d", current_type->unique_record_ID);
					get_field_descriptor(new_type, field_descriptor);
					lowercase(YYTEXT_STRING);
					fieldref_index = cp_add_fieldref(record_name, YYTEXT_STRING, field_descriptor);

					bytecode_append(get_bytecode, getfield$);
					bytecode_append_short_int(get_bytecode, fieldref_index);

					bytecode_append(put_bytecode, putfield$);
					bytecode_append_short_int(put_bytecode, fieldref_index);


					type_destroy(current_type);
					current_type = new_type;
				}


				if (current_type == NULL)
				{
					add_error_message(425, YYTEXT_STRING, "");

					/* Error-recovery */
					while ((current_token != SEMI_COLON)
						&& (current_token != KWD_END)
						&& (current_token != END_OF_INPUT))
					{
						current_token = yylex();
					}

					type_destroy(current_type);
					bytecode_destroy(get_bytecode);
					bytecode_destroy(put_bytecode);
					return;
				}

				current_token = yylex();
			}
			else if (current_token == OPEN_SQ_BR)
			{
				type_list *array_elements;
				bytecode *tmp_code;
				tmp_code = bytecode_create();

				// ako je [, a zadnji je array, procitaj listu, usporedi, vrati slijedeci, skoci gore
				if (current_type->type_class != array_type)
				{
					add_error_message(426, "", "");

					/* Error-recovery */
					while ((current_token != SEMI_COLON)
						&& (current_token != KWD_END)
						&& (current_token != END_OF_INPUT))
					{
						current_token = yylex();
					}

					type_destroy(current_type);
					bytecode_destroy(get_bytecode);
					bytecode_destroy(put_bytecode);
					return;
				}

				current_token = yylex();

				array_elements = RD_expression_list_array(current_block, tmp_code, current_type);

				bytecode_append_bytecode(get_bytecode, tmp_code);
				//bytecode_append_bytecode(put_bytecode, tmp_code);
				bytecode_destroy(put_bytecode);
				put_bytecode = bytecode_duplicate(get_bytecode);

				replace_aaload_instruction(get_bytecode, current_type->element_type);

				/* remove the last aaload from put bytecode, adjust the stack and create the
				 put bytecode */
				put_bytecode->bytecode_pos --;
				bytecode_append(put_bytecode, dup2_x1$);
				bytecode_append(put_bytecode, pop2$);

				switch(current_type->element_type->type_class)
				{
				case integer_type:
				case boolean_type:
				case char_type:
					bytecode_append(put_bytecode, iastore$);
					break;

				case real_type:
					if (mathType == 1)
						bytecode_append(put_bytecode, iastore$);
					else
						bytecode_append(put_bytecode, aastore$);
					break;


				case image_type:
				case string_type:
				case record_type:
				case command_type:
				case record_store_type:
				case http_type:
				case alert_type:
				case stream_type:
					bytecode_append(put_bytecode, aastore$);
					break;

				}

				bytecode_destroy(tmp_code);

				if (type_list_different_parameter_array(array_elements, current_type->dimensions_list) != 0)
				{
					char num1[6], num2[6];

					sprintf(num1, "%d", type_list_length(current_type->dimensions_list));
					sprintf(num2, "%d", type_list_length(array_elements));
					add_error_message(427, num1, num2);

					/* Error-recovery */
					while ((current_token != SEMI_COLON)
						&& (current_token != KWD_END)
						&& (current_token != END_OF_INPUT))
					{
						current_token = yylex();
					}

					type_destroy(current_type);
					bytecode_destroy(get_bytecode);
					bytecode_destroy(put_bytecode);
					return;
				}

				if (current_token != CLOSE_SQ_BR)
				{
					add_error_message(200, "]", YYTEXT_STRING);

					/* Error-recovery */
					while ((current_token != SEMI_COLON)
						&& (current_token != KWD_END)
						&& (current_token != END_OF_INPUT))
					{
						current_token = yylex();
					}

					type_destroy(current_type);
					bytecode_destroy(get_bytecode);
					bytecode_destroy(put_bytecode);
					return;
				}

				current_token = yylex();

				new_type = type_duplicate(current_type->element_type);
				type_destroy(current_type);

				current_type = new_type;

				type_list_destroy(array_elements);
			}
			else
			{
				add_error_message(204, YYTEXT_STRING, "");

				/* Error-recovery */
				while ((current_token != SEMI_COLON)
					&& (current_token != KWD_END)
					&& (current_token != END_OF_INPUT))
				{
					current_token = yylex();
				}
				bytecode_destroy(get_bytecode);
				bytecode_destroy(put_bytecode);
				return;
			}
		}

		assign_linenum = linenum;

		current_token = yylex();

		expression_type = RD_expression(current_block);

		if ((type_equal_cast(current_type, expression_type) != 1)
			&& (current_type->type_class == real_type))
		{
			usesFloat = 1;

			if (mathType == 1)
			{
				bytecode_append(current_block->code, invokestatic$);
				bytecode_append_short_int(current_block->code, cp_add_methodref("F", "fI", "(I)I"));
			}
			else
			{
				bytecode_append(current_block->code, new$);
				bytecode_append_short_int(current_block->code, cp_add_class("Real"));
				bytecode_append(current_block->code, dup_x1$);
				bytecode_append(current_block->code, swap$);
				bytecode_append(current_block->code, invokespecial$);
				bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "<init>", "(I)V"));
			}
		}

		if ((type_equal_cast(current_type, expression_type) == 2)
			&& (current_type->type_class == string_type))
		{
			int method_index = cp_add_methodref("java/lang/String", "valueOf", "(C)Ljava/lang/String;");

			/* string cast */
			bytecode_append(current_block->code, invokestatic$);
			bytecode_append_short_int(current_block->code, method_index);
		}

		bytecode_append_bytecode(current_block->code, put_bytecode);

		if ((type_equal_cast(current_type, expression_type) != 1)
			&& (type_equal_cast(current_type, expression_type) != 2))
		{
			new_linenum=assign_linenum;
			add_error_message(423, "", "");
		}
		else
			if (current_type->type_class == array_type)
			{
				new_linenum=assign_linenum;
				add_error_message(443, "", "");
			}



		type_destroy(expression_type);
		type_destroy(current_type);
		bytecode_destroy(get_bytecode);
		bytecode_destroy(put_bytecode);
	}

	if (name != NULL)
		identifier_destroy(name);

	if (element_name != NULL)
		string_destroy(element_name);
}


/*
	The list of expressions, used when calling the function
	or procedure.

	<RD_expression_list> -> <RD_expression> (, <RD_expression>)*
*/
type_list* RD_expression_list(block *current_block)
{
	type_list *return_list;
	type *expression_type;

	return_list = type_list_create();

	expression_type = RD_expression(current_block);

	type_list_append(return_list, expression_type);

	type_destroy(expression_type);

	while (current_token == COMMA)
	{
		current_token = yylex();

		expression_type = RD_expression(current_block);

		type_list_append(return_list, expression_type);

		type_destroy(expression_type);
	}

	return return_list;
}

/*
	Same as the above, only the types are casted according to the second parameter
*/
type_list* RD_expression_list_cast(block *current_block, type_list *formal_params)
{
	type_list *return_list;
	type *expression_type;
	type_list *it;

	it = formal_params;

	return_list = type_list_create();

	expression_type = RD_expression(current_block);

	if ((it != NULL) && (it->data != NULL))
	{
		int cast;

		cast = type_equal_cast(it->data, expression_type);

		if ((cast == 2) && (it->data->type_class == string_type))
		{
			int method_index = cp_add_methodref("java/lang/String", "valueOf", "(C)Ljava/lang/String;");

			bytecode_append(current_block->code, invokestatic$);
			bytecode_append_short_int(current_block->code, method_index);
		}

		if ((cast == 2) && (it->data->type_class == real_type))
		{
			usesFloat = 1;

			if (mathType == 1)
			{
				bytecode_append(current_block->code, invokestatic$);
				bytecode_append_short_int(current_block->code, cp_add_methodref("F", "fI", "(I)I"));
			}
			else
			{
				bytecode_append(current_block->code, new$);
				bytecode_append_short_int(current_block->code, cp_add_class("Real"));
				bytecode_append(current_block->code, dup_x1$);
				bytecode_append(current_block->code, swap$);
				bytecode_append(current_block->code, invokespecial$);
				bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "<init>", "(I)V"));
			}
		}

		it = it->next;
	}

	type_list_append(return_list, expression_type);

	type_destroy(expression_type);

	while (current_token == COMMA)
	{
		current_token = yylex();

		expression_type = RD_expression(current_block);

		if ((it != NULL) && (it->data != NULL))
		{
			int cast;

			cast = type_equal_cast(it->data, expression_type);

			if ((cast == 2) && (it->data->type_class == string_type))
			{
				int method_index = cp_add_methodref("java/lang/String", "valueOf", "(C)Ljava/lang/String;");

				bytecode_append(current_block->code, invokestatic$);
				bytecode_append_short_int(current_block->code, method_index);
			}

			if ((cast == 2) && (it->data->type_class == real_type))
			{
				usesFloat = 1;

				if (mathType == 1)
				{
					bytecode_append(current_block->code, invokestatic$);
					bytecode_append_short_int(current_block->code, cp_add_methodref("F", "fI", "(I)I"));
				}
				else
				{
					bytecode_append(current_block->code, new$);
					bytecode_append_short_int(current_block->code, cp_add_class("Real"));
					bytecode_append(current_block->code, dup_x1$);
					bytecode_append(current_block->code, swap$);
					bytecode_append(current_block->code, invokespecial$);
					bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "<init>", "(I)V"));
				}
			}

			it = it->next;
		}

		type_list_append(return_list, expression_type);

		type_destroy(expression_type);
	}

	return return_list;
}

/*
	Same as RD_expression_list, only aaload is generated after each element and the indices
	are adjusted.For example, if array is [a..b], the a value must be substracted from the index
*/
type_list* RD_expression_list_array(block *current_block, bytecode *code, type *array_type)
{
	type_list *return_list;
	type *expression_type;
	bytecode *oldBytecode;

	type_list *it;
	it = array_type->dimensions_list;

	oldBytecode = current_block->code;
	current_block->code = code;

	return_list = type_list_create();

	expression_type = RD_expression(current_block);
	adjust_indices(code, it->data);

	if(it != NULL)
		it = it->next;

	bytecode_append(code, aaload$);

	type_list_append(return_list, expression_type);

	type_destroy(expression_type);

	while (current_token == COMMA)
	{
		current_token = yylex();

		expression_type = RD_expression(current_block);
		adjust_indices(code, it->data);

		if(it != NULL)
			it = it->next;
		bytecode_append(code, aaload$);

		type_list_append(return_list, expression_type);

		type_destroy(expression_type);
	}

	current_block->code = oldBytecode;

	return return_list;
}


/*
	The with statement, currently not supported. This
	function is used as an error recovery function.

	<RD_with_statement> -> IDN (. IDN)* do <RD_statement>
*/
void RD_with_statement(block *current_block)
{
	/* simplified parsing */
	while (current_token != KWD_DO)
	{
		if (current_token == END_OF_INPUT)
			return;

		current_token = yylex();
	}

	current_token = yylex();

	RD_statement(current_block);
}


/*
	Anything that has value: constant, identifier, identifier inside
	a record or a function call.

	<RD_value> -> CONST
	            | IDN [ "[" <RD_expression_list> "]" ] ( . IDN [ "[" <RD_expression_list> "]" ])*
				| IDN [ "(" <RD_expression_list> ")"]
				| "(" <RD_expression> ")"
*/
type* RD_value(block *current_block)
{
	type *return_type;
	return_type = type_create();

	return_type->type_class = error_type;

	if (current_token == CST_INTEGER)
	{
		switch(integer_constant)
		{
		case -1:
			bytecode_append(current_block->code, iconst_m1$);
			break;
		case 0:
			bytecode_append(current_block->code, iconst_0$);
			break;
		case 1:
			bytecode_append(current_block->code, iconst_1$);
			break;
		case 2:
			bytecode_append(current_block->code, iconst_2$);
			break;
		case 3:
			bytecode_append(current_block->code, iconst_3$);
			break;
		case 4:
			bytecode_append(current_block->code, iconst_4$);
			break;
		case 5:
			bytecode_append(current_block->code, iconst_5$);
			break;
		default:
			{
				if (abs(integer_constant) < 127)
				{
					bytecode_append(current_block->code, bipush$);
					bytecode_append(current_block->code, (char)(integer_constant));
				}
				else if (abs(integer_constant) < 16000)
				{
					bytecode_append(current_block->code, sipush$);
					bytecode_append_short_int(current_block->code, (short)(integer_constant));
				}
				else
				{
					int cp_index;
					cp_index = cp_add_integer(integer_constant);
					if (cp_index <= 255)
					{
						bytecode_append(current_block->code, ldc$);
						bytecode_append(current_block->code, (char) cp_index);
					}
					else
					{
						bytecode_append(current_block->code, ldc_w$);
						bytecode_append_short_int(current_block->code, (short) cp_index);
					}
				}
				break;
			}
		}

		current_token = yylex();
		return_type->type_class = integer_type;
		return return_type;
	}

	if (current_token == CST_REAL)
	{
		float cst;
		int csti; /* the integer part */
		int cstf; /* the fraction part */

		/* calculate the exponent and the integer part */
		cst = real_constant;

		csti = (int)cst;
		cstf = (cst - (int)cst)*100000;

		usesFloat = 1;

		if (mathType == 1)
		{
			/* put the integer part on the stack */
			if (abs(csti) < 127)
			{
				bytecode_append(current_block->code, bipush$);
				bytecode_append(current_block->code, csti);
			}
			else if (abs(csti) < 15000)
			{
				bytecode_append(current_block->code, sipush$);
				bytecode_append_short_int(current_block->code, csti);
			}
			else
			{
				bytecode_append(current_block->code, ldc_w$);
				bytecode_append_short_int(current_block->code, cp_add_integer(csti));
			}

			/* put the fraction part to the stack */
			if (abs(cstf) < 127)
			{
				bytecode_append(current_block->code, bipush$);
				bytecode_append(current_block->code, cstf);
			}
			else if (abs(cstf) < 15000)
			{
				bytecode_append(current_block->code, sipush$);
				bytecode_append_short_int(current_block->code, cstf);
			}
			else
			{
				bytecode_append(current_block->code, ldc_w$);
				bytecode_append_short_int(current_block->code, cp_add_integer(cstf));
			}

			bytecode_append(current_block->code, invokestatic$);
			bytecode_append_short_int(current_block->code, cp_add_methodref("F", "fI", "(II)I"));
		}
		else
		{
			char number[64];

			bytecode_append(current_block->code, new$);
			bytecode_append_short_int(current_block->code, cp_add_class("Real"));
			bytecode_append(current_block->code, dup$);

			sprintf(number, "%f", real_constant);
			bytecode_append(current_block->code, ldc_w$);
			bytecode_append_short_int(current_block->code, cp_add_string(number));

			bytecode_append(current_block->code, invokespecial$);
			bytecode_append_short_int(current_block->code, cp_add_methodref("Real", "<init>", "(Ljava/lang/String;)V"));
		}

		current_token = yylex();
		return_type->type_class = real_type;
		return return_type;
	}

	if (current_token == CST_BOOLEAN)
	{
		if (boolean_constant)
		{
			bytecode_append(current_block->code, iconst_m1$);
		}
		else
		{
			bytecode_append(current_block->code, iconst_0$);
		}

		current_token = yylex();
		return_type->type_class = boolean_type;
		return return_type;
	}

	if (current_token == CST_CHAR)
	{
		bytecode_append(current_block->code, bipush$);
		bytecode_append(current_block->code, char_constant);

		current_token = yylex();
		return_type->type_class = char_type;
		return return_type;
	}

	if (current_token == CST_STRING)
	{
		int cp_index;
		cp_index = cp_add_string(string_constant->cstr);
		if (cp_index <= 255)
		{
			bytecode_append(current_block->code, ldc$);
			bytecode_append(current_block->code, (char) cp_index);
		}
		else
		{
			bytecode_append(current_block->code, ldc_w$);
			bytecode_append_short_int(current_block->code, (short) cp_index);
		}

		current_token = yylex();
		return_type->type_class = string_type;
		return return_type;
	}

	if (current_token == IDENTIFIER)
	{
		identifier *name;
		char *identifier_text;
		identifier_text = (char*) mem_alloc(strlen(YYTEXT_STRING) + 1);
		strcpy(identifier_text, YYTEXT_STRING);
		lowercase(identifier_text);

		name = get_identifier(current_block, identifier_text);

		if ((name->identifier_class == none)
			|| (name->identifier_class == program_name)
			|| (name->identifier_class == type_name)
			|| (name->identifier_class == procedure_name))
		{
			add_error_message(429, YYTEXT_STRING, "");
			current_token = yylex();
			identifier_destroy(name);
			mem_free(identifier_text);
			return return_type;
		}


		current_token = yylex();

		if (name->identifier_class == constant_name)
		{
constant_value:
			create_constant_bytecode(name, current_block->code);
			type_destroy(return_type);
			return_type = type_duplicate(name->constant_type);
			identifier_destroy(name);
			mem_free(identifier_text);
			return return_type;
		}

		/*
			Parse the unit functions.
		*/
		if (name->identifier_class == unit_name)
		{
			identifier *unit_name;

			if (current_token != DOT)
			{
				add_error_message(200, ".", YYTEXT_STRING);
				while (current_token != SEMI_COLON)
					current_token = yylex();
				return return_type;
			}

			current_token = yylex();
			unit_name = name_table_find(name->unit_block->names, string_from_cstr(YYTEXT_STRING));
			identifier_text = malloc(strlen(YYTEXT_STRING) + 1);
			strcpy(identifier_text, YYTEXT_STRING);

			if (unit_name == NULL)
			{
				add_error_message(453, YYTEXT_STRING, "");
				add_error_message(200, ".", YYTEXT_STRING);
				while (current_token != SEMI_COLON)
					current_token = yylex();
				return return_type;
			}

			name = identifier_duplicate(unit_name);
			current_token = yylex();

			if (name->identifier_class == function_name)
				goto function_call;

			if (name->identifier_class == constant_name)
				goto constant_value;

			if (name->identifier_class == variable_name)
				goto variable_value;

			add_error_message(458, YYTEXT_STRING, "");
		}

		if ((name->identifier_class == variable_name)
			|| (name->identifier_class == parameter_name))
		{
			type *current_type;
			type *new_type;
			int is_field;
variable_value:
			is_field = name->belongs_to_program_block;

			if (name->identifier_class == variable_name)
			{
				create_variable_bytecode(name, current_block->code, identifier_text, is_field);
				current_type = type_duplicate(name->variable_type);
			}


			if(name->identifier_class == parameter_name)
			{
				create_variable_bytecode(name, current_block->code, identifier_text, is_field);
				current_type = type_duplicate(name->parameter_type);
			}

			mem_free(identifier_text);

			identifier_destroy(name);

			name = NULL;

			while ((current_token == DOT)
				|| (current_token == OPEN_SQ_BR))
			{
				/* get an element from the record */
				if (current_token == DOT)
				{
					char record_name[5];
					char field_descriptor[128];
					int fieldref_index;

					if (current_type->type_class != record_type)
					{
						add_error_message(424, "", "");

						/* Error-recovery */
						while ((current_token != SEMI_COLON)
							&& (current_token != KWD_END)
							&& (current_token != END_OF_INPUT))
						{
							current_token = yylex();
						}

						type_destroy(current_type);
						return return_type;
					}

					current_token = yylex();

					new_type = type_find_record_element(current_type, YYTEXT_STRING);
					sprintf(record_name, "R_%d", current_type->unique_record_ID);
					type_destroy(current_type);
					current_type = new_type;

					if (current_type == NULL)
					{
						add_error_message(425, YYTEXT_STRING, "");

						/* Error-recovery */
						while ((current_token != SEMI_COLON)
							&& (current_token != KWD_END)
							&& (current_token != END_OF_INPUT))
						{
							current_token = yylex();
						}

						type_destroy(current_type);
						return return_type;
					}

					lowercase(YYTEXT_STRING);
					get_field_descriptor(new_type, field_descriptor);
					fieldref_index = cp_add_fieldref(record_name, YYTEXT_STRING, field_descriptor);

					bytecode_append(current_block->code, getfield$);
					bytecode_append_short_int(current_block->code, fieldref_index);

					current_token = yylex();
				}
				/* get an element from an array */
				else if (current_token == OPEN_SQ_BR)
				{
					type_list *array_elements;

					if (current_type->type_class != array_type)
					{
						add_error_message(426, "", "");

						/* Error-recovery */
						while ((current_token != SEMI_COLON)
							&& (current_token != KWD_END)
							&& (current_token != END_OF_INPUT))
						{
							current_token = yylex();
						}

						type_destroy(current_type);
						return return_type;
					}

					current_token = yylex();

					array_elements = RD_expression_list_array(current_block, current_block->code, current_type);

					/* replace the last aaload in the bytecode with the proper loading instruction */
					replace_aaload_instruction(current_block->code, current_type->element_type);

					if (type_list_different_parameter_array(array_elements, current_type->dimensions_list) != 0)
					{
						char num1[6], num2[6];

						sprintf(num1, "%d", type_list_length(current_type->dimensions_list));
						sprintf(num2, "%d", type_list_length(array_elements));
						add_error_message(427, num1, num2);

						/* Error-recovery */
						while ((current_token != SEMI_COLON)
							&& (current_token != KWD_END)
							&& (current_token != END_OF_INPUT))
						{
							current_token = yylex();
						}

						type_destroy(current_type);
						return return_type;
					}

					if (current_token != CLOSE_SQ_BR)
					{
						add_error_message(200, "]", YYTEXT_STRING);

						/* Error-recovery */
						while ((current_token != SEMI_COLON)
							&& (current_token != KWD_END)
							&& (current_token != END_OF_INPUT))
						{
							current_token = yylex();
						}

						type_destroy(current_type);
						return return_type;
					}

					current_token = yylex();

					new_type = type_duplicate(current_type->element_type);
					type_destroy(current_type);

					current_type = new_type;

					type_list_destroy(array_elements);
				}

			}

			type_destroy(return_type);

			return_type = current_type;

			return return_type;
		}

function_call:

		/* a function call */
		if (name->identifier_class == function_name)
		{
			int methodref_index;
			char *method_type;
			int method_index;
			method_type = (char*) mem_alloc(1024*5);

			if (name->standard_function)
			{
				create_std_function_prefix(current_block->code, identifier_text);
			}

			if (type_list_length(name->parameters) == 0)
			{
				int br_depth = 0;
				if (current_token == OPEN_BR)
				{
					add_error_message(419, "", "");
					br_depth = 1;
					while (br_depth > 0)
					{
						if ((current_token == END_OF_INPUT)
							|| (current_token == KWD_END))
						{
							identifier_destroy(name);
							return return_type;
						}

						current_token = yylex();

						if (current_token == OPEN_BR)
							br_depth ++;

						if (current_token == CLOSE_BR)
							br_depth --;
					}

					current_token = yylex();
				}

				/* create the bytecode */
				if (name->unit_function)
				{
					string *full_unit_name;
					if (name->container_unit->is_library == 1)
						full_unit_name = string_from_cstr("Lib_");
					else
						full_unit_name = string_create();
					string_append(full_unit_name, name->container_unit->unit_name);

					lowercase(identifier_text);
					strcpy(method_type, "()");
					get_field_descriptor(name->return_type, method_type + 2);
					method_index = cp_add_methodref(full_unit_name->cstr,
						identifier_text, method_type);
					bytecode_append(current_block->code, invokestatic$);
					bytecode_append_short_int(current_block->code, method_index);
					string_destroy(full_unit_name);
				}
				else if (!name->standard_function)
				{
					/* the function is user-defined */
					strcpy(method_type, "()");
					get_field_descriptor(name->return_type, method_type + 2);
					if (compiling_unit == 0)
						methodref_index = cp_add_methodref("M", identifier_text, method_type);
					else
						methodref_index = cp_add_methodref(string_get_cstr(str_program_name), identifier_text, method_type);
					bytecode_append(current_block->code, invokestatic$); /* call */
					bytecode_append_short_int(current_block->code, methodref_index);
				}
				else
				{
					/* handle standard functions and procedures */
					create_std_function_code(current_block->code, identifier_text);
				}

				type_destroy(return_type);

				mem_free(identifier_text);
				mem_free(method_type);

				return_type = type_duplicate(name->return_type);

				identifier_destroy(name);

				return return_type;
			}
			else
			{
				int compare_result;
				int old_linenum;
				type_list *parameter_list;

				if (current_token != OPEN_BR)
				{
					add_error_message(200, "(", YYTEXT_STRING);
				}

				current_token = yylex();

				old_linenum = linenum;

				parameter_list = RD_expression_list_cast(current_block, name->parameters);

				compare_result = type_list_different_parameter_cast(parameter_list, name->parameters);

				if (compare_result == -1){
					new_linenum=old_linenum;
					add_error_message(421, "", "");
				}

				if (compare_result > 0)
				{
					char par_num[4];
					sprintf(par_num, "%d", compare_result);
					new_linenum=old_linenum;
					add_error_message(422, par_num, "");
				}

				type_list_destroy(parameter_list);

				if (current_token != CLOSE_BR)
				{
					add_error_message(200, ")", YYTEXT_STRING);
				}
				else
					current_token = yylex();

				type_destroy(return_type);

				/* create the bytecode */
				if (!name->standard_function)
				{
					{
						int pos = 1;
						type_list *it;

						it = name->parameters;

						method_type[0] = '(';

						while (it != NULL)
						{
							if (it->data == NULL)
								break;


							get_field_descriptor(it->data, method_type + pos);
							pos = strlen(method_type);

							it = it->next;
						}

						strcat(method_type, ")");
					}


					get_field_descriptor(name->return_type, method_type + strlen(method_type));

					if (name->unit_function)
					{
						string *full_unit_name;
						if (name->container_unit->is_library == 1)
							full_unit_name = string_from_cstr("Lib_");
						else
							full_unit_name = string_create();
						string_append(full_unit_name, name->container_unit->unit_name);
						lowercase(identifier_text);
						methodref_index = cp_add_methodref(full_unit_name->cstr, identifier_text, method_type);
						string_destroy(full_unit_name);
					}
					else
					{
						if (compiling_unit == 0)
							methodref_index = cp_add_methodref("M", identifier_text, method_type);
						else
							methodref_index = cp_add_methodref(string_get_cstr(str_program_name), identifier_text, method_type);
					}

					bytecode_append(current_block->code, invokestatic$); /* call */
					bytecode_append_short_int(current_block->code, methodref_index);
				}
				else
				{
					/* handle standard functions and procedures */
					create_std_function_code(current_block->code, identifier_text);
				}
				/* END create the bytecode */

				return_type = type_duplicate(name->return_type);

				mem_free(identifier_text);
				mem_free(method_type);

				identifier_destroy(name);

				return return_type;
			}
		}

		mem_free(identifier_text);

		return return_type;
	}

	/* brackets, priority change, no code generated in here */
	if (current_token == OPEN_BR)
	{
		current_token = yylex();

		type_destroy(return_type);
		return_type =  RD_expression(current_block);

		if (current_token != CLOSE_BR)
		{
			add_error_message(200, ")", YYTEXT_STRING);
		}

		current_token = yylex();

		return return_type;
	}

	add_error_message(204, YYTEXT_STRING, "");

	return return_type;
}


/*
	Create the bytecode that loads the given constant
*/
void create_constant_bytecode(identifier *item, bytecode *code)
{
	int cp_index;

	switch(item->constant_type->type_class)
	{
	case integer_type:
		cp_index = cp_add_integer(item->constant_int_value);
		if (cp_index <= 255)
		{
			bytecode_append(code, ldc$);
			bytecode_append(code, (char) cp_index);
		}
		else
		{
			bytecode_append(code, ldc_w$);
			bytecode_append_short_int(code, (short) cp_index);
		}
		break;

	case real_type:
		{
		float cst;
		int csti; /* the integer part */
		int cstf; /* the fraction part */

		/* calculate the exponent and the integer part */
		cst = item->constant_real_value;

		csti = (int)cst;
		cstf = (cst - (int)cst)*100000;

		usesFloat = 1;

		if (mathType == 1)
		{
			/* put the integer part on the stack */
			if (abs(csti) < 127)
			{
				bytecode_append(code, bipush$);
				bytecode_append(code, csti);
			}
			else if (abs(csti) < 15000)
			{
				bytecode_append(code, sipush$);
				bytecode_append_short_int(code, csti);
			}
			else
			{
				bytecode_append(code, ldc_w$);
				bytecode_append_short_int(code, cp_add_integer(csti));
			}

			/* put the fraction part to the stack */
			if (abs(cstf) < 127)
			{
				bytecode_append(code, bipush$);
				bytecode_append(code, cstf);
			}
			else if (abs(cstf) < 15000)
			{
				bytecode_append(code, sipush$);
				bytecode_append_short_int(code, cstf);
			}
			else
			{
				bytecode_append(code, ldc_w$);
				bytecode_append_short_int(code, cp_add_integer(cstf));
			}

			bytecode_append(code, invokestatic$);
			bytecode_append_short_int(code, cp_add_methodref("F", "fI", "(II)I"));
		}
		else
		{
			char number[64];

			bytecode_append(code, new$);
			bytecode_append_short_int(code, cp_add_class("Real"));
			bytecode_append(code, dup$);

			sprintf(number, "%f", real_constant);
			bytecode_append(code, ldc_w$);
			bytecode_append_short_int(code, cp_add_string(number));

			bytecode_append(code, invokespecial$);
			bytecode_append_short_int(code, cp_add_methodref("Real", "<init>", "(Ljava/lang/String;)V"));

		}



		}
		break;

	case char_type:
		cp_index = cp_add_integer(item->constant_int_value);
		if (cp_index <= 255)
		{
			bytecode_append(code, ldc$);
			bytecode_append(code, (char) cp_index);
		}
		else
		{
			bytecode_append(code, ldc_w$);
			bytecode_append_short_int(code, (short) cp_index);
		}
		break;

	case boolean_type:
		if (item->constant_int_value)
		{
			bytecode_append(code, iconst_m1$);
		}
		else
		{
			bytecode_append(code, iconst_0$);
		}
		break;

	case string_type:
		cp_index = cp_add_string(item->constant_string_value->cstr);
		if (cp_index <= 255)
		{
			bytecode_append(code, ldc$);
			bytecode_append(code, (char) cp_index);
		}
		else
		{
			bytecode_append(code, ldc_w$);
			bytecode_append_short_int(code, (short) cp_index);
		}
		break;
	}
}


/*
	Creates the code that loads the variable or parameter value to the stack
*/
void create_variable_bytecode(identifier *item, bytecode *code, char *name, int is_field)
{
	int index;
	type *item_type;

	/* if the block is program block, load field instead of variable */
	if ((is_field) || (item->unit_function == 1))
	{
		if (item->identifier_class == variable_name)
		{
			char type_descriptor[128];

			lowercase(name);
			bytecode_append(code, getstatic$);
			get_field_descriptor(item->variable_type, type_descriptor);

			if (item->unit_function == 0)
			{
				if (compiling_unit == 0)
					bytecode_append_short_int(code, cp_add_fieldref("M", name, type_descriptor));
				else
					bytecode_append_short_int(code, cp_add_fieldref(string_get_cstr(str_program_name), name, type_descriptor));
			}
			else
			{
				string *unit_name;

				if (item->container_unit->is_library == 0)
					unit_name = string_create();
				else
					unit_name = string_from_cstr("Lib_");

				lowercase(string_get_cstr(item->container_unit->unit_name));

				string_append(unit_name, item->container_unit->unit_name);

				bytecode_append_short_int(code, cp_add_fieldref(string_get_cstr(unit_name), name, type_descriptor));

				string_destroy(unit_name);
			}
		}

		return;
	}

	if (item->identifier_class == variable_name)
	{
		index = item->variable_index;
		item_type = item->variable_type;
	}
	else if (item->identifier_class == function_name)
	{
		/* this is used for error recovery and returning values form functions */
		index = type_list_length(item->parameters);
		item_type = item->return_type;
	}
	else if (item->identifier_class == parameter_name)
	{
		index = item->parameter_index;
		item_type = item->parameter_type;
	}
	else
	{
		add_error_message(444, "", "");
		return 0;
	}

	switch (item_type->type_class)
	{
	case integer_type:
	case char_type:
	case boolean_type:
		switch(index)
		{
		case 0:
			bytecode_append(code, iload_0$);
			break;
		case 1:
			bytecode_append(code, iload_1$);
			break;
		case 2:
			bytecode_append(code, iload_2$);
			break;
		case 3:
			bytecode_append(code, iload_3$);
			break;
		default:
			bytecode_append(code, iload$);
			bytecode_append(code, index);
			break;
		}

		break;

	case real_type:
		if (mathType == 1)
		{
			switch(index)
			{
			case 0:
				bytecode_append(code, iload_0$);
				break;
			case 1:
				bytecode_append(code, iload_1$);
				break;
			case 2:
				bytecode_append(code, iload_2$);
				break;
			case 3:
				bytecode_append(code, iload_3$);
				break;
			default:
				bytecode_append(code, iload$);
				bytecode_append(code, index);
				break;
			}
		}
		else
		{
			switch(index)
			{
			case 0:
				bytecode_append(code, aload_0$);
				break;
			case 1:
				bytecode_append(code, aload_1$);
				break;
			case 2:
				bytecode_append(code, aload_2$);
				break;
			case 3:
				bytecode_append(code, aload_3$);
				break;
			default:
				bytecode_append(code, aload$);
				bytecode_append(code, index);
				break;
			}
		}
		break;

	case string_type:
	case record_type:
	case array_type:
	case image_type:
	case command_type:
	case stream_type:
	case record_store_type:
	case http_type:
	case alert_type:
		switch(index)
		{
		case 0:
			bytecode_append(code, aload_0$);
			break;
		case 1:
			bytecode_append(code, aload_1$);
			break;
		case 2:
			bytecode_append(code, aload_2$);
			break;
		case 3:
			bytecode_append(code, aload_3$);
			break;
		default:
			bytecode_append(code, aload$);
			bytecode_append(code, index);
			break;
		}

		break;

	default:
		die(15);
	}
}

/*
	Creates the code that puts the variable or parameter value from the stack into memory
*/
void create_put_variable_bytecode(identifier *item, bytecode *code, char *name, int is_field)
{
	int index;
	type *item_type;

	if (item->identifier_class == variable_name)
	{
		index = item->variable_index;
		item_type = item->variable_type;
	}
	else if (item->identifier_class == parameter_name)
	{
		index = item->parameter_index;
		item_type = item->parameter_type;
	}
	else if (item->identifier_class == function_name)/* function name, it is return value variable */
	{
		index = type_list_length(item->parameters);
		is_field = 0;
		item_type = item->return_type;
	}
	else
	{
		add_error_message(444, "", "");
		return;
	}

	/* if the value on the top of the stack is string, copy it */
	if (item_type->type_class == string_type)
	{
		int class_index;
		int method_index;

		class_index = cp_add_class("java/lang/String");
		method_index = cp_add_methodref("java/lang/String", "<init>", "(Ljava/lang/String;)V");

		bytecode_append(code, new$);
		bytecode_append_short_int(code, class_index);
		bytecode_append(code, dup_x1$);
		bytecode_append(code, swap$);
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, method_index);
	}

	if ((item_type->type_class == real_type) && (mathType != 1))
	{
		int class_index;
		int method_index;

		usesFloat = 1;

		class_index = cp_add_class("Real");
		method_index = cp_add_methodref("Real", "<init>", "(LReal;)V");

		bytecode_append(code, new$);
		bytecode_append_short_int(code, class_index);
		bytecode_append(code, dup_x1$);
		bytecode_append(code, swap$);
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, method_index);
	}

	/* if the value on the top of the stack is record type, copy it */
	if (item_type->type_class == record_type)
	{
		int class_index;
		int constructor_index;
		int copy_index;

		char type_name[16];
		char copy_sig[64];

		sprintf(type_name, "R_%d", item_type->unique_record_ID);
		sprintf(copy_sig, "(L%s;)L%s;", type_name, type_name);

		class_index = cp_add_class(type_name);
		constructor_index = cp_add_methodref(type_name, "<init>", "()V");
		copy_index = cp_add_methodref(type_name, "Copy", copy_sig);

		bytecode_append(code, new$);
		bytecode_append_short_int(code, class_index);
		bytecode_append(code, dup_x1$);
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, constructor_index);
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, copy_index);
	}

	/* if the block is program block, load field instead of variable */
	if ((is_field) || (item->unit_function == 1))
	{
		if (item->identifier_class == variable_name)
		{
			char type_descriptor[128];

			lowercase(name);
			bytecode_append(code, putstatic$);
			get_field_descriptor(item->variable_type, type_descriptor);

			if (item->unit_function == 0)
			{
				if (compiling_unit == 0)
					bytecode_append_short_int(code, cp_add_fieldref("M", name, type_descriptor));
				else
					bytecode_append_short_int(code, cp_add_fieldref(string_get_cstr(str_program_name), name, type_descriptor));
			}
			else
			{
				string *unit_name;

				if (item->container_unit->is_library == 0)
					unit_name = string_create();
				else
					unit_name = string_from_cstr("Lib_");

				lowercase(string_get_cstr(item->container_unit->unit_name));

				string_append(unit_name, item->container_unit->unit_name);

				bytecode_append_short_int(code, cp_add_fieldref(string_get_cstr(unit_name), name, type_descriptor));

				string_destroy(unit_name);
			}
		}

		return;
	}

	switch (item_type->type_class)
	{
	case integer_type:
	case char_type:
	case boolean_type:
		switch(index)
		{
		case 0:
			bytecode_append(code, istore_0$);
			break;
		case 1:
			bytecode_append(code, istore_1$);
			break;
		case 2:
			bytecode_append(code, istore_2$);
			break;
		case 3:
			bytecode_append(code, istore_3$);
			break;
		default:
			bytecode_append(code, istore$);
			bytecode_append(code, index);
			break;
		}

		break;

	case real_type:
			if (mathType == 1)
			{
				switch(index)
				{
				case 0:
					bytecode_append(code, istore_0$);
					break;
				case 1:
					bytecode_append(code, istore_1$);
					break;
				case 2:
					bytecode_append(code, istore_2$);
					break;
				case 3:
					bytecode_append(code, istore_3$);
					break;
				default:
					bytecode_append(code, istore$);
					bytecode_append(code, index);
					break;
				}
			}
			else
			{
				switch(index)
				{
				case 0:
					bytecode_append(code, astore_0$);
					break;
				case 1:
					bytecode_append(code, astore_1$);
					break;
				case 2:
					bytecode_append(code, astore_2$);
					break;
				case 3:
					bytecode_append(code, astore_3$);
					break;
				default:
					bytecode_append(code, astore$);
					bytecode_append(code, index);
					break;
				}
			}
			break;

	case string_type:
	case record_type:
	case array_type:
	case image_type:
	case command_type:
	case stream_type:
	case record_store_type:
	case http_type:
	case alert_type:
		switch(index)
		{
		case 0:
			bytecode_append(code, astore_0$);
			break;
		case 1:
			bytecode_append(code, astore_1$);
			break;
		case 2:
			bytecode_append(code, astore_2$);
			break;
		case 3:
			bytecode_append(code, astore_3$);
			break;
		default:
			bytecode_append(code, astore$);
			bytecode_append(code, index);
			break;
		}

		break;

	default:
		die(15);
	}
}

/*
	The last instruction in the bytecode is aaload; it should be replaced with
	the iaload, faload, aload or something like that.
*/
void replace_aaload_instruction(bytecode *code, type *element_type)
{
	/* delete the last instruction */
	code->bytecode_pos --;

	/* create the new instruction */
	switch(element_type->type_class)
	{
	case integer_type:
	case boolean_type:
	case char_type:
		bytecode_append(code, iaload$);
		break;

	case real_type:
		if (mathType == 1)
		{
			bytecode_append(code, iaload$);
		}
		else
		{
			bytecode_append(code, aaload$);
		}
		break;

	case record_type:
	case string_type:
	case image_type:
	case command_type:
	case stream_type:
	case record_store_type:
	case http_type:
	case alert_type:
		bytecode_append(code, aaload$);
		break;

	default:
		die(24);
	}
}

/*
	Creates the code that adjustes array indices
*/
void adjust_indices(bytecode *code, type *dimension)
{
	if (dimension == NULL)
		return;

	switch(dimension->first_element)
	{
	case -1:
		bytecode_append(code, iconst_m1$);
		break;
	case 0:
		//bytecode_append(code, iconst_0$);
		break;
	case 1:
		bytecode_append(code, iconst_1$); break;
	case 2:
		bytecode_append(code, iconst_2$); break;
	case 3:
		bytecode_append(code, iconst_3$); break;
	case 4:
		bytecode_append(code, iconst_4$); break;
	case 5:
		bytecode_append(code, iconst_5$); break;
	default:
		if (dimension->first_element < 128)
		{
			bytecode_append(code, bipush$);
			bytecode_append(code, dimension->first_element);
		}
		else
		{
			bytecode_append(code, sipush$);
			bytecode_append_short_int(code, dimension->first_element);
		}
	}
	if (dimension->first_element != 0)
		bytecode_append(code, isub$);

}
