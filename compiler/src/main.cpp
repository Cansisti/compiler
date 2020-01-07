#include <spdlog/spdlog.h>
#include "bison.tab.h"
#include "program.h"

extern int yylineno;
extern char* yytext;

void yyerror(const char* msg) {
	spdlog::error("{}: {} (near '{}')", yylineno, msg, yytext);
}

Program* program;

int main(int argc, char** argv) {
	spdlog::set_pattern("%^[%l]%$ %v");
	spdlog::set_level(spdlog::level::debug);
    spdlog::enable_backtrace(32);
	program = new Program();
	auto rc = yyparse();
	if(rc != 0) {
		spdlog::info("Aborting ({})", rc);
		return rc;
	}

	spdlog::info("Declarations: {}", program->declarations.size());
	for(auto declaration: program->declarations) {
		spdlog::debug("- {}", declaration);
	}

	spdlog::info("Commands: {}", program->commands.size());
	for(auto command: program->commands) {
		// spdlog::debug("- {}", command);
	}

	return 0;
}