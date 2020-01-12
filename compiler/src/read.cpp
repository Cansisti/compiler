#include "commands/read.h"
#include <sstream>

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
