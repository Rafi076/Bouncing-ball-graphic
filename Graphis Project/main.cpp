#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <ctime>  // For handling the timer

// Ball position and speed
float ballX = 0.0f;
float ballY = 0.0f;
float ballSpeedX = 0.015f;  // Increased Speed on X-axis
float ballSpeedY = 0.02f;   // Increased Speed on Y-axis
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
int playerScore = 0;
int computerScore = 0;
const int maxScore = 5;  // Score needed to win the game

// Timer
int timerValue = 10;  // 10 seconds for the player to click the ball
time_t startTime;

// Function prototypes
void update(int value);

// Function to convert screen coordinates to world coordinates
void screenToWorld(int x, int y, float &worldX, float &worldY) {
    float aspect = (float)windowWidth / (float)windowHeight;
    worldX = (float)x / (float)windowWidth * 2.0f - 1.0f;
    worldY = 1.0f - (float)y / (float)windowHeight * 2.0f;
    worldX *= aspect;  // Adjust for aspect ratio
}

// Function to draw the ball with a white outline
void drawBall(float x, float y, float radius) {
    int numSegments = 100;

    // Draw the white outline (slightly larger than the ball)
    glColor3f(1.0f, 1.0f, 1.0f);  // White outline
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);  // Center of circle
    for (int i = 0; i <= numSegments; i++) {
        float angle = 2.0f * M_PI * i / numSegments;
        float dx = cos(angle) * (radius + 0.01f);  // Slightly larger radius for the outline
        float dy = sin(angle) * (radius + 0.01f);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();

    // Draw the inner black ball
    glColor3f(0.0f, 0.0f, 0.0f);  // Black color for the ball
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

// Mouse click event handling for ball
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float worldX, worldY;
        screenToWorld(x, y, worldX, worldY);

        // Check if the mouse click is inside the ball
        if (!gameOver && isInsideBall(worldX, worldY, ballX, ballY, ballRadius)) {
            playerScore++;     // Player scores if the ball is clicked
            if (playerScore == maxScore) {
                gameOver = true;
                playerWon = true;
            }
        }
    }
}

// Function to display win/lose messages, scores, and reset button
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the rectangle boundary
    drawRectangle();

    if (!gameOver) {
        // Draw the ball
        drawBall(ballX, ballY, ballRadius);

        // Display message above the rectangle
        glColor3f(1.0f, 1.0f, 1.0f);  // White color for text
        drawText("Touch the ball & win points!!", -0.4f, 0.95f);

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
            drawText("Player Wins the Game!", -0.2f, 0.0f);
        } else {
            drawText("Computer Wins the Game!", -0.25f, 0.0f);
        }
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
        timerValue = 10 - difftime(currentTime, startTime);

        // Check if time is up
        if (timerValue <= 0) {
            computerScore++;  // Increment computer's score if time is up
            if (computerScore == maxScore) {
                gameOver = true;
                playerWon = false;
            } else {
                // Restart timer
                startTime = time(0);
                timerValue = 10;
            }
        }

        glutPostRedisplay();
        glutTimerFunc(16, update, 0);  // Call update function after 16 ms
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

// Change the mouse cursor to resemble a bat
void setMouseCursor() {
    glutSetCursor(GLUT_CURSOR_CROSSHAIR);
}

// Main function
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Bouncing Ball with Scoring and Time Limit");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);              // Register mouse click event
    glutTimerFunc(16, update, 0);      // Start the update loop

    startTime = time(0);               // Start the timer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Black background

    setMouseCursor();  // Set the mouse cursor to a custom style

    glutMainLoop();
    return 0;
}
