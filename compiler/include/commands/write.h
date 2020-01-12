#pragma once
#include "command.h"
#include "value.h"

class Write: public virtual Command {
	friend class Program;
	public:
		Write(Value*, size_t line);
		virtual const std::string describe() const override;
	protected:
		const Value* value;
		const size_t line;
};