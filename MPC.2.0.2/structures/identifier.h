/********************************************************************
	
	identifier.h - structures used to hold identifier descriptions

  Niksa Orlic, 2004-04-28

********************************************************************/

/*
  When an unit compiles, it exports public symbols into

  unit_name.bsf (binary symbol file)

  The bsf file contains an array of symbol entries.

  Symbols are divided into the following categories:

  Constant
  ---------
  1 byte identifier: set to 1 to indicate that it is an constant
  STRING - symbol name
  1 byte to indicate the constant type, (1-integer, 2-real, 3-boolean, 4-char, 5-string)
  value: if a constant is integer or float, this is 4-byte field
         if a constant is string, then a STRING entry follows
		 if a constant is a boolean or a char, 1 byte follows

  Variable
  --------
  1 byte indentifier: set to 2 for variable
  STRING - symbol name
  TYPE - variable type

  Type
  ----
  1 byte identifier: set to 3 for type definition
  STRING - symbol name
  TYPE - the type description

  Procedure
  ---------
  1 byte identifier: set to 4 for procedures
  STRING - name
  1 byte - the number of parameters (n)
  TYPE[n] - parameters type

  Function
  --------
  1 byte identifier: set to 5 for functions
  STRING - name
  TYPE - return type
  1 byte - number of parameters
  TYPE[n] - parameter types

  
	Special fields used are:

  STRING
  ------
  1 byte containing the string length (n)
  n bytes with the string data (NOT zero terminated)

  TYPE
  ----
  1 byte : type class, a value from en_type_class enumeration
  
     if a type is an array: TYPE element_type
	                        1 byte dimensions num
							TYPE[n] dimension interval types

     if a type is a record: 1 byte number of elements
	                        {STRING, TYPE} [n] for each element there is a pair name/type describing the member
							4 bytes record ID

     if a type is an interval: 1 byte interval base type (from the en_type_class)
	                           4 bytes: starting value
							   4 bytes: ending value

*/

#include <stdio.h>

/*
	The possible identifier classes
*/
enum en_identifier_class
{
	none,
	program_name,
	constant_name,
	variable_name,
	type_name,
	procedure_name,
	function_name,
	parameter_name,
	unit_name
};


/*
	The structure used to hold the description of
	a single identifier
*/
struct identifier_descriptor_struct
{
	enum en_identifier_class identifier_class;

	/* for constants */
	type *constant_type;
	int constant_int_value;
	float constant_real_value;
	string *constant_string_value;

	/* for variables */
	type *variable_type;
	int variable_index; 

	/* for types */
	type *defined_type;

	/* for procedures and functions */
	type *return_type; 
	type_list *parameters;	/* the list of formal parameters to the function, each one is identifier_descriptor */
	type_list *variables;   /* the list of names of variables */
	int forward_declaration;
	int standard_function;
	int subprogram_linenum; /* the number of the first line of the subprogram */
	int unit_function; /* used by all items that are defined inside an unit , 1 says that the item is inside a unit */
	struct unit_struct *container_unit;

	/* for parameters */
	type *parameter_type;
	short int is_parameter_variable;
	int parameter_index;

	/* for units */
	struct unit_struct *unit_block;

	/* this field is set in get_identifier function and is true
	if this struct describes identifier directly located inside
	the program block*/
	int belongs_to_program_block;
};


typedef struct identifier_descriptor_struct identifier;

identifier *identifier_create();
void identifier_destroy(identifier*);
identifier *identifier_duplicate(identifier*);

struct type_struct;
struct string_list_struct;
struct string_struct;

void bsf_write_integer_constant(long int value, char* name);
void bsf_write_real_constant(float value, char* name);
void bsf_write_boolean_constant(char value, char* name);
void bsf_write_char_constant(char value, char* name);
void bsf_write_string_constant(string* value, char* name);
void bsf_write_variables(struct string_list_struct* names, struct type_struct* var_type);
void bsf_write_type(string* type_name, struct type_struct* type_type);
void bsf_write_procedure(string* name, type_list* parameters);
void bsf_write_function(string* name, type_list* parameters, type* return_type);
void bsf_write_STRING(char* value);
void bsf_write_TYPE(type* type_desc);

struct string_struct* bsf_read_STRING(FILE*);
struct type_struct* bsf_read_TYPE(FILE*);

