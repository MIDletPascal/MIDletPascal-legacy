/********************************************************************
	
	block.h - structures and functions used to hold program
	block descriptions

  Niksa Orlic, 2004-04-23

********************************************************************/

/*
	The structure to hold description for
	one block;
*/
struct block_struct
{
	struct block_struct *parent_block;

	name_table *names;

	string *block_name;

	int next_variable_index;
	int next_parameter_index;

	bytecode *code;

	struct block_struct **children;
	int children_count;
};

typedef struct block_struct block;

/* canvas types */
#define NORMAL		0
#define FULL_MIDP20 1
#define FULL_NOKIA	2


block* block_create(block*, string*);
void block_destroy(block*);

int block_check_name(block*, char*);

void add_real_constant(block*, float, char*);
void add_integer_constant(block*, long int, char*);
void add_char_constant(block*, char, char*);
void add_boolean_constant(block*, char, char*);
void add_string_constant(block*, string*, char*);

void add_variables(block*, string_list*, type*);

void add_type(block*, string*, type*);

void add_procedure(block*, string*, type_list*, int, int);
void add_function(block*, string*, type_list*,type*, int, int);

void load_extern_library(string*);
void read_library_file(FILE*, struct unit_struct*);
void read_symbol_file(FILE*, struct unit_struct*);

void add_parameters(block*, string_list*, type*, int);

void check_forward_declaration(identifier*, type_list*,
							   int, type*);
void check_unmatched_forward_declarations(block*, name_table*);

void transform_break_stmts(bytecode*, int, int, int);

type *type_from_name(block*, char*);
type *get_constant_type(block*, char*);
type *get_variable_type(block*, char*);
identifier *get_constant_identifier(block*, char*);
identifier *get_identifier(block*, char*);

void initialize_variable(block*, identifier*, char*, int, char*);

void create_class_file(block *root_block, FILE *fp);
void write_method(block *current_block, FILE *fp);
void write_block_fields(name_table *, FILE*);

void write_paint_method(FILE *fp);
void write_constructor(FILE *fp);
void write_run_method(FILE *fp);
void write_keypressed_method(FILE *fp);
void write_keyreleased_method(FILE *fp);

void write_locals_stackmap(FILE *fp, identifier *block_identifier, int* stack_map_size);
void write_stackmap(FILE *fp, type_list *it, int *stack_map_size);
