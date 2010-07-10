/********************************************************************
	
	bytecode.c - funcions for creating the bytecode

  Niksa Orlic, 2004-06-11

********************************************************************/

#include "bytecode.h"
#include "../util/error.h"
//#include "../util/message.h"
#include "../util/memory.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//#include "../main/static_entry.h"

extern int linenum;

//extern FILE* yyin;
//extern int fileSize;

/*
	Allocate a new bytecode
*/
bytecode *bytecode_create()
{
	bytecode *new_bytecode = (bytecode*) mem_alloc(sizeof(bytecode));

	if (new_bytecode == NULL)
		die(1);

	new_bytecode->bytecode_allocated_size = 0;
	new_bytecode->bytecode_pos = 0;

	return new_bytecode;
}

/*
	Destroy the bytecode
*/
void bytecode_destroy(bytecode *code)
{
	if (code->bytecode_allocated_size > 0)
		mem_free(code->bytecode);

	mem_free(code);
}

bytecode *bytecode_duplicate(bytecode *code)
{
	bytecode *new_bytecode = bytecode_create();
	bytecode_append_bytecode(new_bytecode, code);

	return new_bytecode;
}


/*
	Appends a single byte to the bytecode
*/
void bytecode_append(bytecode *code, char c)
{
	//if (c == swap$)
	//{
	//	int a = 1;
	//}
	if (code->bytecode_pos == code->bytecode_allocated_size)
	{
		if (code->bytecode_allocated_size == 0)
			code->bytecode = (char*) mem_alloc(128);
		else
			code->bytecode = (char*) mem_realloc(code->bytecode, 
			                                 code->bytecode_allocated_size + 128);
		
		if (code->bytecode == NULL)
			die(1);

		code->bytecode_allocated_size += 128;
	}

	code->bytecode[code->bytecode_pos] = c;
	code->bytecode_pos ++;
}

/*
	Append one bytecode to another.
*/
void bytecode_append_bytecode(bytecode *dest, bytecode *src)
{
	if (src->bytecode_pos == 0)
		return;

	if (dest->bytecode_allocated_size == 0)
	{
		dest->bytecode_allocated_size += src->bytecode_pos;
		dest->bytecode = (char*) mem_alloc(dest->bytecode_allocated_size);
	}
	else
	{
		dest->bytecode_allocated_size += src->bytecode_pos;
		dest->bytecode = (char*) mem_realloc(dest->bytecode, dest->bytecode_allocated_size);
	}

	if (dest->bytecode == NULL)
		die(1);

	memcpy(dest->bytecode + dest->bytecode_pos, src->bytecode, src->bytecode_pos);
	dest->bytecode_pos += src->bytecode_pos;
}

/*
	Appends a short int to the bytecode
*/
void bytecode_append_short_int(bytecode *code, short int s)
{
	char ch;

	ch = (char) (s >> 8);
	bytecode_append(code, ch);
	
	ch = (char) s;
	bytecode_append(code, ch);
}

/*
	Appends a long int to the bytecode
*/
void bytecode_append_long_int(bytecode *code, long int l)
{
	char ch;

	ch = (char) (l >> 24);
	bytecode_append(code, ch);

	ch = (char) (l >> 16);
	bytecode_append(code, ch);

	ch = (char) (l >> 8);
	bytecode_append(code, ch);
	
	ch = (char) l;
	bytecode_append(code, ch);
}
