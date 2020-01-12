#pragma once
#include "common/anything.h"

class ConstantTableIdentifier: public virtual Anything {
	public:
		const Num n;
		ConstantTableIdentifier(const std::string name, const size_t line, Num n) :
			Anything("table reference", name, line),
			n(n)
		{}
};
