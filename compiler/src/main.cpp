#include <spdlog/spdlog.h>
#include <cxxopts.hpp>
#include "bison.tab.hpp"
#include "common/program.h"
#include <fstream>

extern int yylineno;
extern char* yytext;
extern FILE* yyin;

void yyerror(const char* msg) {
	spdlog::error("{}: {} (near '{}')", yylineno, msg, yytext);
}

Program* __program;

int main(int argc, char** argv) {
	spdlog::set_pattern("%^[%l]%$ %v");
    spdlog::enable_backtrace(32);

	cxxopts::Options options("cmp", "JFTT 2019 Compiler");
	options.add_options()
		("input", "input code", cxxopts::value<std::string>())
		("output", "output program", cxxopts::value<std::string>())
		("v,verbose", "be verbose")
		("d,debug", "be even more verbose")
		("intercode", "print intercode to give file", cxxopts::value<std::string>());

	options.parse_positional({"input", "output"});
	auto result = options.parse(argc, argv);

	if(result["debug"].as<bool>()) {
		spdlog::warn("Setting log level to debug");
		spdlog::set_level(spdlog::level::debug);
	}
	else if(result["verbose"].as<bool>()) {
		spdlog::warn("Setting log level to verbose (info)");
		spdlog::set_level(spdlog::level::info);
	}
	else {
		spdlog::set_level(spdlog::level::warn);
	}

	if(!result.count("input") or !result.count("output")) {
		spdlog::critical("No input or output provided");
		return 100;
	}

	const std::string file = result["input"].as<std::string>();
	spdlog::debug("Input file: {}", file);
	yyin = fopen(file.c_str(), "r");
	if(!yyin) {
		spdlog::critical("Could not open {}", file);
		return 101;
	}

	__program = new Program();
	auto rc = yyparse();
	if(rc != 0) {
		spdlog::info("Aborting ({})", rc);
		return rc;
	}

	spdlog::info("Declarations: {}", __program->declarations.size());
	for(auto& declaration: __program->declarations) {
		spdlog::debug(declaration);
	}

	spdlog::info("Commands: {}", __program->commands->size());
	for(auto& anyCommand: *__program->commands) {
		spdlog::debug(*std::visit(AnyCommandVisitor(), *anyCommand));
	}

	__program->progagateParents();

	if(!__program->validate()) {
		spdlog::debug("Validation failed");
		return 102;
	}

	Intercode* ic = new Intercode();
	__program->declare(ic);
	__program->translate(ic);

	if(result.count("intercode")) {
		std::ofstream icfile(result["intercode"].as<std::string>());
		ic->save(icfile);
		icfile.close();
	}

	Machinecode* mc = new Machinecode();
	ic->translate(mc);

	mc->legalize();
	
	std::ofstream output(result["output"].as<std::string>());
	if(!output.is_open()) {
		spdlog::error("Cannot open file {}", result["output"].as<std::string>());
		return 103;
	}
	mc->save(output);
	output.close();

	return 0;
}