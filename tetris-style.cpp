#include <iostream>
#include <chrono>
#include <thread>
#include <ncurses.h>
#include <vector>
#include <random>

#define SCREEN_WIDTH 12
#define SCREEN_HEIGHT 20
#define BLOCK_SIZE 4
#define FRAME_RATE 1

class TetrisGame {
private:
    int playerX, playerY;
    std::vector<std::vector<bool> > board;
    std::vector<std::vector<bool> > currentBlock;

    void setNonBlockingInput() {
        timeout(0);
    }

    void clearScreen() {
        clear();
    }

    void initializeBlock() {
        currentBlock = std::vector<std::vector<bool> >(BLOCK_SIZE, std::vector<bool>(BLOCK_SIZE, false));

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 6);

        int blockType = dis(gen);

        switch (blockType) {
            case 0: // I
                currentBlock[1][0] = true;
                currentBlock[1][1] = true;
                currentBlock[1][2] = true;
                currentBlock[1][3] = true;
                break;
            case 1: // J
                currentBlock[0][0] = true;
                currentBlock[1][0] = true;
                currentBlock[1][1] = true;
                currentBlock[1][2] = true;
                break;
            case 2: // L
                currentBlock[0][2] = true;
                currentBlock[1][0] = true;
                currentBlock[1][1] = true;
                currentBlock[1][2] = true;
                break;
            case 3: // O
                currentBlock[0][0] = true;
                currentBlock[0][1] = true;
                currentBlock[1][0] = true;
                currentBlock[1][1] = true;
                break;
            case 4: // S
                currentBlock[0][1] = true;
                currentBlock[0][2] = true;
                currentBlock[1][0] = true;
                currentBlock[1][1] = true;
                break;
            case 5: // T
                currentBlock[0][1] = true;
                currentBlock[1][0] = true;
                currentBlock[1][1] = true;
                currentBlock[1][2] = true;
                break;
            case 6: // Z
                currentBlock[0][0] = true;
                currentBlock[0][1] = true;
                currentBlock[1][1] = true;
                currentBlock[1][2] = true;
                break;
        }
    }

    void drawBlock(int x, int y) {
        for (int i = 0; i < BLOCK_SIZE; i++) {
            for (int j = 0; j < BLOCK_SIZE; j++) {
                if (currentBlock[i][j]) {
                    mvaddch(y + i, x + j, 'X');
                }
            }
        }
    }

    bool isMovable(int x, int y, std::vector<std::vector<bool> >& block) {
        for (int i = 0; i < BLOCK_SIZE; i++) {
            for (int j = 0; j < BLOCK_SIZE; j++) {
                if (block[i][j] && (x + j < 0 || x + j >= SCREEN_WIDTH || y + i >= SCREEN_HEIGHT || (y + i >= 0 && board[y + i][x + j]))) {
                    return false;
                }
            }
        }
        return true;
    }

    void fixBlock() {
        for (int i = 0; i < BLOCK_SIZE; i++) {
            for (int j = 0; j < BLOCK_SIZE; j++) {
                if (currentBlock[i][j]) {
                    board[playerY + i][playerX + j] = true;
                }
            }
        }
    }

    void rotateBlock() {
        std::vector<std::vector<bool> > newBlock = currentBlock;

        for (int i = 0; i < BLOCK_SIZE; i++) {
            for (int j = 0; j < BLOCK_SIZE; j++) {
                newBlock[i][j] = currentBlock[BLOCK_SIZE - j - 1][i];
            }
        }

        if (isMovable(playerX, playerY, newBlock)) {
            currentBlock = newBlock;
        }
    }

    void checkLines() {
        for (int y = SCREEN_HEIGHT - 1; y >= 0; y--) {
            bool isLineFull = true;
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                if (!board[y][x]) {
                    isLineFull = false;
                    break;
                }
            }

            if (isLineFull) {
                for (int i = y; i > 0; i--) {
                    for (int j = 0; j < SCREEN_WIDTH; j++) {
                        board[i][j] = board[i - 1][j];
                    }
                }
                for (int j = 0; j < SCREEN_WIDTH; j++) {
                    board[0][j] = false;
                }
                y++;
            }
        }
    }

    void generateNewBlock() {
        initializeBlock();
        playerX = SCREEN_WIDTH / 2 - BLOCK_SIZE / 2;
        playerY = 0;
        if (!isMovable(playerX, playerY, currentBlock)) {
            endwin();
            std::cout << "Game Over!" << std::endl;
            exit(0);
        }
    }

    void drawBoard() {
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                if (board[y][x]) {
                    mvaddch(y, x, 'X');
                }
            }
        }
    }

public:
    TetrisGame() {
        initscr();
        noecho();
        curs_set(0);

	board = std::vector<std::vector<bool> >(SCREEN_HEIGHT, std::vector<bool>(SCREEN_WIDTH, false));
        generateNewBlock();

        setNonBlockingInput();
    }

    ~TetrisGame() {
        endwin();
    }

    void run() {
        int input;

        while (1) {
            input = getch();

            if (input == 'a') {
                if (isMovable(playerX - 1, playerY, currentBlock)) {
                    playerX--;
                }
            }

            if (input == 'd') {
                if (isMovable(playerX + 1, playerY, currentBlock)) {
                    playerX++;
                }
            }

            if (input == 's') {
                if (isMovable(playerX, playerY + 1, currentBlock)) {
                    playerY++;
                }
            }

            if (input == 'w') {
                rotateBlock();
            }

            if (!isMovable(playerX, playerY + 1, currentBlock)) {
                fixBlock();
                checkLines();
                generateNewBlock();
            }

            clearScreen();
            drawBlock(playerX, playerY);
            drawBoard();

            std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_RATE));
        }
    }
};

int main() {
    TetrisGame game;
    game.run();

    return 0;
}

