#pragma once
#include "command.h"
#include "identifier.h"
#include "value.h"

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
