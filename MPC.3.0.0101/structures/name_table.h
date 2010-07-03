/********************************************************************
	name_table.h - structures and function prototypes for handling
	names
  Niksa Orlic, 2004-04-28
********************************************************************/

struct name_table_struct
{
	string *name;
	struct name_table_struct *left_child;
	struct name_table_struct *right_child;
	struct name_table_struct *last; // used only in the first item
	struct name_table_struct *next;
	identifier *descriptor;
};

typedef struct name_table_struct name_table;
name_table *name_table_create();
//name_table *name_table_duplicate(name_table*);
void name_table_destroy(name_table *item);
void name_table_insert(name_table *root, string *name, identifier *descriptor);
identifier* name_table_find(name_table *item, string *name);
identifier *name_table_find_cstr(name_table *item, char *name);

/*
	Define the case insensitive string-compare routine.
  _stricmp on MSVC
  strcasecmp on gcc (I think)
*/
#define STRING_COMPARE _stricmp