#include "common/program.h"
#include "common/expression.h"
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
		const auto declaration = program->findDeclaration(id.name);
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
		const auto declaration = program->findDeclaration(id.name);
		if(declaration == nullptr) {
			spdlog::error("{}: unknown variable '{}'", id.line, id.name);
			return false;
		}
		const auto index = program->findDeclaration(id.p);
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
		const auto declaration = program->findDeclaration(id.name);
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
		if(std::visit(IdentifierValidateVisitor(program), *id)) {
			const auto declaration = program->findDeclaration(std::visit(Anything::AnyVisitor(), *id).name);
			if(declaration->initiated) {
				return true;
			}
			auto theId = std::visit(Anything::AnyVisitor(), *id);
			spdlog::warn("{}: use of not initiated variable '{}'", theId.line, theId.name);
		}
		return true;
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
		if(std::visit(IdentifierValidateVisitor(program), *assign->id) and std::visit(ExpressionValidateVisitor(program), *assign->expr)) {
			program->findDeclaration(std::visit(Anything::AnyVisitor(), *assign->id).name)->initiated = true;
			return true;
		}
		return false;
    }

	bool operator()(const ForLoop* forLoop) {
		return
			std::visit(ValueValidateVisitor(program), *forLoop->from) and
			std::visit(ValueValidateVisitor(program), *forLoop->to) and
			forLoop->program->validate() and
			forLoop->validateCounterViolation();
	}

	bool operator()(const Read* read) {
		if(std::visit(IdentifierValidateVisitor(program), *read->id)) {
			program->findDeclaration(std::visit(Anything::AnyVisitor(), *read->id).name)->initiated = true;
			return true;
		}
		return false;
	}

	bool operator()(const Write* write) {
		return std::visit(ValueValidateVisitor(program), *write->value);
	}

	bool operator()(const ConditionalLoop* loop) {
		return
			program->validateCondition(loop->condition) and
			loop->program->validate();
	}

	bool operator()(const IfStatement* ifs) {
		return
			program->validateCondition(ifs->condition) and
			ifs->positive->validate() and
			(ifs->negative ? ifs->negative->validate() : true);
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

	void operator()(ConditionalLoop* loop) {
		loop->program->parent = const_cast<Program*>(program);
		loop->program->progagateParents();
	}

	void operator()(IfStatement* ifs) {
		ifs->positive->parent = const_cast<Program*>(program);
		ifs->positive->progagateParents();
		if(ifs->negative) {
			ifs->negative->parent = const_cast<Program*>(program);
			ifs->negative->progagateParents();
		}
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

Declaration* Program::findDeclaration(const PId id) const {
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

bool Program::validateCondition(const Condition* condition) const {
	return
		std::visit(ValueValidateVisitor(this), *condition->left) and
		std::visit(ValueValidateVisitor(this), *condition->right);
}

void Program::declare(Intercode* code) const {
	for(auto declaration: declarations) {
		switch(declaration->type()) {
			case 0: {
				code->declare(declaration->address, Address::Type::variable, declaration->id);
				break;
			}
			case 1: {
				auto table = dynamic_cast<const Table*>(declaration);
				code->declare(table->address, Address::Type::table, declaration->id, table->getSize() + 1, table->getOffset());
				break;
			}
		}
	}
}

void Program::translate(Intercode* code) const {
	for(auto command: *commands) {
		std::visit(AnyCommandVisitor(), *command)->translate(this, code);
	}
}