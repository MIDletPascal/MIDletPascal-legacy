/********************************************************************
	unit.h - structures and functions used to hold unit
	descriptions
  Niksa Orlic, 2005-03-30
********************************************************************/
struct unit_struct
{
	name_table *names;
	string *unit_name;
	int is_library;  /* set to 1 if it is an external library, or 0 if it is a plain unit */
};

typedef struct unit_struct unit;
unit* unit_create(string*);
void unit_destroy(unit*);
unit* unit_duplicate(unit*);
