%code requires{
#include <variant>
#include <string>
#define YYSTYPE std::variant<long long, std::string>
}

%{
#include "program.h"
extern Program* program;

extern int yylex();
extern void yyerror(const char*);
%}

%token DECLARE
%token _BEGIN
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

%token UNEXPECTED_SYMBOL

%%

program: DECLARE declarations _BEGIN commands END {
	return 0;
}| _BEGIN commands END {
	return 0;
};

declarations: declarations COMMA pidentifier {
	auto declaration = new Declaration();
	declaration->id = std::get<std::string>($3);
	program->declarations.push_back(declaration);
}| declarations COMMA pidentifier BR_OPEN num COLON num BR_CLOSE {
	auto declaration = new Declaration();
	declaration->id = std::get<std::string>($3);
	program->declarations.push_back(declaration);
}| pidentifier {
	auto declaration = new Declaration();
	declaration->id = std::get<std::string>($1);
	program->declarations.push_back(declaration);
}| pidentifier BR_OPEN num COLON num BR_CLOSE {
	auto declaration = new Declaration();
	declaration->id = std::get<std::string>($1);
	program->declarations.push_back(declaration);
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
