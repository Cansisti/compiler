#pragma once
#include "common/command.h"
#include "common/condition.h"

class ConditionalLoop: public virtual Command {
	friend class Program;
	public:
		enum class Modifier {
			do_while,
			while_do
		};

		ConditionalLoop(Condition*, ConditionalLoop::Modifier, Program*);
		virtual const std::string describe() const override;
		virtual void translate(const Program*, Intercode*) const override;
	protected:
		const Condition* condition;
		const Modifier modifier;
		Program* program;
};
