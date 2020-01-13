#pragma once
#include "anything.h"
#include "value.h"
#include "intercode/intercode.h"

class Program;

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

		struct Translation {
			bool swap;
			Intercode::Operation op;
		};

		Condition(PId, size_t, const Value*, const Value*, Condition::Modifier);
		Condition::Translation translate(const Program* program, Intercode* code) const;
	protected:
		const Value* left;
		const Value* right;
		const Modifier modifier;
};
