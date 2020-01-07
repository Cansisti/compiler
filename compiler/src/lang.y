%code requires{

#include <variant>
#include <string>
#define YYSTYPE std::variant<int, std::string>

}

%{

extern int yylex();
extern void yyerror(const char*);

%}

%token DECLARE
%token BEGIN
%token END
%token ASSIGN
%token IF
%token THEN
%token ELSE
%token ENDIF
%token WHILE
%token ENDWHILE
%token DO
%token ENDDO
%token FOR
%token FROM
%token TO
%token DOWNTO
%token ENDFOR
%token READ
%token WRITE
%token PLUS
%token MINUS
%token TIMES
%token DIV
%token MOD
%token EQ
%token NEQ
%token LE
%token GE
%token LEQ
%token GEQ

%token COMMA
%token COLON
%token BR_OPEN
%token BR_CLOSE
%token SEMICOLON

%token pidentifier
%token num

%%

program: DECLARE declarations BEGIN commands END {
	return 0;
}| BEGIN commands END {
	return 0;
};

declarations: declarations COMMA pidentifier {

}| declarations COMMA pidentifier BR_OPEN num COLON num BR_CLOSE {

}| pidentifier {

}| pidentifier BR_OPEN num COLON num BR_CLOSE {

};

commands: commands command {

}| command {

};

command: identifier ASSIGN expression SEMICOLON {

}| IF condition THEN commands ELSE commands ENDIF {

}| IF condition THEN commands ENDIF {

}| WHILE condition DO commands ENDWHILE {

}| DO commands WHILE condition ENDDO {

}| FOR pidentifier FROM value TO value DO commands ENDFOR {

}| FOR pidentifier FROM value DOWNTO value DO commands ENDFOR {

}| READ identifier SEMICOLON {

}| WRITE value SEMICOLON {

};

expression: value {

}| value PLUS value {

}| value MINUS value {

}| value TIMES value {

}| value DIV value {

}| value MOD value {

};

condition: value EQ value {

}| value NEQ value {

}| value LE value {

}| value GE value {

}| value LEQ value {

}| value GEQ value {

};

value: num {

}| identifier {

};

identifier: pidentifier {

}| pidentifier BR_OPEN pidentifier BR_CLOSE {

}| pidentifier BR_OPEN num BR_CLOSE {

};
