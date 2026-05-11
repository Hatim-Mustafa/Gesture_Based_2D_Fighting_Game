#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <iostream>
#include "socket_server.cpp" 
#include "game_state_sender.cpp"

using namespace sf;
using namespace std;

#define height 250.f
#define hamza2k21 60.f
#define screenWidth 1400.f
#define screenHeight 800.f
#define attackState "ATTACK"
#define idleState "IDLE"
#define fmoveState "FORWARD"
#define bmoveState "BACK"
#define jumpState "JUMP"
#define shieldState "SHIELD"

class Character {
protected:
    bool jumping = false;
    bool jumpStarted = false;
    float jumpDuration = 0.5f;
    float jumpTimeElapsed = 0;
    float jumpDistance = 150.f;
    float moveDistance = 70.f;
    float moveDuration = 0.25f;
    float moveTimeElapsed = 0;
	float shieldDuration = 0.5f;
	float shieldTimeElapsed = 0;
	float attackDuration = 0.35f;
	float attackTimeElapsed = 0;

    int health = 100;
    string state = idleState;
    bool movingLeft = false;
    bool movingRight = false;
    bool shielding = false;
    bool attacking = false;
    bool attackdone = false;

public:
    int framecycle = 7;
	int frameWidth = 64;
	int frameHeight = 64;

    virtual ~Character() {}

    virtual void draw(sf::RenderWindow& window) = 0;
    virtual Vector2f getPosition() = 0;
    virtual void move(const sf::Vector2f& velocity) = 0;

    bool isJumping() { 
        return jumping || jumpStarted; 
    }

    bool isMoving() { 
        return movingLeft || movingRight; 
    }

    bool isShielding () {
        return shielding;
    }
    
	bool isAttacking() {
		return attacking;
	}

    string getState() { 
        return state; 
    }

    int getHealth() { 
        return health; 
    }

    void takeDamage(int damage) {
        health -= damage;
        if (health < 0) health = 0;
    }

    void handleInput(String action, float dt) {
        if (action == "left") {
            if (isMoving()) moveTimeElapsed = 0; // Reset move time to allow immediate direction change
            movingLeft = true;
            movingRight = false;
            shielding = false;
            state = bmoveState;
            handleMove(dt);
        }
        else if (action == "right") {
            if (isMoving()) moveTimeElapsed = 0;
            movingRight = true;
            movingLeft = false;
            shielding = false;
            state = fmoveState;
            handleMove(dt);
        }
        else if (action == "jump") {
            state = jumpState;
            shielding = false;
            handleJump(dt);
        }
    }

    //once jump is initiated, use time to simulate gravity and bring Player back down
    void handleJump(float dt) {
        if (jumping) {
            jumpTimeElapsed += dt;
            if (getPosition().y < (screenHeight - height)) {
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
                jumpStarted = true;
            }
            else {
                if (jumpTimeElapsed >= jumpDuration) {
                    jumping = true;
                    jumpStarted = false;
                    jumpTimeElapsed = 0;
                }
            }
        }
    }

    virtual void handleMove(float dt) {
        if (movingLeft) {
            move(Vector2f(-moveDistance * dt / moveDuration, 0));
            moveTimeElapsed += dt;
            if (moveTimeElapsed >= moveDuration) {
                movingLeft = false;
                moveTimeElapsed = 0;
                state = idleState;
            }
        }
        else if (movingRight) {
            move(Vector2f(moveDistance * dt / moveDuration, 0));
            moveTimeElapsed += dt;
            if (moveTimeElapsed >= moveDuration) {
                movingRight = false;
                moveTimeElapsed = 0;
                state = idleState;
            }
        }
    }

    virtual void handleShield(float dt) {
		state = shieldState;
		shielding = true;
		shieldTimeElapsed += dt;
        if (shieldTimeElapsed >= shieldDuration) {
            shieldTimeElapsed = 0;
            state = idleState;
			shielding = false;
        }
    }

    virtual void performAttack(Character& target, float dt) {
        shielding = false;
		attacking = true;
        state = attackState;
		attackTimeElapsed += dt;
        Vector2f tPos = target.getPosition();
        Vector2f myPos = getPosition();

        if (target.getState() != shieldState) {
            if (abs(myPos.x - tPos.x) < 200.f && abs(myPos.y - tPos.y) < 60.f 
                && attackTimeElapsed > attackDuration/2 && !attackdone) {
                attackdone = true;
                target.takeDamage(10);
                cout << "Hit registered! Target health: " << target.getHealth() << endl;
            }
        }

        if (attackTimeElapsed >= attackDuration) {
            attackTimeElapsed = 0;
            state = idleState;
            attacking = false;
            attackdone = false;
        }
    }
};

class Player : public Character {
public:
    Sprite PlayerSprite;
    Texture charTexture;
    Texture walkTexture;
	Texture attack1Texture;
	Texture attack2Texture;
	Texture attack3Texture;
    int spriteSpeed = 100;
    int attackCount = 0;
	int kuchtouhai = 96;
	int currentFrame = 0;

    Player() {
        charTexture.loadFromFile("with_outline/IDLE.png"); 
        walkTexture.loadFromFile("with_outline/RUN.png");
		attack1Texture.loadFromFile("with_outline/ATTACK 1.png");
        attack2Texture.loadFromFile("with_outline/ATTACK 2.png");
        attack3Texture.loadFromFile("with_outline/ATTACK 3.png");
        PlayerSprite.setTexture(charTexture);
        PlayerSprite.setPosition(200.f, screenHeight - height);
        PlayerSprite.setTextureRect(IntRect(0, 0, 64, 64));
        PlayerSprite.setScale(3.0f, 3.0f);
    }

    void draw(sf::RenderWindow& window) override { 
        window.draw(PlayerSprite); 
    }

    Vector2f getPosition() override { 
        return PlayerSprite.getPosition(); 
    }

    void move(const sf::Vector2f& velocity) override { 
        PlayerSprite.move(velocity); 
    }

    void handleMove(float dt) override {
        if (movingLeft) {
            PlayerSprite.setScale(-3.f, 3.f);
            PlayerSprite.setOrigin(100.f, 0.f);

            if (!attacking) {
                PlayerSprite.setTexture(walkTexture);
                framecycle = 8;
                spriteSpeed = 60; // ms per frame for running
            }
        }
        else if (movingRight) {
            PlayerSprite.setScale(3.f, 3.f);
            PlayerSprite.setOrigin(0.f, 0.f);

            if (!attacking) {
                PlayerSprite.setTexture(walkTexture);
                framecycle = 8;
                spriteSpeed = 60; // ms per frame for running
            }
        }

        Character::handleMove(dt); 

        if (state == idleState) {
            PlayerSprite.setTexture(charTexture);
            framecycle = 7;
			spriteSpeed = 100; // ms per frame for idle
        }
    }

	void performAttack(Character& target, float dt) override {
        if (!attacking) {
            frameWidth = 90;
            //kuchtouhai = 106;
			currentFrame = 0; // Reset to first frame of attack animation
            if (attackCount == 0) {
                framecycle = 6;
                spriteSpeed = 100; // ms per frame for attack 1
                PlayerSprite.setTexture(attack1Texture);
                attackCount++;

            }
            else if (attackCount == 1) {
                framecycle = 5;
                spriteSpeed = 100; // ms per frame for attack 2
                PlayerSprite.setTexture(attack2Texture);
                attackCount++;
            }
            else if (attackCount == 2) {
                framecycle = 6;
                spriteSpeed = 100; // ms per frame for attack 3
                PlayerSprite.setTexture(attack3Texture);
                attackCount = 0;
            }
        }
        

        Character::performAttack(target, dt);

        if (state == idleState) {
            PlayerSprite.setTexture(charTexture);
            framecycle = 7;
			frameWidth = 64;
			currentFrame = 0; // Reset to first frame of idle animation
			//kuchtouhai = 96;
            spriteSpeed = 100; // ms per frame for idle
        }

	}
	    
        
};

class Enemy : public Character {
    RectangleShape shape;
public:
    Enemy() {
        shape.setSize(sf::Vector2f(hamza2k21, height)); 
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(1000.f, screenHeight - height);
    }

    void draw(sf::RenderWindow& window) override { 
        window.draw(shape); 
    }

    Vector2f getPosition() override { 
        return shape.getPosition(); 
    }

    void move(const sf::Vector2f& velocity) override { 
        shape.move(velocity); 
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Fighting Game");
    
    Texture backgroundTexture;
    Sprite backgroundSprite;
    backgroundTexture.loadFromFile("E:/Background1.png"); 
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setPosition(0.f, 0.f);

    Player c1;
    Enemy e1;
    Clock clock;
    Clock animationClock;
    float dt;

    auto onGesture = [&c1, &dt, &e1](int player, const string& gesture) {
        if (player == 1) {
            if (gesture == "FORWARD") c1.handleInput("right", dt);
            else if (gesture == "BACK") c1.handleInput("left", dt);
            else if (gesture == "JUMP" && !c1.isJumping()) c1.handleInput("jump", dt);
            else if (gesture == "ATTACK" && !c1.isJumping()) c1.performAttack(e1,dt);
			else if (gesture == "SHIELD") c1.handleShield(dt);
        }
        if (player == 0) {
            if (gesture == "FORWARD") e1.handleInput("right", dt);
            else if (gesture == "BACK") e1.handleInput("left", dt);
            else if (gesture == "JUMP" && !e1.isJumping()) e1.handleInput("jump", dt);
            else if (gesture == "ATTACK" && !e1.isJumping()) e1.performAttack(c1, dt);
            else if (gesture == "SHIELD") e1.handleShield(dt);
        }
    };

    GestureReceiver recv(5005, onGesture);
    GameStateSender stateTx("127.0.0.1", 5006);

    while (window.isOpen()) {
        Event event;
        dt = clock.restart().asSeconds();
        recv.poll();

        stateTx.send(
            { c1.getPosition().x, c1.getPosition().y, c1.getState(), c1.getHealth() },
            { e1.getPosition().x, e1.getPosition().y, "IDLE", e1.getHealth() }
        );

        if (animationClock.getElapsedTime().asMilliseconds() > c1.spriteSpeed) { // Change frame every 100ms
            c1.currentFrame = (c1.currentFrame + 1) % c1.framecycle; // Cycle through 7 frames
            c1.PlayerSprite.setTextureRect(IntRect(c1.currentFrame * c1.kuchtouhai, 0, c1.frameWidth, c1.frameHeight));
            animationClock.restart();
        }

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Left) c1.handleInput("left", dt);
                else if (event.key.code == sf::Keyboard::Right) c1.handleInput("right", dt);
                else if (event.key.code == sf::Keyboard::Up && !c1.isJumping()) c1.handleInput("jump", dt);
                else if (event.key.code == sf::Keyboard::Space && !c1.isJumping()) c1.performAttack(e1, dt);
				else if (event.key.code == sf::Keyboard::Down) c1.handleShield(dt);
            }
        }

        if (c1.isJumping()) c1.handleJump(dt);
        if (c1.isMoving()) c1.handleMove(dt);
		if (c1.isShielding()) c1.handleShield(dt);
		if (c1.isAttacking()) c1.performAttack(e1, dt);


        window.clear(sf::Color::Black);
        window.draw(backgroundSprite);
        c1.draw(window);
        e1.draw(window);
        window.display();
    }
    return 0;
}