/********************************************************************
	classgen.c - methods for creating the .class binary file
  Niksa Orlic, 2004-05-16
********************************************************************/
#pragma warning (disable: 4996)
#include "../util/strings.h"
#include "../structures/type_list.h"
#include "../structures/string_list.h"
#include "../structures/type.h"
#include "../structures/identifier.h"
#include "../structures/name_table.h"
#include "../classgen/bytecode.h"
#include "../structures/block.h"
#include "../classgen/constant_pool.h"
#include "../util/error.h"
#include "classgen.h"
#include "../util/memory.h"
#include <string.h>


extern char* output_path;
extern int linenum;
extern constant_pool *constants;
extern int constant_pool_size;
extern int mathType;
char class_name[16];

/*
	Creates a class file for pascal record.
*/
void create_record_class(type *record)
{	
	char class_file_name[16];
	char *output_file_name;
	char copy_sig[64];
	FILE *record_file;
	bytecode *init_method;
	bytecode *copy_method;
	/* save the previous constant pool state */
	constant_pool *global_cp;
	int global_cp_count;
	global_cp = constants;
	global_cp_count = constant_pool_size;
	constants = NULL;
	constant_pool_size = 0;
	/* check for consistency */
	if (record->type_class != record_type) die(12);
	sprintf(class_name, "R_%d", record->unique_record_ID);
	sprintf(class_file_name, "%s.class", class_name);
	output_file_name = (char*) mem_alloc(strlen(output_path) + strlen(class_file_name) + 5);
	if (output_file_name == NULL) die(1);
	sprintf(output_file_name, "%s\\%s", output_path, class_file_name);
	record_file = fopen(output_file_name, "wb");
	mem_free(output_file_name);
	if (record_file == NULL) die(3);
	/* write the class file header */
	create_class_file_header(record_file);
	/* add some stuff into the constant pool */
	cp_add_class(class_name);
	cp_add_class("java/lang/Object");
	cp_add_utf8("<init>");
	cp_add_utf8("()V");
	cp_add_utf8("Code");
	cp_add_utf8("Copy");
	sprintf(copy_sig, "(LR_%d;)LR_%d;", record->unique_record_ID, record->unique_record_ID);
	cp_add_utf8(copy_sig);
	create_record_fields(record);
	/* create the bytecode for the init method */
	init_method = bytecode_create();
	create_init_method(record, init_method);
	/* create the bytecode for the copy method */
	copy_method = bytecode_create();
	create_copy_method(record, copy_method);
	/* write the constant pool into the file */
	write_constant_pool(record_file);
	/* write the access flags, set to ACC_PUBLIC and ACC_SUPER */
	write_short_int(record_file, 0x0021); 
	/* write the index to constat pool with this class description */
	write_short_int(record_file, cp_add_class(class_name));
	/* write the index to constant pool with the super class description */
	write_short_int(record_file, cp_add_class("java/lang/Object"));
	/* we have no interfaces */
	write_short_int(record_file, 0);
	/* write the fields */
	write_short_int(record_file, (short) type_list_length(record->elements_type_list));
	write_record_fields(record_file, record);
	/* we have <init> and Copy method */
	write_short_int(record_file, 2);
	/* write the init method */
	write_init_method(record_file, init_method);
	/* write the copy method */
	write_copy_method(record, record_file, copy_method);
	/* we have no attributes */
	write_short_int(record_file, 0);
	fclose(record_file);
	/* restore the constant pool */
	constants = global_cp;
	constant_pool_size = global_cp_count;
}

/*
	Writes a class file header into the output file
*/
void create_class_file_header(FILE *class_file)
{
	char magic[4] = { (char)0xCA, (char)0xFE, (char)0xBA, (char)0xBE };
	fwrite(magic, 1, 4, class_file);
	/* Write the minor version number 3 */
	write_short_int(class_file, 0x0003);
	/* Wrte the major version number 45 */
	write_short_int(class_file, 0x002d);
}

/*
	Creates entries into constant pool with names and types of 
	the elements of the structure.
*/
void create_record_fields(type *record)
{
	string_list *names;
	type_list *types;
	char descriptor[128];
	names = record->elements_name_list;
	types = record->elements_type_list;
	while (names != NULL)
	{
		if (names->data != NULL)
		{
			////!!!!lowercase(names->data->cstr);
			get_field_descriptor(types->data, descriptor);
			cp_add_nameandtype(names->data->cstr, descriptor);			
		}
		names = names->next;
		types = types->next;
	}
}

/*
	Writes the fields from the record into the 
	class file.
*/
void write_record_fields(FILE *class_file, type *record)
{
	string_list *names;
	type_list *types;
	char descriptor[128];
	int field_num = 0;
	names = record->elements_name_list;
	types = record->elements_type_list;
	while (names != NULL)
	{
		if (names->data != NULL)
		{
			/* write the access flags: ACC_PUBLIC */
			write_short_int(class_file, 0x0001);
			/* write the name index */
			////!!!!lowercase(names->data->cstr);
			write_short_int(class_file, cp_add_utf8(names->data->cstr));
			/* write the descriptor index */
			get_field_descriptor(types->data, descriptor);
			write_short_int(class_file, cp_add_utf8(descriptor));
			/* we have no attributes */
			write_short_int(class_file, 0);
		}
		names = names->next;
		types = types->next;
		field_num ++;  //??? test
	}
}

/*
	Writes a short int in bigendian format. If
	class_file is NULL, does nothing.
*/
void write_short_int(FILE *class_file, short int num)
{
	char ch;
	if (class_file == NULL) return;
	ch = (char) (num >> 8);
	fwrite(&ch, 1, 1, class_file);
	ch = (char) num;
	fwrite(&ch, 1, 1, class_file);
}

void write_long_int(FILE *class_file, long int num)
{
	char ch;
	if (class_file == NULL) return;
	ch = (char) (num >> 24);
	fwrite(&ch, 1, 1, class_file);
	ch = (char) (num >> 16);
	fwrite(&ch, 1, 1, class_file);
	ch = (char) (num >> 8);
	fwrite(&ch, 1, 1, class_file);
	ch = (char) num;
	fwrite(&ch, 1, 1, class_file);
}

short int read_short_int(FILE *class_file)
{
	short int return_value = fgetc(class_file) << 8;
	return_value |= (fgetc(class_file) & 0x00FF);
	return return_value;
}

long int read_long_int(FILE *class_file)
{
	long int return_value = fgetc(class_file) << 24;
	return_value |= (fgetc(class_file) & 0x00FF) << 16;
	return_value |= (fgetc(class_file) & 0x00FF) << 8;
	return_value |= (fgetc(class_file) & 0x00FF);
	return return_value;
}

/*
	Construct a valid Java field descriptor for a given type.
*/
void get_field_descriptor(type *field_type, char *descriptor)
{
	switch (field_type->type_class)
	{
	case void_type:
		sprintf(descriptor, "V");
		return;  //break;
	case integer_type: 
	case char_type:
	case boolean_type:
		sprintf(descriptor, "I");
		return;  //break;
	case real_type:
		if (mathType == 1) sprintf(descriptor, "I");
		else sprintf(descriptor, "LReal;");
		return;  //break;
	case string_type:
		sprintf(descriptor, "Ljava/lang/String;");
		return;  //break;
	case command_type:
		sprintf(descriptor, "Ljavax/microedition/lcdui/Command;");
		return;  //break;
	case stream_type:
		sprintf(descriptor, "Ljava/io/InputStream;");
		return;  //break;
	case record_store_type:
		sprintf(descriptor, "Ljavax/microedition/rms/RecordStore;");
		return;  //break;
	case http_type:
		sprintf(descriptor, "LH;");
		return;  //break;
	case alert_type:
		sprintf(descriptor, "Ljavax/microedition/lcdui/AlertType;");
		return;  //break;
	case image_type:
		sprintf(descriptor, "Ljavax/microedition/lcdui/Image;");
		return;  //break;
	case array_type:
		{
			char element_type[128];
			int i, len;
			get_field_descriptor(field_type->element_type, element_type);
			len = type_list_length(field_type->dimensions_list);
			for(i=0; i < len; i++) descriptor[i] = '[';
			descriptor[len] = '\0';
			sprintf(descriptor + len, "%s", element_type);
		}
		return;  //break;
	case record_type:
		sprintf(descriptor, "LR_%d;", field_type->unique_record_ID);
		return;  //break;
	case error_type:
		return;  //break;
	default:
		die(13);
	}
}

/*
	Creates the <init> method for records.
*/
void create_init_method(type *record, bytecode *code)
{
	type_list *type_it;
	string_list *name_it;
	int counter = 0;
	if (code == NULL) die(1);
	/* create the code */
	type_it = record->elements_type_list;
	name_it = record->elements_name_list;
	bytecode_append(code, aload_0$);
	bytecode_append(code, invokespecial$);
	bytecode_append_short_int(code, cp_add_methodref("java/lang/Object", "<init>", "()V"));
	while (type_it != NULL)
	{
		if ((type_it->data != NULL) && (name_it->data != NULL))
		{
			block *code_wrapper;
			identifier *type_wrapper;
			code_wrapper = block_create(NULL, string_create());
			code_wrapper->code = code;
			type_wrapper = identifier_create();
			type_wrapper->identifier_class = variable_name;
			type_wrapper->variable_type = type_it->data;
			type_wrapper->belongs_to_program_block = 1;
			/* put 'this' to the stack */
			bytecode_append(code, aload_0$);
			/* create the code to initialize the variable */
			initialize_variable(code_wrapper, type_wrapper, name_it->data->cstr, 0, class_name);
			if (type_it->data->type_class == array_type) add_error_message(446, name_it->data->cstr, "");
			code_wrapper->code = bytecode_create();
			block_destroy(code_wrapper);
			type_wrapper->variable_type = type_create();
			identifier_destroy(type_wrapper);
		}
		type_it = type_it->next;
		if (name_it != NULL) name_it = name_it->next;
		counter ++;
	}
	bytecode_append(code, return$);
}

/*
	Creates the Copy method for records.
*/
void create_copy_method(type *record, bytecode *code)
{
	char type_name[64];
	type_list *type_it;
	string_list *name_it;
	int counter = 0;
	if (code == NULL) die(1);
	sprintf(type_name, "R_%d", record->unique_record_ID);
	/* create the code */
	type_it = record->elements_type_list;
	name_it = record->elements_name_list;
	while (type_it != NULL)
	{
		if ((type_it->data != NULL) && (name_it->data != NULL))
		{
			////!!!!lowercase(name_it->data->cstr);
			if (type_it->data->type_class == record_type)
			{
				char copy_sig[64];
				char type_descriptor[128];
				get_field_descriptor(type_it->data, type_descriptor);
				sprintf(copy_sig, "(LR_%d;)LR_%d;", record->unique_record_ID, record->unique_record_ID);
				bytecode_append(code, aload_0$);
				bytecode_append(code, getfield$);				
				bytecode_append_short_int(code, cp_add_fieldref(type_name, name_it->data->cstr, type_descriptor));
				bytecode_append(code, aload_1$);
				bytecode_append(code, getfield$);				
				bytecode_append_short_int(code, cp_add_fieldref(type_name, name_it->data->cstr, type_descriptor));
				bytecode_append(code, invokevirtual$);
				bytecode_append_short_int(code, cp_add_methodref(type_name, "Copy", copy_sig));
			}
			else if (type_it->data->type_class == string_type)
			{
				bytecode_append(code, aload_0$);
				bytecode_append(code, aload_1$);
				bytecode_append(code, getfield$);
				bytecode_append_short_int(code, cp_add_fieldref(type_name, name_it->data->cstr, "Ljava/lang/String;"));
				bytecode_append(code, new$);
				bytecode_append_short_int(code, cp_add_class("java/lang/String"));
				bytecode_append(code, dup_x1$);
				bytecode_append(code, swap$);
				bytecode_append(code, invokespecial$);
				bytecode_append_short_int(code, cp_add_methodref("java/lang/String", "<init>", "(Ljava/lang/String;)V"));
				bytecode_append(code, putfield$);
				bytecode_append_short_int(code, cp_add_fieldref(type_name, name_it->data->cstr, "Ljava/lang/String;"));
			}
			else if ((type_it->data->type_class == real_type) && (mathType != 1))
			{
				bytecode_append(code, aload_0$);
				bytecode_append(code, aload_1$);
				bytecode_append(code, getfield$);
				bytecode_append_short_int(code, cp_add_fieldref(type_name, name_it->data->cstr, "LReal;"));
				bytecode_append(code, new$);
				bytecode_append_short_int(code, cp_add_class("Real"));
				bytecode_append(code, dup_x1$);
				bytecode_append(code, swap$);
				bytecode_append(code, invokespecial$);
				bytecode_append_short_int(code, cp_add_methodref("Real", "<init>", "(LReal;)V"));
				bytecode_append(code, putfield$);
				bytecode_append_short_int(code, cp_add_fieldref(type_name, name_it->data->cstr, "LReal;"));
			}
			else
			{
				char type_descriptor[128];
				get_field_descriptor(type_it->data, type_descriptor);
				bytecode_append(code, aload_0$);
				bytecode_append(code, aload_1$);
				bytecode_append(code, getfield$);				
				bytecode_append_short_int(code, cp_add_fieldref(type_name, name_it->data->cstr, type_descriptor));
				bytecode_append(code, putfield$);
				bytecode_append_short_int(code, cp_add_fieldref(type_name, name_it->data->cstr, type_descriptor));
			}
		}
		type_it = type_it->next;
		if (name_it != NULL) name_it = name_it->next;
		counter ++;
	}
	// return this
	bytecode_append(code, aload_0$);
	bytecode_append(code, areturn$);
}

/*
	Writes the <init> method to the file.
*/
void write_init_method(FILE *fp, bytecode *code)
{
	/* write the method headers */
	/* write access flags, ACC_PUBLIC  */
	write_short_int(fp, 0x0001);
	/* write method name '<init>' */
	write_short_int(fp, cp_add_utf8("<init>"));
	/* write method descriptor '()V' */
	write_short_int(fp, cp_add_utf8("()V"));
	/* write 1 attribute */
	write_short_int(fp, 1);
	/* write the Code attribute 'Code' */
	write_short_int(fp, cp_add_utf8("Code"));
	write_long_int(fp, code->bytecode_pos + 12);
	/* write the max stack */
	write_short_int(fp, 10);
	/* max locals for program block */
	write_short_int(fp, 2);	
	/* code length */
	write_long_int(fp, code->bytecode_pos);
	/* write the code itself */
	if (fp != NULL) fwrite(code->bytecode, 1, code->bytecode_pos, fp);
	bytecode_destroy(code);
	write_short_int(fp, 0);
	write_short_int(fp, 0);
}

/*
	Writes the Copy method to the file.
*/
void write_copy_method(type* record, FILE *fp, bytecode *code)
{
	char method_sig[64];
	/* write the method headers */
	/* write access flags, ACC_PUBLIC  */
	write_short_int(fp, 0x0001);
	/* write method name '<init>' */
	write_short_int(fp, cp_add_utf8("Copy"));
	/* write method descriptor (record)record */
	sprintf(method_sig, "(LR_%d;)LR_%d;", record->unique_record_ID, record->unique_record_ID);
	write_short_int(fp, cp_add_utf8(method_sig));
	/* write 1 attribute */
	write_short_int(fp, 1);
	/* write the Code attribute 'Code' */
	write_short_int(fp, cp_add_utf8("Code"));
	write_long_int(fp, code->bytecode_pos + 12);
	/* write the max stack */
	write_short_int(fp, 10);
	/* max locals for program block */
	write_short_int(fp, 2);	
	/* code length */
	write_long_int(fp, code->bytecode_pos);
	/* write the code itself */
	if (fp != NULL) fwrite(code->bytecode, 1, code->bytecode_pos, fp);
	bytecode_destroy(code);
	write_short_int(fp, 0);
	write_short_int(fp, 0);
}
