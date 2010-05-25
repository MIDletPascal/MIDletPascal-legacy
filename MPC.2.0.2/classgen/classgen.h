/********************************************************************
	
	classgen.h - methods for creating the .class binary file

  Niksa Orlic, 2004-05-16

********************************************************************/


void create_record_class(type *record_type);

void create_class_file_header(FILE *class_file);
void write_record_constant_pool(FILE *class_file, type *record);
void create_record_fields(type *record);
void write_record_fields(FILE *class_file, type *record);
void write_constant_pool_class(FILE *class_file, char *class_name);
void write_constant_pool_utf8(FILE *class_file, char *string);
void write_constant_pool_fieldref(FILE *class_file, int type_index, int name_index);
void write_constant_pool_methodref(FILE *class_file, int, int, int);
void get_field_descriptor(type *field_type, char *descriptor);
void write_short_int(FILE *class_file, short int num);
void write_long_int(FILE *class_file, long int num);

short int read_short_int(FILE *class_file);
long int read_long_int(FILE *class_file);

void create_init_method(type *record, bytecode*);
void write_init_method(FILE *class_file, bytecode*);
void create_copy_method(type *record, bytecode*);
void write_copy_method(type* record, FILE *class_file, bytecode*);
void initialize_record_variable(bytecode *code, type *variable_type, int fieldref_index, int index);
