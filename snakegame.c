
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h> 

#define cols 40
#define rows 20

// Board buffer
char board[cols * rows];

// Game State Variables
int isGameOver = 0;
int snakeX, snakeY;
int fruitX, fruitY;
int score = 0;

// Direction variables (0 means not moving)
int dirX = 0; 
int dirY = 0;

// Tail arrays (Max length 100)
int tailX[100], tailY[100];
int nTail = 0;

// Speed Control Variables
int speedLevel = 1; // 1 (Slow) to 3 (Fast)
int delayTime = 150; // Initial delay in milliseconds (Slowest speed)


// --- Helper Functions (Windows Console) ---

// Helper to stop screen flickering
void set_cursor_position(int x, int y) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = { (short)x, (short)y };
    SetConsoleCursorPosition(hOut, coord);
}

// Hide the blinking cursor for better visuals
void hide_cursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

// --- Instructions and Speed Setup ---

void set_speed(int level) {
    speedLevel = level;
    switch (level) {
        case 1: delayTime = 150; break; // Slow
        case 2: delayTime = 100; break; // Medium
        case 3: delayTime = 50;  break; // Fast
        default: delayTime = 100; // Default to medium
    }
}

void display_instructions() {
    char ch;
    
    // Clear screen before showing instructions
    system("cls"); 
    
    printf("########################################\n");
    printf("#           🐍 C SNAKE GAME 🐍         #\n");
    printf("########################################\n");
    printf("\n");
    printf(">> Controls:\n");
    printf("   - W, A, S, D: Move Direction\n");
    printf("   - X: Quit Game\n");
    printf("\n");
    printf(">> Speed Selection:\n");
    printf("   1. Slow\n");
    printf("   2. Medium\n");
    printf("   3. Fast\n");
    printf("\n");
    printf("Enter speed level (1-3) and press Enter: ");

    // Wait for speed input
    while (scanf("%d", &speedLevel) != 1 || speedLevel < 1 || speedLevel > 3) {
        printf("Invalid input. Enter 1, 2, or 3: ");
        // Clear input buffer (essential after scanf failure)
        while ((ch = getchar()) != '\n' && ch != EOF);
    }
    
    // Apply selected speed
    set_speed(speedLevel);

    printf("\nPress ANY key to start the game...\n");
    
    // Clear input buffer again before _getch()
    while ((ch = getchar()) != '\n' && ch != EOF);
    
    // Wait for the user to press a key to begin
    _getch(); 
    
    // Clear screen again before drawing the board
    system("cls"); 
}


// --- Game Core Functions ---

void setup() {
    // Show instructions and get speed setting
    display_instructions(); 

    isGameOver = 0;
    dirX = 1; // Start moving right immediately
    dirY = 0;
    nTail = 0;
    score = 0;
    
    // Start in the middle
    snakeX = cols / 2;
    snakeY = rows / 2;

    // Randomize fruit position
    srand(time(NULL));
    fruitX = rand() % (cols - 2) + 1; // +1 to avoid wall
    fruitY = rand() % (rows - 2) + 1; 
}

void update_board_buffer() {
    int x, y, k;
    
    // 1. Clear the board and draw walls
    for (y = 0; y < rows; y++) {
        for (x = 0; x < cols; x++) {
            if (x == 0 || y == 0 || x == cols - 1 || y == rows - 1) {
                board[y * cols + x] = '#'; // Wall
            } else {
                board[y * cols + x] = ' '; // Empty space
            }
        }
    }

    // 2. Place Fruit
    board[fruitY * cols + fruitX] = '*';

    // 3. Place Snake Head
    board[snakeY * cols + snakeX] = 'O';

    // 4. Place Snake Tail
    for (k = 0; k < nTail; k++) {
        int tx = tailX[k];
        int ty = tailY[k];
        board[ty * cols + tx] = 'o';
    }
}

void draw() {
    // Move cursor to top-left instead of clearing screen (removes flickering)
    set_cursor_position(0, 0);

    update_board_buffer();

    int x, y;
    for (y = 0; y < rows; y++) {
        for (x = 0; x < cols; x++) {
            printf("%c", board[y * cols + x]);
        }
        printf("\n");
    }
    
    printf("Score: %d | Speed: %d\n", score, speedLevel);
    printf("Controls: W A S D | X to Quit");
}

void input() {
    if (_kbhit()) {
        char ch = _getch();
        
        // Convert to lowercase to handle Caps Lock
        if (ch >= 'A' && ch <= 'Z') {
            ch = ch + 32; 
        }

        switch (ch) {
            case 'a': 
                if(dirX != 1) { dirX = -1; dirY = 0; } // Prevent reversing
                break;
            case 'd': 
                if(dirX != -1) { dirX = 1; dirY = 0; }
                break;
            case 'w': 
                if(dirY != 1) { dirX = 0; dirY = -1; }
                break;
            case 's': 
                if(dirY != -1) { dirX = 0; dirY = 1; }
                break;
            case 'x': 
                isGameOver = 1; 
                break;
        }
    }
}

void logic() {
    // 1. Logic for Tail (Follow the leader)
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    
    tailX[0] = snakeX;
    tailY[0] = snakeY;

    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    // 2. Move Head based on current direction
    snakeX += dirX;
    snakeY += dirY;

    // 3. Wall Collision
    if (snakeX <= 0 || snakeX >= cols - 1 || snakeY <= 0 || snakeY >= rows - 1) {
        isGameOver = 1;
    }

    // 4. Tail Collision (Bit the self)
    for (int i = 0; i < nTail; i++) {
        if (tailX[i] == snakeX && tailY[i] == snakeY) {
            isGameOver = 1;
        }
    }

    // 5. Eating Fruit
    if (snakeX == fruitX && snakeY == fruitY) {
        score += 10;
        nTail++; // Increase length
        
        // Spawn new fruit (ensure it doesn't land on the snake's body)
        int valid_spawn = 0;
        while (!valid_spawn) {
            fruitX = rand() % (cols - 2) + 1;
            fruitY = rand() % (rows - 2) + 1;
            valid_spawn = 1;
            
            if (fruitX == snakeX && fruitY == snakeY) { valid_spawn = 0; continue; }
            for (int i = 0; i < nTail; i++) {
                if (fruitX == tailX[i] && fruitY == tailY[i]) {
                    valid_spawn = 0;
                    break;
                }
            }
        }
    }
}


int main() {
    hide_cursor();
    setup();

    while (!isGameOver) {
        draw();
        input();
        logic();
        
        // Control speed based on selected level
        Sleep(delayTime); 
    }

    // Clean exit
    set_cursor_position(0, rows + 3);
    printf("\nGame Over! Final Score: %d\n", score);
    
    // System pause waits for the user to press a key before the console closes
    printf("Press any key to exit...");
    _getch();
    return 0;
}