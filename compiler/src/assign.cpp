#include "commands/assign.h"
#include <sstream>

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

void Assign::translate(const Program*, Intercode*) const {
	
};