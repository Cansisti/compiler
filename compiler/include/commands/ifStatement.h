#pragma once
#include "command.h"
#include "condition.h"

class Program;

class IfStatement: public virtual Command {
	friend class Program;
	public:
		IfStatement(Condition*, Program*, Program*);
		virtual const std::string describe() const override;
	protected:
		const Condition* condition;
		Program* positive;
		Program* negative;
};
