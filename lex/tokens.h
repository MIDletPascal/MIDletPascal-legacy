/********************************************************************
	
	tokens.h - definitions of token constants used by lexical
	scanner and the parser.

  Niksa Orlic, 2004-04-19

********************************************************************/

#define	CST_INTEGER		0
#define CST_REAL		1
#define CST_BOOLEAN		2
#define CST_CHAR		3
#define CST_STRING		4
#define OP_MOD			5
#define OP_DIV			6
#define OP_PLUS			7
#define OP_MINUS		8
#define OP_MULT			9
#define OP_SLASH		10
#define OP_EQUAL		11
#define KWD_BEGIN		12
#define KWD_END			13
#define OP_AND			14
#define KWD_PROGRAM		15
#define KWD_PROCEDURE	16
#define KWD_FUNCTION	17
#define SEMI_COLON		18
#define COLON			19
#define OP_ASSIGN		20
#define OP_LESS			21
#define OP_GREATER		22
#define OP_LESS_EQUAL	23
#define OP_GREATER_EQUAL 24
#define OP_NOT_EQUAL	25
#define DOT				26
#define OPEN_SQ_BR		27
#define CLOSE_SQ_BR		28
#define KWD_VAR			29
#define KWD_FOR			30
#define KWD_TO			31
#define KWD_DOWNTO		32
#define KWD_DO			33
#define OPEN_BR			34
#define CLOSE_BR		35
#define KWD_CONST		36
#define KWD_TYPE		37
#define KWD_IF			38
#define KWD_THEN		39
#define KWD_ELSE		40
#define KWD_CASE		41
#define KWD_OF			42
#define KWD_WHILE		43
#define KWD_REPEAT		44
#define KWD_UNTIL		45
#define KWD_WITH		46
#define KWD_PACKED		47
#define KWD_ARRAY		48
#define OP_OR			49
#define KWD_FILE		50
#define KWD_SET			51
#define KWD_RECORD		52
#define OP_IN			53
#define OP_NOT			54
#define OP_XOR			55
#define KWD_FORWARD		56
#define IDENTIFIER		57
#define COMMA			58
#define DOTDOT			59
#define KWD_BREAK		60
#define KWD_USES		61
#define KWD_UNIT		62
#define KWD_INTERFACE	63
#define KWD_IMPLEMENTATION 64
#define KWD_INITIALIZATION 65
#define KWD_FINALIZATION 66

#define END_OF_INPUT	100