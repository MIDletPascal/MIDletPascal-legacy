/********************************************************************
	string_list.h - list of strings
  Niksa Orlic, 2004-04-29
********************************************************************/

struct string_list_struct
{
	struct string_list_struct *next;
	string *data;
};

typedef struct string_list_struct string_list;
string_list *string_list_create();
void string_list_destroy(string_list*);
string_list *string_list_duplicate(string_list*);
void string_list_append(string_list*, string*);
int string_list_length(string_list*);
