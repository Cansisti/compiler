%code requires{
#include <variant>
#include "any.h"
#define YYSTYPE Any
}

%{
#include "program.h"
extern Program* program;

#include "declarations/variable.h"
#include "declarations/table.h"

#include "commands/assign.h"

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
	program->declarations.push_back(new Variable(
		std::get<std::string>($3),
		@3.first_line
	));
}| declarations COMMA pidentifier BR_OPEN num COLON num BR_CLOSE {
	program->declarations.push_back(new Table(
		std::get<std::string>($3),
		@3.first_line,
		std::get<long long>($5),
		std::get<long long>($7)
	));
}| pidentifier {
	program->declarations.push_back(new Variable(
		std::get<std::string>($1),
		@1.first_line
	));
}| pidentifier BR_OPEN num COLON num BR_CLOSE {
	program->declarations.push_back(new Table(
		std::get<std::string>($1),
		@1.first_line,
		std::get<long long>($3),
		std::get<long long>($5)
	));
};

commands: commands command | command;
command: identifier ASSIGN expression SEMICOLON {
	program->commands.push_back(new Assign(
		std::get<std::string>($1), // todo: identifier is variable or table
		new Expression()
	));
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
	// todo
}| pidentifier BR_OPEN pidentifier BR_CLOSE {
	// todo
}| pidentifier BR_OPEN num BR_CLOSE {
	// todo
};
