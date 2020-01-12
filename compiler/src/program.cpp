#include "program.h"
#include "expression.h"
#include <algorithm>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include "declarations/table.h"
#include "declarations/variable.h"

#define ProgramVisitorConstructor(name) const Program* program; name(const Program* program): program(program) {}

extern Program* __program;

struct Program::IdentifierValidateVisitor {
	ProgramVisitorConstructor(IdentifierValidateVisitor)

	bool operator()(const ConstantTableIdentifier& id) {
		auto declaration = program->findDeclaration(id.name);
		if(declaration == nullptr) {
			spdlog::error("{}: unknown variable '{}'", id.line, id.name);
			return false;
		}
		auto table = dynamic_cast<const Table*>(declaration);
		if(table == nullptr) {
			spdlog::error("{}: variable '{}' is not a table", id.line, id.name);
			return false;
		}
		if(!table->checkRange(id.n)) {
			spdlog::error("{}: index '{}' is out of '{}' range", id.line, id.n, id.name);
			return false;
		}
		return true;
	}

	bool operator()(const LabeledTableIdentifier& id) {
		auto declaration = program->findDeclaration(id.name);
		if(declaration == nullptr) {
			spdlog::error("{}: unknown variable '{}'", id.line, id.name);
			return false;
		}
		auto index = program->findDeclaration(id.p);
		if(index == nullptr) {
			spdlog::error("{}: unknown variable '{}'", id.line, id.p);
			return false;
		}
		if(index->type() != 0) {
			spdlog::error("{}: table '{}' cannot be referenced with other table '{}'", id.line, id.name, id.p);
			return false;
		}
		if(declaration->type() != 1) {
			spdlog::error("{}: variable '{}' is not a table", id.line, id.name);
			return false;
		}
		return true;
	}

	bool operator()(const VariableIdentifier& id) {
		auto declaration = program->findDeclaration(id.name);
		if(declaration == nullptr) {
			spdlog::error("{}: unknown variable '{}'", id.line, id.name);
			return false;
		}
		if(declaration->type() != 0) {
			spdlog::error("{}: wrong usage of table '{}'", id.line, id.name);
			return false;
		}
		return true;
	}
};

struct Program::ValueValidateVisitor {
	ProgramVisitorConstructor(ValueValidateVisitor)

	bool operator()(const Identifier* id) {
		return std::visit(IdentifierValidateVisitor(program), *id);
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
			std::visit(IdentifierValidateVisitor(program), *assign->id) and
			std::visit(ExpressionValidateVisitor(program), *assign->expr);
    }

	bool operator()(const ForLoop* forLoop) {
		return
			std::visit(ValueValidateVisitor(program), *forLoop->from) and
			std::visit(ValueValidateVisitor(program), *forLoop->to) and
			forLoop->program->validate();
	}

	bool operator()(const Read* read) {
		return std::visit(IdentifierValidateVisitor(program), *read->id);
	}

	bool operator()(const Write* write) {
		return std::visit(ValueValidateVisitor(program), *write->value);
	}

	bool operator()(const NotACommand* nac) {
		return true;
	}
};

struct Program::SetParentVisitor {
	ProgramVisitorConstructor(SetParentVisitor)

	template<class T>
	void operator()(const T* t) {
	}

	void operator()(ForLoop* forLoop) {
		forLoop->program->parent = const_cast<Program*>(program);
		forLoop->program->progagateParents();
	}
};

bool Program::validate() const {
	for(auto declaration: declarations) {
		for(auto dd: declarations) {
			if(dd == declaration) continue;
			if(dd->id == declaration->id) {
				spdlog::error("{}: redeclaration of {}", dd->line, dd->id);
				spdlog::info("Validation failed due to redeclaration");
				return false;
			}
		}
		if(!declaration->validate()) {
			spdlog::info("Validation failed for:");
			spdlog::info(declaration);
			return false;
		};
	}
	for(auto command: *commands) {
		if(!std::visit(CommandValidateVisitor(this), *command)) {
			spdlog::info("Validation failed for:");
			spdlog::info(*std::visit(AnyCommandVisitor(), *command));
			return false;
		}
	}
	return true;
}

const Declaration* Program::findDeclaration(const PId id) const {
	for(auto declaration: declarations) {
		if(declaration->id == id) {
			return declaration;
		}
	}
	return parent ? parent->findDeclaration(id) : nullptr;
}

void Program::progagateParents() {
	for(auto command: *commands) {
		std::visit(SetParentVisitor(this), *command);
	}
}