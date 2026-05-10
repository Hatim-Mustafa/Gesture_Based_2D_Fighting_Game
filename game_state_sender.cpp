#include <SFML/Network.hpp>
#include <string>

class GameStateSender {
public:
    GameStateSender(const std::string& host, unsigned short port)
        : m_host(host), m_port(port)
    {
        m_socket.setBlocking(false);
    }

    struct PlayerState {
        float       x, y;
        std::string action;
        int         hp;
    };

    void send(const PlayerState& player, const PlayerState& enemy) {
        std::string payload = "{"
            "\"player\":{"
            "\"x\":" + std::to_string(player.x) + ","
            "\"y\":" + std::to_string(player.y) + ","
            "\"action\":\"" + player.action + "\","
            "\"hp\":" + std::to_string(player.hp) +
            "},"
            "\"enemy\":{"
            "\"x\":" + std::to_string(enemy.x) + ","
            "\"y\":" + std::to_string(enemy.y) + ","
            "\"action\":\"" + enemy.action + "\","
            "\"hp\":" + std::to_string(enemy.hp) +
            "}"
            "}";

        char buffer[512];
        std::size_t len = payload.copy(buffer, payload.size());
        m_socket.send(buffer, len, m_host, m_port);
    }

private:
    sf::UdpSocket  m_socket;
    sf::IpAddress  m_host;
    unsigned short m_port;
};