#pragma once
#include "common/command.h"
#include "common/identifier.h"
#include "common/value.h"

class Program;

class ForLoop: public virtual Command {
	friend class Program;
	public:
		enum class Modifier {
			up,
			down
		};

		ForLoop(PId, ForLoop::Modifier, Value*, Value*, Program*);
		virtual const std::string describe() const override;
	protected:
		const PId counter;
		const Modifier modifier;
		const Value* from;
		const Value* to;
		Program* program;
};
