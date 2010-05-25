/********************************************************************
	
	preverify.c - methods for preverifing class files

  Niksa Orlic, 2004-05-16

********************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#include "../util/message.h"
#include "../util/error.h"
#include "../util/strings.h"
#include "../structures/type_list.h"
#include "../structures/string_list.h"
#include "../structures/type.h"
#include "../structures/identifier.h"
#include "../structures/name_table.h"
#include "constant_pool.h"
#include "bytecode.h"
#include "preverify.h"
#include "../util/error.h"
#include "../util/message.h"
#include "../util/memory.h"

extern constant_pool *constants;
extern int constant_pool_size;

stack_map_list *pending_list;
stack_map_list *output_list;


stack_map_list *preverify_bytecode(bytecode *code, identifier *block_identifier)
{
	stack_map *map;
	pending_list = NULL;
	output_list = NULL;

	/* go through the bytecode */
	map = stack_map_create();
	map->bytecode_offset = 0;
	preverify_bytecode_from(map, code, block_identifier);
	stack_map_destroy(map);

	/* revisit the offsets in the pending list */
	while (pending_list != NULL)
	{
		map = stack_map_list_get(&pending_list);
		if (map == NULL)
			break;

		stack_map_list_append(&output_list, stack_map_duplicate(map));
		preverify_bytecode_from(map, code, block_identifier);
		stack_map_destroy(map);
	}

	output_list = sort_map_list(output_list);

	return output_list;
}


/*
	Sorts the list according to offsets
*/
stack_map_list* sort_map_list(stack_map_list *list)
{
	int range = 0;

	stack_map_list *it;
	stack_map_list *sorted_list = NULL;

	/* repeat until the list is empty */
	while (list != NULL)
	{
		/* find the next smallest number */
		int smallest = 0x0fffffff;

		it = list;
		while (it != NULL)
		{
			if ((it->data->bytecode_offset < smallest)
				&& (it->data->bytecode_offset >= range))
				smallest = it->data->bytecode_offset;
			it = it->next;
		}

		if (smallest == 0x0fffffff)
			break;

		range = smallest + 1;

		/* extract the smallest number into the output list */
		it = list;
		while (it != NULL)
		{
			if (it->data->bytecode_offset == smallest)
			{
				stack_map_list_append(&sorted_list, it->data);

				break;
			}			
			it = it->next;
		}

	}

	return sorted_list;
}


/*
	Preverifies the bytecode starting with the given stackmap
*/
void preverify_bytecode_from(stack_map *map, bytecode *code, identifier *block_identifier)
{
	int offset;
	offset = map->bytecode_offset;

	while (preverify_consume_bytecode(code, map, &offset, block_identifier))
	{
		/* do nothing */
	}
}


/*
	Reads one bytecode and updates stack map. Returns 0 if
	'return' bytocode is found, or 1 otherwise.

	It seems that most of the jumps have nothing on the stack. The few 
	jumps that have something on the stack will only have integers.
*/
int preverify_consume_bytecode(bytecode *code, stack_map *map, int *offset, identifier *block_identifier)
{
	unsigned char opcode = code->bytecode[*offset];
	switch ((unsigned char)(code->bytecode[*offset]))
	{
	case nop$: 
		(*offset) ++; 
		break;

	case aconst_null$: 
		(*offset) ++;
		stack_map_push_entry(map, ITEM_Null, 0);
		break;

	case iconst_m1$:
	case iconst_0$:
	case iconst_1$:
	case iconst_2$:
	case iconst_3$:
	case iconst_4$:
	case iconst_5$:
		(*offset) ++;
		stack_map_push_entry(map, ITEM_Integer, 0);
		break;

	case lconst_0$:
	case lconst_1$:
		(*offset) ++;
		stack_map_push_entry(map, ITEM_Long, 0);
		break;

	case bipush$:
		(*offset) += 2;
		stack_map_push_entry(map, ITEM_Integer, 0);
		break;

	case sipush$:
		(*offset) += 3;
		stack_map_push_entry(map, ITEM_Integer, 0);
		break;

	case iload$:
		(*offset) += 2;
		stack_map_push_entry(map, ITEM_Integer, 0);
		break;

	case lload$:
		(*offset) += 2;
		stack_map_push_entry(map, ITEM_Long, 0);
		break;

	case iload_0$:
	case iload_1$:
	case iload_2$:
	case iload_3$:
		(*offset) ++;
		stack_map_push_entry(map, ITEM_Integer, 0);
		break;

	case lload_0$:
	case lload_1$:
	case lload_2$:
	case lload_3$:
		(*offset) ++;
		stack_map_push_entry(map, ITEM_Long, 0);
		break;

	case iaload$:
	case baload$:
	case caload$:
	case saload$:
		(*offset) ++;
		stack_entry_destroy(stack_map_pop(map));
		stack_entry_destroy(stack_map_pop(map));
		stack_map_push_entry(map, ITEM_Integer, 0);
		break;

	case laload$:
		(*offset) ++;
		stack_entry_destroy(stack_map_pop(map));
		stack_entry_destroy(stack_map_pop(map));
		stack_map_push_entry(map, ITEM_Long, 0);
		break;

	case istore$:
	case lstore$:
	case astore$:
		(*offset) += 2;
		stack_entry_destroy(stack_map_pop(map));
		break;

	case istore_0$:
	case istore_1$:
	case istore_2$:
	case istore_3$:
	case lstore_0$:
	case lstore_1$:
	case lstore_2$:
	case lstore_3$:
	case astore_0$:
	case astore_1$:
	case astore_2$:
	case astore_3$:
		(*offset) ++;
		stack_entry_destroy(stack_map_pop(map));
		break;

	case iastore$:
	case lastore$:
	case fastore$:
	case dastore$:
	case aastore$:
	case bastore$:
	case castore$:
	case sastore$:
		(*offset) ++;
		stack_entry_destroy(stack_map_pop(map));
		stack_entry_destroy(stack_map_pop(map));
		stack_entry_destroy(stack_map_pop(map));
		break;

	case pop$:
		(*offset) ++;
		stack_entry_destroy(stack_map_pop(map));
		break;

	case pop2$:
		(*offset) ++;
		stack_entry_destroy(stack_map_pop(map));
		stack_entry_destroy(stack_map_pop(map));
		break;

	case dup$:
		{
			stack_entry *entry;
			(*offset) ++;
			entry = stack_map_pop(map);
			stack_map_push(map, entry);
			stack_map_push(map, entry);
			stack_entry_destroy(entry);
		}
		break;

	case dup_x1$:
	case dup_x2$:
		{
			stack_entry *e1, *e2;
			(*offset) ++;
			e1 = stack_map_pop(map);
			e2 = stack_map_pop(map);
			stack_map_push(map, e1);
			stack_map_push(map, e2);
			stack_map_push(map, e1);
			stack_entry_destroy(e1);
			stack_entry_destroy(e2);
		}
		break;

	case dup2_x1$:
		{
			stack_entry *e1, *e2, *e3;
			(*offset) ++;
			e1 = stack_map_pop(map);
			e2 = stack_map_pop(map);
			e3 = stack_map_pop(map);
			stack_map_push(map, e2);
			stack_map_push(map, e1);
			stack_map_push(map, e3);
			stack_map_push(map, e2);
			stack_map_push(map, e1);			
			stack_entry_destroy(e1);
			stack_entry_destroy(e2);
			stack_entry_destroy(e3);
		}
		break;

	case swap$:
		{
			stack_entry *e1, *e2;
			(*offset) ++;
			e1 = stack_map_pop(map);
			e2 = stack_map_pop(map);
			stack_map_push(map, e1);
			stack_map_push(map, e2);
			stack_entry_destroy(e1);
			stack_entry_destroy(e2);
		}
		break;

	case iadd$:
	case ladd$:
	case fadd$:
	case dadd$:
	case isub$:
	case lsub$:
	case fsub$:
	case dsub$:
	case imul$:
	case lmul$:
	case fmul$:
	case dmul$:
	case idiv$:
	case ldiv$:
	case fdiv$:
	case ddiv$:
	case irem$:
	case lrem$:
	case frem$:
	case drem$:
	case iand$:
	case land$:
	case ior$:
	case lor$:
	case ixor$:
	case lxor$:
		(*offset) ++;
		stack_entry_destroy(stack_map_pop(map));
		break;

	case ineg$:
	case lneg$:
	case fneg$:
	case dneg$:
	case i2b$:
	case i2c$:
	case i2s$:
		(*offset) ++;
		break;

	case ishl$:
	case lshl$:
	case ishr$:
	case lshr$:
	case iushr$:
	case lushr$:
		(*offset) ++;
		stack_entry_destroy(stack_map_pop(map));
		break;

	case iinc$:
		(*offset) += 3;
		break;

	case i2l$:
		(*offset) ++;
		stack_entry_destroy(stack_map_pop(map));
		stack_map_push_entry(map, ITEM_Long, 0);
		break;

	case l2i$:
		(*offset) ++;
		stack_entry_destroy(stack_map_pop(map));
		stack_map_push_entry(map, ITEM_Integer, 0);
		break;

	case lcmp$:
	case fcmpl$:
	case fcmpg$:
	case dcmpl$:
	case dcmpg$:
		(*offset) ++;
		stack_entry_destroy(stack_map_pop(map));
		stack_entry_destroy(stack_map_pop(map));
		stack_map_push_entry(map, ITEM_Integer, 0);
		break;

	case ifeq$:
	case ifne$:
	case iflt$:
	case ifge$:
	case ifgt$:
	case ifle$:
		(*offset) ++;
		stack_entry_destroy(stack_map_pop(map));
		process_jump(map, *offset - 1 + (int)((code->bytecode[*offset] << 8) | (unsigned char)(code->bytecode[(*offset) + 1])));		
		(*offset) ++;
		(*offset) ++;
		break;

	case if_icmpeq$:
	case if_icmpne$:
	case if_icmplt$:
	case if_icmpge$:
	case if_icmpgt$:
	case if_icmple$:
	case if_acmpeq$:
	case if_acmpne$:
		(*offset) ++;
		stack_entry_destroy(stack_map_pop(map));
		stack_entry_destroy(stack_map_pop(map));
		process_jump(map, *offset - 1 + (int)((code->bytecode[*offset] << 8) | (unsigned char)(code->bytecode[(*offset) + 1])));		
		(*offset) ++;
		(*offset) ++;
		break;

	case goto$:
		{
			(*offset) ++;
			process_jump(map, *offset - 1 + (int)((code->bytecode[*offset] << 8) | (unsigned char)(code->bytecode[(*offset) + 1])));
			return 0;
		}
		break;

	case ireturn$:
	case lreturn$:
	case freturn$:
	case dreturn$:
	case areturn$:
		stack_entry_destroy(stack_map_pop(map));
	case return$:
		return 0;

	case aload$:
		(*offset) += 2;
		stack_map_push_local(map, block_identifier, (unsigned char) code->bytecode[*offset-1]);
		break;

	case aload_0$:
		(*offset) ++;
		stack_map_push_local(map, block_identifier, 0);
		break;

	case aload_1$:
		(*offset) ++;
		stack_map_push_local(map, block_identifier, 1);
		break;

	case aload_2$:
		(*offset) ++;
		stack_map_push_local(map, block_identifier, 2);
		break;

	case aload_3$:
		(*offset) ++;
		stack_map_push_local(map, block_identifier, 3);
		break;

	case aaload$:
		{
			char object_class[128];
			stack_entry* entry;
			int classname_index;
			int delta = 1;

			(*offset) ++;
			stack_entry_destroy(stack_map_pop(map));

			entry = stack_map_pop(map);
			classname_index = constants[entry->additional_data - 1].param1 - 1;
			strncpy(object_class, constants[classname_index].data, constants[classname_index].data_len);
			object_class[constants[classname_index].data_len] = '\0';
			
			if (object_class[1] == 'L')
			{
				delta = 2;
				object_class[strlen(object_class) - 1] = '\0';
			}
			stack_entry_destroy(entry);
			stack_map_push_entry(map, ITEM_Object, cp_add_class(object_class + delta));
		}
		break;

	case tableswitch$:
	case lookupswitch$:
		// TODO::
		break;

	case instanceof$:
		(*offset) += 3;
		stack_entry_destroy(stack_map_pop(map));		
		stack_map_push_entry(map, ITEM_Integer, 0);
		break;

	case ldc$:
		(*offset) ++;
		(*offset) ++;
		stack_map_push_entry(map, ITEM_Bogus, 0);
		break;

	case ldc_w$:
		(*offset) += 3;
		stack_map_push_entry(map, ITEM_Bogus, 0);
		break;

	case getstatic$:
		{
			int t;
			(*offset) ++;
			t = code->bytecode[*offset];
			t = t << 8 | (unsigned char)(code->bytecode[(*offset) + 1]);
			t = constants[t-1].param2;
			t = constants[t-1].param2;
			(*offset) += 2;
			switch(constants[t-1].data[0])
			{
			case  'L':
			case '[':
			{
				char *class_name = (char*) mem_alloc(sizeof(char) * strlen(constants[t-1].data));
				strcpy(class_name, constants[t-1].data + 1);
				class_name[strlen(constants[t-1].data) - 2] = '\0';
				stack_map_push_entry(map, ITEM_Object, cp_add_class(class_name));
				mem_free(class_name);
				break;
			}
	//		case '[':
	//			stack_map_push_entry(map, ITEM_Object, t);
	//			break;
			case 'I':
				stack_map_push_entry(map, ITEM_Integer, 0);
				break;
			default:
				stack_map_push_entry(map, ITEM_Bogus, 0);
			
			}
		}
		break;

	case new$:
		(*offset) += 3;
		stack_map_push_entry(map, ITEM_Bogus, 0);
		break;

	case getfield$:
		(*offset) += 3;
		break;

	case putstatic$:
		(*offset) += 3;
		stack_entry_destroy(stack_map_pop(map));
		break;

	case putfield$:
		(*offset) += 3;
		stack_entry_destroy(stack_map_pop(map));
		stack_entry_destroy(stack_map_pop(map));
		break;

	case invokevirtual$:
	case invokespecial$:
	case invokestatic$:
		{
			unsigned short int num;
			int len, i = 0;
			int oldstate, state = 0;
			int count = 0;
			int isVoid = 0;
			char *descriptor;
			char *class_name;
			int state_machine_table[4][6] = 
			{   /*        (   )   L   ;   V   else       */
				/* 0 */ { 1, -1, -1, -1, -1, -1  },
				/* 1 */ {-1,  2,  3, -1,  1,  1  },
				/* 2 */ {-1, -1, -1, -1, -1, -1  },
				/* 3 */ { 3,  3,  3,  1,  3,  3  }
			};

			(*offset) ++;
			num = (code->bytecode[*offset] << 8) | (unsigned char)(code->bytecode[(*offset) + 1]);
			(*offset) += 2;

			num = constants[num-1].param2;
			num = constants[num-1].param2;
			len = constants[num-1].data_len;
			descriptor = constants[num-1].data;
			
			while (i < len)
			{
				oldstate = state;

				switch (descriptor[i])
				{
				case '(': state = state_machine_table[state][0]; break;
				case ')': state = state_machine_table[state][1]; break;
				case 'L': state = state_machine_table[state][2]; break;
				case ';': state = state_machine_table[state][3]; break;
				case 'V': state = state_machine_table[state][4]; break;
				default : state = state_machine_table[state][5]; break;
				}
				i ++;

				if ((state == 1) && (oldstate != 0))
					count ++;

				if (state == 2)
				{
					if (descriptor[i] == 'V')
						isVoid = 1;

					class_name = (char*) mem_alloc(len);

					strcpy(class_name, descriptor + i);

					break;
				}
			}

			if (opcode != invokestatic$)
				stack_entry_destroy(stack_map_pop(map));
			while (count > 0)
			{
				stack_entry_destroy(stack_map_pop(map));
				count --;
			}

			if (class_name[0] == 'L')
			{
				class_name[strlen(class_name) - 1] = '\0';
				if (!isVoid)
					stack_map_push_entry(map, ITEM_Object, cp_add_class(class_name+1));
			}
			else if (class_name[0] == 'I')
			{
				if (!isVoid)
					stack_map_push_entry(map, ITEM_Integer,0);
			}
			else
			{
				if (!isVoid)
					stack_map_push_entry(map, ITEM_Bogus, 0);

			}

			mem_free(class_name);
		}
		break;

	
	/*case invokestatic$:
				{
			unsigned short int num;
			int len, i = 0;
			int oldstate, state = 0;
			int count = 0;
			int isVoid = 0;
			char *descriptor;
			int state_machine_table[4][6] = 
			{   /*        (   )   L   ;   V   else       
				/* 0  { 1, -1, -1, -1, -1, -1  },
				/* 1  {-1,  2,  3, -1,  1,  1  },
				/* 2  {-1, -1, -1, -1, -1, -1  },
				/* 3  { 3,  3,  3,  1,  3,  3  }
			};

			(*offset) ++;
			num = (code->bytecode[*offset] << 8) | (unsigned char)(code->bytecode[(*offset) + 1]);
			(*offset) += 2;

			num = constants[num-1].param2;
			num = constants[num-1].param2;
			len = constants[num-1].data_len;
			descriptor = constants[num-1].data;
			
			while (i < len)
			{
				oldstate = state;

				switch (descriptor[i])
				{
				case '(': state = state_machine_table[state][0]; break;
				case ')': state = state_machine_table[state][1]; break;
				case 'L': state = state_machine_table[state][2]; break;
				case ';': state = state_machine_table[state][3]; break;
				case 'V': state = state_machine_table[state][4]; break;
				default : state = state_machine_table[state][5]; break;
				}
				i ++;

				if ((state == 1) && (oldstate != 0))
					count ++;

				if (state == 2)
				{
					if (descriptor[i] == 'V')
						isVoid = 1;

					break;
				}
			}

			while (count > 0)
			{
				stack_entry_destroy(stack_map_pop(map));
				count --;
			}

			if (!isVoid)
				stack_map_push_entry(map, ITEM_Bogus, 0);
		}
		break;*/
	
	
	case newarray$:
		(*offset) += 2;
		break;

	case anewarray$:
		(*offset) += 3;
		break;

	case arraylength$:
		(*offset) ++;
		stack_entry_destroy(stack_map_pop(map));
		stack_map_push_entry(map, ITEM_Integer, 0);
		break;
	
	case multianewarray$:
		{
			unsigned int i, t, s;
			(*offset) ++;
			t = code->bytecode[*offset];
			t = t << 8 | (unsigned char)(code->bytecode[(*offset) + 1]);
			(*offset) += 2;
			i = code->bytecode[*offset];
			(*offset) ++;

			s = i;
			while (i>0)
			{
				stack_entry_destroy(stack_map_pop(map));
				i --;
			}

			stack_map_push_entry(map, ITEM_NewObject, *offset - 4);
			break;
		}

	case ifnull$:
	case ifnonnull$:
		stack_entry_destroy(stack_map_pop(map));
		(*offset) ++;
		process_jump(map, *offset - 1 + (int)((code->bytecode[*offset] << 8) | (unsigned char)(code->bytecode[(*offset) + 1])));
		(*offset) ++;
		(*offset) ++;
		break;

	default:
		die(MSG_022);

	}

	return 1;
}


/*
	When a branch instruction is found, adds the target offset into the list.
*/
void process_jump(stack_map *map, int position)
{
	stack_map *new_map;

	/* check if the position is already in the pending or output list */
	stack_map_list *it;
	it = pending_list;

	while (it != NULL)
	{
		if (it->data->bytecode_offset == position)
			return;
		it = it->next;
	}

	it = output_list;

	while (it != NULL)
	{
		if (it->data->bytecode_offset == position)
			return;
		it = it->next;
	}

	/* if here, add the offset into the pendign list */
	new_map = stack_map_duplicate(map);
	new_map->bytecode_offset = position;
	stack_map_list_append(&pending_list, new_map);
}


/*
	Create anew stack_entry object.
*/
stack_entry* stack_entry_create()
{
	stack_entry *new_entry = (stack_entry*) mem_alloc(sizeof(stack_entry));

	if (new_entry == NULL)
		die(MSG_001);

	return new_entry;
}


/*
	Delete all data used by stack_entry
*/
void stack_entry_destroy(stack_entry *entry)
{
	mem_free(entry);
}


/*
	Create a copy of the stack entry.
*/
stack_entry* stack_entry_duplicate(stack_entry *old_entry)
{
	stack_entry* new_entry = (stack_entry*) mem_alloc(sizeof(stack_entry));

	if (new_entry == NULL)
		die(MSG_001);

	new_entry->item_type = old_entry->item_type;
	new_entry->additional_data = old_entry->additional_data;

	return new_entry;
}


/*
	Create a stackmap object
*/
stack_map* stack_map_create()
{
	stack_map *new_map = (stack_map*) mem_alloc(sizeof(stack_map));

	if (new_map == NULL)
		die(MSG_001);

	new_map->allocated_number_of_items = 0;
	new_map->number_of_items = 0;

	return new_map;
}


/*
	Destroy the stack map
*/
void stack_map_destroy(stack_map *map)
{
	int i;
	for (i=0; i<map->number_of_items; i++)
	{
		stack_entry_destroy(map->stack[i]);
	}

	if (map->allocated_number_of_items > 0)
		mem_free(map->stack);
	
	mem_free(map);
}


/*
	Create a copy of the map
*/
stack_map* stack_map_duplicate(stack_map *old_map)
{
	int i;

	stack_map *new_map = (stack_map*) mem_alloc(sizeof(stack_map));

	if (new_map == NULL)
		die(MSG_001);

	new_map->number_of_items = old_map->number_of_items;
	new_map->allocated_number_of_items = old_map->allocated_number_of_items;
	new_map->bytecode_offset = old_map->bytecode_offset;

	if (old_map->allocated_number_of_items > 0)
	{
		new_map->stack = (stack_entry**) mem_alloc(sizeof(stack_entry) * old_map->allocated_number_of_items);

		if (new_map->stack == NULL)
			die(MSG_001);
	}

	for (i=0; i<old_map->number_of_items; i++)
		new_map->stack[i] = stack_entry_duplicate(old_map->stack[i]);

	return new_map;
}


/*
	Take the top element from the stack
*/
stack_entry* stack_map_pop(stack_map* map)
{
	stack_entry* top_element;

	top_element = map->stack[map->number_of_items - 1];

	map->number_of_items --;

	return top_element;
}


/*
	Add a new element to the top of the stack
*/
void stack_map_push(stack_map *map, stack_entry *entry)
{
	stack_map_push_entry(map, entry->item_type, entry->additional_data);
}


/*
	Add a new element to the top of the stack.
*/
void stack_map_push_entry(stack_map *map, enum stack_item item_type, short int additional_data)
{
	stack_entry *new_entry = stack_entry_create();
	new_entry->item_type = item_type;
	new_entry->additional_data = additional_data;

	/*
		Do realloc or malloc if needeed
	*/
	if (map->allocated_number_of_items <= map->number_of_items)
	{
		if (map->allocated_number_of_items == 0)
		{
			map->stack = (stack_entry**) mem_alloc(sizeof(stack_entry*));

			if (map->stack == NULL)
				die(MSG_001);

			map->allocated_number_of_items = 1;
		}
		else
		{
			map->stack = (stack_entry**) mem_realloc(map->stack, sizeof(stack_entry*) * (map->allocated_number_of_items + 1));

			if (map->stack == NULL)
				die(MSG_001);

			map->allocated_number_of_items ++;
		}
	}

	map->number_of_items ++;
	map->stack[map->number_of_items - 1] = new_entry;
}

void stack_map_list_destroy(stack_map_list*);

/*
	Push a local variable, determine its type
*/
void stack_map_push_local(stack_map *map, identifier *block_identifier, int localNum)
{
	type *variable_type = NULL;
	char descriptor[256];
	type_list *it;
	int i = 0;
	
	if (block_identifier == NULL)
	{
		/* if block is the root block, this should never happen */
		stack_map_push_entry(map, ITEM_Bogus, 0);
		return;
	}

	it = block_identifier->parameters;	
	while (it != NULL)
	{
		if (it->data == NULL)
			break;

		if (i == localNum)
		{
			variable_type = type_duplicate(it->data);
			break;
		}
		i ++;
		it = it->next;
	}


	if ((block_identifier->identifier_class == function_name)
		&& (variable_type == NULL))
	{
		if (i == localNum)
			variable_type = type_duplicate(block_identifier->return_type);
		i ++;
	}

	it = block_identifier->variables;
	while ((it != NULL) && (variable_type == NULL))
	{
		if (it->data == NULL)
			break;

		if (i == localNum)
		{
			variable_type = type_duplicate(it->data);
			break;
		}
		i ++;
		it = it->next;
	}

	switch (variable_type->type_class)
	{
	case integer_type:
	case real_type:
	case char_type:
	case boolean_type:
		stack_map_push_entry(map, ITEM_Integer, 0);
		break;

	case array_type:
		get_field_descriptor(variable_type, descriptor);
		stack_map_push_entry(map, ITEM_Object, cp_add_class(descriptor));
		break;

	default:
		get_field_descriptor(variable_type, descriptor);
		descriptor[strlen(descriptor)-1] = '\0';
		stack_map_push_entry(map, ITEM_Object, cp_add_class(descriptor+1));
		break;
	}

	type_destroy(variable_type);

}

/*
	Append data to the end of the list
*/
void stack_map_list_append(stack_map_list **list, stack_map* data)
{
	stack_map_list *it;

	if (*list == NULL)
	{
		*list = (stack_map_list*)mem_alloc(sizeof(stack_map_list));
		if (*list == NULL)
			die(MSG_001);

		(*list)->data = data;
		(*list)->next = NULL;
		return;
	}

	it = *list;

	while (it->next != NULL)
		it = it->next;

	it->next = (stack_map_list*) mem_alloc(sizeof(stack_map_list));

	if (it->next == NULL)
		die(MSG_001);

	it->next->data = data;
	it->next->next = NULL;


}


/*
	Get the first element from the list	
*/
stack_map* stack_map_list_get(stack_map_list **list)
{
	stack_map_list *next;
	stack_map *data;

	if (*list == NULL)
		return NULL;

	next = (*list)->next;
	data = (*list)->data;

	*list = next;

	return data;
}