#include <SFML/Graphics.hpp>
#include <stdio.h>
using namespace sf;
using namespace std;

#define height 150.f
#define hamza2k21 60.f
#define screenWidth 1400.f
#define screenHeight 800.f

//Character Class
class Character {
private:
	sf::RectangleShape shape;
    bool jumping;
    float jumpDuration = 0.5f;
    float jumpTimeElapsed;
	float jumpDistance = 150.f; // Total distance to jump (adjust as needed)
public:
    Character() {
        shape.setSize(sf::Vector2f(hamza2k21, height));
        shape.setFillColor(sf::Color::Blue);
        shape.setPosition(200.f, screenHeight-height);
		jumping = false;
        jumpTimeElapsed = 0;
    }
    void move(const sf::Vector2f& velocity) {
        shape.move(velocity);
    }
    void draw(sf::RenderWindow& window) {
        window.draw(shape);
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
				move(Vector2f(0, jumpDistance * dt / jumpDuration)); 
			}
            else {
                jumping = false;
                jumpTimeElapsed = 0;
            }
        }
        else {
            jumpTimeElapsed += dt;
            if (jumpTimeElapsed < jumpDuration) {
                // Simulate gravity (simple linear fall)
                move(Vector2f(0, -jumpDistance * dt / jumpDuration));  
            }
            else {
                jumping = true;
                jumpTimeElapsed = 0;
            }
        }
    }

	Vector2f getPosition() {
		Vector2f pos = shape.getPosition();
        return pos;
	}
};

int main() {
    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "SFML Bouncing Ball");

    Character c1;
    Clock clock;

    while (window.isOpen()) {
        Event event;
        float dt = clock.restart().asSeconds();
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

    return 0;
}
