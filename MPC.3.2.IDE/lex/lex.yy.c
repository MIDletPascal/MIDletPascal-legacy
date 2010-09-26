#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"
#include "../util/strings.h"
#include "../util/error.h"

extern int mathType;
extern int goverify;
char charbuf[10240];
char textbuf[10240];
char strconst[10240];
int charcount;
FILE *yyin; //, *yyout;  //source file
char *yytext;  //current token string
string *string_constant;
char char_constant;
char boolean_constant;
float real_constant;
long int integer_constant;
int warning_101_count;
int error_count;
int linenum;
int fileSize;
char c;

void yyrestart(char *fname) {  //FILE *new_file){  //open source file
	yyin = fopen(fname, "r");
	if (yyin == NULL) die(5);
	fseek(yyin, 0, SEEK_END);
	fileSize = ftell(yyin);
	fseek(yyin, 0, SEEK_SET);
	charcount=0;
    string_constant=&strconst;
    yytext=&textbuf;
	linenum=1;
};

char nextchar(){
	int d;
    if (charcount>0){
		charcount--;
		return charbuf[charcount];
	}
	d=fgetc(yyin);
	if (d==10) linenum++;
	if (d==EOF) return '\255';
	return d;
}

void backchar(int c){ //put the char back to stream
	charbuf[charcount]=c;
	charcount++;
};


int yylex(){  //get token
	int i,ncount,r;
	char *p;
	i = ftell(yyin);
	compile_progress(i * 100 / fileSize);
	do {
		r=0;
//lab:
		//if (linenum==1059) {
		//	linenum=1059;
		//}
		p=yytext;
	    *p='\0';
		c=nextchar();
		while ((c<=' ')&&(c!='\255')) c=nextchar();
		if (((c>='A') && (c<='Z')) || ((c>='a') && (c<='z')) || (c=='_')) {
			while (((c>='A') && (c<='Z')) || ((c>='a') && (c<='z')) || ((c>='0') && (c<='9')) || (c=='_')) {
				*p=c;
				p=p+1;
				c=nextchar();
			}
		    backchar(c);
			*p='\0';
			lowercase(yytext);
			if (strcmp(yytext, "begin") == 0) return KWD_BEGIN;
			if (strcmp(yytext, "end") == 0) return KWD_END;
			if (strcmp(yytext, "and") == 0) return OP_AND;
			if (strcmp(yytext, "program") == 0) return KWD_PROGRAM;
			if (strcmp(yytext, "procedure") == 0) return KWD_PROCEDURE;
			if (strcmp(yytext, "function") == 0) return KWD_FUNCTION;
			if (strcmp(yytext, "var") == 0) return KWD_VAR;
			if (strcmp(yytext, "for") == 0) return KWD_FOR;
			if (strcmp(yytext, "to") == 0) return KWD_TO;
			if (strcmp(yytext, "downto") == 0) return KWD_DOWNTO;
			if (strcmp(yytext, "do") == 0) return KWD_DO;
			if (strcmp(yytext, "const") == 0) return KWD_CONST;
			if (strcmp(yytext, "type") == 0) return KWD_TYPE;
			if (strcmp(yytext, "if") == 0) return KWD_IF;
			if (strcmp(yytext, "then") == 0) return KWD_THEN;
			if (strcmp(yytext, "else") == 0) return KWD_ELSE;
			if (strcmp(yytext, "case") == 0) return KWD_CASE;
			if (strcmp(yytext, "of") == 0) return KWD_OF;
			if (strcmp(yytext, "while") == 0) return KWD_WHILE;
			if (strcmp(yytext, "repeat") == 0) return KWD_REPEAT;
			if (strcmp(yytext, "until") == 0) return KWD_UNTIL;
			if (strcmp(yytext, "forever") == 0) return KWD_FOREVER;
			if (strcmp(yytext, "with") == 0) return KWD_WITH;
			if (strcmp(yytext, "packed") == 0) return KWD_PACKED;
			if (strcmp(yytext, "array") == 0) return KWD_ARRAY;
			if (strcmp(yytext, "or") == 0) return OP_OR;
			if (strcmp(yytext, "file") == 0) return KWD_FILE;
			if (strcmp(yytext, "set") == 0) return KWD_SET;
			if (strcmp(yytext, "record") == 0) return KWD_RECORD;
			if (strcmp(yytext, "in") == 0) return OP_IN;
			if (strcmp(yytext, "not") == 0) return OP_NOT;
			if (strcmp(yytext, "xor") == 0) return OP_XOR;
			if (strcmp(yytext, "forward") == 0) return KWD_FORWARD;
			if (strcmp(yytext, "break") == 0) return KWD_BREAK;
			if (strcmp(yytext, "uses") == 0) return KWD_USES;
			if (strcmp(yytext, "unit") == 0) return KWD_UNIT;
			if (strcmp(yytext, "interface") == 0) return KWD_INTERFACE;
			if (strcmp(yytext, "implementation") == 0) return KWD_IMPLEMENTATION;
			if (strcmp(yytext, "initialization") == 0) return KWD_INITIALIZATION;
			if (strcmp(yytext, "finalization") == 0) return KWD_FINALIZATION;
			if (strcmp(yytext, "mod") == 0) return OP_MOD;
			if (strcmp(yytext, "div") == 0) return OP_DIV;
			if (strcmp(yytext, "shr") == 0) return OP_SHR; //***
			if (strcmp(yytext, "shl") == 0) return OP_SHL; //***
			if (strcmp(yytext, "inline") == 0) return KWD_INLINE; //***?
			if (strcmp(yytext, "exit") == 0) return KWD_EXIT; //***?
			//////
			if (strcmp(yytext, "true") == 0) {boolean_constant=1; return CST_BOOLEAN;}
			if (strcmp(yytext, "false") == 0) {boolean_constant=0; return CST_BOOLEAN;}
			//////
			return IDENTIFIER;
		} else if ((c>='0') && (c<='9')) {
			integer_constant=0;
			while ((c>='0') && (c<='9')) {
				integer_constant=integer_constant*10+c-48;
				c=nextchar();
			}
			if (c!='.') {backchar(c); return CST_INTEGER;}
			c=nextchar();
			if (c=='.') {backchar(c);backchar(c);return CST_INTEGER;}
			real_constant=(float)integer_constant;
			ncount=1;
			integer_constant=0;
			while ((c>='0') && (c<='9')) {
				integer_constant=integer_constant*10+c-48;
				ncount++;
				c=nextchar();
			}
			//real_constant=real_constant+(float)integer_constant/(10*ncount);
			// j-a-s-d: little but important bugfix, it was corrupting the decimal part
			real_constant+=(float)integer_constant/pow(10,--ncount);
			/*
			i=1;
			j=0;
			ncount=1;
			real_constant=(float)integer_constant;
			while (((c>='0') && (c<='9'))) {
				j=j*10+c-48;
				i++;
				c=nextchar();
			}
			real_constant=real_constant+(j/(10*i));
			*/
			backchar(c);
			return CST_REAL;
		} else if (c=='$') {
			int ok;
			integer_constant=0;
			do {
				c=nextchar();
				ok=0;
				if ((c>='0') && (c<='9')) {
					integer_constant = integer_constant*16+c-48;
					ok=1;
				}
				if ((c>='A') && (c<='Z')) {
					integer_constant = integer_constant*16+10+c-65;
					ok=1;
				}
				if ((c>='a') && (c<='z')) {
					integer_constant = integer_constant*16+10+c-97;
					ok=1;
				}
			} while (ok==1);
			backchar(c);
			return CST_INTEGER;
		} else if (c=='/') {
			c=nextchar();
			if (c=='*') {   // j-a-s-d: C-style /* */ multiline comments support
                char ch=c;
                while (!((c=='/')&&(ch=='*'))) {ch=c; c=nextchar();}
                r=1;
	        } else
			if (c=='/') {
				while (c!=10) c=nextchar();
				//goto lab;
				r=1;
			} else {
				backchar(c);
				return OP_SLASH;
			}
		} else if ((c=='\'')||(c=='#'))  {
			i=0;
			do {
				if (c=='#') {
					c=nextchar();
					integer_constant=0;
					if (c=='$') {
						int ok;
						do {
							c=nextchar();
							ok=0;
							if ((c>='0') && (c<='9')) {
								integer_constant = integer_constant*16+c-48;
								ok=1;
							}
							if ((c>='A') && (c<='Z')) {
								integer_constant = integer_constant*16+10+c-65;
								ok=1;
							}
							if ((c>='a') && (c<='z')) {
								integer_constant = integer_constant*16+10+c-97;
								ok=1;
							}
						} while (ok==1);
					} else {
						while (((c>='0') && (c<='9'))) {
							integer_constant=integer_constant*10+c-48;
							c=nextchar();
						}
					}
					*p=(char)integer_constant;
					p=p+1;
					i=i+1;
				} else {
					c=nextchar();
					while (c!='\'') {
						if (c==10) {
							int current_token;
							add_error_message(101, "", "");
							do {
								current_token = yylex();
							} while ((current_token != END_OF_INPUT) && (current_token != SEMI_COLON));
							break;
						}
rep:					*p=c;
						p=p+1;
						i++;
						c=nextchar();
					}
					c=nextchar();
					if (c=='\'') goto rep;
				}
			} while ((c=='\'')||(c=='#'));
			*p='\0';
			backchar(c);
			if (i==1) {p=p-1; char_constant=*p; return CST_CHAR;}
			string_constant=string_from_cstr(yytext);
			return CST_STRING;
		} else if (c=='+') {
		    return OP_PLUS;
		} else if (c=='-') {
			return OP_MINUS;
		} else if (c=='*') {
	        return OP_MULT;
		} else if (c=='=') {
	        return OP_EQUAL;
		} else if (c==';') {
		    return SEMI_COLON;
		} else if (c==':') {
		    c=nextchar();
	        if (c=='=') return OP_ASSIGN;
		    backchar(c);
			return COLON;
		} else if (c=='<') {
		    c=nextchar();
			if (c=='=') return OP_LESS_EQUAL;
	        if (c=='>') return OP_NOT_EQUAL;
		    backchar(c);
			return OP_LESS;
		} else if (c=='>') {
		    c=nextchar();
			if (c=='=') return OP_GREATER_EQUAL;
	        backchar(c);
		    return OP_GREATER;
		} else if (c=='.') {
		    c=nextchar();
			if (c=='.') return DOTDOT;
	        backchar(c);
		    return DOT;
		} else if (c=='[') {
		    return OPEN_SQ_BR;
		} else if (c==']') {
		    return CLOSE_SQ_BR;
		} else if (c=='(') {
			char ch;
		    c=nextchar();
			if (c!='*') {
				backchar(c);
				return OPEN_BR;
	        }
            ch=c;
	        while (!((c==')')&&(ch=='*'))) {ch=c; c=nextchar();}
			//goto lab;
			r=1;
		} else if (c==')') {
			return CLOSE_BR;
		} else if (c==',') {
	        return COMMA;
		} else if (c=='{') {  //skip comment
			c=nextchar();
			// j-a-s-d: removing $R and $V to avoid ambiguities
			/*
			if (c=='$') {
				c=nextchar();
				if (c=='R') { //real type switch
					c=nextchar();
					if (c=='-') mathType=1;
					if (c=='+') mathType=2;
				}
				if (c=='V') { //preverify_bytecode switch
					c=nextchar();
					if (c=='-') goverify=0;
					if (c=='+') goverify=1;
				}
			}
			*/
			while (c!='}') c=nextchar();
			//goto lab;
			r=1;
		}
	} while (r==1);
	return END_OF_INPUT;
}

//#define	CST_INTEGER		0
//#define CST_REAL		1
//#define CST_BOOLEAN		2
//#define CST_CHAR		3
//#define CST_STRING		4
//
//#define OP_MOD			5
//#define OP_DIV			6
//#define OP_PLUS			7
//#define OP_MINUS		8
//#define OP_MULT			9
//#define OP_SLASH		10
//#define OP_EQUAL		11
//#define SEMI_COLON		18
//#define COLON			19
//#define OP_ASSIGN		20
//#define OP_LESS			21
//#define OP_GREATER		22
//#define OP_LESS_EQUAL	23
//#define OP_GREATER_EQUAL 24
//#define OP_NOT_EQUAL	25
//#define DOT				26
//#define OPEN_SQ_BR		27
//#define CLOSE_SQ_BR		28
//#define OPEN_BR			34
//#define CLOSE_BR		35
//#define IDENTIFIER		57
//#define COMMA			58
//#define DOTDOT			59
//#define END_OF_INPUT	100
/*
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

/*
"mod" = (OP_MOD);
"div" = (OP_DIV);
"+" = (OP_PLUS);
"-" = (OP_MINUS);
"*" = (OP_MULT);
"/" = (OP_SLASH);
"=" = (OP_EQUAL);
"begin" = (KWD_BEGIN);
"end" = (KWD_END);
"and" = (OP_AND);
"program" = (KWD_PROGRAM);
"procedure" = (KWD_PROCEDURE);
"function" = (KWD_FUNCTION);
";" = (SEMI_COLON);
":" = (COLON);
":=" = (OP_ASSIGN);
"<" = (OP_LESS);
">" = (OP_GREATER);
"<=" = (OP_LESS_EQUAL);
">=" = (OP_GREATER_EQUAL);
"<>" = (OP_NOT_EQUAL);
"." = (DOT);
".." = (DOTDOT);
"[" = (OPEN_SQ_BR);
"]" = (CLOSE_SQ_BR);
"var" = (KWD_VAR);
"for" = (KWD_FOR);
"to" = (KWD_TO);
"downto" = (KWD_DOWNTO);
"do" = (KWD_DO);
"(" = (OPEN_BR);
")" = (CLOSE_BR);
"const" = (KWD_CONST);
"type" = (KWD_TYPE);
"if" = (KWD_IF);
"then" = (KWD_THEN);
"else" = (KWD_ELSE);
"case" = (KWD_CASE);
"of" = (KWD_OF);
"while" = (KWD_WHILE);
"repeat" = (KWD_REPEAT);
"until" = (KWD_UNTIL);
"with" = (KWD_WITH);
"packed" = (KWD_PACKED);
"array" = (KWD_ARRAY);
"or" = (OP_OR);
"file" = (KWD_FILE);
"set" = (KWD_SET);
"record" = (KWD_RECORD);
"in" = (OP_IN);
"not" = (OP_NOT);
"xor" = (OP_XOR);
"forward" = (KWD_FORWARD);
"break" = (KWD_BREAK);
"," = (COMMA);
"uses" = (KWD_USES);
"unit" = (KWD_UNIT);
"interface" = (KWD_INTERFACE);
"implementation" = (KWD_IMPLEMENTATION);
"initialization" = (KWD_INITIALIZATION);
"finalization" = (KWD_FINALIZATION);
*/


/*
		c=nextchar();
		i=0;
rep:
		while (c!='\'') {
			*p=c;
			p=p+1;
			i++;
			c=nextchar();
		}
		c=nextchar();
		if (c=='\'') {*p=c; p=p+1; i=i+1; c=nextchar(); goto rep;}
		if (c=='#') {
			j=0;
			c=nextchar();
			while (((c>='0') && (c<='9'))) {
				j=j*10+c-48;
				c=nextchar();
			}
			*p=(char)j;
			p=p+1;
			i=i+1;
			goto rep;
		}
*/

/*
int getnum() {  //get number
	int sum;
	sum=0;
	ncount=1;
	if (c=='$') {
		c=nextchar();
		do {
			if ((c >= 'A') && (c <= 'Z')) {sum = sum*16+10+c-65; ncount=ncount+1;}
			else if ((c >= 'a') && (c <= 'z')) {sum = sum*16+10+c-97; ncount=ncount+1;}
			else if (('0'>=c) && (c<='9')) {sum = sum*16+c-48; ncount=ncount+1;}
			else break;
			c=nextchar();
		} while (1==1);
	} else {
		while (('0'>=c) && (c<='9')) {
			sum = sum*10+c-48;
			ncount=ncount+1;
			c=nextchar();
		}
	}
	//if (numcount==1) backchar(255); //bad number
	return sum;
}
*/
