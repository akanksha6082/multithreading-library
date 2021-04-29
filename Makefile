# Define Locations of all directories

#Directory of all Executable files
DIR_BIN  	:= bin

#Directory of all Header files
DIR_INCLUDE := include

#Directory of all Source files
DIR_SRC  	:= src

#Directory of all Object files
DIR_OBJ  	:= obj

#Directory of all test cases files
DIR_TEST 	:= test

SRC := $(wildcard $(DIR_SRC)/*.c)

OBJ := $(SRC:$(DIR_SRC)/%.c=$(DIR_OBJ)/%.o)

TEST_SRC := $(wildcard $(DIR_TEST)/*.c)

TEST_OBJ := $(TEST_SRC:$(DIR_TEST)/%.c=$(DIR_TEST)/%.o)

EXE := $(addprefix $(DIR_BIN)/, $(notdir $(basename $(TEST_SRC))))

CC 		 := gcc
CPPFLAGS := -I include

TESTCFLAGS := -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -Wno-return-type -Wunused-variable


.PHONY: all clean exe 

all: $(EXE)

$(DIR_OBJ):
	mkdir $@

$(DIR_BIN):
	mkdir $@		

$(DIR_OBJ)/%.o: $(DIR_SRC)/%.c | $(DIR_OBJ)
	$(CC) $(CPPFLAGS) -c $< -o $@

#doubt 
$(DIR_BIN)/%: $(DIR_TEST)/%.o
	mkdir -p $(DIR_BIN)
	$(CC) $^ $(LDLIBS) -o $@

$(DIR_TEST)/%.o: $(DIR_TEST)/%.c | $(DIR_OBJ)
	$(CC) $(CPPFLAGS) $(TESTCFLAGS) -c $< -o $@

clean:
	$(RM) -r $(OBJ) $(TEST_OBJ) $(EXE)
