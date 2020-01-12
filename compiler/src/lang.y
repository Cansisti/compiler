%code requires{
#include <variant>
#include "any.h"
#define YYSTYPE Any
}

%{
#include "program.h"
extern Program* __program;

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
	__program->commands = std::get<Commands*>($4);
	return 0;
}| _BEGIN commands END {
	__program->commands = std::get<Commands*>($2);
	return 0;
};

declarations: declarations COMMA pidentifier {
	__program->declarations.push_back(new Variable(
		std::get<std::string>($3),
		@3.first_line
	));
}| declarations COMMA pidentifier BR_OPEN num COLON num BR_CLOSE {
	__program->declarations.push_back(new Table(
		std::get<std::string>($3),
		@3.first_line,
		std::get<long long>($5),
		std::get<long long>($7)
	));
}| pidentifier {
	__program->declarations.push_back(new Variable(
		std::get<std::string>($1),
		@1.first_line
	));
}| pidentifier BR_OPEN num COLON num BR_CLOSE {
	__program->declarations.push_back(new Table(
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
	auto positive = new Program();
	positive->commands = std::get<Commands*>($4);
	auto negative = new Program();
	negative->commands = std::get<Commands*>($6);
	$$ = new AnyCommand(new IfStatement(
		std::get<Condition*>($2),
		positive,
		negative
	));
}| IF condition THEN commands ENDIF {
	auto positive = new Program();
	positive->commands = std::get<Commands*>($4);
	$$ = new AnyCommand(new IfStatement(
		std::get<Condition*>($2),
		positive,
		nullptr
	));
}| WHILE condition DO commands ENDWHILE {
	auto program = new Program();
	program->commands = std::get<Commands*>($4);
	$$ = new AnyCommand(new ConditionalLoop(
		std::get<Condition*>($2),
		ConditionalLoop::Modifier::while_do,
		program
	));
}| DO commands WHILE condition ENDDO {
	auto program = new Program();
	program->commands = std::get<Commands*>($2);
	$$ = new AnyCommand(new ConditionalLoop(
		std::get<Condition*>($4),
		ConditionalLoop::Modifier::do_while,
		program
	));
}| FOR pidentifier FROM value TO value DO commands ENDFOR {
	auto program = new Program();
	program->commands = std::get<Commands*>($8);
	program->declarations.push_back(new Variable(
		std::get<PId>($2),
		@2.first_line
	));
	$$ = new AnyCommand(new ForLoop(
		std::get<PId>($2),
		ForLoop::Modifier::up,
		std::get<Value*>($4),
		std::get<Value*>($6),
		program
	));
}| FOR pidentifier FROM value DOWNTO value DO commands ENDFOR {
	auto program = new Program();
	program->commands = std::get<Commands*>($8);
	program->declarations.push_back(new Variable(
		std::get<PId>($2),
		@2.first_line
	));
	$$ = new AnyCommand(new ForLoop(
		std::get<PId>($2),
		ForLoop::Modifier::down,
		std::get<Value*>($4),
		std::get<Value*>($6),
		program
	));
}| READ identifier SEMICOLON {
	$$ = new AnyCommand(new Read(
		std::get<Identifier*>($2)
	));
}| WRITE value SEMICOLON {
	$$ = new AnyCommand(new Write(
		std::get<Value*>($2),
		@2.first_line
	));
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
	$$ = new Condition(
		"EQ",
		@2.first_line,
		std::get<Value*>($1),
		std::get<Value*>($3),
		Condition::Modifier::equal
	);
}| value NEQ value {
	$$ = new Condition(
		"NEQ",
		@2.first_line,
		std::get<Value*>($1),
		std::get<Value*>($3),
		Condition::Modifier::different
	);
}| value LE value {
	$$ = new Condition(
		"LE",
		@2.first_line,
		std::get<Value*>($1),
		std::get<Value*>($3),
		Condition::Modifier::less
	);
}| value GE value {
	$$ = new Condition(
		"GE",
		@2.first_line,
		std::get<Value*>($1),
		std::get<Value*>($3),
		Condition::Modifier::greater
	);
}| value LEQ value {
	$$ = new Condition(
		"LEQ",
		@2.first_line,
		std::get<Value*>($1),
		std::get<Value*>($3),
		Condition::Modifier::less_or_equal
	);
}| value GEQ value {
	$$ = new Condition(
		"GEQ",
		@2.first_line,
		std::get<Value*>($1),
		std::get<Value*>($3),
		Condition::Modifier::greater_or_equal
	);
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
