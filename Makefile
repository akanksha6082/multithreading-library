DIR_BIN  	:= bin

DIR_INC     := include

DIR_SRC  	:= src

DIR_OBJ  	:= obj

#Directory of all test cases files
DIR_TEST 	:= test

SRC := $(wildcard $(DIR_SRC)/*.c)

OBJ := $(SRC:$(DIR_SRC)/%.c=$(DIR_OBJ)/%.o)

TEST_SRC := $(wildcard $(DIR_TEST)/*.c)
# $(info TEST_SRC is $(TEST_SRC))

TEST_OBJ := $(TEST_SRC:$(DIR_TEST)/%.c=$(DIR_TEST)/%.o)
# $(info TEST_OBJ is $(TEST_OBJ))

EXE := $(addprefix $(DIR_BIN)/, $(notdir $(basename $(TEST_SRC))))
# $(info EXE is $(EXE))

CC 		 := gcc
CPPFLAGS := -I $(DIR_INC)
CFLAGS   := -Wall -Wno-return-type

.PHONY: all clean run

all: $(OBJ) $(EXE)

$(DIR_OBJ):
	mkdir -p $@

$(DIR_BIN):
	mkdir -p $@
    
$(DIR_BIN)/%: $(DIR_TEST)/%.o $(OBJ) | $(DIR_BIN)
	$(CC) $(CPPFLAGS) $^ -o $@

$(DIR_TEST)/%.o: $(DIR_TEST)/%.c | $(DIR_OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(DIR_OBJ)/%.o: $(DIR_SRC)/%.c | $(DIR_OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@


run :
	@$(MAKE) && ./test.sh

clean:
	$(RM) -r $(OBJ) $(TEST_OBJ) $(EXE)
	$(RM) -d $(DIR_OBJ) $(DIR_BIN)
