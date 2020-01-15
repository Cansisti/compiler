#include "declarations/table.h"
#include <spdlog/spdlog.h>

Table::Table(PId _id, size_t _line, Num _from, Num _to) {
	id = _id;
	line = _line;
	from = _from;
	to = _to;
	initiated = true;
}

const int Table::type() const {
	return 1;
}

const std::string Table::type_string() const {
	return "table";
}

bool Table::checkRange(Num n) const {
	return
		n >= from and
		n <= to;
}

bool Table::validate() const {
	if(from > to) {
		spdlog::error("{}: invalid table declaration '{}'", line, id);
		return false;
	}
	return true;
}

size_t Table::getSize() const {
	return to - from + 1;
}

long long Table::getOffset() const {
	return -from + 1;
}