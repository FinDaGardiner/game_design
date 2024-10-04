#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

Vector2f ballVelocity;
bool isPlayer1Serving = false;
const float initialVelocityX = 100.f;
const float initialVelocityY = 60.f;
const float velocityMultiplier = 1.1f;


Font font;
Text text;
int player1Score = 0;
int player2Score = 0;

const Keyboard::Key controls[4] = {
    Keyboard::A,   // Player1 UP
    Keyboard::Z,   // Player1 Down
    Keyboard::K,  // Player2 UP
    Keyboard::M // Player2 Down
};

const Vector2f paddleSize(25.f, 100.f);
const float ballRadius = 10.f;
const int gameWidth = 800;
const int gameHeight = 600;
const float paddleSpeed = 400.f;
const float paddleOffsetWall = 10.f;

CircleShape ball;
RectangleShape paddles[2];

void Load() {
    // Set size and origin of paddles
    for (auto& p : paddles) {
        p.setSize(paddleSize);
        p.setOrigin(paddleSize / 2.f);
    }
    // Set size and origin of ball
    ball.setRadius(ballRadius);
    ball.setOrigin(ballRadius, ballRadius); //Should be half the ball width and height

    // Reset paddle position
    paddles[0].setPosition(Vector2f(paddleOffsetWall + paddleSize.x / 2.f, gameHeight / 2.f));
    paddles[1].setPosition(Vector2f(gameWidth - paddleOffsetWall - paddleSize.x / 2.f, gameHeight / 2.f));

    // Reset ball position
    ball.setPosition(Vector2f(gameWidth / 2.f, gameHeight / 2.f));

    // Initialize ball velocity
    ballVelocity = { (isPlayer1Serving ? initialVelocityX : -initialVelocityX), initialVelocityY };
}

void Reset() {
    // Reset paddle positions
    paddles[0].setPosition(Vector2f(paddleOffsetWall + paddleSize.x / 2.f, gameHeight / 2.f));
    paddles[1].setPosition(Vector2f(gameWidth - paddleOffsetWall - paddleSize.x / 2.f, gameHeight / 2.f));

    // Reset ball position
    ball.setPosition(Vector2f(gameWidth / 2.f, gameHeight / 2.f));

    // Reverse ball velocity based on the last scorer
    isPlayer1Serving = !isPlayer1Serving;
    ballVelocity = { (isPlayer1Serving ? initialVelocityX : -initialVelocityX), initialVelocityY };
}


void Update(RenderWindow& window) {
    // Reset clock, recalculate deltatime
    static Clock clock;
    float dt = clock.restart().asSeconds();

    // check and consume events
    Event event;
    while (window.pollEvent(event)) {
        if (event.type == Event::Closed) {
            window.close();
            return;
        }
    }

    // Quit Via ESC Key
    if (Keyboard::isKeyPressed(Keyboard::Escape)) {
        window.close();
    }

    // handle paddle movement for Player 1
    float direction1 = 0.0f;
    if (Keyboard::isKeyPressed(controls[0])) {
        direction1--;
    }
    if (Keyboard::isKeyPressed(controls[1])) {
        direction1++;
    }
    paddles[0].move(Vector2f(0.f, direction1 * paddleSpeed * dt));

    // handle paddle movement for Player 2
    float direction2 = 0.0f;
    if (Keyboard::isKeyPressed(controls[2])) {
        direction2--;
    }
    if (Keyboard::isKeyPressed(controls[3])) {
        direction2++;
    }
    paddles[1].move(Vector2f(0.f, direction2 * paddleSpeed * dt));

    // Move ball
    ball.move(ballVelocity * dt);

    // check ball collision with walls
    const float bx = ball.getPosition().x;
    const float by = ball.getPosition().y;

    // Collision with top and bottom walls
    if (by > gameHeight) {
        ballVelocity.y *= -velocityMultiplier;
        ball.move(Vector2f(0.f, -10.f));
    }
    else if (by < 0) {
        ballVelocity.y *= -velocityMultiplier;
        ball.move(Vector2f(0.f, 10.f));
    }

    // Collision with paddles
    if (
        // Left paddle collision
        bx < paddleSize.x + paddleOffsetWall &&
        by > paddles[0].getPosition().y - (paddleSize.y * 0.5f) &&
        by < paddles[0].getPosition().y + (paddleSize.y * 0.5f)
        ) {
        ballVelocity.x *= -velocityMultiplier;
        ball.move(Vector2f(10.f, 0.f));
    }
    else if (
        // Right paddle collision
        bx > gameWidth - paddleSize.x - paddleOffsetWall &&
        by > paddles[1].getPosition().y - (paddleSize.y * 0.5f) &&
        by < paddles[1].getPosition().y + (paddleSize.y * 0.5f)
        ) {
        ballVelocity.x *= -velocityMultiplier;
        ball.move(Vector2f(-10.f, 0.f));
    }

    // Check if ball went past left or right boundary (scoring walls)
    if (bx > gameWidth) {
        // Right wall - Player 1 scores
        Reset();
    }
    else if (bx < 0) {
        // Left wall - Player 2 scores
        Reset();
    }

    // Make sure paddles don't go off screen
    for (auto& p : paddles) {
        if (p.getPosition().y < paddleSize.y / 2.f) {
            p.setPosition(p.getPosition().x, paddleSize.y / 2.f);
        }
        if (p.getPosition().y > gameHeight - paddleSize.y / 2.f) {
            p.setPosition(p.getPosition().x, gameHeight - paddleSize.y / 2.f);
        }
    }
}

void AIUpdate(float dt) {
    // AI moves up or down to follow the ball
    float ballY = ball.getPosition().y;
    float paddle2Y = paddles[1].getPosition().y;

    if (ballY < paddle2Y) {
        paddles[1].move(0.f, -paddleSpeed * dt);
    }
    if (ballY > paddle2Y) {
        paddles[1].move(0.f, paddleSpeed * dt);
    }

    // Ensure the AI paddle doesn't leave the screen
    if (paddles[1].getPosition().y < paddleSize.y / 2.f) {
        paddles[1].setPosition(paddles[1].getPosition().x, paddleSize.y / 2.f);
    }
    if (paddles[1].getPosition().y > gameHeight - paddleSize.y / 2.f) {
        paddles[1].setPosition(paddles[1].getPosition().x, gameHeight - paddleSize.y / 2.f);
    }
}

//Render and Main
void Render(RenderWindow& window) {
    // Draw Everything
    window.draw(paddles[0]);
    window.draw(paddles[1]);
    window.draw(ball);
}

int main() {
    RenderWindow window(VideoMode(gameWidth, gameHeight), "PONG");
    Load();
    while (window.isOpen()) {
        window.clear();
        Update(window);
        Render(window);
        window.display();
    }
    return 0;
}