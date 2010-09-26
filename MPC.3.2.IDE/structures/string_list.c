/********************************************************************
	
	string_list.c - function for handling string lists

  Niksa Orlic, 2004-04-28

********************************************************************/

#include "../util/strings.h"
#include "../util/error.h"
//#include "../util/message.h"
#include "string_list.h"


#include <string.h>
#include <stdlib.h>

/*
	Create a new empty list
*/
string_list *string_list_create()
{
	string_list *new_list = (string_list*) mem_alloc(sizeof(string_list));

	if (new_list == NULL)
		die(1);

	new_list->data = NULL;
	new_list->next = NULL;

	return new_list;
}


/*
	Delete the list and the data in the list
*/
void string_list_destroy(string_list* item)
{
	string_list *it, *next;

	it = item;
	while (it != NULL)
	{
		next = it->next;

		if (it->data != NULL)
			string_destroy(it->data);

		mem_free (it);

		it = next;
	}
}

/*
	Creates a copy of the list, the data
	values are also copied
*/
string_list *string_list_duplicate(string_list *item)
{
	string_list *new_list;

	new_list = string_list_create();

	if (item->data == NULL)
		return new_list;

	do
	{
		if(item->data == NULL)
			break;

		string_list_append(new_list, string_duplicate(item->data));
		item = item->next;
	} while (item != NULL);

	return new_list;
}


/*
	Add an element into the list, the data is 
	copied.
*/
void string_list_append(string_list *item, string *data)
{
	string_list *new_element;
	
	if (item->data == NULL)
		item->data = string_duplicate(data);
	else
	{
		new_element = (string_list*) mem_alloc(sizeof(string_list));

		if (new_element == NULL)
			die(1);

		new_element->data = string_duplicate(data);
		new_element->next = NULL;

		/* move to the end of the list */
		while (item->next != NULL)
			item = item->next;

		item->next = new_element;
	}
}


/*
	Returns the number of elements in the list
*/
int string_list_length(string_list *item)
{
	int counter = 0;

	if (item->data == NULL)
		return counter;

	do
	{
		item = item->next;
		counter ++;
	} while (item != NULL);

	return counter;
}
