#include "declarations/table.h"

Table::Table(std::string _id, size_t _line, int _from, int _to) {
	id = _id;
	line = _line;
	from = _from;
	to = _to;
}

const int Table::type() const {
	return 1;
}

const std::string Table::type_string() const {
	return "table";
}
