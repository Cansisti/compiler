#include "commands/read.h"
#include <sstream>
#include "commands/translate.h"

Read::Read(Identifier* id) :
	id(id)
{

}

const std::string Read::describe() const {
	std::stringstream ss;
	const auto& identifier = std::visit(Anything::AnyVisitor(), *id);
	ss << "read to '" << identifier.name << "' at " << identifier.line;
	return ss.str();
}

void Read::translate(const Program* program, Intercode* code) const {
	auto ass = std::visit(IdentifierTranslateVisitor(program, code), *id);
	code->add(Intercode::Operation::get, ass.a0, ass.a1);
};

bool Read::modifies(PId p) const {
	return std::visit(Anything::AnyVisitor(), *id).name == p;
}
