/********************************************************************
	stdpas.h - initializes the parser/semantic checker with a
	standard pascal types, constants etc.
  Niksa Orlic, 2004-04-29
********************************************************************/
block* initialize_root_block();
void add_std_types(block*);
void add_std_constants(block*);
void add_std_functions(block*);
void add_std_type(block*, enum en_type_class , char*);
void add_std_function(block*, char*, type_list*, type*);
void add_std_procedure(block*, char*, type_list*);
void create_std_function_code(bytecode*, char*);
void create_std_function_prefix(bytecode*, char*);
void add_special_function(block*, enum en_type_class, enum en_type_class, char*);
void add_special_function2(block*, enum en_type_class, enum en_type_class, enum en_type_class, char*);
void add_special_function3(block*, enum en_type_class, enum en_type_class, enum en_type_class, enum en_type_class, char*);
void add_special_function4(block*, enum en_type_class, enum en_type_class, enum en_type_class, enum en_type_class, enum en_type_class, char*);
void add_special_function5(block*, enum en_type_class, enum en_type_class, enum en_type_class, enum en_type_class, enum en_type_class, enum en_type_class, char*);
void add_special_function6(block*, enum en_type_class, enum en_type_class, enum en_type_class, enum en_type_class, enum en_type_class, enum en_type_class, enum en_type_class, char*);
