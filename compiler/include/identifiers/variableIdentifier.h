#pragma once
#include "anything.h"

class VariableIdentifier: public virtual Anything {
	public:
		VariableIdentifier(const std::string name, const size_t line) :
			Anything("variable", name, line)
		{}
};
