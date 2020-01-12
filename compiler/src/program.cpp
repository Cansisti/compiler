#include "program.h"
#include "expression.h"
#include <algorithm>
#include <spdlog/spdlog.h>
#include <stdexcept>

#define ProgramVisitorConstructor(name) const Program* program; name(const Program* program): program(program) {}

extern Program* __program;

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

	bool operator()(const ForLoop* forLoop) {
		return
			std::visit(ValueValidateVisitor(program), *forLoop->from) and
			std::visit(ValueValidateVisitor(program), *forLoop->to) and
			forLoop->program->validate();
	}

	bool operator()(const NotACommand* nac) {
		return true;
	}
};

bool Program::validate() const {
	for(auto command: *commands) {
		if(!std::visit(CommandValidateVisitor(this), *command)) {
			spdlog::info("Validation failed for:");
			spdlog::info(*std::visit(AnyCommandVisitor(), *command));
			return false;
		}
	}
	return true;
}

bool Program::checkForPresence(const Identifier* id) const { // todo error8 / index of table can be id also and needs to be checked
	for(auto declaration: declarations) {
		if(declaration->id == std::visit(Anything::AnyVisitor(), *id).name) {
			return true;
		}
	}
	for(auto declaration: __program->declarations) {
		if(declaration->id == std::visit(Anything::AnyVisitor(), *id).name) {
			return true;
		}
	}
	auto& theId = std::visit(Anything::AnyVisitor(), *id);
	spdlog::error("{}: unknown variable '{}'", theId.line, theId.name);
	return false;
}
