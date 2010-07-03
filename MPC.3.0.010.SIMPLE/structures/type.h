/********************************************************************
	
	type.h - structures used to hold descriptions of pascal types

  Niksa Orlic, 2004-04-28

********************************************************************/


/*
	The possible type classes
*/
enum en_type_class
{ 
	error_type, /* used to detect errors */
	void_type,
	integer_type,
	real_type,
	char_type,
	boolean_type,
	string_type,
	array_type,
	record_type,
	interval_type,
	image_type,
	command_type,
	stream_type,
	record_store_type,
	http_type,
	alert_type /* this is only an internal type, the user cannot declare variables to be alert_type */
	
	/* add our own types */
} en_type_class;

/*
	The structure to hold description for a single
	type.
*/
struct type_struct
{
	enum en_type_class type_class;

	/* for array types */
	struct type_struct* element_type; /* the type of array elements */
	type_list *dimensions_list; /* the type for each dimension */

	/* for record type */
	string_list *elements_name_list; /* the elements in record, their names */
	type_list *elements_type_list; /* the elements in record, their types */
	int unique_record_ID; /* the name that identifies the record */

	/* for interval type */
	enum en_type_class interval_base_type;
	long int first_element;
	long int last_element;
};

typedef struct type_struct type;


type* type_create();
void type_destroy(type*);

type* type_duplicate(type*);

int type_equal(type*, type*);
int type_equal_cast(type*, type*);

string *type_get_name(type*);

void type_record_check_duplicate_names(type*, string_list*);
void type_add_record(type*, string_list*, type*);

type *type_find_record_element(type*, char*);