#pragma once
#include "common/declaration.h"
#include "common/anything.h"

class Table: public Declaration {
	public:
		Table(PId id, size_t line, Num from, Num to);
		virtual const int type() const override;
		virtual const std::string type_string() const override;
		virtual bool validate() const override;
		bool checkRange(Num n) const;
		size_t getSize() const;
		long long getOffset() const;
	private:
		Num from;
		Num to;
};
