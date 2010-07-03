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
#define OP_SHR			11
#define OP_SHL			12
#define OP_EQUAL		13
#define KWD_BEGIN		14
#define KWD_END			15
#define OP_AND			16
#define KWD_PROGRAM		17
#define KWD_PROCEDURE	18
#define KWD_FUNCTION	19
#define SEMI_COLON		20
#define COLON			21
#define OP_ASSIGN		22
#define OP_LESS			23
#define OP_GREATER		24
#define OP_LESS_EQUAL	25
#define OP_GREATER_EQUAL 26
#define OP_NOT_EQUAL	27
#define DOT				28
#define OPEN_SQ_BR		29
#define CLOSE_SQ_BR		30
#define KWD_VAR			31
#define KWD_FOR			32
#define KWD_TO			33
#define KWD_DOWNTO		34
#define KWD_DO			35
#define OPEN_BR			36
#define CLOSE_BR		37
#define KWD_CONST		38
#define KWD_TYPE		39
#define KWD_IF			40
#define KWD_THEN		41
#define KWD_ELSE		42
#define KWD_CASE		43
#define KWD_OF			44
#define KWD_WHILE		45
#define KWD_REPEAT		46
#define KWD_UNTIL		47
#define KWD_WITH		48
#define KWD_PACKED		49
#define KWD_ARRAY		50
#define OP_OR			51
#define KWD_FILE		52
#define KWD_SET			53
#define KWD_RECORD		54
#define OP_IN			55
#define OP_NOT			56
#define OP_XOR			57
#define KWD_FORWARD		58
#define IDENTIFIER		59
#define COMMA			60
#define DOTDOT			61
#define KWD_BREAK		62
#define KWD_USES		63
#define KWD_UNIT		64
#define KWD_INTERFACE	65
#define KWD_IMPLEMENTATION 66
#define KWD_INITIALIZATION 67
#define KWD_FINALIZATION 68
#define KWD_INLINE		69
#define KWD_EXIT		70
#define END_OF_INPUT	255