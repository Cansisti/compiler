#pragma once
#include "command.h"
#include "expression.h"
#include <string>

class Assign: public Command {
	friend class Program;
	public:
		Assign(std::string id, Expression* value);
		virtual void translate() override;
		virtual const int type() const override;
		virtual const std::string type_string() const override;
	protected:
		std::string id;
		Expression* value;
};