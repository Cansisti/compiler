#pragma once
#include "common/command.h"
#include "common/value.h"

class Write: public virtual Command {
	friend class Program;
	public:
		Write(Value*, size_t line);
		virtual const std::string describe() const override;
		virtual void translate(const Program*, Intercode*) const override;
	protected:
		const Value* value;
		const size_t line;
};