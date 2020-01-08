#include "commands/assign.h"

Assign::Assign(std::string _id, Expression* _value) {
	id = id;
	value = _value;
}

void Assign::translate() {

}

const int Assign::type() const {
	return 0;
}

const std::string Assign::type_string() const {
	return "assign";
}
