#pragma once
#include "anything.h"
#include "value.h"

class Condition: public virtual Anything {
	friend class Program;
	public:
		enum class Modifier {
			equal,
			different,
			less,
			greater,
			less_or_equal,
			greater_or_equal
		};

		Condition(PId, size_t, const Value*, const Value*, Condition::Modifier);
	protected:
		const Value* left;
		const Value* right;
		const Modifier modifier;
};
