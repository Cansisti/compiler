#pragma once
#include "declaration.h"
#include "anything.h"

class Table: public Declaration {
	public:
		Table(PId id, size_t line, Num from, Num to);
		virtual const int type() const override;
		virtual const std::string type_string() const override;
		virtual bool validate() const override;
		bool checkRange(Num n) const;
	private:
		Num from;
		Num to;
};
