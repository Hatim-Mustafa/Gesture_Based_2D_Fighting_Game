#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <mutex>
#include "socket_server.cpp" 
#include "game_state_sender.cpp"
using namespace sf;
using namespace std;

#define height 210.f
#define hamza2k21 60.f
#define screenWidth 1400.f
#define screenHeight 800.f
#define attackState "ATTACK"
#define idleState "IDLE"
#define fmoveState "FORWARD"
#define bmoveState "BACK"
#define jumpState "JUMP"
#define shieldState "SHIELD"

//Character Class
//He He
class Character {
private:
	RectangleShape shape;
    bool jumping;
    float jumpDuration = 0.5f;
    float jumpTimeElapsed;
	float jumpDistance = 150.f;
    float moveDistance = 70.f;
    float moveDuration = 0.25f;
	float moveTimeElapsed;
    int health;
    string state;


    string texturePath = "with_outline/IDLE.png";
    string walkTexturePath = "with_outline/RUN.png";
public:
    Sprite characterSprite;
    Texture charTexture;
    Texture walkTexture;
    int framecycle = 7;
    bool movingLeft;
    bool movingRight;

    Character() {
        //shape.setSize(sf::Vector2f(hamza2k21, height));
        //shape.setFillColor(sf::Color::Blue);
        //shape.setPosition(200.f, screenHeight-height);
        charTexture.loadFromFile(texturePath);
        walkTexture.loadFromFile(walkTexturePath);
        characterSprite.setTexture(charTexture);
        characterSprite.setPosition(200.f, screenHeight - height);
        characterSprite.setTextureRect(IntRect(0, 0, 64, 64));
        characterSprite.setScale(3.0f, 3.0f);

		jumping = false;
        jumpTimeElapsed = 0;
		movingLeft = false;
		movingRight = false;
		moveTimeElapsed = 0;
        health = 100;
        state = idleState;
    }
    void move(const sf::Vector2f& velocity) {
		characterSprite.move(velocity);
        // shape.move(velocity);
    }
    void draw(sf::RenderWindow& window) {
		window.draw(characterSprite);
        //window.draw(shape);
    }

	void handleInput(String action, float dt) {
		if (action == "left") {
            if (isMoving()) {
				moveTimeElapsed = 0;
                // Reset move time to allow immediate direction change
            }
            characterSprite.setTexture(walkTexture);
            framecycle = 8;
			movingLeft = true;
			movingRight = false;
            state = bmoveState;
			handleMove(dt);
		}
		else if (action == "right") {
            if (isMoving()) {
                moveTimeElapsed = 0;  // Reset move time to allow immediate direction change
            }
            characterSprite.setTexture(walkTexture);
            framecycle = 8;
			movingRight = true;
			movingLeft = false;
            state = fmoveState;
			handleMove(dt);
		}
		else if (action == "jump") {
            state = jumpState;
			handleJump(dt);
		}
	}

	bool isJumping() {
		return jumping;
	}

	bool isMoving() {
		return movingLeft || movingRight;
	}

    void handleMove(float dt) {
		if (movingLeft) {
            characterSprite.setScale(-3.f, 3.f);
            characterSprite.setOrigin(100.f, 0.f);
			move(Vector2f(-moveDistance * dt / moveDuration, 0));
			moveTimeElapsed += dt;
			if (moveTimeElapsed >= moveDuration) {
				movingLeft = false;
				moveTimeElapsed = 0;
				framecycle = 7;
                characterSprite.setTexture(charTexture);
                state = idleState;
			}
		}
		else if (movingRight) {
            characterSprite.setScale(3.f, 3.f);
            characterSprite.setOrigin(0.f, 0.f);
			move(Vector2f(moveDistance * dt / moveDuration, 0));
			moveTimeElapsed += dt;
			if (moveTimeElapsed >= moveDuration) {
				movingRight = false;
				moveTimeElapsed = 0;
				framecycle = 7;
                characterSprite.setTexture(charTexture);
                state = idleState;

			}
		}
    }

	//once jump is initiated, use time to simulate gravity and bring character back down
    void handleJump(float dt) {
        if (jumping) {
            jumpTimeElapsed += dt;
			if (getPosition().y < (screenHeight-height)) {
				move(Vector2f(0, jumpDistance * dt / (jumpDuration - jumpTimeElapsed)));
			}
            else {
				move(Vector2f(0, screenHeight - height - getPosition().y));
                jumping = false;
                state = idleState;
                jumpTimeElapsed = 0;
            }
        }
        else {
            jumpTimeElapsed += dt;
            if (getPosition().y > (screenHeight - height - jumpDistance)) {
                move(Vector2f(0, -jumpDistance * dt / (jumpDuration - jumpTimeElapsed)));
            }
            else {
                if (jumpTimeElapsed >= jumpDuration) {
                    jumping = true;
                    jumpTimeElapsed = 0;
                }
            }
        }
    }

	Vector2f getPosition() {
		Vector2f pos = characterSprite.getPosition();
        return pos;
	}

	string getState() {
		return state;
	}

	int getHealth() {
		return health;
	}
};

class Enemy {
    RectangleShape shape;
	int health;
public:
	Enemy() {
		shape.setSize(sf::Vector2f(hamza2k21, height-70));
		shape.setFillColor(sf::Color::Red);
		shape.setPosition(1000.f, screenHeight - height+70);
		health = 100;
	}

	void draw(sf::RenderWindow& window) {         
        window.draw(shape);
	}

    Vector2f getPosition() {
        Vector2f pos = shape.getPosition();
        return pos;
    }

	int getHealth() {
		return health; 
	}
};


int main() {
    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Fighting Game");

    Character c1;
    Enemy e1;
    Clock clock;
	Clock animationClock;
    int currentFrame = 0;
    float dt;

    auto onGesture = [&c1, &dt](int player, const string& gesture) {
        if (player == 1) {
            if (gesture == "FORWARD") c1.handleInput("right", dt);
            else if (gesture == "BACK") c1.handleInput("left", dt);
            else if (gesture == "JUMP") {
                if (!c1.isJumping()) {
                    c1.handleInput("jump", dt);
                }
            }
            // Add more as needed
        }
    };

    GestureReceiver  recv(5005, onGesture);
    GameStateSender  stateTx("127.0.0.1", 5006);


    while (window.isOpen()) {
        Event event;
        dt = clock.restart().asSeconds();
        recv.poll();

        stateTx.send(
            { c1.getPosition().x, c1.getPosition().y, c1.getState(), c1.getHealth()},
            { e1.getPosition().x,  e1.getPosition().y, "IDLE", e1.getHealth()}
        );

        if (animationClock.getElapsedTime().asMilliseconds() > 100) {  // Change frame every 100ms
            currentFrame = (currentFrame + 1) % c1.framecycle;  // Cycle through 7 frames
            c1.characterSprite.setTextureRect(IntRect(currentFrame * 96, 0, 64, 64));
            animationClock.restart();
        }

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
			if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Left) {

					c1.handleInput("left", dt);
                }
                else if (event.key.code == sf::Keyboard::Right) {

                    c1.handleInput("right", dt);
                }
                else if (event.key.code == sf::Keyboard::Up) {
                    if (!c1.isJumping()) {
                        c1.handleInput("jump", dt);
                    }
				}

			}
        }

        if (c1.isJumping()) {
			c1.handleJump(dt);
        }
        else if (c1.getPosition().y < (screenHeight - height))
		{
			c1.handleJump(dt);
		}
        
        if (c1.isMoving()) {
            c1.handleMove(dt);
        }

        
        window.clear(sf::Color::Black);
        c1.draw(window);
		e1.draw(window);
        window.display();
    }

    return 0;
}
