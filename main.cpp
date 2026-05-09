#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <iostream>
using namespace sf;
using namespace std;

#define height 150.f
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
	float jumpDistance = 150.f; // Total distance to jump (adjust as needed)
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
        characterSprite.setTextureRect(IntRect(0, 0, 65, 64));
        characterSprite.setScale(2.0f, 2.0f);

		jumping = false;
        jumpTimeElapsed = 0;
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
			move(Vector2f(-50.f, 0));
		}
		else if (action == "right") {
			move(Vector2f(50.f, 0));
		}
		else if (action == "jump") {
			handleJump(dt);
		}
	}

	bool isJumping() {
		return jumping;
	}

	//once jump is initiated, use time to simulate gravity and bring character back down
    void handleJump(float dt) {
        if (jumping) {
            jumpTimeElapsed += dt;
			if (getPosition().y < (screenHeight-height)) {
				// Simulate gravity (simple linear fall)
				move(Vector2f(0, jumpDistance * dt / (jumpDuration - jumpTimeElapsed)));
			}
            else {
                jumping = false;
                jumpTimeElapsed = 0;
            }
        }
        else {
            jumpTimeElapsed += dt;
            if (getPosition().y > (screenHeight - height - jumpDistance)) {
                // Simulate gravity (simple linear fall)
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
        //Vector2f pos = shape.getPosition();
        return pos;
	}
};

int main() {
    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "SFML Bouncing Ball");

    Character c1;
    Clock clock;
	Clock animationClock;
    int currentFrame = 0;

    while (window.isOpen()) {
        Event event;
        float dt = clock.restart().asSeconds();

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

        
        window.clear(sf::Color::Black);
        c1.draw(window);
        window.display();
    }
    // hatim i hate you
	// i hope you fail all your classes and get a 0.0 in everything
    // base banaleta hai khud comeback humain kerna parta hai

    return 0;
}
