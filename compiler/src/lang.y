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

#include "spdlog/spdlog.h"

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
	program->commands = std::get<Commands*>($4);
	return 0;
}| _BEGIN commands END {
	program->commands = std::get<Commands*>($2);
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

commands: commands command {
	auto cmds = std::get<Commands*>($1);
	cmds->push_back(std::get<AnyCommand*>($2));
}| command {
	auto cmds = new Commands;
	cmds->push_back(std::get<AnyCommand*>($1));
	$$ = cmds;
};
command: identifier ASSIGN expression SEMICOLON {
	$$ = new AnyCommand(new Assign(
		std::get<Identifier*>($1),
		std::get<Expression*>($3)
	));
}| IF condition THEN commands ELSE commands ENDIF {
	$$ = new AnyCommand((void*) nullptr);
}| IF condition THEN commands ENDIF {
	$$ = new AnyCommand((void*) nullptr);
}| WHILE condition DO commands ENDWHILE {
	$$ = new AnyCommand((void*) nullptr);
}| DO commands WHILE condition ENDDO {
	$$ = new AnyCommand((void*) nullptr);
}| FOR pidentifier FROM value TO value DO commands ENDFOR {
	$$ = new AnyCommand((void*) nullptr);
}| FOR pidentifier FROM value DOWNTO value DO commands ENDFOR {
	$$ = new AnyCommand((void*) nullptr);
}| READ identifier SEMICOLON {
	$$ = new AnyCommand((void*) nullptr);
}| WRITE value SEMICOLON {
	$$ = new AnyCommand((void*) nullptr);
};

expression: value {
	$$ = new Expression(std::get<Value*>($1));
}| value PLUS value {
	$$ = new Expression(new AnyExpression(new AddExpression(
		"PLUS",
		@2.first_line,
		std::get<Value*>($1),
		std::get<Value*>($3)
	)));
}| value MINUS value {
	$$ = new Expression(new AnyExpression(new SubExpression(
		"MINUS",
		@2.first_line,
		std::get<Value*>($1),
		std::get<Value*>($3)
	)));
}| value TIMES value {
	$$ = new Expression(new AnyExpression(new MulExpression(
		"TIMES",
		@2.first_line,
		std::get<Value*>($1),
		std::get<Value*>($3)
	)));
}| value DIV value {
	$$ = new Expression(new AnyExpression(new DivExpression(
		"DIV",
		@2.first_line,
		std::get<Value*>($1),
		std::get<Value*>($3)
	)));
}| value MOD value {
	$$ = new Expression(new AnyExpression(new ModExpression(
		"MOD",
		@2.first_line,
		std::get<Value*>($1),
		std::get<Value*>($3)
	)));
};

condition: value EQ value {

}| value NEQ value {

}| value LE value {

}| value GE value {

}| value LEQ value {

}| value GEQ value {

};

value: num {
	$$ = new Value(std::get<Num>($1));
}| identifier {
	$$ = new Value(std::get<Identifier*>($1));
};

identifier: pidentifier {
	$$ = new Identifier(VariableIdentifier(
		std::get<PId>($1),
		@1.first_line
	));
}| pidentifier BR_OPEN pidentifier BR_CLOSE {
	$$ = new Identifier(LabeledTableIdentifier(
		std::get<PId>($1),
		@1.first_line,
		std::get<PId>($3)
	));
}| pidentifier BR_OPEN num BR_CLOSE {
	$$ = new Identifier(ConstantTableIdentifier(
		std::get<PId>($1),
		@1.first_line,
		std::get<Num>($3)
	));
};
