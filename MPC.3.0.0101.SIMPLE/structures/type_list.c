/********************************************************************
	type_list.c - function for handling type lists
  Niksa Orlic, 2004-04-28
********************************************************************/
#include "../util/strings.h"
#include "type_list.h"
#include "string_list.h"
#include "type.h"
#include "identifier.h"
#include "name_table.h"
#include "../classgen/bytecode.h"
#include "../util/memory.h"
#include "../util/error.h"


/*
	Create a new empty list
*/
type_list *type_list_create()
{
	type_list *new_list = (type_list*) mem_alloc(sizeof(type_list));
	if (new_list == NULL) die(1);
	new_list->data = NULL;
	new_list->next = NULL;
	return new_list;
}

/*
	Delete the list with all associated data
*/
void type_list_destroy(type_list* item)
{
	type_list *it, *next;
	it = item;
	while (it != NULL)
	{	
		next = it->next;
		if (it->data != NULL) mem_free(it->data);
		mem_free (it);
		it = next;		
	}
}

/*
	Creates a copy of the list, the data
	values are also copied
*/
type_list *type_list_duplicate(type_list *item)
{
	type_list *new_list;
	if (item == NULL) return NULL;
	new_list = type_list_create();
	if ((item == NULL) || (item->data == NULL)) return new_list;
	do
	{
		type_list_append(new_list, item->data);
		item = item->next;
	} while (item != NULL);
	return new_list;
}

/*
	Add an element into the list, the data is 
	copied.
*/
void type_list_append(type_list *item, struct type_struct *data)
{
	type_list *new_element;
	if (item->data == NULL) item->data = type_duplicate(data);
	else
	{
		new_element = (type_list*) mem_alloc(sizeof(type_list));
		if (new_element == NULL) die(1);
		new_element->data = type_duplicate(data);
		new_element->next = NULL;
		/* move to the end of the list */
		while (item->next != NULL) item = item->next;
		item->next = new_element;
	}
}

/*
	Returns the number of elements in the list
*/
int type_list_length(type_list *item)
{
	int counter = 0;
	if (item->data == NULL) return counter;
	do
	{
		item = item->next;
		counter ++;
	} while (item != NULL);
	return counter;
}

/*
	Returns the index (starting with one) of the first
	item in the list1 different than the parameetr in the list2,
	0 if the lists are equal, or -1 if the list1 is shorter than
	the list2 and all elements in the list1 correspond to the first
	n elements in the list2.
*/
int type_list_different_parameter(type_list *list1, type_list *list2)
{
	int counter = 1;
	while (list1 != NULL)
	{
		if ((list2 == NULL) 
			|| (list1->data == NULL)
			|| (list2->data == NULL)
			|| (!type_equal(list1->data, list2->data)))
		{
			if ((list2 != NULL) && (list1->data == NULL) && (list2->data == NULL)) return 0;
			return counter;
		}
		counter ++;
		list1 = list1->next;
		list2 = list2->next;
	}
	if (list2 != NULL) return -1;
	return 0;
}

/*
	Same as the previous function, except that if the list1 element
	can be casted into list2 element, it is OK !!!
*/
int type_list_different_parameter_cast(type_list *list1, type_list *list2)
{
	int counter = 1;
	while (list1 != NULL)
	{
		if ((list2 == NULL) 
			|| (list1->data == NULL)
			|| (list2->data == NULL)
			|| (type_equal_cast(list1->data, list2->data) == 0)
			|| (type_equal_cast(list1->data, list2->data) == 2))
		{
			if ((list2 != NULL) && (list1->data == NULL) && (list2->data == NULL)) return 0;
			return counter;
		}
		counter ++;
		list1 = list1->next;
		list2 = list2->next;
	}
	if (list2 != NULL) return -1;
	return 0;
}

/*
	Same as the above, but compares interval base types against the types; 
	this is used when checking array dimensions
*/
int type_list_different_parameter_array(type_list *real_types, type_list *interval_types)
{
	int counter = 1;
	while (real_types != NULL)
	{
		if ((interval_types == NULL) 
			|| (real_types->data == NULL)
			|| (interval_types->data == NULL)
			|| (real_types->data->type_class != interval_types->data->interval_base_type))
				return counter;
		counter ++;
		real_types = real_types->next;
		interval_types = interval_types->next;
	}
	if (interval_types != NULL) return -1;
	return 0;
}
