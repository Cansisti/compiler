#pragma once
#include "anything.h"

class LabeledTableIdentifier: public virtual Anything {
	public:
		const PId p;
		LabeledTableIdentifier(const std::string name, const size_t line, PId p) :
			Anything("table reference", name, line),
			p(p)
		{}
};
