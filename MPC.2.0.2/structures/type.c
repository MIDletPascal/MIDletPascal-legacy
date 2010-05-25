/********************************************************************
	
	type.c - function for handling types in pascal

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
#include "../util/memory.h"

#include <string.h>
#include <stdlib.h>

extern int linenum;

/*
	Create an empty type
*/
type* type_create()
{
	type *new_type;
	new_type = (type*) mem_alloc(sizeof(type));

	if (new_type == NULL)
		die(MSG_001);

	new_type->type_class = void_type;

	return new_type;
}


/*
	Deletes a type
*/
void type_destroy(type *item)
{
	if (item == NULL)
		return;

	switch(item->type_class)
	{
	case array_type:
		{
			type_list_destroy(item->dimensions_list);
			type_destroy(item->element_type);
			break;
		}

	case record_type:
		{
			type_list_destroy(item->elements_type_list);
			string_list_destroy(item->elements_name_list);
			break;
		}
	}

	mem_free(item);
}


/*
	Create a copy of a type
*/
type* type_duplicate(type *item)
{
	type *new_item;
	new_item = (type*) mem_alloc(sizeof(type));

	if (new_item == NULL)
		die(MSG_001);

	memcpy(new_item, item, sizeof(type));

	if (new_item->type_class == array_type)
	{
		new_item->dimensions_list = type_list_duplicate(item->dimensions_list);
		new_item->element_type = type_duplicate(item->element_type);
	}

	if (new_item->type_class == record_type)
	{
		new_item->elements_type_list = type_list_duplicate(item->elements_type_list);
		new_item->elements_name_list = string_list_duplicate(item->elements_name_list);
		new_item->unique_record_ID = item->unique_record_ID;
	}

	return new_item;
}


/*
	Return a nonzero value if both types are equal.
*/
int type_equal(type *type1, type *type2)
{
	if (type1 == type2)
		return 1;

	if ((type1 == NULL) || (type2 == NULL))
		return 0;

	if (type1->type_class != type2->type_class)
		return 0;

	switch (type1->type_class)
	{
	case array_type:
		{
			if (!type_equal(type1->element_type, type2->element_type))
				return 0;

			if (type_list_different_parameter(type1->dimensions_list,
							type2->dimensions_list) != 0)
				return 0;

			break;
		}

	case record_type:
		{
			return (type1->unique_record_ID == type2->unique_record_ID);

			break;
		}

	case interval_type:
		{
			if (type1->interval_base_type != type2->interval_base_type)
				return 0;

			if (type1->first_element != type2->first_element)
				return 0;

			if (type1->last_element != type2->last_element)
				return 0;

			break;
		}
	}

	return 1;
}

/*
	Return 0 if types are not equal; 1 if the types
	are exactly the same, 2 if type1 is real and type2 int
	or 3 if type2 is real and type1 int

	Also, char can be casted into string
*/
int type_equal_cast(type *type1, type* type2)
{
	if (type_equal(type1, type2))
		return 1;

	if ((type1->type_class == real_type)
		&& (type2->type_class == integer_type))
		return 2;

	if ((type1->type_class == integer_type)
		&& (type2->type_class == real_type))
		return 3;

	if ((type1->type_class == string_type)
		&& (type2->type_class == char_type))
		return 2;

	if ((type1->type_class == char_type)
		&& (type2->type_class == string_type))
		return 3;

	return 0;
}


/*
	Returns a name for any basic type, string 'unknown type' otherwise
*/
string *type_get_name(type *basic_type)
{
	switch(basic_type->type_class)
	{
	case integer_type:
		return string_from_cstr("integer");

	case real_type:
		return string_from_cstr("real");

	case boolean_type:
		return string_from_cstr("boolean");

	case char_type:
		return string_from_cstr("char");

	case image_type:
		return string_from_cstr("image");

	case command_type:
		return string_from_cstr("command");

	case stream_type:
		return string_from_cstr("stream");

	case string_type:
		return string_from_cstr("string");

	case array_type:
		return string_from_cstr("array");

	case record_type:
		return string_from_cstr("record");

	case record_store_type:
		return string_from_cstr("recordStore");

	case http_type:
		return string_from_cstr("http");

	case alert_type:
		return string_from_cstr("alert type");

	case interval_type:
		return string_from_cstr("interval");

	default:
		return string_from_cstr("unknown type");
	}

	return NULL;
}


/*
	Check if any name from element_names already exists in
	item->elements_name_list
*/
void type_record_check_duplicate_names(type *item, string_list *element_names)
{
	string_list *item_list;

	while (element_names != NULL)
	{
		if (element_names->data != NULL)
		{
			item_list = item->elements_name_list;

			while (item_list != NULL)
			{
				if (item_list->data != NULL)
				{
					if (STRING_COMPARE(string_get_cstr(item_list->data), 
						 string_get_cstr(element_names->data)) == 0)
					{
						add_error_message(MSG_418, linenum, string_get_cstr(item_list->data), "");
					}
				}
			
				item_list = item_list->next;
			}
		}
	
		element_names = element_names->next;
	}
}


/*
	Add elements into a record type
*/
void type_add_record(type *item, string_list *element_names, type *element_type)
{
	/* check for duplicate names in the names */
	type_record_check_duplicate_names(item, element_names);

	while (element_names != NULL)
	{
		if (element_names->data != NULL)
		{
			string_list_append(item->elements_name_list, element_names->data);
			type_list_append(item->elements_type_list, element_type);
		}
	
		element_names = element_names->next;
	}
}


/*
	Returns the type of an element of a given record type identified
	by a given name. In case of an error, NULL is returned.
*/
type* type_find_record_element(type *record, char *cstr)
{
	string_list *names;
	type_list *types;

	if (record->type_class != record_type)
		return NULL;

	names = record->elements_name_list;
	types = record->elements_type_list;

	while (names != NULL)
	{
		if (names->data != NULL)
		{
			if(STRING_COMPARE(cstr, string_get_cstr(names->data)) == 0)
			{
				return type_duplicate(types->data);
			}
		}
	
		names = names->next;
		types = types->next;
	}

	return NULL;
}