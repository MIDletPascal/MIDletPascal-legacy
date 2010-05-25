/********************************************************************
	
	preverify.h - methods and structures for bytecode preverification

  Niksa Orlic, 2004-08-18

********************************************************************/


/*** Preverifier structures ***/
enum stack_item
{
	ITEM_Bogus = 0,
	ITEM_Integer,
	ITEM_Float,
	ITEM_Double,
	ITEM_Long,
	ITEM_Null,
	ITEM_InitObject,
	ITEM_Object,
	ITEM_NewObject
};

struct stack_entry_struct
{
	enum stack_item item_type;
	short int additional_data;
};

typedef struct stack_entry_struct stack_entry;

struct stack_map_struct
{
	int number_of_items;
	int allocated_number_of_items;
	stack_entry **stack;
	int bytecode_offset;
};

typedef struct stack_map_struct stack_map;



stack_entry* stack_entry_create();
void stack_entry_destroy(stack_entry*);
stack_entry* stack_entry_duplicate(stack_entry*);

stack_map* stack_map_create();
void stack_map_destroy(stack_map*);
stack_map* stack_map_duplicate(stack_map*);

stack_entry* stack_map_pop(stack_map*);
void stack_map_push(stack_map*, stack_entry*);
void stack_map_push_entry(stack_map*, enum stack_item, short int);
void stack_map_push_local(stack_map*, identifier*, int);


struct stack_map_list_struct
{
	struct stack_map_list_struct *next;
	stack_map *data;
};

typedef struct stack_map_list_struct stack_map_list;

void stack_map_list_destroy(stack_map_list*);

void stack_map_list_append(stack_map_list**, stack_map*);
stack_map* stack_map_list_get(stack_map_list**);


/*** Preverifier methods ***/
stack_map_list *preverify_bytecode(bytecode*, identifier *);
int preverify_consume_bytecode(bytecode*, stack_map*, int *offset, identifier*);
void process_jump(stack_map *map, int position);
void preverify_bytecode_from(stack_map*, bytecode*, identifier*);
stack_map_list* sort_map_list(stack_map_list*);


