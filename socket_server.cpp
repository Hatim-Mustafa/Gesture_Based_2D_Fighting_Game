#include <SFML/Network.hpp>
#include <string>
#include <iostream>
#include <functional>
using namespace std;

class GestureReceiver {
public:
    // callback type: void(int player, const std::string& gesture)
    using Callback = std::function<void(int, const std::string&)>;

    GestureReceiver(unsigned short port, Callback cb)
        : m_callback(cb)
    {
        m_socket.bind(port);
        m_socket.setBlocking(false);   // non-blocking — call poll() every frame
    }

    void poll() {
        char buffer[1024];
        std::size_t received;
        sf::IpAddress sender;
        unsigned short senderPort;

        while (m_socket.receive(buffer, sizeof(buffer), received, sender, senderPort)
            == sf::Socket::Done)
        {
            // received = number of bytes actually received
            std::string json(buffer, received);   // construct from raw bytes
            int         player = parsePlayer(json);
            std::string gesture = parseGesture(json);
            if (!gesture.empty())
                m_callback(player, gesture);
        }
    }

private:
    sf::UdpSocket m_socket;
    Callback      m_callback;

    // Minimal JSON field extractor (no lib needed for {"player":1,"gesture":"ATTACK"})
    static std::string fieldStr(const std::string& json, const std::string& key) {
        auto k = json.find("\"" + key + "\"");
        if (k == std::string::npos) return "";
        auto colon = json.find(':', k);
        auto q1 = json.find('"', colon);
        auto q2 = json.find('"', q1 + 1);
        if (q1 == std::string::npos || q2 == std::string::npos) return "";
        return json.substr(q1 + 1, q2 - q1 - 1);
    }
    static int fieldInt(const std::string& json, const std::string& key) {
        auto k = json.find("\"" + key + "\"");
        if (k == std::string::npos) return -1;
        auto colon = json.find(':', k);
        return std::stoi(json.substr(colon + 1));
    }
    static std::string parseGesture(const std::string& j) { return fieldStr(j, "gesture"); }
    static int         parsePlayer(const std::string& j) { return fieldInt(j, "player"); }
};