#include <spdlog/spdlog.h>
#include "bison.tab.h"

void yyerror(const char* msg) {
	spdlog::error(msg);
}

int main(int argc, char** argv) {
	yyparse();
	return 0;
}