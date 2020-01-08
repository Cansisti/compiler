%option yylineno
%option nounput
%{
#include <string>
#include "bison.tab.h"
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;
%}

%%

[ \t\n]+ ;
\[[^]]*\] ;

DECLARE return DECLARE;
BEGIN return _BEGIN;
END return END;
ASSIGN return ASSIGN;
IF return IF;
THEN return THEN;
ELSE return ELSE;
ENDIF return ENDIF;
WHILE return WHILE;
ENDWHILE return ENDWHILE;
DO return DO;
ENDDO return ENDDO;
FOR return FOR;
FROM return FROM;
TO return TO;
DOWNTO return DOWNTO;
ENDFOR return ENDFOR;
READ return READ;
WRITE return WRITE;
PLUS return PLUS;
MINUS return MINUS;
TIMES return TIMES;
DIV return DIV;
MOD return MOD;
EQ return EQ;
NEQ return NEQ;
LE return LE;
GE return GE;
LEQ return LEQ;
GEQ return GEQ;

, return COMMA;
: return COLON;
\( return BR_OPEN;
\) return BR_CLOSE;
; return SEMICOLON;

[_a-z]+ {
	yylval = std::string(yytext);
	return pidentifier;
}
(-?[1-9][0-9]*)|0 {
	yylval = std::stoll(yytext);
	return num;
}

. return UNEXPECTED_SYMBOL;
