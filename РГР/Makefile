CC = g++
CFLAGS = -Wall -g -I./include
CPPFLAGS = -MP -MMD

SRC = src
OBJ = obj
INCLUDE = include

GAME_DIR = game
WEB_DIR = web

GAME_SRC = $(SRC)/$(GAME_DIR)/game.cpp
CLIENT_SRC = $(SRC)/$(WEB_DIR)/client.cpp
SERVER_SRC = $(SRC)/$(WEB_DIR)/server.cpp

GAME_OBJ_FILE = $(OBJ)/$(GAME_DIR)/game.o
GAME_DEPS = $(OBJ)/$(GAME_DIR)/game.d
CLIENT_OBJ = $(OBJ)/$(WEB_DIR)/client.o
SERVER_OBJ = $(OBJ)/$(WEB_DIR)/server.o
CLIENT_DEPS = $(OBJ)/$(WEB_DIR)/client.d
SERVER_DEPS = $(OBJ)/$(WEB_DIR)/server.d

CLIENT_TARGET = client
SERVER_TARGET = server
GAME_LIB = $(OBJ)/$(GAME_DIR)/libgame.a

DIRS = $(OBJ)/$(GAME_DIR) $(OBJ)/$(WEB_DIR)
$(shell mkdir -p $(DIRS))

.PHONY: all clean game player

all: $(GAME_LIB) $(CLIENT_TARGET) $(SERVER_TARGET)

$(GAME_LIB): $(GAME_OBJ_FILE)
	ar rcs $@ $^

$(GAME_OBJ_FILE): $(GAME_SRC)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(CLIENT_OBJ): $(CLIENT_SRC)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(SERVER_OBJ): $(SERVER_SRC)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(CLIENT_TARGET): $(CLIENT_OBJ) $(GAME_LIB)
	$(CC) $(CFLAGS) $^ -o $@

$(SERVER_TARGET): $(SERVER_OBJ) $(GAME_LIB)
	$(CC) $(CFLAGS) $^ -o $@

game:
	./$(SERVER_TARGET)

player:
	./$(CLIENT_TARGET)

-include $(GAME_DEPS) $(CLIENT_DEPS) $(SERVER_DEPS)

clean:
	rm -rf $(OBJ) $(CLIENT_TARGET) $(SERVER_TARGET)