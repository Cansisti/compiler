#pragma once
#include "command.h"
#include "expression.h"
#include <string>

class Assign: public Command {
	public:
		Assign(std::string id, Expression* value);
		virtual void translate() override;
	private:
		std::string id;
		Expression* value;
};