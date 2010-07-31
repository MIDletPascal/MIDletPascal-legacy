/********************************************************************
	type_list.h - functions to work with typelists
  Niksa Orlic, 2004-04-28
********************************************************************/
struct type_list_struct
{
	struct type_list_struct *next;
	struct type_struct *data;
};

typedef struct type_list_struct type_list;
type_list *type_list_create();
void type_list_destroy(type_list*);
type_list *type_list_duplicate(type_list*);
void type_list_append(type_list*, struct type_struct*);
int type_list_length(type_list*);
int type_list_different_parameter(type_list*, type_list*);
int type_list_different_parameter_array(type_list*, type_list*);
int type_list_different_parameter_cast(type_list*, type_list*);
