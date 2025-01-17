/*
 * Kod interpretera maszyny rejestrowej do projektu z JFTT2019
 *
 * Autor: Maciek Gębala
 * http://ki.pwr.edu.pl/gebala/
 * 2019-11-12
 * (wersja long long)
*/
#include <iostream>

#include <utility>
#include <vector>
#include <map>

#include <cstdlib> 	// rand()
#include <ctime>
#include <spdlog/spdlog.h>

#include "instructions.hh"

void run_machine( std::vector< std::pair<int,long long> > & program )
{
  spdlog::set_pattern("%^[%l]%$ %v");
  std::map<long long,long long> pam;

  long long lr, adr;

  long long t;

  // std::cerr << "Uruchamianie programu." << std::endl;
  lr = 0;
  srand( time(NULL) );
  pam[0] = rand();
  t = 0;
  while( program[lr].first != HALT )	// HALT
  {
    spdlog::debug("k: {}\tp: {}\td: {}\tm: {}", lr, program[lr].first, program[lr].second, pam[0]);
    switch( program[lr].first )
    {
      case GET:		std::cout << "? "; std::cin >> pam[0]; t+=100; lr++; break;
      case PUT:		std::cout << "> " << pam[0] << std::endl; t+=100; lr++; break;

      case LOAD:	pam[0] = pam[program[lr].second]; t+=10; lr++; break;
      case STORE:	pam[program[lr].second] = pam[0]; t+=10; lr++; break;
      case LOADI:	adr = pam[program[lr].second];
                        if( adr<0 ) { std::cerr << "Błąd: Wywołanie nieistniejącej komórki pamięci " << adr << "." << std::endl; exit(-1); }
                        pam[0] = pam[adr]; t+=20; lr++; break;
      case STOREI:	adr = pam[program[lr].second];
                        if( adr<0 ) { std::cerr << "Błąd: Wywołanie nieistniejącej komórki pamięci " << adr << "." << std::endl; exit(-1); }
                        pam[adr] = pam[0]; t+=20; lr++; break;

      case ADD:		pam[0] += pam[program[lr].second] ; t+=10; lr++; break;
      case SUB:		pam[0] -= pam[program[lr].second] ; t+=10; lr++; break;
      case SHIFT:	if( pam[program[lr].second] >= 0 ) pam[0] <<= pam[program[lr].second]; else pam[0] >>= -pam[program[lr].second]; t+=5; lr++; break;

      case INC:		pam[0]++ ; t+=1; lr++; break;
      case DEC:		pam[0]--; t+=1; lr++; break;

      case JUMP: 	lr = program[lr].second; t+=1; break;
      case JPOS:	if( pam[0]>0 ) lr = program[lr].second; else lr++; t+=1; break;
      case JZERO:	if( pam[0]==0 ) lr = program[lr].second; else lr++; t+=1; break;
      case JNEG:	if( pam[0]<0 ) lr = program[lr].second; else lr++; t+=1; break;
      default: break;
    }
    if( lr<0 || lr>=(long long)program.size() )
    {
      std::cerr << "Błąd: Wywołanie nieistniejącej instrukcji nr " << lr << "." << std::endl;
      exit(-1);
    }
  }
  // std::cerr << "Skończono program (koszt: " << t << ")." << std::endl;
  spdlog::debug("Koszt: {}", t);
}
