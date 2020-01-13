#pragma once
#include "common/command.h"
#include "common/condition.h"

class IfStatement: public virtual Command {
	friend class Program;
	public:
		IfStatement(Condition*, Program*, Program*);
		virtual const std::string describe() const override;
		virtual void translate(const Program*, Intercode*) const override;
	protected:
		const Condition* condition;
		Program* positive;
		Program* negative;
};
