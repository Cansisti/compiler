C_FLAGS		:= --std=c++17 -Wall -g

BIN			:= build
SRC			:= compiler/src
OBJ			:= build/obj

INCLUDE		:= -Icompiler/include -Ispdlog/include -I$(BIN) -Icxxopts/include
LIBRARIES	:= -ll
EXECUTABLE	:= cmp

SOURCES		:= $(wildcard ./$(SRC)/*.c ./$(SRC)/*.cpp)
OBJECTS		:= $(patsubst %.cpp, %.o, $(SOURCES))
OBJECTS		:= $(patsubst %.c, %.o, $(OBJECTS))
OBJECTS		:= $(patsubst ./$(SRC)%, ./$(OBJ)%, $(OBJECTS))
DEPENDS		:= $(patsubst %.o, %.d, $(OBJECTS))

all: prepare cmp

.PHONY: clean
clean:
	rm -f $(OBJ)/*.o
	rm -f $(OBJ)/*.d
	rm -f $(BIN)/$(EXECUTABLE)
	rm -f $(BIN)/lex.yy.c
	rm -f $(BIN)/bison.tab.c
	rm -f $(BIN)/bison.tab.h

prepare:
	mkdir -p $(BIN)
	mkdir -p $(OBJ)

front:
	bison -d -o $(BIN)/bison.tab.c $(SRC)/lang.y
	flex -o $(BIN)/lex.yy.c $(SRC)/lang.l

cmp: $(BIN)/bison.tab.c $(BIN)/lex.yy.c $(OBJECTS)
	$(CXX) $(INCLUDE) $(C_FLAGS) $^ $(OTHER_OBJS) -o ./$(BIN)/$(EXECUTABLE) $(LIBRARIES)

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) $(C_FLAGS) $(INCLUDE) -c $< -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CXX) $(C_FLAGS) $(INCLUDE) -c $< -o $@

-include $(DEPENDS)

$(OBJ)/%.d: $(SRC)/%.cpp $(BIN)/bison.tab.h
	$(CPP) $(C_FLAGS) $(INCLUDE) $< -MM -MT $(@:.d=.o) > $@

$(OBJ)/%.d: $(SRC)/%.c $(BIN)/bison.tab.h
	$(CPP) $(C_FLAGS) $(INCLUDE) $< -MM -MT $(@:.d=.o) > $@

$(BIN)/lex.yy.c: front
$(BIN)/bison.tab.c: front
$(BIN)/bison.tab.h: front

run:
	python3 ./tests/_runner.py ./external/tests
	python3 ./tests/_runner.py ./external/programs

test:
	pbbt ./tests/all.yaml

test/errors:
	pbbt ./tests/errors.yaml

test/compiling:
	pbbt ./tests/programs-compiling.yaml

test/tests:
	pbbt ./tests/tests.yaml