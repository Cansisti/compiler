#pragma once
#include <string>
#include <spdlog/fmt/ostr.h>

class Declaration {
	public:
		std::string id;
		size_t line;

		virtual const int type() const = 0;

		virtual const std::string type_string() const = 0;

		template<typename OStream>
		friend OStream &operator<<(OStream &os, const Declaration* declaration)
		{
			return os << declaration->type_string() << " '" << declaration->id << "' at " << declaration->line;
		}
};
