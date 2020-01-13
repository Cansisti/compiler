#pragma once
#include <vector>
#include <variant>
#include "declaration.h"
#include "anyCommand.h"
#include "intercode/intercode.h"

class Program {
	public:
		std::vector<Declaration*> declarations;
		Commands* commands;

		void progagateParents();
		bool validate() const;
		void translate(Intercode*) const;
		Declaration* findDeclaration(const PId) const;
	protected:
		struct CommandValidateVisitor;
		struct ExpressionValidateVisitor;
		struct AnyExpressionValidateVisitor;
		struct ValueValidateVisitor;
		struct IdentifierValidateVisitor;

		struct SetParentVisitor;

		bool validateCondition(const Condition*) const;
		void markInitiated(const PId) const;
	private:
		Program* parent = nullptr;
};
