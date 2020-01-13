#include "commands/assign.h"
#include <sstream>
#include "commands/translate.h"

Assign::Assign(Identifier* id, Expression* expr) :
	id(id),
	expr(expr)
{

}

const std::string Assign::describe() const {
	std::stringstream ss;
	const auto& identifier = std::visit(Anything::AnyVisitor(), *id);
	ss << "assignment to '" << identifier.name << "' at " << identifier.line;
	return ss.str();
}

struct Assign::AnyExpressionTranslateVisitor {
	TranslateVisitorConstructor(AnyExpressionTranslateVisitor)

	Addresses operator()(const ExpressionType* expr) {
		auto left = std::visit(ValueTranslateVisitor(program, code), *expr->left);
		auto right = getValueAddress(program, code, expr->right);
		code->add(expr->icop(), left.a0, left.a1, right);
		return { Intercode::temp_addr };
	}
};

struct Assign::ExpressionTranslateVisitor {
	TranslateVisitorConstructor(ExpressionTranslateVisitor)

	Addresses operator()(const AnyExpression* expr) {
		return std::visit(AnyExpressionTranslateVisitor(program, code), *expr);
	}

	Addresses operator()(const Value* value) {
		return { getValueAddress(program, code, value) };
	}
};

void Assign::translate(const Program* program, Intercode* code) const {
	auto location = std::visit(IdentifierTranslateVisitor(program, code), *id);
	auto what = std::visit(ExpressionTranslateVisitor(program, code), *expr);
	code->add(Intercode::Operation::assign, location.a0, location.a1, what.a0);
}

bool Assign::modifies(PId p) const {
	return std::visit(Anything::AnyVisitor(), *id).name == p;
}
