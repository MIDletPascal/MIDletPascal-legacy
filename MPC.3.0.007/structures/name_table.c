/********************************************************************
	
	name-table.c - the functions used for name table handling. The
	name table is a binary tree. The node's left child has the name
	alphabetically before the name in the node, and the right child
	has the name which is alphabetically after the name in the node.

  Niksa Orlic, 2004-04-25

********************************************************************/

#include "../util/strings.h"
#include "../util/error.h"
//#include "../util/message.h"
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


/*
	Create an empty name table.
*/
name_table *name_table_create()
{
	name_table *new_item;
	new_item = (name_table*) mem_alloc(sizeof(name_table));

	if (new_item == NULL)
		die(1);

	new_item->left_child = NULL;
	new_item->right_child = NULL;
	new_item->name = NULL;
	new_item->descriptor = NULL;
	
	new_item->last = new_item;
	new_item->next = NULL;

	return new_item;
}


/*
	Destroy an allocated name table and free all memory.
*/
void name_table_destroy(name_table *item)
{
	if (item == NULL)
		return;

	if (item->name != NULL)
		string_destroy(item->name);

	if (item->descriptor != NULL)
		identifier_destroy(item->descriptor);

	if (item->left_child != NULL)
		name_table_destroy(item->left_child);

	if (item->right_child != NULL)
		name_table_destroy(item->right_child);
}

/*
name_table *name_table_duplicate(name_table *item)
{
	name_table *new_item = mem_alloc(sizeof(name_table));

	new_item->descriptor = identifier_duplicate(item->descriptor);
	new_item->name = string_duplicate(item->name);

	if (item->left_child != NULL)
		new_item->left_child = name_table_duplicate(item->left_child);
	else
		new_item->left_child = NULL;

	if (item->right_child != NULL)
		new_item->right_child = name_table_duplicate(item->right_child);
	else
		new_item->right_child = NULL;

	return new_item;
}*/


/*
	Inserts an entry into the name table. The string and the descriptor are
	not copied, only pointers to the string and descriptor are copied.
*/
void name_table_insert(name_table *root, string *name, 
					   identifier *descriptor)
{
	int string_compare;
	name_table *item, *new_item;

	if (root == NULL)
		return;

	if (root->name == NULL)
	{
		/* the root item is empty, insert the value
		   into the root item */

		root->name = name;
		root->descriptor = descriptor;
		return;
	}


	/* go through the tree to the first free element */
	item = root;

	do
	{
		string_compare = STRING_COMPARE(string_get_cstr(name), string_get_cstr(item->name));

		if (string_compare == 0)
		{
			/* an error occured, just return */
			return;
		}

		if (string_compare < 0)
		{
			if (item->left_child == NULL)
				break;

			item = item->left_child;
		}

		if (string_compare > 0)
		{
			if (item->right_child == NULL)
				break;

			item = item->right_child;
		}
	} while (1); /* until the break is called */

	new_item = name_table_create();
	new_item->name = name;
	new_item->descriptor = descriptor;
	new_item->next = NULL;

	root->last->next = new_item;
	root->last = new_item;

	if (string_compare < 0)
		item->left_child = new_item;

	if (string_compare > 0)
		item->right_child = new_item;

}


/*
	Searches name table for an element with a given name.
	Return the identifier descriptor if found or NULL if
	not found.
*/
identifier* name_table_find(name_table *item, string *name)
{
	int string_compare;

	while ((item != NULL) && (item->name != NULL))
	{
		string_compare = STRING_COMPARE(string_get_cstr(name), string_get_cstr(item->name));

		if (string_compare == 0)
			return item->descriptor;

		if (string_compare < 0)
			item = item->left_child;

		if (string_compare > 0)
			item = item->right_child;

	}

	return NULL;
}

identifier *name_table_find_cstr(name_table *item, char *name)
{
	int string_compare;

	while ((item != NULL) && (item->name != NULL))
	{
		string_compare = STRING_COMPARE(name, string_get_cstr(item->name));

		if (string_compare == 0)
			return item->descriptor;

		if (string_compare < 0)
			item = item->left_child;

		if (string_compare > 0)
			item = item->right_child;

	}

	return NULL;
}