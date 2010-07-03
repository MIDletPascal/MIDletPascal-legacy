/********************************************************************
	
	constant_pool.c - methods for handling constant pool

  Niksa Orlic, 2004-06-11

********************************************************************/

#include <stdio.h>
#include "constant_pool.h"
#include "../util/strings.h"

constant_pool *constants;
int constant_pool_size = 0;

/*
	Insert a utf8 string into the constant pool table and return its index in the table.
*/
int cp_add_utf8(char *str)
{
	int i, j, len;
	int size = 0;
	constant_pool *entry;
	
	/* check if the string already exists in the table */
	for (i=0; i<constant_pool_size; i++)
	{
		if ((constants[i].tag == 1)
			&& (strcmp(constants[i].data, str) == 0))
		{
			return i+1; /* the indexes for the constant pool are in range 1..N (not zero based) */
		}
	}

	/* create a new entry */
	entry = cp_create_new_entry();
	entry->tag = 1;

	/* calculate the needeed size for the string */
	len = strlen(str);
	for (i=0; i<len; i++)
	{
	/*	if ((unsigned char)str[i] > 127)
			size += 2;
		else*/
			size ++;
	}

	entry->data = (char*) mem_alloc(size + 1);
	entry->data_len = size;

	if (entry->data == NULL)
		die(1);

	for(i=0, j=0; i<len; i++)
	{
	/*	if ((unsigned char)str[i] <= 127)
		{*/
			entry->data[j] = str[i];
			j++;
	/*	}
		else
		{
			entry->data[j] = (char)0xC2 | (str[i]>>7)&0x01;
			j++;
			entry->data[j] = (str[i] & 0x3F) | 0x80;
			j++;
		}*/
	}

	entry->data[j] = '\0';

	return constant_pool_size;
}

/*
	Add a string entry into the constant pool.
*/
int cp_add_string(char *str)
{
	int i;
	constant_pool *entry;

	/* first add the utf8 constant */
	int utf8_const = cp_add_utf8(str);

	/* search if the same string already exists */
	for(i=0; i<constant_pool_size; i++)
	{
		if ((constants[i].tag == 8)
			&& (constants[i].param1 == utf8_const))
		{
			return i+1;
		}
	}

	/* create the new entry */
	entry = cp_create_new_entry();
	entry->tag = 8;
	entry->param1 = utf8_const;

	return constant_pool_size;
}

/*
	Add an integer into the constant pool.
*/
int cp_add_integer(int data)
{
	int i;
	constant_pool *entry;
	
	/* check if the string already exists in the table */
	for (i=0; i<constant_pool_size; i++)
	{
		if (constants[i].tag == 3)
		{
			int *p_value;
			p_value = (int*)(constants[i].data);
			if(*p_value == data)
				return i+1; /* the indexes for the constant pool are in range 1..N (not zero based) */
		}
	}

	/* create a new entry */
	entry = cp_create_new_entry();
	entry->tag = 3;
	entry->data = (char*) mem_alloc(sizeof(int));

	if (entry->data == NULL)
		die(1);

	*((int*)(entry->data)) = data;

	return constant_pool_size;
}

/*
	Add a long constant into the constant pool
*/
int cp_add_long(long data)
{
	die(18);
	
	return constant_pool_size;
}

/*
	Add a float constant into the constant pool
*/
int cp_add_double(double data)
{
	die(18);

	return constant_pool_size;
}

/*
	Add a double constant into the constant pool
*/
int cp_add_float(float data)
{
	int i;
	constant_pool *entry;
	
	/* check if the string already exists in the table */
	for (i=0; i<constant_pool_size; i++)
	{
		if (constants[i].tag == 6)
		{
			float *p_value;
			p_value = (float*)(constants[i].data);
			if(*p_value == data)
				return i+1; /* the indexes for the constant pool are in range 1..N (not zero based) */
		}
	}

	/* create a new entry */
	entry = cp_create_new_entry();
	entry->tag = 6;
	entry->data = (char*) mem_alloc(sizeof(float));

	if (entry->data == NULL)
		die(1);

	*((float*)(entry->data)) = data;

	return constant_pool_size;
}


/*
	Adds a class information into the constants pool
*/
int cp_add_class(char *class_name)
{
	int i;
	constant_pool *entry;

	/* first add the utf8 constant */
	int utf8_const = cp_add_utf8(class_name);

	/* search if the same string already exists */
	for(i=0; i<constant_pool_size; i++)
	{
		if ((constants[i].tag == 7)
			&& (constants[i].param1 == utf8_const))
		{
			return i+1;
		}
	}

	/* create the new entry */
	entry = cp_create_new_entry();
	entry->tag = 7;
	entry->param1 = utf8_const;

	return constant_pool_size;
}

/*
	Add name and type entry into the constant pool
*/
int cp_add_nameandtype(char *name, char *type)
{
	int i;
	int name_index;
	int type_index;
	constant_pool *entry;

	/* first add the utf8 constant */
	name_index = cp_add_utf8(name);
	type_index = cp_add_utf8(type);

	/* search if the same string already exists */
	for(i=0; i<constant_pool_size; i++)
	{
		if ((constants[i].tag == 12)
			&& (constants[i].param1 == name_index)
			&& (constants[i].param2 == type_index))
		{
			return i+1;
		}
	}

	/* create the new entry */
	entry = cp_create_new_entry();
	entry->tag = 12;
	entry->param1 = name_index;
	entry->param2 = type_index;

	return constant_pool_size;
}

/*
	Adds the fieldref entry into the constant pool
*/
int cp_add_fieldref(char *class_name, char *name, char *type)
{
	int i;
	int class_index;
	int nameandtype_index;
	constant_pool *entry;

	/* first add the utf8 constant */
	class_index = cp_add_class(class_name);
	nameandtype_index = cp_add_nameandtype(name, type);

	/* search if the same string already exists */
	for(i=0; i<constant_pool_size; i++)
	{
		if ((constants[i].tag == 9)
			&& (constants[i].param1 == class_index)
			&& (constants[i].param2 == nameandtype_index))
		{
			return i+1;
		}
	}

	/* create the new entry */
	entry = cp_create_new_entry();
	entry->tag = 9;
	entry->param1 = class_index;
	entry->param2 = nameandtype_index;

	return constant_pool_size;
}

/*
	Add the fieldref entry into the constant pool
*/
int cp_add_methodref(char *class_name, char *name, char *type)
{
	int i;
	int class_index;
	int nameandtype_index;
	constant_pool *entry;

	/* first add the utf8 constant */
	class_index = cp_add_class(class_name);
	nameandtype_index = cp_add_nameandtype(name, type);

	/* search if the same string already exists */
	for(i=0; i<constant_pool_size; i++)
	{
		if ((constants[i].tag == 10)
			&& (constants[i].param1 == class_index)
			&& (constants[i].param2 == nameandtype_index))
		{
			return i+1;
		}
	}

	/* create the new entry */
	entry = cp_create_new_entry();
	entry->tag = 10;
	entry->param1 = class_index;
	entry->param2 = nameandtype_index;

	return constant_pool_size;
}

/*
	Creates a new entry in the constant pool.
*/
constant_pool *cp_create_new_entry()
{
	constant_pool_size ++;

	if (constant_pool_size == 1)
		constants = (constant_pool*) mem_alloc(sizeof(constant_pool));
	else
		constants = (constant_pool*) mem_realloc(constants, 
		                                     sizeof(constant_pool) * constant_pool_size);

	if (constants == NULL)
		die(1);

	return &constants[constant_pool_size-1];
}

/*
	Writes the constant pool to the disk
*/
void write_constant_pool(FILE *fp)
{
	int i;

	/* write the constant pool count */
	write_short_int(fp, (short)(constant_pool_size + 1));

	/* write the constant pool entries */
	for(i=0; i<constant_pool_size; i++)
	{
		/* write the tag */
		char tag = constants[i].tag;

		fwrite(&tag, 1, 1, fp);

		switch (tag)
		{
		case 7: /* constant_class */
			{
				write_short_int(fp, constants[i].param1);
			}
		break;

		case 9:  /* fieldref */
		case 10: /* methodref */
		case 12: /* name and type */
			{
				write_short_int(fp, constants[i].param1);
				write_short_int(fp, constants[i].param2);
			}
		break;

		case 8: /* string */
			{
				write_short_int(fp, constants[i].param1);
			}
		break;

		case 3:  /* integer */
			{
				write_long_int(fp, *((int*)constants[i].data));
			}
		break;

		case 4: /* float */
			{
				int *data;
				data = (int*) constants[i].data;
				write_long_int(fp, *data);
			}
		break;

		case 1: /* Utf-8 */
			{
				write_short_int(fp, (short) constants[i].data_len);
				fwrite(constants[i].data, 1, constants[i].data_len, fp);
			}
		break;

		default:
			die(16);
		}
	}
}