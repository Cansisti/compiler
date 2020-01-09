#pragma once
#include "anything.h"

class Condition: public virtual Anything {
	public:
		Condition(const std::string name, const size_t line) :
			Anything("condition", name, line)
		{}
};
