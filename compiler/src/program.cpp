#include "program.h"
#include "expression.h"
#include <algorithm>
#include <spdlog/spdlog.h>
#include <stdexcept>

#define ProgramVisitorConstructor(name) Program* program; name(Program* program): program(program) {}

struct Program::ValueValidateVisitor {
	ProgramVisitorConstructor(ValueValidateVisitor)

	bool operator()(const Identifier* id) {
		return program->checkForPresence(id);
	}

	bool operator()(const Num num) {
		return true;
	}
};

struct Program::AnyExpressionValidateVisitor {
	ProgramVisitorConstructor(AnyExpressionValidateVisitor)

	bool operator()(const ExpressionType* expressionType) {
		return
			std::visit(ValueValidateVisitor(program), *expressionType->left) and
			std::visit(ValueValidateVisitor(program), *expressionType->right);
	}
};

struct Program::ExpressionValidateVisitor {
	ProgramVisitorConstructor(ExpressionValidateVisitor)

	bool operator()(const AnyExpression* anyExpression) {
		return std::visit(AnyExpressionValidateVisitor(program), *anyExpression);
	}

	bool operator()(const Value* value) {
		return std::visit(ValueValidateVisitor(program), *value);
	}
};

struct Program::CommandValidateVisitor {
	ProgramVisitorConstructor(CommandValidateVisitor)

	bool operator()(const Assign* assign) {
		return
			program->checkForPresence(assign->id) and
			std::visit(ExpressionValidateVisitor(program), *assign->expr);
    }

	template<class T>
	bool operator()(const T t) {
		// todo
		// assert(false);
		return true;
	}
};

bool Program::validate() {
	for(auto command: *commands) {
		if(!std::visit(CommandValidateVisitor(this), *command)){
			return false;
		}
	}
	return true;
}

bool Program::checkForPresence(const Identifier* id) {
	for(auto declaration: declarations) {
		if(declaration->id == std::visit(Anything::AnyVisitor(), *id).name) {
			return true;
		}
	}
	auto& theId = std::visit(Anything::AnyVisitor(), *id);
	spdlog::error("{}: unknown variable '{}'", theId.line, theId.name);
	return false;
}