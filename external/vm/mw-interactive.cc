/*
 * Kod interpretera maszyny rejestrowej do projektu z JFTT2019
 *
 * Autor: Maciek Gębala
 * http://ki.pwr.edu.pl/gebala/
 * 2019-11-12
 * (wersja long long)
*/
#include <iostream>

#include <map>
#include <string>
#include <utility>
#include <vector>

#include <spdlog/spdlog.h>
#include <cstdlib>  // rand()
#include <ctime>

#include "instructions.hh"

void run_machine(std::vector<std::pair<int, long long> >& program) {
	spdlog::set_pattern("%^[%l]%$ %v");
	spdlog::set_level(spdlog::level::debug);
	std::map<long long, long long> pam;

	long long lr, adr;

	long long t;

	lr = 0;
	srand(time(NULL));
	pam[0] = rand();
	t = 0;

	spdlog::info("Starting machine in interactive mode.");

	long long stop_at = 0;
	static const std::string cmds[] = {"GET  ", "PUT  ", "LOAD ", "STORE", "LOADI", "STOREI", "ADD  ", "SUB  ", "SHIFT", "INC  ", "DEC  ", "JUMP ", "JPOS ", "JZERO", "JNEG ", "HALT "};

	spdlog::info("? - show help");

	bool watch = false;
	std::vector<long long> watched;

	bool run = false;
	std::map<long long, bool> breakpoints;

	while (program[lr].first != HALT) {
		std::string cc;
		std::cout << ".. ";
		std::cin >> cc;
		if (cc == "?") {
			spdlog::info("s - single step");
			spdlog::info("ss [n] - n steps");
			spdlog::info("m [i] - print memory[i]");
			spdlog::info("w [n] ... - watch following n memory fields");
			spdlog::info("w+ [i] [n] - watch n memory fields starting from i");
			spdlog::info("k - print next instruction");
			spdlog::info("r - run until the breakpoint");
			spdlog::info("q - force quit");
			spdlog::info("b [i] - set / unset breakpoint");
			spdlog::info("b? - show all breakpoints");
			spdlog::info("w? - show all watched fields");
		}
		if (cc == "s") stop_at++;
		if (cc == "ss") {
			long long x;
			std::cin >> x;
			stop_at += x;
		}
		if (cc == "m") {
			long long x;
			std::cin >> x;
			spdlog::debug("[{}]: {}", x, pam[x]);
		}
		if (cc == "k") {
			spdlog::debug("k: {}", lr);
		}
		if (cc == "r") {
			spdlog::warn("Running until next breakpoint");
			run = true;
		}
		if (cc == "q") {
			spdlog::warn("User requested termination");
			exit(0);
		}
		if (cc == "w") {
			long long x;
			std::cin >> x;
			if (x == 0)
				watch = false;
			else {
				watched.clear();
				for (long long i = 0; i < x; i++) {
					long long y;
					std::cin >> y;
					watched.push_back(y);
				}
				watch = true;
			}
		}
		if(cc == "b") {
			long long x;
			std::cin >> x;
			if(breakpoints[x]) {
				spdlog::debug("Unsetting breakpoint :{}", x);
				breakpoints[x] = false;
			}
			else {
				spdlog::debug("Setting breakpoint :{}", x);
				breakpoints[x] = true;
			}
		}
		if(cc == "b?") {
			std::cout << "[break?] [ ";
			for(auto bp: breakpoints) {
				if(bp.second) std::cout << bp.first << " ";
			}
			std::cout << "]" << std::endl;
		}
		if(cc == "w?") {
			std::cout << "[watch?] [ ";
			for(auto wv: watched) {
				std::cout << wv << " ";
			}
			std::cout << "]" << std::endl;
		}
		if(cc == "w+") {
			long long x;
			std::cin >> x;
			long long y;
			std::cin >> y;
			if (y == 0)
				watch = false;
			else {
				watched.clear();
				for (long long i = x; i < x+y; i++) {
					watched.push_back(i);
				}
				watch = true;
			}
		}
		while ((lr < stop_at or run) and (program[lr].first != HALT)) {
			spdlog::debug("{}:\t{}\t{}", lr, cmds[program[lr].first], program[lr].second);
			switch (program[lr].first) {
				case GET:
					std::cout << "? ";
					std::cin >> pam[0];
					t += 100;
					lr++;
					break;
				case PUT:
					std::cout << "> " << pam[0] << std::endl;
					t += 100;
					lr++;
					break;

				case LOAD:
					pam[0] = pam[program[lr].second];
					t += 10;
					lr++;
					break;
				case STORE:
					pam[program[lr].second] = pam[0];
					t += 10;
					lr++;
					break;
				case LOADI:
					adr = pam[program[lr].second];
					if (adr < 0) {
						std::cerr << "Błąd: Wywołanie nieistniejącej komórki pamięci " << adr << "." << std::endl;
						exit(-1);
					}
					pam[0] = pam[adr];
					t += 20;
					lr++;
					break;
				case STOREI:
					adr = pam[program[lr].second];
					if (adr < 0) {
						std::cerr << "Błąd: Wywołanie nieistniejącej komórki pamięci " << adr << "." << std::endl;
						exit(-1);
					}
					pam[adr] = pam[0];
					t += 20;
					lr++;
					break;

				case ADD:
					pam[0] += pam[program[lr].second];
					t += 10;
					lr++;
					break;
				case SUB:
					pam[0] -= pam[program[lr].second];
					t += 10;
					lr++;
					break;
				case SHIFT:
					if (pam[program[lr].second] >= 0)
						pam[0] <<= pam[program[lr].second];
					else
						pam[0] >>= -pam[program[lr].second];
					t += 5;
					lr++;
					break;

				case INC:
					pam[0]++;
					t += 1;
					lr++;
					break;
				case DEC:
					pam[0]--;
					t += 1;
					lr++;
					break;

				case JUMP:
					lr = program[lr].second;
					spdlog::warn("Jumping to :{}", lr);
					stop_at = lr;
					t += 1;
					break;
				case JPOS:
					if (pam[0] > 0) {
						lr = program[lr].second;
						spdlog::warn("Jumping to :{}", lr);
						stop_at = lr;
					} else
						lr++;
					t += 1;
					break;
				case JZERO:
					if (pam[0] == 0) {
						lr = program[lr].second;
						spdlog::warn("Jumping to :{}", lr);
						stop_at = lr;
					} else
						lr++;
					t += 1;
					break;
				case JNEG:
					if (pam[0] < 0) {
						lr = program[lr].second;
						spdlog::warn("Jumping to :{}", lr);
						stop_at = lr;
					} else
						lr++;
					t += 1;
					break;
				case HALT:
					lr++;
					break;
				default:
					break;
			}
			if (lr < 0 || lr >= (long long)program.size()) {
				std::cerr << "Błąd: Wywołanie nieistniejącej instrukcji nr " << lr << "." << std::endl;
				exit(-1);
			}
			if(run) {
				stop_at = lr;
			}
			if(breakpoints[lr]) {
				spdlog::info("Stopping at breakpoint :{}", lr);
				run = false;
				break;
			}
		}
		if (watch) {
			std::cout << "[watch] [ ";
			for (auto x : watched) {
				std::cout << pam[x] << " ";
			}
			std::cout << "]" << std::endl;
		}
	}

	spdlog::debug("Cost: {}", t);
}
