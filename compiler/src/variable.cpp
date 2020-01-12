#include "declarations/variable.h"

Variable::Variable(PId _id, size_t _line) {
	id = _id;
	line = _line;
}

const int Variable::type() const {
	return 0;
}

const std::string Variable::type_string() const {
	return "variable";
}

bool Variable::validate() const {
	return true;
}
