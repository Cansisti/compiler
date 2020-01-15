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

		/**
		 * If @positive then @op is a jump that will be executed for true condition.
		 * 
		 * In other words, @positive answers the question: "Is @op a jump to a positive condition?".
		 **/
		struct Translation {
			bool positive;
			Intercode::Operation op;
		};

		Condition(PId, size_t, const Value*, const Value*, Condition::Modifier);
		Condition::Translation translate(const Program* program, Intercode* code) const;
	protected:
		const Value* left;
		const Value* right;
		const Modifier modifier;
};
