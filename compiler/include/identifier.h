#pragma once
#include <spdlog/fmt/ostr.h>
#include <string>

class Identifier {
	public:
		std::string name;

		template<typename OStream>
		friend OStream &operator<<(OStream &os, const Identifier* identifier)
		{
			return os << name;
		}
};