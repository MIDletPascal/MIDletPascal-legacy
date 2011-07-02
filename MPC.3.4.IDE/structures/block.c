/********************************************************************

	block.h - structures and functions used to hold program
	block descriptions

  Niksa Orlic, 2004-04-29

********************************************************************/


#include "../util/strings.h"
#include "../util/error.h"
//#include "../util/message.h"
#include "../classgen/bytecode.h"
#include "type_list.h"
#include "string_list.h"
#include "type.h"
#include "identifier.h"
#include "name_table.h"
#include "unit.h"
#include "block.h"
#include "../classgen/preverify.h"

#include "../util/memory.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../classgen/constant_pool.h"
#include "../classgen/classgen.h"

extern int linenum;
extern int new_linenum;
extern block* root_block;
extern char *global_library_directory;
extern char *project_library_directory;

extern char *user_libraries;
extern char **units;

extern int detect_units_only;

extern constant_pool *constants;
extern int constant_pool_size;

extern int usesFloat;
extern int usesRecordStore;
extern int usesHttp;

extern int canvasType;

extern int mathType;

extern int compiling_unit;
extern string* str_program_name;

extern char* output_path;

#pragma warning (disable:4305)
#pragma warning (disable:4761)

/*
	Create an empty block
*/
block* block_create(block *parent_block, string *block_name)
{
	block *new_block;
	new_block = (block*) mem_alloc(sizeof(block));

	new_block->names = name_table_create();
	new_block->parent_block = parent_block;
	new_block->block_name = block_name; /* do not copy the name, just point to it */

	new_block->next_variable_index = 0;
	new_block->next_parameter_index = 0;

	new_block->code = bytecode_create();

	new_block->children_count = 0;

	return new_block;
}


/*
	Delete a block
*/
void block_destroy(block *item)
{
	int i;
	name_table_destroy(item->names);
	bytecode_destroy(item->code);

	/* delete all of its children */
	for(i=0; i<item->children_count; i++)
	{
		block_destroy(item->children[i]);
	}

	if (item->children_count > 0)
	{
		mem_free(item->children);
	}

	/* do not delete the item->block_name */
	mem_free(item);
}


/*
	Check if a given name is legal name for a
	new identifier in a block.
*/
int block_check_name(block *item, char *cstr)
{
	int return_value = 1;

	string *name;
	identifier *descriptor;
	name = string_from_cstr(cstr);

	descriptor = name_table_find(item->names, name);

	if (descriptor != NULL)
	{
		if (((descriptor->identifier_class == procedure_name)
			    || (descriptor->identifier_class == function_name))
			&& (descriptor->forward_declaration == 1))
			return_value = 1;
		else
			return_value = 0;
	}

	/* check the root block's types - those types are protected */
	if (return_value == 1)
	{
		while (item->parent_block != NULL)
			item = item->parent_block;

		descriptor = name_table_find(item->names, name);

		if ((descriptor != NULL) && (descriptor->identifier_class == type_name))
			return_value = 0;
	}

	string_destroy(name);

	return return_value;
}

/*
	Adds a single real constant into the block
*/
void add_real_constant(block *item, float value, char *cstr_name)
{
	string *name;
	identifier *descriptor;

	name = string_from_cstr(cstr_name);
	descriptor = identifier_create();
	descriptor->identifier_class = constant_name;
	descriptor->constant_type = type_create();
	descriptor->constant_type->type_class = real_type;
	descriptor->constant_real_value = value;
	name_table_insert(item->names, name, descriptor);
}


/*
	Adds a single integer constant into the block
*/
void add_integer_constant(block *item, long int value, char *cstr_name)
{
	string *name;
	identifier *descriptor;

	name = string_from_cstr(cstr_name);
	descriptor = identifier_create();
	descriptor->identifier_class = constant_name;
	descriptor->constant_type = type_create();
	descriptor->constant_type->type_class = integer_type;
	descriptor->constant_int_value = value;
	name_table_insert(item->names, name, descriptor);
}


/*
	Adds a single character constant into the block
*/
void add_char_constant(block *item, char value, char *cstr_name)
{
	string *name;
	identifier *descriptor;

	name = string_from_cstr(cstr_name);
	descriptor = identifier_create();
	descriptor->identifier_class = constant_name;
	descriptor->constant_type = type_create();
	descriptor->constant_type->type_class = char_type;
	descriptor->constant_int_value = value;
	name_table_insert(item->names, name, descriptor);
}


/*
	Adds a single boolean constant into the block
*/
void add_boolean_constant(block *item, char value, char *cstr_name)
{
	string *name;
	identifier *descriptor;

	name = string_from_cstr(cstr_name);
	descriptor = identifier_create();
	descriptor->identifier_class = constant_name;
	descriptor->constant_type = type_create();
	descriptor->constant_type->type_class = boolean_type;
	descriptor->constant_int_value = value;
	name_table_insert(item->names, name, descriptor);
}


/*
	Adds a single string constant into the block
*/
void add_string_constant(block *item, string *value, char *cstr_name)
{
	string *name;
	identifier *descriptor;

	name = string_from_cstr(cstr_name);
	descriptor = identifier_create();
	descriptor->identifier_class = constant_name;
	descriptor->constant_type = type_create();
	descriptor->constant_type->type_class = string_type;
	descriptor->constant_string_value = string_duplicate(value);
	name_table_insert(item->names, name, descriptor);
}


/*
	Adds variables into a block. The variable names are
	located inside a string list, and a type for all
	the variables is described in var_type parameter.
*/
void add_variables(block *item, string_list *identifier_list, type *var_type)
{
	string_list *it;
	identifier *descriptor;
	identifier *block_identifier;

	it = identifier_list;

	if (it->data == NULL)
	{
		type_destroy(var_type);
		return;
	}

	if(item->parent_block != root_block)
	{
		block_identifier = name_table_find(item->parent_block->names, item->block_name);
		if (block_identifier->variables == NULL)
			block_identifier->variables = type_list_create();
	}
	else
		block_identifier = NULL;

	while (it != NULL)
	{
		descriptor = identifier_create();
		descriptor->identifier_class = variable_name;
		descriptor->variable_type = type_duplicate(var_type);
		descriptor->variable_index = item->next_variable_index;
		descriptor->belongs_to_program_block = (item->parent_block == root_block);
		name_table_insert(item->names, string_duplicate(it->data), descriptor);

		if (compiling_unit == 0)
			initialize_variable(item, descriptor, it->data->cstr, 1, "M");
		else
			initialize_variable(item, descriptor, it->data->cstr, 1, string_get_cstr(str_program_name));

		if (block_identifier != NULL)
			type_list_append(block_identifier->variables, descriptor->variable_type);

		item->next_variable_index ++;

		it = it->next;
	}

}

/*
	Add a type into block. The name and type_type
	are NOT copied.
*/
void add_type(block *item, string *name, type *type_type)
{
	identifier *descriptor;

	descriptor = identifier_create();
	descriptor->identifier_class = type_name;
	descriptor->defined_type = type_type;
	name_table_insert(item->names, name, descriptor);
}


/*
	Adds a procedure declaration into the block
*/
void add_procedure(block *item, string *name, type_list *parameters,
				   int forward_declaration, int linenum)
{
	identifier *descriptor;

	descriptor = name_table_find(item->names, name);

	if (descriptor != NULL)
	{
		if ((forward_declaration != -1)
			&& (descriptor->forward_declaration != -1))
			check_forward_declaration(descriptor, parameters,
				forward_declaration, NULL);

		if ((descriptor->forward_declaration == -1)
			|| (forward_declaration != -1))
			descriptor->forward_declaration = forward_declaration;
	}
	else
	{
		descriptor = identifier_create();
		descriptor->identifier_class = procedure_name;
		descriptor->parameters = parameters;
		descriptor->forward_declaration = forward_declaration;
		descriptor->subprogram_linenum = linenum;
		name_table_insert(item->names, name, descriptor);
	}
}


/*
	Adds a function declaration into the block
*/
void add_function(block *item, string *name, type_list *parameters,
				  type *return_type, int forward_declaration, int linenum)
{
	identifier *descriptor;

	descriptor = name_table_find(item->names, name);

	if (descriptor != NULL)
	{
		if ((forward_declaration != -1)
			&& (descriptor->forward_declaration != -1))
			check_forward_declaration(descriptor, parameters,
				forward_declaration, return_type);

		if ((descriptor->forward_declaration == -1)
			|| (forward_declaration != -1))
		descriptor->forward_declaration = forward_declaration;
	}
	else
	{
		descriptor = identifier_create();
		descriptor->identifier_class = function_name;
		descriptor->parameters = parameters;
		descriptor->return_type = return_type;
		descriptor->forward_declaration = forward_declaration;
		descriptor->subprogram_linenum = linenum;
		name_table_insert(item->names, name, descriptor);
	}
}


/*
	Loads (inserts) an external library into the project - load the class file and
	enumerate all static functions.
*/
void load_extern_library(string* library_name)
{
	FILE *library_file = NULL;
	FILE *symbol_file = NULL;
	string *global_library_path;
	string *project_library_path;
	unit *library_unit;
	identifier  *unit_identifier;

	lowercase(library_name->cstr);

	/* create the full path to the library file */
	/* global library directory */
	global_library_path = string_from_cstr(global_library_directory);
	string_append(global_library_path, string_from_cstr("\\Lib_"));
	string_append(global_library_path, library_name);
	string_append(global_library_path, string_from_cstr(".class"));
	/* project library directory */
	project_library_path = string_from_cstr(project_library_directory);
	string_append(project_library_path, string_from_cstr("\\Lib_"));
	string_append(project_library_path, library_name);
	string_append(project_library_path, string_from_cstr(".class"));

	if (name_table_find(root_block->names, library_name) != NULL)
	{
		add_error_message(450, string_get_cstr(library_name), "");
		fclose(library_file);
		goto lel_finally;
	}

	/* create a new unit object */
	library_unit = unit_create(string_duplicate(library_name));
	unit_identifier = identifier_create();
	unit_identifier->identifier_class = unit_name;
	unit_identifier->unit_block = library_unit;
	name_table_insert(root_block->names, string_duplicate(library_name), unit_identifier);

	/* try open the library file from the libraries directories */
	library_file = fopen(project_library_path->cstr, "rb");
	if (library_file == NULL)
	{
        library_file = fopen(global_library_path->cstr, "rb");
	}
	if (library_file == NULL)
	{
		/* if this fails, try to open an unit file */
		char *filename = (char*)malloc(strlen(output_path) + string_length(library_name) + 10);

		sprintf(filename, "%s\\%s.bsf", output_path, string_get_cstr(library_name));

		symbol_file = fopen(filename, "rb");

		free(filename);

		if (symbol_file == NULL)
		{
		    // j-a-s-d: if unit is not found it must stop only if compiling
			if (!detect_units_only)
			{
				add_error_message(448, library_name->cstr, "");
                goto lel_finally;
			}

		}

		if (detect_units_only)
            requires(0, library_name->cstr);
	} else {
		if (!detect_units_only)
            requires(1, library_name->cstr);
	}

	/* read the library file */
	if (library_file)
	{
		library_unit->is_library = 1;
		if (detect_units_only == 0)
			read_library_file(library_file, library_unit);
		fclose(library_file);
	}

	if (symbol_file)
	{
		library_unit->is_library = 0;
		if (detect_units_only == 0)
			read_symbol_file(symbol_file, library_unit);
		fclose(symbol_file);
	}

	unit_identifier->identifier_class = unit_name;
	unit_identifier->unit_block = library_unit;

	name_table_insert(root_block->names, library_name , unit_identifier);

	if (library_file)
	{
		if (user_libraries == NULL)
		{
			user_libraries = malloc(library_name->length+1);
			strcpy(user_libraries, library_name->cstr);
		}
		else
		{
			int len = strlen(user_libraries);
			user_libraries = realloc(user_libraries, len + 3 + library_name->length);
			user_libraries[strlen(user_libraries)] = ' ';
			strcpy(user_libraries + len + 1, library_name->cstr);
		}
	}

	if (symbol_file)
	{
save_unit_name:
		if (*units == NULL)
		{
			*units = malloc(library_name->length+1);
			strcpy(*units, library_name->cstr);
		}
		else
		{
			int len = strlen(*units);
			*units = realloc(*units, len + 3 + library_name->length);
			(*units)[len] = ' ';
			strcpy(*units + len + 1, library_name->cstr);
		}
	}


lel_finally:
	string_destroy(library_name);
	string_destroy(project_library_path);
	string_destroy(global_library_path);
}


/*
	Reads the library file.
*/
void read_library_file(FILE* library_file, unit* library_unit)
{
	int constant_pool_count;
	struct constant_pool_struct *constant_pool;
	int access_flags, this_class, super_class;
	int interface_count, field_count, methods_count;
	int i, j;

	/* read and check magic number */
	if ((fgetc(library_file) != 0xca)
		|| (fgetc(library_file) != 0xfe)
		|| (fgetc(library_file) != 0xba)
		|| (fgetc(library_file) != 0xbe)
		)
	{
		add_error_message(449, "", "");
		fclose(library_file);
		die(6);
		return;
	}

	/* skip major and minor count */
	read_long_int(library_file);

	/* read the number of constant pool entries */
	constant_pool_count = read_short_int(library_file);

	/* allocate and read the constant pool */
	constant_pool = mem_alloc(sizeof(struct constant_pool_struct) * constant_pool_count);

	for (i = 1; i<constant_pool_count; i++)
	{
		int tag;

		tag = fgetc(library_file);
		constant_pool[i].tag = tag;
		constant_pool[i].data = NULL;

		switch (tag)
		{
		case  7: /* ClassInfo */
		case 8: /* StringInfo */
			constant_pool[i].param1 = read_short_int(library_file);
			break;
		case 9: /* Fieldref */
		case 10: /* Methodref */
		case 11: /* Instanceref*/
		case 12: /* NameAndType */
			constant_pool[i].param1 = read_short_int(library_file);
			constant_pool[i].param2 = read_short_int(library_file);
			break;
		case 3: /* Integer */
		case 4: /* Float */
			constant_pool[i].param1 = read_long_int(library_file);
			break;
		case 5: /* Long */
		case 6: /* double */
			constant_pool[i].param1 = read_long_int(library_file);
			constant_pool[i].param2 = read_long_int(library_file);
			break;

		case 1: /* Utf8 */
			constant_pool[i].data_len = read_short_int(library_file);
			constant_pool[i].data = mem_alloc(sizeof(char) * constant_pool[i].data_len);
			fread(constant_pool[i].data, 1, constant_pool[i].data_len, library_file);
			break;

		default:
			add_error_message(449, "", "");
			goto rlf_finally;
		}
	}

	access_flags = read_short_int(library_file);
	this_class= read_short_int(library_file);
	super_class = read_short_int(library_file);

	/* TODO:: check that this class has the proper name, no package etc. */
	/* check access flags */
	if ((access_flags & 0x0200) // ACC_INTERFACE
		|| (access_flags & 0x0400)) // ACC_ABSTRACT
	{
		add_error_message(451, "", "");
	}

	/* TODO:: are there some limitations on super class? */

	/* skip over the implemented interfaces */
	interface_count = read_short_int(library_file);
	for(i=0; i<interface_count; i++)
	{
		read_short_int(library_file);
	}

	/* skip over the fields */
	field_count = read_short_int(library_file);
	for(i=0; i<field_count; i++)
	{
		int attributes_count;
		read_short_int(library_file);
		read_short_int(library_file);
		read_short_int(library_file);
		attributes_count = read_short_int(library_file);
		for(j=0; j<attributes_count; j++)
		{
			int length;
			read_short_int(library_file);
			length = read_long_int(library_file);
			fseek(library_file, length, SEEK_CUR);
		}
	}

	/* read the methods and add them into the program scope */
	methods_count = read_short_int(library_file);

	for (i = 0; i<methods_count; i++)
	{
		short int method_access_flags;
		short int method_name_index;
		short int method_descriptor_index;
		short int attributes_count;

		method_access_flags = read_short_int(library_file);
		method_name_index = read_short_int(library_file);
		method_descriptor_index = read_short_int(library_file);

		/* read the method's attributes */
		attributes_count = read_short_int(library_file);

		/* if the method is declared as 'public static' consider
		adding it into the unit interface */
		if(method_access_flags == 0x0009)
		{
			char *descriptor;
			int parsing_return_type = 0;
			identifier *method;
			string *method_name;
			int j;

			method = identifier_create();
			method->forward_declaration = 0;
			method->standard_function = 0;
			method->parameters = type_list_create();

			/* check that method parameters and return value are OK */
			descriptor = constant_pool[method_descriptor_index].data;

			for(j = 1; j<constant_pool[method_descriptor_index].data_len; j++)
			{
				if (descriptor[j] == ')')
				{
					parsing_return_type = 1;
				}
				else
				if ((descriptor[j] == 'L') && (strncmp(descriptor+j, "Ljava/lang/String;", 18) == 0))
				{
					type* new_type = type_create();
					new_type->type_class = string_type;

					if (!parsing_return_type)
						type_list_append(method->parameters, new_type);
					else
					{
						method->return_type = new_type;
						method->identifier_class = function_name;
					}
					j += 17;
				}
				else
				if ((descriptor[j] == 'L') && (strncmp(descriptor+j, "Ljavax/microedition/lcdui/Image;", 32) == 0))
				{
					type* new_type = type_create();
					new_type->type_class = image_type;

					if (!parsing_return_type)
						type_list_append(method->parameters, new_type);
					else
					{
						method->return_type = new_type;
						method->identifier_class = function_name;
					}
					j += 31;
				}
				else
				if ((descriptor[j] == 'L') && (strncmp(descriptor+j, "Ljavax/microedition/lcdui/Command;", 34) == 0))
				{
					type* new_type = type_create();
					new_type->type_class = command_type;

					if (!parsing_return_type)
						type_list_append(method->parameters, new_type);
					else
					{
						method->return_type = new_type;
						method->identifier_class = function_name;
					}
					j += 33;
				}
				else
				if ((descriptor[j] == 'L') && (strncmp(descriptor+j, "Ljava/io/InputStream;", 21) == 0))
				{
					type* new_type = type_create();
					new_type->type_class = stream_type;

					if (!parsing_return_type)
						type_list_append(method->parameters, new_type);
					else
					{
						method->return_type = new_type;
						method->identifier_class = function_name;
					}
					j += 20;
				}
				else
				if ((descriptor[j] == 'L') && (strncmp(descriptor+j, "Ljavax/microedition/rms/RecordStore;", 36) == 0))
				{
					type* new_type = type_create();
					new_type->type_class = record_store_type;

					if (!parsing_return_type)
						type_list_append(method->parameters, new_type);
					else
					{
						method->return_type = new_type;
						method->identifier_class = function_name;
					}
					j += 35;
				}
				else
				if (descriptor[j] == 'I')
				{
					type* new_type = type_create();
					new_type->type_class = integer_type;
					if (!parsing_return_type)
						type_list_append(method->parameters, new_type);
					else
					{
						method->return_type = new_type;
						method->identifier_class = function_name;
					}
				}
				else
				if ((descriptor[j] == 'V') && (parsing_return_type))
				{
					method->identifier_class = procedure_name;
				}
				else
				{
					add_error_message(452, "", "");
					break;
				}
			}

			method->unit_function = 1;
			method->container_unit = library_unit;
			method_name = string_create();
			method_name->length = constant_pool[method_name_index].data_len;
			method_name->cstr = malloc(method_name->length + 1);
			strcpy(method_name->cstr, constant_pool[method_name_index].data);
			method_name->cstr[method_name->length] = '\0';
			name_table_insert(library_unit->names, method_name, method);
		}

		/* skip over all attributes */
		for (j=0; j<attributes_count; j++)
		{
			long int length;
			int name = read_short_int(library_file);
			length = read_long_int(library_file);
			fseek(library_file, length, SEEK_CUR);
		}
	}



rlf_finally:
	/* free the constant pool from the memory */
	for (i = 1; i<constant_pool_count; i++)
	{
		if (constant_pool[i].data != NULL)
			mem_free(constant_pool[i].data);
	}

	mem_free(constant_pool);

}

void read_symbol_file(FILE *symbol_file, struct unit_struct* library_unit)
{
	char tag;

	while(fread(&tag, 1, 1, symbol_file) == 1)
	{
		switch(tag)
		{
		case 1:
			{ /* constant */
				identifier *constant;
				string* name;
				char type;

				constant = identifier_create();
				constant->identifier_class = constant_name;
				constant->unit_function = 1;
				constant->container_unit = library_unit;

				name = bsf_read_STRING(symbol_file);
				fread(&type, 1, 1, symbol_file);

				constant->constant_type = type_create();

				switch (type)
				{
				case 1:
					constant->constant_type->type_class = integer_type;
					fread(&constant->constant_int_value, sizeof(int), 1, symbol_file);
					break;
				case 2:
					constant->constant_type->type_class = real_type;
					fread(&constant->constant_real_value, sizeof(float), 1, symbol_file);
					break;
				case 3:
					constant->constant_type->type_class = boolean_type;
					fread(&constant->constant_int_value, sizeof(char), 1, symbol_file);
					break;
				case 4:
					constant->constant_type->type_class = char_type;
					fread(&constant->constant_int_value, sizeof(char), 1, symbol_file);
					break;
				case 5:
					constant->constant_type->type_class = string_type;
					constant->constant_string_value = bsf_read_STRING(symbol_file);
					break;
				}

				name_table_insert(library_unit->names, name, constant);
			}
			break;

		case 2:
			{
				string *var_name;
				type  *var_type;
				identifier *variable = identifier_create();

				variable->identifier_class = variable_name;

				var_name = bsf_read_STRING(symbol_file);
				var_type = bsf_read_TYPE(symbol_file);

				variable->variable_type = var_type;
				variable->unit_function = 1;
				variable->container_unit = library_unit;

				name_table_insert(library_unit->names, var_name, variable);
			}
			break;

		case 3:
			{
				string *name;
				identifier *type_identifier = identifier_create();

				type_identifier->identifier_class = type_name;

				name = bsf_read_STRING(symbol_file);
				type_identifier->defined_type = bsf_read_TYPE(symbol_file);
				type_identifier->unit_function = 1;
				type_identifier->container_unit = library_unit;

				name_table_insert(library_unit->names, name, type_identifier);
			}
			break;

		case 4:
			{
				string *name;
				char params;
				identifier* procedure = identifier_create();

				procedure->forward_declaration = 0;
				procedure->standard_function = 0;
				procedure->identifier_class = procedure_name;
				procedure->unit_function = 1;
				procedure->container_unit = library_unit;
				procedure->subprogram_linenum = 1;

				procedure->parameters = type_list_create();

				name = bsf_read_STRING(symbol_file);
				fread(&params, 1, 1, symbol_file);

				while (params > 0)
				{
					type_list_append(procedure->parameters, bsf_read_TYPE(symbol_file));
					params --;
				}

				name_table_insert(library_unit->names, name, procedure);
			}
			break;

		case 5:
			{
				string *name;
				char params;
				identifier* function = identifier_create();

				function->forward_declaration = 0;
				function->standard_function = 0;
				function->identifier_class = function_name;
				function->unit_function = 1;
				function->container_unit = library_unit;
				function->subprogram_linenum = 1;

				function->parameters = type_list_create();

				name = bsf_read_STRING(symbol_file);
				function->return_type = bsf_read_TYPE(symbol_file);
				fread(&params, 1, 1, symbol_file);

				while (params > 0)
				{
					type_list_append(function->parameters, bsf_read_TYPE(symbol_file));
					params --;
				}

				name_table_insert(library_unit->names, name, function);
			}
			break;
		}
	}
}


/*
	Adds parameters names into block.
*/
void add_parameters(block *item, string_list *parameters_list,
					type *parameters_type, int is_parameter_variable)
{
	string_list *it;
	identifier *descriptor;

	it = parameters_list;

	if (it->data == NULL)
	{
		type_destroy(parameters_type);
		return;
	}

	while (it != NULL)
	{
		descriptor = identifier_create();
		descriptor->identifier_class = parameter_name;
		descriptor->parameter_index = item->next_parameter_index;
		item-> next_parameter_index ++;
		descriptor->parameter_type = type_duplicate(parameters_type);
		descriptor->is_parameter_variable = is_parameter_variable;
		name_table_insert(item->names, string_duplicate(it->data), descriptor);

		it = it->next;
	}

	item->next_variable_index = item->next_parameter_index;

	type_destroy(parameters_type);
}


/*
	Checks if a forward declaration corresponds to a real
	function definition.

    The identifier 'declaration' is a function or a
	procedure that is forward.
*/
void check_forward_declaration(identifier *declaration,
							   type_list *parameters,
							   int forward_declaration,
							   type *return_type)
{
	int different_parameter;

	if ((declaration->identifier_class != procedure_name)
		&& (declaration->identifier_class != function_name))
	{
		return;
	}

	if (forward_declaration)
	{
		add_error_message(401, "", "");
		return;
	}

	if ((return_type == NULL) /* it is a procedure */
		&& (declaration->identifier_class == function_name))
		add_error_message(402, "function", "");

	if ((return_type != NULL) /* it is a function*/
		&& (declaration->identifier_class == procedure_name))
		add_error_message(402, "procedure", "");

	different_parameter = type_list_different_parameter(
				parameters, declaration->parameters);

	if (different_parameter == -1)
		add_error_message(403, "", "");

	if (different_parameter > 0)
	{
		char number[8];

		sprintf(number, "%d", different_parameter);
		add_error_message(404, number, "");
	}

	if (return_type != NULL)
	{
		if ((declaration->identifier_class != function_name) || (!type_equal(return_type, declaration->return_type)))
			add_error_message(405, "", "");
	}
}


/*
	Checks if there is any identifier contained in the current block that is
	forward function or procedure delcaration. If there is, report an error.
*/
void check_unmatched_forward_declarations(block *current_block, name_table *node)
{
	int i;
	identifier *declaration;

	if ((node == NULL) || (node->descriptor == NULL))
		return;

	declaration = node->descriptor;

	if (
		((declaration->identifier_class == procedure_name) ||
		  (declaration->identifier_class == function_name))
		&& (declaration->forward_declaration == 1)
		)
	{
		new_linenum=declaration->subprogram_linenum;
		add_error_message(441, "", "");
	}

	check_unmatched_forward_declarations(current_block, node->left_child);
	check_unmatched_forward_declarations(current_block, node->right_child);

}


/*
	Uses a given name to access all the defined types.
	Return a found type, or an error type if no type
	was found.
*/
type *type_from_name(block *current_block, char *cstr)
{
	type *found_type;
	string *name;
	identifier *declaration;


	found_type = type_create();
	found_type->type_class = error_type;

	name = string_from_cstr(cstr);

	while (current_block != NULL)
	{
		declaration = name_table_find(current_block->names, name);

		if (declaration != NULL)
		{
			if (declaration->identifier_class == type_name)
			{
				type_destroy(found_type);
				found_type = type_duplicate(declaration->defined_type);
			}

			break;
		}

		current_block = current_block->parent_block;
	}

	string_destroy(name);
	return found_type;
}


/*
	Retuns the type of the constant given by its name,
	or return error_type if cstr is not a constant name.
*/
type *get_constant_type(block *current_block, char *cstr)
{
	type *found_type;
	string *name;
	identifier *declaration;

	found_type = type_create();
	found_type->type_class = error_type;

	name = string_from_cstr(cstr);

	while (current_block != NULL)
	{
		declaration = name_table_find(current_block->names, name);

		if (declaration != NULL)
		{
			if (declaration->identifier_class == constant_name)
			{
				type_destroy(found_type);
				found_type = type_duplicate(declaration->constant_type);
			}

			break;
		}

		current_block = current_block->parent_block;
	}

	string_destroy(name);
	return found_type;
}


/*
	Return a copy of an identifier associated with a
	constant of a given name, or a none identifier
	if identifier is not a constant or not found.
*/
identifier *get_constant_identifier(block *current_block, char *cstr)
{
	string *name;
	identifier *found_constant = NULL;
	identifier *declaration;

	name = string_from_cstr(cstr);

	while (current_block != NULL)
	{
		declaration = name_table_find(current_block->names, name);

		if (declaration != NULL)
		{
			if (declaration->identifier_class == constant_name)
				found_constant = identifier_duplicate(declaration);

			break;
		}

		current_block = current_block->parent_block;
	}

	string_destroy(name);

	if (found_constant == NULL)
		found_constant = identifier_create();

	return found_constant;
}


/*
	Return an identifier from a given name
*/
identifier *get_identifier(block *current_block, char *cstr)
{
	string *name;
	identifier *found_identifier = NULL;
	identifier *declaration;
	name_table *name_table_it;

	name = string_from_cstr(cstr);

	while (current_block != NULL)
	{
		declaration = name_table_find(current_block->names, name);

		if (declaration != NULL)
		{
			found_identifier = identifier_duplicate(declaration);

			if (current_block->parent_block == root_block)
				found_identifier->belongs_to_program_block = 1;
			else
				found_identifier->belongs_to_program_block = 0;

			break;
		}

		current_block = current_block->parent_block;
	}

	string_destroy(name);

	if (found_identifier != NULL)
		return found_identifier;

	/*
		Search all units for a given identifier. If the same identifier is found
		in more then one unit, output ambiguity error.
	*/
	if (root_block->names->name != NULL)
	{
		name_table_it = root_block->names;

		while (name_table_it != NULL)
		{
			identifier *descriptor = name_table_it->descriptor;

			if (descriptor->identifier_class == unit_name)
			{
				/*
					Search inside the unit for the given name
				*/
				name_table *unit_table_it = descriptor->unit_block->names;

				if (unit_table_it->name != NULL)
				{
					while (unit_table_it != NULL)
					{
						if(STRING_COMPARE(string_get_cstr(unit_table_it->name),
							              cstr) == 0)
						{
							if (found_identifier == NULL)
								found_identifier = identifier_duplicate(unit_table_it->descriptor);
							else
							{
								add_error_message(459, cstr, "");
								found_identifier = NULL;
								goto unit_search_done;
							}
						}


						unit_table_it = unit_table_it->next;
					}
				}
			}

			name_table_it = name_table_it->next;
		}
	}

unit_search_done:

	if (found_identifier == NULL)
	{
		found_identifier = identifier_create();
		found_identifier->belongs_to_program_block = 0;
	}

	return found_identifier;
}


/*
	Returns the type of a variable given by its name.
	Return an error type if no variable was found.
*/
type *get_variable_type(block *current_block, char *cstr)
{
	type *found_type;
	string *name;
	identifier *declaration;

	found_type = type_create();
	found_type->type_class = error_type;

	name = string_from_cstr(cstr);

	while (current_block != NULL)
	{
		declaration = name_table_find(current_block->names, name);

		if (declaration != NULL)
		{
			if (declaration->identifier_class == variable_name)
				found_type = type_duplicate(declaration->variable_type);

			break;
		}

		current_block = current_block->parent_block;
	}

	string_destroy(name);
	return found_type;
}

/*
	Create the bytecode to initialize the new variable.
*/
void initialize_variable(block *item, identifier *variable, char *name, int is_static, char *class_name)
{
	int storeIntoField = 0;

	/* if the variable belongs to the program block */
	if (variable->belongs_to_program_block)
	{
		storeIntoField = 1;
	}

	switch(variable->variable_type->type_class)
	{
	case integer_type:
	case char_type:
	case boolean_type:
		{
			bytecode_append(item->code, iconst_0$);

			if (!storeIntoField)
			{
				switch(variable->variable_index)
				{
				case 0:
					bytecode_append(item->code, istore_0$);
					break;
				case 1:
					bytecode_append(item->code, istore_1$);
					break;
				case 2:
					bytecode_append(item->code, istore_2$);
					break;
				case 3:
					bytecode_append(item->code, istore_3$);
					break;
				default:
					bytecode_append(item->code, istore$);
					bytecode_append(item->code, (char)variable->variable_index);
					break;
				}
			}
		}
		break; // case integer_type

	case real_type:
		{
			if (mathType == 1)
			{
				bytecode_append(item->code, iconst_0$);

				if (!storeIntoField)
				{
					switch(variable->variable_index)
					{
					case 0:
						bytecode_append(item->code, istore_0$);
						break;
					case 1:
						bytecode_append(item->code, istore_1$);
						break;
					case 2:
						bytecode_append(item->code, istore_2$);
						break;
					case 3:
						bytecode_append(item->code, istore_3$);
						break;
					default:
						bytecode_append(item->code, istore$);
						bytecode_append(item->code, (char)variable->variable_index);
						break;
					}
				}
			}
			else
			{
				int method_index;
				int class_index;

				usesFloat = 1;

				class_index = cp_add_class("Real");
				method_index = cp_add_methodref("Real", "<init>", "()V");

				bytecode_append(item->code, new$);
				bytecode_append_short_int(item->code, (short)class_index);
				bytecode_append(item->code, dup$);
				bytecode_append(item->code, invokespecial$);
				bytecode_append_short_int(item->code, (short)method_index);

				if (!storeIntoField)
				{
					switch(variable->variable_index)
					{
					case 0:
						bytecode_append(item->code, astore_0$);
						break;
					case 1:
						bytecode_append(item->code, astore_1$);
						break;
					case 2:
						bytecode_append(item->code, astore_2$);
						break;
					case 3:
						bytecode_append(item->code, astore_3$);
						break;
					default:
						bytecode_append(item->code, astore$);
						bytecode_append(item->code, variable->variable_index);
						break;
					}
				}
			}
		}
		break;


	case string_type:
		{
			int method_index;
			int class_index;

			class_index = cp_add_class("java/lang/String");
			method_index = cp_add_methodref("java/lang/String", "<init>", "()V");

			bytecode_append(item->code, new$);
			bytecode_append_short_int(item->code, (short)class_index);
			bytecode_append(item->code, dup$);
			bytecode_append(item->code, invokespecial$);
			bytecode_append_short_int(item->code, (short)method_index);

			if (!storeIntoField)
			{
				switch(variable->variable_index)
				{
				case 0:
					bytecode_append(item->code, astore_0$);
					break;
				case 1:
					bytecode_append(item->code, astore_1$);
					break;
				case 2:
					bytecode_append(item->code, astore_2$);
					break;
				case 3:
					bytecode_append(item->code, astore_3$);
					break;
				default:
					bytecode_append(item->code, astore$);
					bytecode_append(item->code, variable->variable_index);
					break;
				}
			}
		}
		break;

	case image_type:
		{
			int method_index = cp_add_methodref("javax/microedition/lcdui/Image", "createImage", "(II)Ljavax/microedition/lcdui/Image;");

			bytecode_append(item->code, iconst_1$);
			bytecode_append(item->code, iconst_1$);
			bytecode_append(item->code, invokestatic$);
			bytecode_append_short_int(item->code, method_index);

			if (!storeIntoField)
			{
				switch(variable->variable_index)
				{
				case 0:
					bytecode_append(item->code, astore_0$);
					break;
				case 1:
					bytecode_append(item->code, astore_1$);
					break;
				case 2:
					bytecode_append(item->code, astore_2$);
					break;
				case 3:
					bytecode_append(item->code, astore_3$);
					break;
				default:
					bytecode_append(item->code, astore$);
					bytecode_append(item->code, (char)variable->variable_index);
					break;
				}
			}
		}
		break; // case image_type


		case stream_type:
		{
			bytecode_append(item->code, aconst_null$);

			if (!storeIntoField)
			{
				switch(variable->variable_index)
				{
				case 0:
					bytecode_append(item->code, astore_0$);
					break;
				case 1:
					bytecode_append(item->code, astore_1$);
					break;
				case 2:
					bytecode_append(item->code, astore_2$);
					break;
				case 3:
					bytecode_append(item->code, astore_3$);
					break;
				default:
					bytecode_append(item->code, astore$);
					bytecode_append(item->code, (char)variable->variable_index);
					break;
				}
			}
		}
		break; // case stream_type

	case command_type:
		{
			int method_index = cp_add_methodref("javax/microedition/lcdui/Command", "<init>", "(Ljava/lang/String;II)V");

			bytecode_append(item->code, new$);
			bytecode_append_short_int(item->code, cp_add_class("javax/microedition/lcdui/Command"));
			bytecode_append(item->code, dup$);

			bytecode_append(item->code, new$);
			bytecode_append_short_int(item->code, cp_add_class("java/lang/String"));
			bytecode_append(item->code, dup$);
			bytecode_append(item->code, invokespecial$);
			bytecode_append_short_int(item->code, cp_add_methodref("java/lang/String", "<init>", "()V"));

			bytecode_append(item->code, iconst_1$);
			bytecode_append(item->code, iconst_1$);
			bytecode_append(item->code, invokespecial$);
			bytecode_append_short_int(item->code, method_index);

			if (!storeIntoField)
			{
				switch(variable->variable_index)
				{
				case 0:
					bytecode_append(item->code, astore_0$);
					break;
				case 1:
					bytecode_append(item->code, astore_1$);
					break;
				case 2:
					bytecode_append(item->code, astore_2$);
					break;
				case 3:
					bytecode_append(item->code, astore_3$);
					break;
				default:
					bytecode_append(item->code, astore$);
					bytecode_append(item->code, (char)variable->variable_index);
					break;
				}
			}
		}
		break; // case command_type

		case http_type:
		{
			int method_index = cp_add_methodref("H", "<init>", "()V");
			usesHttp = 1;

			bytecode_append(item->code, new$);
			bytecode_append_short_int(item->code, cp_add_class("H"));
			bytecode_append(item->code, dup$);

			bytecode_append(item->code, invokespecial$);
			bytecode_append_short_int(item->code, method_index);

			if (!storeIntoField)
			{
				switch(variable->variable_index)
				{
				case 0:
					bytecode_append(item->code, astore_0$);
					break;
				case 1:
					bytecode_append(item->code, astore_1$);
					break;
				case 2:
					bytecode_append(item->code, astore_2$);
					break;
				case 3:
					bytecode_append(item->code, astore_3$);
					break;
				default:
					bytecode_append(item->code, astore$);
					bytecode_append(item->code, (char)variable->variable_index);
					break;
				}
			}
		}
		break; // case http_type

	case record_store_type:
		{
			usesRecordStore = 1;
			/* no initialization needeed */
		}
		break;

	case array_type:
		{
			int count = 0;
			type_list *it = variable->variable_type->dimensions_list;
			char *descriptor;
			descriptor = (char*) mem_alloc(1024);
			if (descriptor == NULL)
				die(1);

			get_field_descriptor(variable->variable_type, descriptor);

			/* put dimensions on to the stack */
			while (it != NULL)
			{
				if (it->data != NULL)
				{
					int size;
					size = it->data->last_element - it->data->first_element + 1;

					if (size <= 0) {
						add_error_message(437, "", "");
					} else if (size <= 127) {
						bytecode_append(item->code, bipush$);
						bytecode_append(item->code, (char)size);
					} else if (size <= 32767) {
						bytecode_append(item->code, sipush$);
						bytecode_append_short_int(item->code, (short)size);
					} else {
						add_error_message(438, "", "");
					}
				}
				it = it->next;

				count ++;
			}

			bytecode_append(item->code, multianewarray$);
			bytecode_append_short_int(item->code, cp_add_class(descriptor));
			bytecode_append(item->code, (char)count);

			// NEW:
			create_put_variable_bytecode(variable, item->code, name, storeIntoField);

			if((variable->variable_type->element_type->type_class == record_type)
				|| (variable->variable_type->element_type->type_class == string_type)
				|| ((variable->variable_type->element_type->type_class == real_type) && (mathType != 1))
				|| (variable->variable_type->element_type->type_class == image_type)
				|| (variable->variable_type->element_type->type_class == command_type))
			{
				/* initialize the fields */
				unsigned short int c = 0;
				char descriptor[128];
				unsigned short int num;
				int *offsets;
				int *dimensions_sizes; // j-a-s-d: OOB in initialization of complex-type multidimensional arrays bug fix
				type_list *it = variable->variable_type->dimensions_list;

				while (it != NULL)
				{
					it = it->next;
					c ++;
				}

				if (variable->variable_type->element_type->type_class == real_type)
					usesFloat = 1;

				offsets = (int*) mem_alloc(c * sizeof(int));
				dimensions_sizes = (int*) mem_alloc(c * sizeof(int)); // j-a-s-d: OOB in initialization of complex-type multidimensional arrays bug fix
				num = c;

				/* reset */
				it = variable->variable_type->dimensions_list;
				c = 0;
				while (it != NULL)
				{
					if (it->data != NULL)
					{
						bytecode_append(item->code, getstatic$);
						bytecode_append_short_int(item->code, cp_add_fieldref("M", "IC", "[I"));
						bytecode_append(item->code, sipush$);
						bytecode_append_short_int(item->code, c);
						bytecode_append(item->code, iconst_0$);
						bytecode_append(item->code, iastore$);

						offsets[c] = item->code->bytecode_pos;
						dimensions_sizes[c] = it->data->last_element-it->data->first_element; // j-a-s-d: OOB in initialization of complex-type multidimensional arrays bug fix
					}
					c++;
					it = it->next;
				}

				/* load */
				for(c=0; c<num; c++)
				{
					if ((c < (num-1)) || (c == 0))
					{
						// OLD: bytecode_append(item->code, dup$);
						// NEW:
						create_variable_bytecode(variable, item->code, name, storeIntoField);
					}
					bytecode_append(item->code, getstatic$);
					bytecode_append_short_int(item->code, cp_add_fieldref("M", "IC", "[I"));
					bytecode_append(item->code, sipush$);
					bytecode_append_short_int(item->code, c);
					bytecode_append(item->code, iaload$);
					if (c < (num-1))
						bytecode_append(item->code, aaload$);
				}

				/* new */
				get_field_descriptor(variable->variable_type->element_type, descriptor);
				descriptor[strlen(descriptor)-1] = '\0';

				if (variable->variable_type->element_type->type_class == image_type)
				{
					bytecode_append(item->code, iconst_1$);
					bytecode_append(item->code, iconst_1$);
					bytecode_append(item->code, invokestatic$);
					bytecode_append_short_int(item->code, cp_add_methodref("javax/microedition/lcdui/Image", "createImage", "(II)Ljavax/microedition/lcdui/Image;"));
				}
				else if (variable->variable_type->element_type->type_class == command_type)
				{
					int method_index = cp_add_methodref("javax/microedition/lcdui/Command", "<init>", "(Ljava/lang/String;II)V");

					bytecode_append(item->code, new$);
					bytecode_append_short_int(item->code, cp_add_class("javax/microedition/lcdui/Command"));
					bytecode_append(item->code, dup$);

					bytecode_append(item->code, new$);
					bytecode_append_short_int(item->code, cp_add_class("java/lang/String"));
					bytecode_append(item->code, dup$);
					bytecode_append(item->code, invokespecial$);
					bytecode_append_short_int(item->code, cp_add_methodref("java/lang/String", "<init>", "()V"));

					bytecode_append(item->code, iconst_1$);
					bytecode_append(item->code, iconst_1$);
					bytecode_append(item->code, invokespecial$);
					bytecode_append_short_int(item->code, method_index);
				}
				else
				{
					bytecode_append(item->code, new$);
					bytecode_append_short_int(item->code, cp_add_class(descriptor+1));
					bytecode_append(item->code, dup$);
					bytecode_append(item->code, invokespecial$);
					bytecode_append_short_int(item->code, cp_add_methodref(descriptor+1, "<init>", "()V"));
				}

				bytecode_append(item->code, aastore$);

				/* jump back */
				it = variable->variable_type->dimensions_list;
				c = num-1;
				while (it != NULL)
				{
					if (it->data != NULL)
					{
						short int offset;
						bytecode_append(item->code, getstatic$);
						bytecode_append_short_int(item->code, cp_add_fieldref("M", "IC", "[I"));
						bytecode_append(item->code, dup$);
						bytecode_append(item->code, sipush$);
						bytecode_append_short_int(item->code, c);
						bytecode_append(item->code, iaload$);
						bytecode_append(item->code, iconst_1$);
						bytecode_append(item->code, iadd$);
						bytecode_append(item->code, sipush$);
						bytecode_append_short_int(item->code, c);
						bytecode_append(item->code, swap$);
						bytecode_append(item->code, iastore$);
						/* the value is increased by one */

						bytecode_append(item->code, getstatic$);
						bytecode_append_short_int(item->code, cp_add_fieldref("M", "IC", "[I"));
						bytecode_append(item->code, sipush$);
						bytecode_append_short_int(item->code, c);
						bytecode_append(item->code, iaload$);
						bytecode_append(item->code, sipush$);
						bytecode_append_short_int(item->code,
							//it->data->last_element-it->data->first_element);
							dimensions_sizes[c]); // j-a-s-d: OOB in initialization of complex-type multidimensional arrays bug fix
						bytecode_append(item->code, if_icmple$);
						offset = offsets[c] - item->code->bytecode_pos + 1;
						bytecode_append_short_int(item->code, offset);
					}
					c--;
					it = it->next;
				}

				mem_free(offsets);
			}

		/*OLD:	if (!storeIntoField)
			{
				switch(variable->variable_index)
				{
				case 0:
					bytecode_append(item->code, astore_0$);
					break;
				case 1:
					bytecode_append(item->code, astore_1$);
					break;
				case 2:
					bytecode_append(item->code, astore_2$);
					break;
				case 3:
					bytecode_append(item->code, astore_3$);
					break;
				default:
					bytecode_append(item->code, astore$);
					bytecode_append(item->code, (char)variable->variable_index);
					break;
				}
			} */

			mem_free(descriptor);

			break;
		} // case array_type

	case record_type:
		{
			char *descriptor;
			descriptor = (char*) mem_alloc(1024);

			if(descriptor == NULL)
				die(1);

			get_field_descriptor(variable->variable_type, descriptor);

			descriptor[strlen(descriptor) - 1] = '\0';

			bytecode_append(item->code, new$);
			bytecode_append_short_int(item->code, cp_add_class(descriptor + 1));
			bytecode_append(item->code, dup$);
			bytecode_append(item->code, invokespecial$);
			bytecode_append_short_int(item->code, cp_add_methodref(descriptor + 1, "<init>", "()V"));

			mem_free(descriptor);

			if (!storeIntoField)
			{
				switch(variable->variable_index)
				{
				case 0:
					bytecode_append(item->code, astore_0$);
					break;
				case 1:
					bytecode_append(item->code, astore_1$);
					break;
				case 2:
					bytecode_append(item->code, astore_2$);
					break;
				case 3:
					bytecode_append(item->code, astore_3$);
					break;
				default:
					bytecode_append(item->code, astore$);
					bytecode_append(item->code, (char)variable->variable_index);
					break;
				}
			}
		}
		break;

	case error_type:
		break;

	default:
		{
			die(14);
		}
	}


	/* create the bytecode for storing data into the field */
	if ((storeIntoField)
		&& (variable->variable_type->type_class != record_store_type)
		&& (variable->variable_type->type_class != array_type)) /* array initialization stores the variable */
	{
		int field_index;
		char descriptor[512];
		get_field_descriptor(variable->variable_type, descriptor);

		if (name == NULL)
			die(23);

		lowercase(name);
		field_index = cp_add_fieldref(class_name, name, descriptor);

		if (is_static)
			bytecode_append(item->code, putstatic$);
		else
			bytecode_append(item->code, putfield$);
		bytecode_append_short_int(item->code, (short)field_index);
	}
}


void transform_break_stmts(bytecode *code, int start_offset, int end_offset, int jump_pos)
{
	int jump_offset;
	int pos = start_offset;

	while (pos < end_offset)
	{
		if (((unsigned char)(code->bytecode[pos]) == (unsigned char)break_stmt$)
			&& ((pos + 2) < end_offset))
		{
			if ((code->bytecode[pos+1] == 0)
				&& (code->bytecode[pos+2] == 0))
			{
				jump_offset = jump_pos - pos;
				code->bytecode[pos] = goto$;
				code->bytecode[pos+1] = (jump_offset) >> 8;
				code->bytecode[pos+2] = jump_offset;
			}
		}

		pos ++;
	}
}


/*
	Writes the class file based on the information provided in the
	root block.
*/
void create_class_file(block *program_block, FILE *fp)
{
	int this_class_index;
	int super_class_index;
	int i;

	/*** Insert the additional data into the constant pool ***/
	if (compiling_unit == 0)
	{
		this_class_index = cp_add_class("M");
		switch (canvasType)
		{
		case NORMAL:
			super_class_index = cp_add_class("javax/microedition/lcdui/Canvas");
			break;
		case FULL_NOKIA:
			super_class_index = cp_add_class("com/nokia/mid/ui/FullCanvas");
			break;
		case FULL_MIDP20:
			super_class_index = cp_add_class("javax/microedition/lcdui/game/GameCanvas");
			break;
		}
	}
	else
	{
		this_class_index = cp_add_class(string_get_cstr(str_program_name));
		super_class_index = cp_add_class("java/lang/Object");
	}


	/* Insert the data needed by the methods into the constant pool */
	write_method(program_block, NULL);
	for(i=0; i<program_block->children_count; i++)
	{
		write_method(program_block->children[i], NULL);
	}
	write_block_fields(program_block->names, NULL);

	if (compiling_unit == 0)
	{
		write_constructor(NULL);
		write_paint_method(NULL);
		write_run_method(NULL);
		write_keypressed_method(NULL);
		write_keyreleased_method(NULL);
	}

	/* add data to constant poll that is later needed */
	cp_add_utf8("I");
	cp_add_utf8("T");
	cp_add_utf8("G");
	cp_add_utf8("KC");
	cp_add_utf8("KP");
	cp_add_utf8("IC");
	cp_add_utf8("[I");
	cp_add_utf8("Ljavax/microedition/lcdui/Image;");
	cp_add_utf8("LM;");
	cp_add_class("M");
	cp_add_utf8("Ljavax/microedition/lcdui/Graphics;");
	cp_add_class("java/lang/Runnable");
	cp_add_utf8("StackMap");

	/*** Write data to the file ***/

	/* write the header */
	create_class_file_header(fp);

	/* write constant pool */
	write_constant_pool(fp);

	/* write the access flags, set to ACC_PUBLIC and ACC_SUPER */
	write_short_int(fp, 0x0021);

	/* write the index to constat pool with this class description */
	write_short_int(fp, (short)this_class_index);

	/* write the index to constant pool with the super class description */
	write_short_int(fp, (short)super_class_index);

	if (compiling_unit == 0)
	{
		/* we have 1 interface */
		write_short_int(fp, 1);

		write_short_int(fp, cp_add_class("java/lang/Runnable"));
	}
	else
	{
		/* unit has 0 interfaces */
		write_short_int(fp, 0);
	}

	/* write the fields (global variables in pascal) */
	if (compiling_unit == 0)
	{
		write_short_int(fp, (short)(program_block->next_variable_index + 7));

		/* write the RNG field */
		{
			/* access flags: ACC_PUBLIC and ACC_STATIC */
			write_short_int(fp, 0x0001 | 0x0008);

			/* write the name index */
			write_short_int(fp, cp_add_utf8("RNG"));

			/* write the descriptor index */
			write_short_int(fp, cp_add_utf8("Ljava/util/Random;"));

			/* write 0 attributes */
			write_short_int(fp, 0);
		}

		/* write the image (I) field */
		{
			/* access flags: ACC_PUBLIC and ACC_STATIC */
			write_short_int(fp, 0x0001 | 0x0008);

			/* write the name index */
			write_short_int(fp, cp_add_utf8("I"));

			/* write the descriptor index */
			write_short_int(fp, cp_add_utf8("Ljavax/microedition/lcdui/Image;"));

			/* write 0 attributes */
			write_short_int(fp, 0);
		}

		/* write the this (T) field */
		{
			/* access flags: ACC_PUBLIC and ACC_STATIC */
			write_short_int(fp, 0x0001 | 0x0008);

			/* write the name index */
			write_short_int(fp, cp_add_utf8("T"));

			/* write the descriptor index */
			write_short_int(fp, cp_add_utf8("LM;"));

			/* write 0 attributes */
			write_short_int(fp, 0);
		}


		/* write the graphics (G) field */
		{
			/* access flags: ACC_PUBLIC and ACC_STATIC */
			write_short_int(fp, 0x0001 | 0x0008);

			/* write the name index */
			write_short_int(fp, cp_add_utf8("G"));

			/* write the descriptor index */
			write_short_int(fp, cp_add_utf8("Ljavax/microedition/lcdui/Graphics;"));

			/* write 0 attributes */
			write_short_int(fp, 0);
		}

		/* write the key clicked (KC) field */
		{
			/* access flags: ACC_PUBLIC and ACC_STATIC */
			write_short_int(fp, 0x0001 | 0x0008);

			/* write the name index */
			write_short_int(fp, cp_add_utf8("KC"));

			/* write the descriptor index */
			write_short_int(fp, cp_add_utf8("I"));

			/* write 0 attributes */
			write_short_int(fp, 0);
		}

		/* write the key pressed (KP) field */
		{
			/* access flags: ACC_PUBLIC and ACC_STATIC */
			write_short_int(fp, 0x0001 | 0x0008);

			/* write the name index */
			write_short_int(fp, cp_add_utf8("KP"));

			/* write the descriptor index */
			write_short_int(fp, cp_add_utf8("I"));

			/* write 0 attributes */
			write_short_int(fp, 0);
		}

		/* write the INDEX COUNTER (IC) field */
		{
			/* access flags: ACC_PUBLIC and ACC_STATIC */
			write_short_int(fp, 0x0001 | 0x0008);

			/* write the name index */
			write_short_int(fp, cp_add_utf8("IC"));

			/* write the descriptor index */
			write_short_int(fp, cp_add_utf8("[I"));

			/* write 0 attributes */
			write_short_int(fp, 0);
		}
	}
	else
	{
		write_short_int(fp, (short)(program_block->next_variable_index));
	}

	write_block_fields(program_block->names, fp);


	/* write the methods */
	if (compiling_unit == 0)
	{
		/* write the methods count */
		write_short_int(fp, (short)(program_block->children_count + 6));

		/* write the constructor */
		write_constructor(fp);

		/* write the paint method */
		write_paint_method(fp);

		/* write the run method */
		write_run_method(fp);

		/* write the key pressed and key released methods */
		write_keypressed_method(fp);
		write_keyreleased_method(fp);
	}
	else
	{
		write_short_int(fp, (short)(program_block->children_count + 1));
	}

	/* write all other methods */
	for(i=0; i<program_block->children_count; i++)
	{
		write_method(program_block->children[i], fp);
	}

	/* write the main (R) method - <clinit> for interfaces */
	write_method(program_block, fp);


	/* we have no attributes */
	write_short_int(fp, 0);


	// TODO: deallocate constant pool
}


/*
	Writes the method to the file, if fp is NULL only add needeed
	data to the constant pool
*/
void write_method(block *current_block, FILE *fp)
{
	identifier *block_identifier = NULL;
	stack_map_list *map_list;
	int code_attribute_length, code_attribute_length_offset;
	int stack_map_attribute_offset;
	int stack_map_size;
	int stack_map_entries;
	int tmp_offset;

	/* write access flags, ACC_PUBLIC and ACC_STATIC */
	write_short_int(fp, 0x0001 | 0x0008);

	if(current_block->parent_block == root_block)
	{
		/*** it is the main block ***/

		/* write name index */
		if (compiling_unit == 0)
			write_short_int(fp, cp_add_utf8("R"));
		else
			write_short_int(fp, cp_add_utf8("<clinit>"));

		/* write the descriptor */
		write_short_int(fp, cp_add_utf8("()V"));
	}
	else
	{
		char *descriptor;
		int pos = 1;
		type_list *it;

		descriptor = (char*) mem_alloc(5*1024);

		if (descriptor == NULL)
			die(1);

		descriptor[0] = '(';

		/*** procedure or function ***/

		/* write name index */
		lowercase(current_block->block_name->cstr);
		write_short_int(fp, cp_add_utf8(current_block->block_name->cstr));

		/* write the descriptor */
		block_identifier = name_table_find(current_block->parent_block->names, current_block->block_name);

		if ((block_identifier == NULL)
			|| (block_identifier->identifier_class != function_name
			&& block_identifier->identifier_class != procedure_name))
		{
			mem_free(descriptor);
			die(17);
		}

		it = block_identifier->parameters;

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

		write_short_int(fp, cp_add_utf8(descriptor));

		mem_free(descriptor);
	}

//PREVERIFY	map_list = preverify_bytecode(current_block->code, block_identifier);
	        map_list = NULL;


	/* write the code */
	write_short_int(fp, 2); /* attribute count */

	write_short_int(fp, cp_add_utf8("Code"));
	if (fp != NULL)
		code_attribute_length_offset = ftell(fp);
	write_long_int(fp, current_block->code->bytecode_pos + 12); /* update the size later when we know it */
	code_attribute_length = current_block->code->bytecode_pos + 12;

	write_short_int(fp, 32); /* max stack */

	if (current_block->parent_block == root_block)
		write_short_int(fp, 0); /* max locals for program block */
	else
		write_short_int(fp, current_block->next_variable_index + 2); /* max locals */

	write_long_int(fp, current_block->code->bytecode_pos); /* code length */

	if(fp != NULL)
		fwrite(current_block->code->bytecode, current_block->code->bytecode_pos, 1, fp);

	write_short_int(fp, 0);
	if (map_list != NULL)
	{
		write_short_int(fp, 1); /* The Code has one attribute (The StackMap attribute) */

		write_short_int(fp, cp_add_utf8("StackMap"));
		if (fp != NULL)
			stack_map_attribute_offset = ftell(fp);
		write_long_int(fp, 0); /* attribute length */
		write_short_int(fp, 0); /* number of entries; these two fields will be filled in later */

		stack_map_size = 2;
		stack_map_entries = 0;

		do
		{
			int i;
			stack_map *map = stack_map_list_get(&map_list);

			if (map != NULL)
				write_short_int(fp, (short)(map->bytecode_offset));
			stack_map_size += 2;

			/* write the locals */
			if (current_block->parent_block == root_block)
			{
				write_short_int(fp, 0);
				stack_map_size += 2;
			}
			else
			{
				identifier *block_identifier;

				/* write the StackMap for locals */
				block_identifier = name_table_find(current_block->parent_block->names, current_block->block_name);

				write_short_int(fp, (short)current_block->next_variable_index);
				stack_map_size += 2;

				write_locals_stackmap(fp, block_identifier, &stack_map_size);
	 		}

			if (map == NULL)
				break;

			/* write the stack items */
			write_short_int(fp, (short)map->number_of_items);
			stack_map_size += 2;
			for(i=0; i<map->number_of_items; i++)
			{
				if (fp != NULL)
				{
					fwrite(&(map->stack[i]->item_type), 1, 1, fp);
				}

				if ((map->stack[i]->item_type == ITEM_Object)
					|| (map->stack[i]->item_type == ITEM_NewObject))
				{
					write_short_int(fp, map->stack[i]->additional_data);
					stack_map_size += 2;
				}

				stack_map_size ++;
			}

			stack_map_destroy(map);

			stack_map_entries ++;

		} while (map_list != NULL);

		/* write the stack map size and the number of entries and
		the code attribute length */
		if (fp != NULL)
		{
			tmp_offset = ftell(fp);
			fseek(fp, stack_map_attribute_offset, SEEK_SET);
			write_long_int(fp, stack_map_size); /* attribute length */
			write_short_int(fp, (short)stack_map_entries); /* number of entries */

			fseek(fp, code_attribute_length_offset, SEEK_SET);
			write_long_int(fp, code_attribute_length + 6 + stack_map_size);

			fseek(fp, tmp_offset, SEEK_SET);
		}
	}
	else
	{
		write_short_int(fp, 0);
	}

	/* write the Exceptions attribute */
	write_short_int(fp, cp_add_utf8("Exceptions"));

	write_long_int(fp, 4);

	write_short_int(fp, 1);
	write_short_int(fp, cp_add_class("java/lang/Exception"));

}


/*
	Creates the code for the following method:

  	public void paint(Graphics g)
	{
		g.drawImage(IMG, 0, 0, Graphics.TOP|Graphics.LEFT);
	}
*/
void write_paint_method(FILE *fp)
{
	bytecode *code = bytecode_create();
	if (code == NULL)
		die(1);

	/* create the code */

	/* PUSH param_1 */
	bytecode_append(code, aload_1$);

	/* PUSH (I) */
	bytecode_append(code, getstatic$);
	bytecode_append_short_int(code, cp_add_fieldref("M", "I", "Ljavax/microedition/lcdui/Image;"));

	/* PUSH 0; PUSH 0*/
	bytecode_append(code, iconst_0$);
	bytecode_append(code, iconst_0$);

	/* PUSH Graphics.LEFT|Graphics.TOP */
	bytecode_append(code, bipush$);
	bytecode_append(code, 20);

	/* invoke drawImage(Image, int, int, int) */
	bytecode_append(code, invokevirtual$);
	bytecode_append_short_int(code,
		cp_add_methodref("javax/microedition/lcdui/Graphics", "drawImage", "(Ljavax/microedition/lcdui/Image;III)V"));

	bytecode_append(code, return$);

	/* write the method headers */

	/* write access flags, ACC_PUBLIC  */
	write_short_int(fp, 0x0001);

	/* write method name */
	write_short_int(fp, cp_add_utf8("paint"));

	/* write method descriptor */
	write_short_int(fp, cp_add_utf8("(Ljavax/microedition/lcdui/Graphics;)V"));

	/* write 1 attribute */
	write_short_int(fp, 1);

	/* write the Code attribute */
	write_short_int(fp, cp_add_utf8("Code"));
	write_long_int(fp, code->bytecode_pos + 12);

	/* write the max stack */
	write_short_int(fp, 10);

	/* max locals for program block */
	write_short_int(fp, 2);


	/* code length */
	write_long_int(fp, code->bytecode_pos);

	/* write the code itself */
	if (fp != NULL)
		fwrite(code->bytecode, 1, code->bytecode_pos, fp);

	bytecode_destroy(code);

	write_short_int(fp, 0);
	write_short_int(fp, 0);
}

/*
	Creates the code for the following method:

  	public void run()
	{
		R();
	}
*/
void write_run_method(FILE *fp)
{
	char tag;

	bytecode *code = bytecode_create();
	if (code == NULL)
		die(1);

	/* create the code */
	bytecode_append(code, invokestatic$);
	bytecode_append_short_int(code, cp_add_methodref("M", "R", "()V"));

	bytecode_append(code, goto$);
	bytecode_append_short_int(code, 4);

	bytecode_append(code, pop$);	/* the exception handler */

	bytecode_append(code, return$);

	/* write the method headers */

	/* write access flags, ACC_PUBLIC  */
	write_short_int(fp, 0x0001);

	/* write method name */
	write_short_int(fp, cp_add_utf8("run"));

	/* write method descriptor */
	write_short_int(fp, cp_add_utf8("()V"));

	/* write 1 attribute */
	write_short_int(fp, 1);

	/* write the Code attribute */
	write_short_int(fp, cp_add_utf8("Code"));
	write_long_int(fp, code->bytecode_pos + 20 + 38); /* 38 = stackmap size */

	/* write the max stack */
	write_short_int(fp, 2);

	/* max locals for program block */
	write_short_int(fp, 1);

	/* code length */
	write_long_int(fp, code->bytecode_pos);

	/* write the code itself */
	if (fp != NULL)
		fwrite(code->bytecode, 1, code->bytecode_pos, fp);

	bytecode_destroy(code);

	/* write exception table length*/
	write_short_int(fp, 1);

	/* write the exception table */
	write_short_int(fp, 0);
	write_short_int(fp, 3);
	write_short_int(fp, 6);
	write_short_int(fp, cp_add_class("java/lang/Exception"));


	/* StackMap attribute */
	write_short_int(fp, 1);

		write_short_int(fp, cp_add_utf8("StackMap"));
		write_long_int(fp, 32);

		/* 2 frames*/
		write_short_int(fp, 3);

		/* write the frame 0 */
			write_short_int(fp, 0);

			/* write the locals */
			write_short_int(fp, 1);
			tag = 7;
			if (fp != NULL)
				fwrite(&tag, 1, 1, fp);
			write_short_int(fp, cp_add_class("M"));


			/* 0 stack entries */
			write_short_int(fp, 0);

		/* write the frame 1 */
			write_short_int(fp, 6);

			/* write the locals*/
			write_short_int(fp, 1);
			tag = 7;
			if (fp != NULL)
				fwrite(&tag, 1, 1, fp);
			write_short_int(fp, cp_add_class("M"));

			/* 1 stack entry */
			write_short_int(fp, 1);
			tag = 7;
			if (fp != NULL)
				fwrite(&tag, 1, 1, fp);
			write_short_int(fp, cp_add_class("java/lang/Exception"));


		/* write the frame 2 */
			write_short_int(fp, 7);

			/* write the locals */
			write_short_int(fp, 1);
			tag = 7;
			if (fp != NULL)
				fwrite(&tag, 1, 1, fp);
			write_short_int(fp, cp_add_class("M"));

			/* 0 stack entries */
			write_short_int(fp, 0);

}


/*
	Creates the code for the following methods:

  	public void keyPressed(int keyCode)
	{
		KC = keyCode;
		KP = keyCode;
	}

    public void keyReleased(int keyCode)
	{
		KP = 0;
	}
*/
void write_keypressed_method(FILE *fp)
{
	bytecode *code = bytecode_create();
	if (code == NULL)
		die(1);

	/* create the code */
	bytecode_append(code, iload_1$);
	bytecode_append(code, dup$);
	bytecode_append(code, putstatic$);
	bytecode_append_short_int(code, cp_add_fieldref("M", "KC", "I"));
	bytecode_append(code, putstatic$);
	bytecode_append_short_int(code, cp_add_fieldref("M", "KP", "I"));

	bytecode_append(code, return$);

	/* write the method headers */

	/* write access flags, ACC_PUBLIC  */
	write_short_int(fp, 0x0001);

	/* write method name */
	write_short_int(fp, cp_add_utf8("keyPressed"));

	/* write method descriptor */
	write_short_int(fp, cp_add_utf8("(I)V"));

	/* write 1 attribute */
	write_short_int(fp, 1);

	/* write the Code attribute */
	write_short_int(fp, cp_add_utf8("Code"));
	write_long_int(fp, code->bytecode_pos + 12);

	/* write the max stack */
	write_short_int(fp, 2);

	/* max locals for program block */
	write_short_int(fp, 2);


	/* code length */
	write_long_int(fp, code->bytecode_pos);

	/* write the code itself */
	if (fp != NULL)
		fwrite(code->bytecode, 1, code->bytecode_pos, fp);

	bytecode_destroy(code);

	write_short_int(fp, 0);
	write_short_int(fp, 0);
}
void write_keyreleased_method(FILE *fp)
{
	bytecode *code = bytecode_create();
	if (code == NULL)
		die(1);

	/* create the code */
	bytecode_append(code, iconst_0$);
	bytecode_append(code, putstatic$);
	bytecode_append_short_int(code, cp_add_fieldref("M", "KP", "I"));

	bytecode_append(code, return$);

	/* write the method headers */

	/* write access flags, ACC_PUBLIC  */
	write_short_int(fp, 0x0001);

	/* write method name */
	write_short_int(fp, cp_add_utf8("keyReleased"));

	/* write method descriptor */
	write_short_int(fp, cp_add_utf8("(I)V"));

	/* write 1 attribute */
	write_short_int(fp, 1);

	/* write the Code attribute */
	write_short_int(fp, cp_add_utf8("Code"));
	write_long_int(fp, code->bytecode_pos + 12);

	/* write the max stack */
	write_short_int(fp, 2);

	/* max locals for program block */
	write_short_int(fp, 2);


	/* code length */
	write_long_int(fp, code->bytecode_pos);

	/* write the code itself */
	if (fp != NULL)
		fwrite(code->bytecode, 1, code->bytecode_pos, fp);

	bytecode_destroy(code);

	write_short_int(fp, 0);
	write_short_int(fp, 0);
}


/*
	Creates the code for the class constructor.
*/
void write_constructor(FILE *fp)
{
	bytecode *code = bytecode_create();
	if (code == NULL)
		die(1);

	/* create the code */
	switch (canvasType)
	{
	case NORMAL:
		bytecode_append(code, aload_0$);
	bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Canvas", "<init>", "()V"));
		break;
	case FULL_NOKIA:
		bytecode_append(code, aload_0$);
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, cp_add_methodref("com/nokia/mid/ui/FullCanvas", "<init>", "()V"));
		break;
	case FULL_MIDP20:
		bytecode_append(code, aload_0$);
		bytecode_append(code, iconst_0$);
		bytecode_append(code, invokespecial$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/game/GameCanvas", "<init>", "(Z)V"));

		bytecode_append(code, aload_0$);
		bytecode_append(code, iconst_1$);
		bytecode_append(code, invokevirtual$);
		bytecode_append_short_int(code, cp_add_methodref("javax/microedition/lcdui/Canvas", "setFullScreenMode", "(Z)V"));
		break;
	}

	bytecode_append(code, return$);

	/* write the method headers */

	/* write access flags, ACC_PUBLIC  */
	write_short_int(fp, 0x0001);

	/* write method name */
	write_short_int(fp, cp_add_utf8("<init>"));

	/* write method descriptor */
	write_short_int(fp, cp_add_utf8("()V"));

	/* write 1 attribute */
	write_short_int(fp, 1);

	/* write the Code attribute */
	write_short_int(fp, cp_add_utf8("Code"));
	write_long_int(fp, code->bytecode_pos + 12);

	/* write the max stack */
	write_short_int(fp, 10);

	/* max locals for program block */
	write_short_int(fp, 2);


	/* code length */
	write_long_int(fp, code->bytecode_pos);

	/* write the code itself */
	if (fp != NULL)
		fwrite(code->bytecode, 1, code->bytecode_pos, fp);

	bytecode_destroy(code);

	write_short_int(fp, 0);
	write_short_int(fp, 0);

}


/*
	Write the variables from the block as fields into the
	given class file. If fp==NULL only add constants into
	the constant pool.
*/
void write_block_fields(name_table *names, FILE *fp)
{
	char descriptor[128];

	if ((names->descriptor != NULL)
		&& (names->descriptor->identifier_class == variable_name))
	{
		/* access flags: ACC_PUBLIC and ACC_STATIC */
		write_short_int(fp, 0x0001 | 0x0008);

		/* write the name index */
		lowercase(names->name->cstr);
		write_short_int(fp, cp_add_utf8(names->name->cstr));

		/* write the descriptor index */
		get_field_descriptor(names->descriptor->variable_type, descriptor);
		write_short_int(fp, cp_add_utf8(descriptor));

		/* write 0 attributes */
		write_short_int(fp, 0);
	}

	/* write the left child */
	if ((names->descriptor != NULL)
		&& (names->left_child != NULL))
	{
		write_block_fields(names->left_child, fp);
	}

	/* write the right child */
	if ((names->descriptor != NULL)
		&& (names->right_child != NULL))
	{
		write_block_fields(names->right_child, fp);
	}
}


void write_locals_stackmap(FILE *fp, identifier *block_identifier, int *stack_map_size)
{
	type_list *it = block_identifier->parameters;
	write_stackmap(fp, it, stack_map_size);

	/* for functions, add the return value */
	if (block_identifier->identifier_class == function_name)
	{
		it = type_list_create();
		type_list_append(it, block_identifier->return_type);
		write_stackmap(fp, it, stack_map_size);
		type_list_destroy(it);
	}

	it = block_identifier->variables;
	write_stackmap(fp, it, stack_map_size);
}

void write_stackmap(FILE *fp, type_list *it, int *stack_map_size)
{

	while (it != NULL)
	{
		char c;

		if (it->data != NULL)
		{
			switch(it->data->type_class)
			{
			case integer_type:
			case char_type:
			case boolean_type:
				c = 1;
				if (fp != NULL)
					fwrite(&c, 1, 1, fp);
				(*stack_map_size) ++;
				break;

			case real_type:
				{
					if (mathType == 1)
					{
						c = 1;
						if (fp != NULL)
							fwrite(&c, 1, 1, fp);
						(*stack_map_size) ++;
					}
					else
					{
						c = 7;
						if (fp != NULL)
							fwrite(&c, 1, 1, fp);
						write_short_int(fp, cp_add_class("Real"));
						(*stack_map_size) += 3;
					}
				}
				break;

			case string_type:
				c = 7;
				if (fp != NULL)
					fwrite(&c, 1, 1, fp);
				write_short_int(fp, cp_add_class("java/lang/String"));
				(*stack_map_size) += 3;
				break;

			case image_type:
				c = 7;
				if (fp != NULL)
					fwrite(&c, 1, 1, fp);
				write_short_int(fp, cp_add_class("javax/microedition/lcdui/Image"));
				(*stack_map_size) += 3;
				break;

			case command_type:
				c = 7;
				if (fp != NULL)
					fwrite(&c, 1, 1, fp);
				write_short_int(fp, cp_add_class("javax/microedition/lcdui/Command"));
				(*stack_map_size) += 3;
				break;


			case stream_type:
				c = 7;
				if (fp != NULL)
					fwrite(&c, 1, 1, fp);
				write_short_int(fp, cp_add_class("java/io/InputStream"));
				(*stack_map_size) += 3;
				break;

			case record_store_type:
				c = 7;
				if (fp != NULL)
					fwrite(&c, 1, 1, fp);
				write_short_int(fp, cp_add_class("javax/microedition/rms/RecordStore"));
				(*stack_map_size) += 3;
				break;

			case http_type:
				c = 7;
				if (fp != NULL)
					fwrite(&c, 1, 1, fp);
				write_short_int(fp, cp_add_class("H"));
				(*stack_map_size) += 3;
				break;

			case alert_type:
				c = 7;
				if (fp != NULL)
					fwrite(&c, 1, 1, fp);
				write_short_int(fp, cp_add_class("Ljavax/microedition/lcdui/AlertType;"));
				(*stack_map_size) += 3;
				break;

			case array_type:
				{
					char descriptor[512];
					char element_type[128];
					int i, len;

					get_field_descriptor(it->data->element_type, element_type);

					len = type_list_length(it->data->dimensions_list);
					for(i=0; i < len; i++)
					{
						descriptor[i] = '[';
					}

					descriptor[len] = '\0';

					sprintf(descriptor + len, "%s", element_type);

					c = 7;
					if (fp != NULL)
						fwrite(&c, 1, 1, fp);
					write_short_int(fp, cp_add_class(descriptor));
					(*stack_map_size) += 3;
				}
				break;

			case record_type:
				{
					char descriptor[16];
					c = 7;
					if (fp != NULL)
						fwrite(&c, 1, 1, fp);
					sprintf(descriptor, "R_%d", it->data->unique_record_ID);
					write_short_int(fp, cp_add_class(descriptor));
					(*stack_map_size) += 3;
				}
				break;
			}
		}

		it = it->next;
	}
}
