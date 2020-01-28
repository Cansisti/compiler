# Kompilator na JFTT 2019
	Autor: Tomasz Beneś

## Kompilacja

	Projekt powinien być budowany przy użyciu cmake. Przykład:

	```sh
		# Będąc w głównym katalogu (z plikiem README)
		mkdir build
		cd build
		cmake ..
		make
	```

## Sposób użycia

	Kompilator znajduje się w folderze build. Nazwa programu to `cmp`.

	```sh
		cmp input output
	```

## Wykorzystane wersje oprogramowania

	* g++ (Ubuntu 9.2.1-17ubuntu1~18.04.1) 9.2.1 20191102
	* flex 2.6.4
	* bison (GNU Bison) 3.0.4
	* cmake version 3.10.2

	System:
	* Ubuntu 18.04 LTS
	* WSL (Ubuntu 18.04 LTS)

## Wykorzystane biblioteki open-source

	* spdlog (https://github.com/gabime/spdlog.git)
	* cxxopts (https://github.com/jarro2783/cxxopts.git)
	* stl
