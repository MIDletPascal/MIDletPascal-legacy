/********************************************************************
	
	parser.h - recursive-descent parser declarations

  Niksa Orlic, 2004-04-19

********************************************************************/


void parser_start();
string* RD_program_header();
void RD_uses_list();
block* RD_block(block*);
void RD_const_declaration();
void RD_var_declaration(block*);
string_list* RD_identifier_list(block*, int);
void RD_type_declaration(block*);
void RD_procedure_declaration(block*);
void RD_function_declaration(block*);
int RD_proc_block(block*, type*, type_list*);
type_list* RD_param_list(block*);
void RD_block_body(block*);
void RD_inline_body(block*);
void RD_statement(block *);
void RD_if_statement(block *);
void RD_case_statement(block *);
void RD_case_list(block*, type*);
void RD_while_statement(block*);
void RD_repeat_statement(block*);
void RD_for_statement(block*);
type* RD_type(block*);
type* RD_basic_type(block*);
type* RD_array_declaration(block*);
type* RD_record_declaration(block*);
type* RD_file_declaration(block*);
void RD_set_declaration(block*);
type* RD_expression(block*);
type* RD_sum(block*);
type* RD_mult(block*);
type* RD_not(block*);
type* RD_neg(block*);
void RD_assignment_or_procedure_call(block*); 
type_list* RD_expression_list(block*);
type_list* RD_expression_list_cast(block*, type_list*);
type_list* RD_expression_list_array(block*, bytecode*, type*);
void RD_with_statement(block*);
type* RD_value(block*);

void RD_unit_interface(block*);
void RD_unit_implementation(block*);		
void RD_unit_initialization(block*);
void RD_unit_finalization(block*);

void create_constant_bytecode(identifier*, bytecode*);
void create_variable_bytecode(identifier*, bytecode*, char*, int);
void create_put_variable_bytecode(identifier*, bytecode*, char*, int);

void replace_aaload_instruction(bytecode*, type*);
void adjust_indices(bytecode *code, type *dimension);
