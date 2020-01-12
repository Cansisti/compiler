C_FLAGS		:= -std=c++17 -Wall -g

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

all: prepare front cmp

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

cmp: $(BIN)/bison.tab.c $(BIN)/lex.yy.c $(OBJECTS)
	$(CXX) $(INCLUDE) $(C_FLAGS) $^ $(OTHER_OBJS) -o ./$(BIN)/$(EXECUTABLE) $(LIBRARIES)

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) $(C_FLAGS) $(INCLUDE) -c $< -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CXX) $(C_FLAGS) $(INCLUDE) -c $< -o $@

-include $(DEPENDS)

$(OBJ)/%.d: $(SRC)/%.cpp
	$(CPP) $(C_FLAGS) $(INCLUDE) $< -MM -MT $(@:.d=.o) > $@

$(OBJ)/%.d: $(SRC)/%.c
	$(CPP) $(C_FLAGS) $(INCLUDE) $< -MM -MT $(@:.d=.o) > $@

$(BIN)/lex.yy.c: front
$(BIN)/bison.tab.c: front
$(BIN)/bison.tab.h: front

front:
	bison -d -o $(BIN)/bison.tab.c $(SRC)/lang.y
	flex -o $(BIN)/lex.yy.c $(SRC)/lang.l

run:
	python3 ./test-framework/test_runner.py ./external/tests
	python3 ./test-framework/test_runner.py ./external/programs

test:
	pbbt ./test-framework/all.yaml

test/errors:
	pbbt ./test-framework/errors.yaml
