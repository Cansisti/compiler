#pragma once
#include <spdlog/fmt/ostr.h>

class Command {
	public:
		virtual void translate() = 0;
		size_t line;

		virtual const int type() const = 0;

		virtual const std::string type_string() const = 0;

		template<typename OStream>
		friend OStream &operator<<(OStream &os, const Command* command)
		{
			return os << command->type_string() << " at " << command->line;
		}
};
