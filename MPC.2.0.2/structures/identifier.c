/********************************************************************
	
	identifier.c - handling identifier descriptions

  Niksa Orlic, 2004-04-28

********************************************************************/

#include "../util/strings.h"
#include "../util/error.h"
#include "../util/message.h"
#include "type_list.h"
#include "string_list.h"
#include "type.h"
#include "identifier.h"
#include "name_table.h"
#include "../classgen/bytecode.h"
#include "block.h"
#include "unit.h"
#include "../util/memory.h"

#include <string.h>
#include <stdlib.h>

extern FILE* symbols_file;

/*
	Creates an empty identifier
*/

identifier *identifier_create()
{
	identifier *new_identifier;
	new_identifier = (identifier*) mem_alloc(sizeof(identifier));

	if (new_identifier == NULL)
		die(MSG_001);

	new_identifier->identifier_class = none;

	new_identifier->standard_function = 0;

	new_identifier->variables = NULL;

	new_identifier->forward_declaration = 0;

	new_identifier->unit_block = NULL;

	new_identifier->unit_function = 0;
	new_identifier->container_unit = NULL;

	return new_identifier;
}


/*
	Deletes an identifier
*/
void identifier_destroy(identifier *item)
{
	switch (item->identifier_class)
	{
	case constant_name:
		{
			if (item->constant_type->type_class == string_type)
				string_destroy(item->constant_string_value);
			type_destroy(item->constant_type);
			break;
		}

	case variable_name:
		{
			if (item->variable_type != NULL)
				type_destroy(item->variable_type);
			break;
		}

	case type_name:
		{
			type_destroy(item->defined_type);
			break;
		}

	case procedure_name:
		{
			type_list_destroy(item->parameters);
			if (item->variables != NULL)
				type_list_destroy(item->variables);
			break;		
		}

	case function_name:
		{
			type_destroy(item->return_type);
			type_list_destroy(item->parameters);
			if (item->variables != NULL)
				type_list_destroy(item->variables);
			break;
		}

	case parameter_name:
		{
			type_destroy(item->parameter_type);
			break;
		}

	case unit_name:
		{
			unit_destroy(item->unit_block);
			break;
		}
	}

	mem_free(item);
}

/*
	Create a copy of a given identifier
*/
identifier *identifier_duplicate(identifier *item)
{
	identifier *new_identifier;
	new_identifier = (identifier*) mem_alloc(sizeof(identifier));

	if (new_identifier == NULL)
		die(MSG_001);

	new_identifier->identifier_class = item->identifier_class;
	new_identifier->unit_function = item->unit_function;

	switch(new_identifier->identifier_class)
	{
	case constant_name:
		{
			new_identifier->constant_type = type_duplicate(item->constant_type);
			switch(item->constant_type->type_class)
			{
			case integer_type:
				{
					new_identifier->constant_int_value = item->constant_int_value;
					break;
				}
			case char_type:
				{
					new_identifier->constant_int_value = item->constant_int_value;
					break;
				}
			case string_type:
				{
					new_identifier->constant_string_value = string_duplicate(item->constant_string_value);
					break;
				}
			case boolean_type:
				{
					new_identifier->constant_int_value = item->constant_int_value;
					break;
				}
			case real_type:
				{
					new_identifier->constant_real_value = item->constant_real_value;
					break;
				}
			case command_type:
			case record_store_type:
			case http_type:
			case image_type:
			case stream_type:
			case alert_type:
				{
					break;
				}
			default:
				{
					die(MSG_011);
				}
			}
			break;
		}

	case variable_name:
		{
			new_identifier->variable_type = type_duplicate(item->variable_type);
			new_identifier->variable_index = item->variable_index;
			break;
		}

	case type_name:
		{
			new_identifier->defined_type = type_duplicate(item->defined_type);
			break;
		}

	case procedure_name:
		{
			new_identifier->parameters = type_list_duplicate(item->parameters);
			new_identifier->forward_declaration = item->forward_declaration;
			new_identifier->standard_function = item->standard_function;
			new_identifier->variables = type_list_duplicate(item->variables);			
			if (item->container_unit != NULL)
				new_identifier->container_unit = unit_duplicate(item->container_unit);
			else
				new_identifier->container_unit = NULL;
			break;
		}

	case function_name:
		{
			new_identifier->parameters = type_list_duplicate(item->parameters);
			new_identifier->return_type = type_duplicate(item->return_type);
			new_identifier->forward_declaration = item->forward_declaration;
			new_identifier->standard_function = item->standard_function;
			new_identifier->variables = type_list_duplicate(item->variables);
			break;
		}

	case parameter_name:
		{
			new_identifier->parameter_type = type_duplicate(item->parameter_type);
			new_identifier->is_parameter_variable = item->is_parameter_variable;
			new_identifier->parameter_index = item->parameter_index;
			break;
		}

	case unit_name:
		{
			new_identifier->unit_block = unit_duplicate(item->unit_block);
			break;
		}

	}

	if (item->container_unit != NULL)
		new_identifier->container_unit = unit_duplicate(item->container_unit);
	else
		new_identifier->container_unit = NULL;

	if (item->unit_block != NULL)
		new_identifier->unit_block = unit_duplicate(item->unit_block);
	else
		new_identifier->unit_block = NULL;

	return new_identifier;
}

void bsf_write_integer_constant(long int value, char* name)
{
	char tag = 1;
	char type = 1;
	fwrite(&tag, 1, 1, symbols_file);
	bsf_write_STRING(name);
	fwrite(&type, 1, 1, symbols_file);
	fwrite(&value, sizeof(long int), 1, symbols_file);
}

void bsf_write_real_constant(float value, char* name)
{
	char tag = 1;
	char type = 2;
	fwrite(&tag, 1, 1, symbols_file);
	bsf_write_STRING(name);
	fwrite(&type, 1, 1, symbols_file);
	fwrite(&value, sizeof(float), 1, symbols_file);
}

void bsf_write_boolean_constant(char value, char* name)
{
	char tag = 1;
	char type = 3;
	fwrite(&tag, 1, 1, symbols_file);
	bsf_write_STRING(name);
	fwrite(&type, 1, 1, symbols_file);
	fwrite(&value, sizeof(char), 1, symbols_file);
}

void bsf_write_char_constant(char value, char* name)
{
	char tag = 1;
	char type = 4;
	fwrite(&tag, 1, 1, symbols_file);
	bsf_write_STRING(name);
	fwrite(&type, 1, 1, symbols_file);
	fwrite(&value, sizeof(char), 1, symbols_file);
}

void bsf_write_string_constant(string* value, char* name)
{
	char tag = 1;
	char type = 5;
	fwrite(&tag, 1, 1, symbols_file);
	bsf_write_STRING(name);
	fwrite(&type, 1, 1, symbols_file);
	
	bsf_write_STRING(string_get_cstr(value));
}

void bsf_write_variables(string_list* names, type* var_type)
{
	string_list *it = names;

	while ((it != NULL) && (it->data != NULL))
	{
		char tag = 2;
		fwrite(&tag, 1, 1, symbols_file);
		bsf_write_STRING(string_get_cstr(it->data));
		bsf_write_TYPE(var_type);
		it = it->next;
	}
}

void bsf_write_type(string* type_name, type* type_type)
{
	char tag = 3;
	fwrite(&tag, 1, 1, symbols_file);
	bsf_write_STRING(string_get_cstr(type_name));
	bsf_write_TYPE(type_type);
}

void bsf_write_procedure(string* name, type_list* parameters)
{
	type_list *it;
	char params_count = 0;

	char tag = 4;
	fwrite(&tag, 1, 1, symbols_file);
	bsf_write_STRING(string_get_cstr(name));

	params_count = type_list_length(parameters);
	fwrite(&params_count, 1, 1, symbols_file);

	it = parameters;

	while ((it != NULL) && (it->data != NULL))
	{
		bsf_write_TYPE(it->data);
		it = it->next;
	}	
}

void bsf_write_function(string* name, type_list* parameters, type* return_type)
{
	type_list *it;
	char params_count = 0;

	char tag = 5;
	fwrite(&tag, 1, 1, symbols_file);
	bsf_write_STRING(string_get_cstr(name));

	bsf_write_TYPE(return_type);

	params_count = type_list_length(parameters);
	fwrite(&params_count, 1, 1, symbols_file);

	it = parameters;

	while ((it != NULL) && (it->data != NULL))
	{
		bsf_write_TYPE(it->data);
		it = it->next;
	}	
}

void bsf_write_STRING(char* value)
{
	char len = strlen(value);
	lowercase(value);
	fwrite(&len, 1, 1, symbols_file);
	fwrite(value, 1, len, symbols_file);
}

string* bsf_read_STRING(FILE* file)
{
	char length;
	char *buffer;
	string* string_value;

	fread(&length, 1, 1, file);
	buffer = (char*) malloc(length + 5);
	fread(buffer, 1, length, file);
	buffer[length] = '\0';
	string_value = string_from_cstr(buffer);
	free(buffer);

	return string_value;
}

void bsf_write_TYPE(type* type_desc)
{
	fwrite(&(char)type_desc->type_class, 1, 1, symbols_file);

	if (type_desc->type_class == array_type)
	{
		char dimensions;
		type_list *it;

		bsf_write_TYPE(type_desc->element_type);
		
		dimensions = type_list_length(type_desc->dimensions_list);
		fwrite(&dimensions, 1, 1, symbols_file);

		it = type_desc->dimensions_list;

		while ((it != NULL) && (it->data != NULL))
		{
			bsf_write_TYPE(it->data);
			it = it->next;
		}
	}

	if (type_desc->type_class == record_type)
	{
		char elements;
		string_list *name_it;
		type_list *type_it;

		elements = string_list_length(type_desc->elements_name_list);
		fwrite(&elements, 1, 1, symbols_file);

		name_it = type_desc->elements_name_list;
		type_it = type_desc->elements_type_list;

		while ((name_it != NULL) && (name_it->data != NULL))
		{
			bsf_write_STRING(string_get_cstr(name_it->data));
			bsf_write_TYPE(type_it->data);
		
			name_it = name_it->next;
			type_it = type_it->next;
		}

		fwrite(&type_desc->unique_record_ID, 4, 1, symbols_file);
	}

	if (type_desc->type_class == interval_type)
	{
		fwrite(&(char)type_desc->interval_base_type, 1, 1, symbols_file);
		fwrite(&type_desc->first_element, 4, 1, symbols_file);
		fwrite(&type_desc->last_element, 4, 1, symbols_file);
	}
}

type* bsf_read_TYPE(FILE* symbol_file)
{
	char type_class;

	type* result_type = type_create();

	fread(&type_class, 1, 1, symbol_file);
	result_type->type_class = (enum en_type_class)type_class;

	if (result_type->type_class == array_type)
	{
		char dimensions;
		
		result_type->element_type = bsf_read_TYPE(symbol_file);
		fread(&dimensions, 1, 1, symbol_file);

		result_type->dimensions_list = type_list_create();

		while (dimensions > 0)
		{
			type_list_append(result_type->dimensions_list, bsf_read_TYPE(symbol_file));
			dimensions --;
		}		
	}

	if (result_type->type_class == record_type)
	{
		char elements;
		fread(&elements, 1, 1, symbol_file);
		
		result_type->elements_name_list = string_list_create();
		result_type->elements_type_list = type_list_create();

		while (elements > 0)
		{
			string_list_append(result_type->elements_name_list, bsf_read_STRING(symbol_file));
			type_list_append(result_type->elements_type_list, bsf_read_TYPE(symbol_file));
			elements --;
		}

		fread(&result_type->unique_record_ID, sizeof(int), 1, symbol_file);
	}

	if (result_type->type_class == interval_type)
	{
		char base_type_class;
		fread(&base_type_class, 1, 1, symbol_file);

		result_type->interval_base_type = (enum en_type_class)base_type_class;

		fread(&result_type->first_element, sizeof(long int), 1, symbol_file);
		fread(&result_type->last_element, sizeof(long int), 1, symbol_file);
	}

	return result_type;
}