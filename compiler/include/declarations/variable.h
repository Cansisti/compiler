#pragma once
#include "declaration.h"

class Variable: public Declaration {
	public:
		Variable(std::string id, size_t line);
		virtual const int type() const override;
		virtual const std::string type_string() const override;
};
