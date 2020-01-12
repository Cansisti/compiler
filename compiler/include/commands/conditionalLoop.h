#pragma once
#include "command.h"
#include "condition.h"

class Program;

class ConditionalLoop: public virtual Command {
	friend class Program;
	public:
		enum class Modifier {
			do_while,
			while_do
		};

		ConditionalLoop(Condition*, ConditionalLoop::Modifier, Program*);
		virtual const std::string describe() const override;
	protected:
		const Condition* condition;
		const Modifier modifier;
		Program* program;
};
