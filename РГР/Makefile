CC = g++
CFLAGS = -Wall -g -I./include
CPPFLAGS = -MP -MMD

BIN = bin
SRC = src
OBJ = obj
INCLUDE = include

LOBBY_DIR = lobby
GAME_DIR = game
SET_DIR = set

LOBBY_SRC = $(SRC)/$(LOBBY_DIR)/lobby.cpp
GAME_SRC = $(SRC)/$(GAME_DIR)/game.cpp
CLIENT_SRC = $(SRC)/$(SET_DIR)/client.cpp
SERVER_SRC = $(SRC)/$(SET_DIR)/server.cpp

LOBBY_OBJ_FILE = $(OBJ)/$(LOBBY_DIR)/lobby.o
GAME_OBJ_FILE = $(OBJ)/$(GAME_DIR)/game.o
CLIENT_OBJ = $(OBJ)/$(SET_DIR)/client.o
SERVER_OBJ = $(OBJ)/$(SET_DIR)/server.o

LOBBY_DEPS = $(OBJ)/$(LOBBY_DIR)/lobby.d
GAME_DEPS = $(OBJ)/$(GAME_DIR)/game.d
CLIENT_DEPS = $(OBJ)/$(SET_DIR)/client.d
SERVER_DEPS = $(OBJ)/$(SET_DIR)/server.d

CLIENT_TARGET = $(BIN)/client
SERVER_TARGET = $(BIN)/server
GAME_LIB = $(OBJ)/$(GAME_DIR)/libgame.a
LOBBY_LIB = $(OBJ)/$(LOBBY_DIR)/liblobby.a

DIRS = $(OBJ)/$(GAME_DIR) $(OBJ)/$(SET_DIR) $(OBJ)/$(LOBBY_DIR)
$(shell mkdir -p $(DIRS))

.PHONY: all clean game player

all: $(GAME_LIB) $(LOBBY_LIB) $(CLIENT_TARGET) $(SERVER_TARGET)

$(GAME_LIB): $(GAME_OBJ_FILE)
	@ar rcs $@ $^

$(LOBBY_LIB): $(LOBBY_OBJ_FILE)
	@ar rcs $@ $^

$(GAME_OBJ_FILE): $(GAME_SRC)
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(LOBBY_OBJ_FILE): $(LOBBY_SRC)
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(CLIENT_OBJ): $(CLIENT_SRC)
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(SERVER_OBJ): $(SERVER_SRC)
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(CLIENT_TARGET): $(CLIENT_OBJ) $(GAME_LIB) $(LOBBY_LIB)
	@mkdir -p $(BIN)
	@$(CC) $(CFLAGS) $^ -o $@

$(SERVER_TARGET): $(SERVER_OBJ) $(GAME_LIB) $(LOBBY_LIB)
	@mkdir -p $(BIN)
	@$(CC) $(CFLAGS) $^ -o $@

game:
	@./$(SERVER_TARGET)

player:
	@./$(CLIENT_TARGET)

-include $(GAME_DEPS) $(LOBBY_DEPS) $(CLIENT_DEPS) $(SERVER_DEPS)

clean:
	@rm -rf $(OBJ)/$(GAME_DIR)/* $(OBJ)/$(LOBBY_DIR)/* $(OBJ)/$(SET_DIR)/* $(CLIENT_TARGET) $(SERVER_TARGET)