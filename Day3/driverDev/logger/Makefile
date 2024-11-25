
# Directories
SRC_DIR = src/
INC_DIR = include/
BUILD_DIR = build/
OBJ_DIR = $(BUILD_DIR)obj/

TARGET = $(BUILD_DIR)bin

# Source files
SRC = $(SRC_DIR)main.c
SRC += $(SRC_DIR)myLogger.c

# Object files
OBJ = $(OBJ_DIR)main.o
OBJ += $(OBJ_DIR)myLogger.o

# Include files
INC = $(INC_DIR)myLogger.h

# Create the target executable from object files
$(TARGET): $(OBJ)
	gcc $(OBJ) -o $(TARGET)
	./$(TARGET)

# Compile the object files from source files
$(OBJ_DIR)main.o: $(SRC_DIR)main.c $(INC)
	gcc -c -I$(INC_DIR) $(SRC_DIR)main.c -o $(OBJ_DIR)main.o

$(OBJ_DIR)myLogger.o: $(SRC_DIR)myLogger.c $(INC)
	gcc -c -I$(INC_DIR) $(SRC_DIR)myLogger.c -o $(OBJ_DIR)myLogger.o

# run the executable
run:
	./$(TARGET)

# Clean the build
clean:
	rm -rf $(TARGET) $(OBJ_DIR)*.o

