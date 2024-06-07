#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <random>
#include <ctime>

const int ROWS = 6;
const int COLS = 7;

class Connect4 {
public:
    Connect4() {
        board.resize(ROWS, std::vector<char>(COLS, ' '));
        std::srand(std::time(nullptr)); // Initialize random seed
    }

    void playGame() {
        int mode = displayMenu();
        if (mode == 1) {
            playAgainstFriend();
        } else if (mode == 2) {
            playAgainstBot();
        }
    }

private:
    std::vector<std::vector<char>> board;
    std::string player1, player2;
    std::unordered_map<std::string, int> playerStats;

    void loadPlayerStats() {
        std::ifstream infile("player_stats.txt");
        if (infile.is_open()) {
            std::string name;
            int wins;
            while (infile >> name >> wins) {
                playerStats[name] = wins;
            }
            infile.close();
        }
    }

    void savePlayerStats() {
        std::ofstream outfile("player_stats.txt");
        for (const auto& pair : playerStats) {
            outfile << pair.first << " " << pair.second << "\n";
        }
        outfile.close();
    }

    int displayMenu() {
        int choice;
        std::cout << "Welcome to Connect 4!\n";
        std::cout << "1. Play against a friend\n";
        std::cout << "2. Play against the bot\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        return choice;
    }

    void getPlayerNames() {
        std::cout << "Enter name for Player 1: ";
        std::cin >> player1;
        std::cout << "Enter name for Player 2: ";
        std::cin >> player2;
        if (playerStats.find(player1) == playerStats.end()) playerStats[player1] = 0;
        if (playerStats.find(player2) == playerStats.end()) playerStats[player2] = 0;
    }

    void displayBoard() {
        for (int row = 0; row < ROWS; ++row) {
            for (int col = 0; col < COLS; ++col) {
                std::cout << "| " << board[row][col] << " ";
            }
            std::cout << "|\n";
        }
        for (int col = 0; col < COLS; ++col) {
            std::cout << "----";
        }
        std::cout << "-\n";
        for (int col = 0; col < COLS; ++col) {
            std::cout << "  " << col + 1 << " ";
        }
        std::cout << "\n";
    }

    int getPlayerMove(int player) {
        int column;
        std::cout << "Player " << player << ", enter column (1-" << COLS << "): ";
        while (true) {
            std::cin >> column;
            if (std::cin.fail() || column < 1 || column > COLS) {
                std::cin.clear();
                std::cin.ignore(1000, '\n');
                std::cout << "Invalid input. Enter a column number between 1 and " << COLS << ": ";
            } else {
                break;
            }
        }
        return column - 1;
    }

    int getBotMove() {
        std::vector<int> availableCols;
        for (int col = 0; col < COLS; ++col) {
            if (board[0][col] == ' ') {
                availableCols.push_back(col);
            }
        }
        if (availableCols.empty()) {
            return -1; // Should never reach here
        }
        int randomIndex = std::rand() % availableCols.size();
        return availableCols[randomIndex];
    }

    bool makeMove(int player, int col) {
        for (int row = ROWS - 1; row >= 0; --row) {
            if (board[row][col] == ' ') {
                board[row][col] = (player == 1) ? 'X' : 'O';
                return true;
            }
        }
        return false;
    }

    bool checkWin(int player) {
        char symbol = (player == 1) ? 'X' : 'O';
        // Check horizontal
        for (int row = 0; row < ROWS; ++row) {
            for (int col = 0; col < COLS - 3; ++col) {
                if (board[row][col] == symbol && board[row][col + 1] == symbol &&
                    board[row][col + 2] == symbol && board[row][col + 3] == symbol) {
                    return true;
                }
            }
        }
        // Check vertical
        for (int col = 0; col < COLS; ++col) {
            for (int row = 0; row < ROWS - 3; ++row) {
                if (board[row][col] == symbol && board[row + 1][col] == symbol &&
                    board[row + 2][col] == symbol && board[row + 3][col] == symbol) {
                    return true;
                }
            }
        }
        // Check diagonal (\)
        for (int row = 0; row < ROWS - 3; ++row) {
            for (int col = 0; col < COLS - 3; ++col) {
                if (board[row][col] == symbol && board[row + 1][col + 1] == symbol &&
                    board[row + 2][col + 2] == symbol && board[row + 3][col + 3] == symbol) {
                    return true;
                }
            }
        }
        // Check diagonal (/)
        for (int row = 0; row < ROWS - 3; ++row) {
            for (int col = 3; col < COLS; ++col) {
                if (board[row][col] == symbol && board[row + 1][col - 1] == symbol &&
                    board[row + 2][col - 2] == symbol && board[row + 3][col - 3] == symbol) {
                    return true;
                }
            }
        }
        return false;
    }

    bool isBoardFull() {
        for (int col = 0; col < COLS; ++col) {
            if (board[0][col] == ' ') {
                return false;
            }
        }
        return true;
    }

    void playAgainstFriend() {
        loadPlayerStats();
        getPlayerNames();
        int currentPlayer = 1;
        bool gameWon = false;
        while (!gameWon && !isBoardFull()) {
            displayBoard();
            int column = getPlayerMove(currentPlayer);
            if (makeMove(currentPlayer, column)) {
                gameWon = checkWin(currentPlayer);
                if (gameWon) {
                    displayBoard();
                    std::cout << "Player " << currentPlayer << " (" << (currentPlayer == 1 ? player1 : player2) << ") wins!\n";
                    playerStats[currentPlayer == 1 ? player1 : player2]++;
                    savePlayerStats();
                } else {
                    currentPlayer = 3 - currentPlayer; // Switches between 1 and 2
                }
            } else {
                std::cout << "Column is full! Try a different column.\n";
            }
        }
        if (!gameWon) {
            displayBoard();
            std::cout << "It's a draw!\n";
        }
    }

    void playAgainstBot() {
        loadPlayerStats();
        std::cout << "Enter your name: ";
        std::cin >> player1;
        player2 = "Bot";
        if (playerStats.find(player1) == playerStats.end()) playerStats[player1] = 0;
        if (playerStats.find(player2) == playerStats.end()) playerStats[player2] = 0;

        int currentPlayer = 1;
        bool gameWon = false;
        while (!gameWon && !isBoardFull()) {
            displayBoard();
            int column;
            if (currentPlayer == 1) {
                column = getPlayerMove(currentPlayer);
            } else {
                column = getBotMove();
                std::cout << "Bot chooses column " << column + 1 << "\n";
            }
            if (makeMove(currentPlayer, column)) {
                gameWon = checkWin(currentPlayer);
                if (gameWon) {
                    displayBoard();
                    std::cout << "Player " << currentPlayer << " (" << (currentPlayer == 1 ? player1 : player2) << ") wins!\n";
                    playerStats[currentPlayer == 1 ? player1 : player2]++;
                    savePlayerStats();
                } else {
                    currentPlayer = 3 - currentPlayer; // Switches between 1 and 2
                }
            } else {
                std::cout << "Column is full! Try a different column.\n";
            }
        }
        if (!gameWon) {
            displayBoard();
            std::cout << "It's a draw!\n";
        }
    }
};

int main() {
    Connect4 game;
    game.playGame();
    return 0;
}
