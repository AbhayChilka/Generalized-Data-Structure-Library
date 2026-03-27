CC = g++
INCLUDE = -I../../include

SRC = test_scll.cpp
OBJ = test_scll.o

# Colors
GREEN = \033[0;32m
YELLOW = \033[1;33m
BLUE = \033[0;34m
RED = \033[0;31m
NC = \033[0m

app: $(OBJ)
	@echo "$(BLUE)🔨 Building app...$(NC)"
	@$(CC) $(OBJ) -o app
	@echo "$(GREEN)✅ Build successful!$(NC)"

$(OBJ): $(SRC)
	@echo "$(YELLOW)⚙️ Compiling $(SRC)...$(NC)"
	@$(CC) $(INCLUDE) -c $(SRC) -o $(OBJ)

run:
	@echo "$(GREEN)🚀 Running app...$(NC)"
	@./app

clean:
	@echo "$(RED)🧹 Cleaning...$(NC)"
	@rm -f app *.o
	@echo "$(GREEN)✔ Clean done$(NC)"