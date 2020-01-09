#pragma once
#include <string>
#include <spdlog/fmt/ostr.h>

typedef long long Num;
typedef std::string PId;

class Anything {
	public:
		const std::string type;
		const std::string name;
		const size_t line;

		Anything() = delete;

		template<typename OStream>
		friend OStream &operator<<(OStream &os, const Anything& it)
		{
			return os << it.type << " '" << it.name << "' at " << it.line;
		}

		struct AnyVisitor {
			const Anything& operator()(const Anything& it) const {
				return it;
			}
		};
	protected:
		Anything(const std::string type, const std::string name, const size_t line) :
			type(type),
			name(name),
			line(line)
		{}
};