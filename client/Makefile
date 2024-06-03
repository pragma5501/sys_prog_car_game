

SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin
UTL_DIR = ./src/util
INC_DIR = \
		./src \
		-I ./include \

		
GCCFLAGS += \
	-g \
	-fsanitize=address \
	-Wall \

LDFLAGS = \
	-lpthread \
	-lwiringPi\


OBJ_SRC += \
	$(SRC_DIR)/client.c \
	$(SRC_DIR)/handle_car.c \
	$(SRC_DIR)/algorithm.c \


TARGET_OBJ =\
    $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(OBJ_SRC)))

all : runfile

runfile: $(TARGET_OBJ)
	@mkdir -p $(BIN_DIR)
	gcc -o $(BIN_DIR)/$@ $^ -I$(INC_DIR) $(GCCFLAGS) $(LDFLAGS)
	ln -sf $(BIN_DIR)/$@ ./client

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	gcc -c $< -o $@ -I$(INC_DIR) $(GCCFLAGS) $(LDFLAGS)


.c.o:
	gcc -c $< -o $@ -I$(INC_DIR)

-include $(OBJS:.o=.d)

clean:
	@rm -vf $(BIN_DIR)/*
	@rm -vf $(OBJ_DIR)/*.o
	@rm -vf $(SRC_DIR)/*.o
	@rm -vf $(UTL_DIR)/*.o