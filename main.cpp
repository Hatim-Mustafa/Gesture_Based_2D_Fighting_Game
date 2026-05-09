#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <mutex>
#include "socket_server.cpp" 
using namespace sf;
using namespace std;

#define height 210.f
#define hamza2k21 60.f
#define screenWidth 1400.f
#define screenHeight 800.f

//Character Class
class Character {
private:
	RectangleShape shape;
    bool jumping;
    float jumpDuration = 0.5f;
    float jumpTimeElapsed;
	float jumpDistance = 150.f;
    float moveDistance = 60.f;
    float moveDuration = 0.25f;
	float moveTimeElapsed;
    bool movingLeft;
	bool movingRight;
    string texturePath = "with_outline/IDLE.png";
public:
    Sprite characterSprite;
    Texture charTexture;
    Character() {
        //shape.setSize(sf::Vector2f(hamza2k21, height));
        //shape.setFillColor(sf::Color::Blue);
        //shape.setPosition(200.f, screenHeight-height);
        charTexture.loadFromFile(texturePath);
        characterSprite.setTexture(charTexture);
        characterSprite.setPosition(200.f, screenHeight - height);
        characterSprite.setTextureRect(IntRect(0, 0, 64, 64));
        characterSprite.setScale(3.0f, 3.0f);

		jumping = false;
        jumpTimeElapsed = 0;
		movingLeft = false;
		movingRight = false;
		moveTimeElapsed = 0;
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
				moveTimeElapsed = 0;  // Reset move time to allow immediate direction change
            }
			movingLeft = true;
			movingRight = false;  // Stop moving right if currently moving
			handleMove(dt);
		}
		else if (action == "right") {
            if (isMoving()) {
                moveTimeElapsed = 0;  // Reset move time to allow immediate direction change
            }
			movingRight = true;
			movingLeft = false;  // Stop moving left if currently moving
			handleMove(dt);
		}
		else if (action == "jump") {
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
			move(Vector2f(-moveDistance * dt / moveDuration, 0));
			moveTimeElapsed += dt;
			if (moveTimeElapsed >= moveDuration) {
				movingLeft = false;
				moveTimeElapsed = 0;
			}
		}
		else if (movingRight) {
			move(Vector2f(moveDistance * dt / moveDuration, 0));
			moveTimeElapsed += dt;
			if (moveTimeElapsed >= moveDuration) {
				movingRight = false;
				moveTimeElapsed = 0;
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
};

int main() {
    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "SFML Bouncing Ball");

    Clock clock;
	Clock animationClock;
    int currentFrame = 0;
    Character c1;
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


    while (window.isOpen()) {
        Event event;
        dt = clock.restart().asSeconds();
        recv.poll();

        if (animationClock.getElapsedTime().asMilliseconds() > 100) {  // Change frame every 100ms
            currentFrame = (currentFrame + 1) % 7;  // Cycle through 7 frames
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
        window.display();
    }

    return 0;
}
