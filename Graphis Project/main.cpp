#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <ctime>  // For handling the timer

// Ball position and speed
float ballX = 0.0f;
float ballY = 0.0f;
float ballSpeedX = 0.008f;  // Speed on X-axis (increased)
float ballSpeedY = 0.01f;   // Speed on Y-axis (increased)
float ballRadius = 0.1f;

// Window dimensions
int windowWidth = 500;
int windowHeight = 500;

// Rectangle boundaries (bouncing area)
float rectLeft = -0.9f;
float rectRight = 0.9f;
float rectTop = 0.9f;
float rectBottom = -0.9f;

// Game state
bool gameOver = false;
bool playerWon = false;
bool restartFlag = false;
int playerScore = 0;
int computerScore = 0;

// Timer
int timerValue = 20;  // 20 seconds for the player to click the ball
time_t startTime;

// Function prototypes (including update function)
void update(int value);  // <== Declaration
void restartGame();      // Declaration for restartGame

// Function to convert screen coordinates to world coordinates
void screenToWorld(int x, int y, float &worldX, float &worldY) {
    float aspect = (float)windowWidth / (float)windowHeight;
    worldX = (float)x / (float)windowWidth * 2.0f - 1.0f;
    worldY = 1.0f - (float)y / (float)windowHeight * 2.0f;
    worldX *= aspect;  // Adjust for aspect ratio
}

// Function to draw the ball
void drawBall(float x, float y, float radius) {
    int numSegments = 100;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);  // Center of circle
    for (int i = 0; i <= numSegments; i++) {
        float angle = 2.0f * M_PI * i / numSegments;
        float dx = cos(angle) * radius;
        float dy = sin(angle) * radius;
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

// Function to draw the rectangle boundary
void drawRectangle() {
    glColor3f(0.0f, 1.0f, 0.0f);  // Green color for the rectangle
    glBegin(GL_LINE_LOOP);
    glVertex2f(rectLeft, rectBottom);   // Bottom left
    glVertex2f(rectRight, rectBottom);  // Bottom right
    glVertex2f(rectRight, rectTop);     // Top right
    glVertex2f(rectLeft, rectTop);      // Top left
    glEnd();
}

// Function to draw text
void drawText(const char *text, float x, float y) {
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text);
        text++;
    }
}

// Function to check if a point (x, y) is inside the ball
bool isInsideBall(float x, float y, float ballX, float ballY, float radius) {
    float dist = sqrt((x - ballX) * (x - ballX) + (y - ballY) * (y - ballY));
    return dist <= radius;
}

// Function to restart the game
void restartGame() {
    gameOver = false;
    playerWon = false;
    ballX = 0.0f;
    ballY = 0.0f;
    ballSpeedX = 0.008f;   // Reset ball speed on the X-axis
    ballSpeedY = 0.01f;    // Reset ball speed on the Y-axis
    timerValue = 20;       // Set timer to 20 seconds
    startTime = time(0);   // Start a new timer
    restartFlag = false;
    glutPostRedisplay();   // Force re-render of the window
    glutTimerFunc(16, update, 0);  // Restart the update loop
}

// Mouse click event handling for ball
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float worldX, worldY;
        screenToWorld(x, y, worldX, worldY);

        // Check if the mouse click is inside the ball
        if (!gameOver && isInsideBall(worldX, worldY, ballX, ballY, ballRadius)) {
            gameOver = true;  // End the game if the ball is clicked
            playerWon = true;
            playerScore++;     // Player wins, increment score
        }
    }
}

// Function to display win/lose messages and scores
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the rectangle boundary
    drawRectangle();

    if (!gameOver) {
        // Draw the ball
        glColor3f(1.0f, 0.0f, 0.0f);  // Red color for the ball
        drawBall(ballX, ballY, ballRadius);

        // Display message above the rectangle
        glColor3f(1.0f, 1.0f, 1.0f);  // White color for text
        drawText("Touch the ball & win the game!!", -0.4f, 0.95f);

        // Display timer
        char timerText[20];
        sprintf(timerText, "Time: %d", timerValue);
        drawText(timerText, -0.9f, 0.85f);

        // Display score
        char scoreText[50];
        sprintf(scoreText, "Player: %d   Computer: %d", playerScore, computerScore);
        drawText(scoreText, -0.9f, -0.95f);
    } else {
        // Display Game Over message and the result
        glColor3f(1.0f, 1.0f, 1.0f);  // White color for text
        if (playerWon) {
            drawText("Player Wins!", -0.1f, 0.0f);
        } else {
            drawText("Computer Wins!", -0.15f, 0.0f);
        }
        restartFlag = true;  // Set the flag to restart the game
    }

    glutSwapBuffers();
}

// Update function for moving the ball and handling timer
void update(int value) {
    if (!gameOver) {
        // Update the ball position
        ballX += ballSpeedX;
        ballY += ballSpeedY;

        // Check for collision with the rectangle boundaries
        if (ballX + ballRadius > rectRight || ballX - ballRadius < rectLeft) {
            ballSpeedX = -ballSpeedX;  // Reverse direction on X-axis
        }
        if (ballY + ballRadius > rectTop || ballY - ballRadius < rectBottom) {
            ballSpeedY = -ballSpeedY;  // Reverse direction on Y-axis
        }

        // Timer logic
        time_t currentTime = time(0);
        timerValue = 20 - difftime(currentTime, startTime);

        // Check if time is up
        if (timerValue <= 0) {
            gameOver = true;
            computerScore++;  // Increment computer's score if time is up
        }

        glutPostRedisplay();
        glutTimerFunc(16, update, 0);  // Call update function after 16 ms
    } else if (restartFlag) {
        // Restart the game after game over
        restartGame();
    }
}

// Window resize handler
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)w / (float)h;
    if (w >= h) {
        gluOrtho2D(-aspect, aspect, -1.0, 1.0);
    } else {
        gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    }
    glMatrixMode(GL_MODELVIEW);
}

// Main function
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Bouncing Ball with Game Over");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);              // Register mouse click event
    glutTimerFunc(16, update, 0);      // Start the update loop

    startTime = time(0);               // Start the timer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Black background

    glutMainLoop();
    return 0;
}
