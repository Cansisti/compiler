#include "program.h"
#include <algorithm>
#include <spdlog/spdlog.h>

struct Program::CommandValidateVisitor {
	Program* program;
    CommandValidateVisitor(Program* program):
        program(program)
    {
    }

	bool operator()(Assign* &assign) {
		for(auto declaration: program->declarations) {
			if(declaration->id == assign->id) {
				return true;
			}
		}
		spdlog::error("Unknown variable '{}'", assign->id);
		spdlog::error("Required from {}", assign);
		return false;
    }
};

bool Program::validate() {
	for(auto command: commands) {
		if(!std::visit(CommandValidateVisitor(this), command)){
			return false;
		}
	}
	return true;
}