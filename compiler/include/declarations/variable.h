#pragma once
#include "common/declaration.h"
#include "common/anything.h"

class Variable: public Declaration {
	public:
		Variable(PId id, size_t line);
		virtual const int type() const override;
		virtual const std::string type_string() const override;
		virtual bool validate() const override;
};
