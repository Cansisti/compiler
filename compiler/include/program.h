#pragma once
#include <vector>
#include <variant>
#include "declaration.h"
#include "anyCommand.h"

class Program {
	public:
		std::vector<Declaration*> declarations;
		Commands* commands;

		void progagateParents();
		bool validate() const;
	protected:
		struct CommandValidateVisitor;
		struct ExpressionValidateVisitor;
		struct AnyExpressionValidateVisitor;
		struct ValueValidateVisitor;
		struct IdentifierValidateVisitor;

		struct SetParentVisitor;

		const Declaration* findDeclaration(const PId id) const;
	private:
		Program* parent = nullptr;
};
