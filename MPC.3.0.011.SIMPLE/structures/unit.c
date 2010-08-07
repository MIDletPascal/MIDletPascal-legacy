/********************************************************************
	block.h - structures and functions used to hold program
	block descriptions
  Niksa Orlic, 2005-03-30
********************************************************************/
#include "../util/strings.h"
#include "type_list.h"
#include "string_list.h"
#include "type.h"
#include "identifier.h"
#include "name_table.h"
#include "unit.h"
#include "../util/memory.h"


/*
	Create an empty unit from the given name.
*/
unit* unit_create(string *unit_name)
{
	unit *new_unit;
	new_unit = (unit*)mem_alloc(sizeof(unit));
	if (new_unit == NULL) return NULL;
	new_unit->unit_name = unit_name;
	new_unit->names = name_table_create();
	new_unit->is_library = 0;
	return new_unit;
}

/*
	Destroy a unit.
*/
void unit_destroy(unit *old_unit)
{
	string_destroy(old_unit->unit_name);
	mem_free(old_unit);
}

/*
	Duplicate a unit.
*/
unit* unit_duplicate(unit *item)
{
	unit *new_item;
	new_item = (unit*) mem_alloc(sizeof(unit));
	new_item->names = item->names;
	new_item->unit_name = string_duplicate(item->unit_name);
	new_item->is_library = item->is_library;
	return new_item;
}