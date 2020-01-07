#include <spdlog/spdlog.h>
#include "bison.tab.h"

extern int yylineno;
extern char* yytext;

void yyerror(const char* msg) {
	spdlog::error("{}: {} (near '{}')", yylineno, msg, yytext);
}

int main(int argc, char** argv) {
	spdlog::set_pattern("%^[%l]%$ %v");
    spdlog::enable_backtrace(32);
	yyparse();
	return 0;
}