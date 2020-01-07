#pragma once
#include "declaration.h"

class Table: public Declaration {
	public:
		Table(std::string id, size_t line, int from, int to);
		virtual const int type() const override;
		virtual const std::string type_string() const override;
	private:
		int from;
		int to;
};
