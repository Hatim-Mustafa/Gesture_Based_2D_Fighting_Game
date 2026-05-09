#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

class SocketServer {
private:
    SOCKET serverSocket;
    SOCKET clientSocket;
    int port;
    string lastCommand;

public:
    SocketServer(int p = 5000) : port(p), serverSocket(INVALID_SOCKET), clientSocket(INVALID_SOCKET), lastCommand("") {}

    bool initialize() {
        WSADATA wsaData;
        
        // Initialize Winsock
        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            cerr << "WSAStartup failed: " << iResult << endl;
            return false;
        }

        // Create a SOCKET for connecting to server
        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == INVALID_SOCKET) {
            cerr << "socket failed with error: " << WSAGetLastError() << endl;
            WSACleanup();
            return false;
        }

        // Setup the TCP listening socket
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        serverAddr.sin_port = htons(port);

        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            cerr << "bind failed with error: " << WSAGetLastError() << endl;
            closesocket(serverSocket);
            WSACleanup();
            return false;
        }

        // Listen for incoming connections
        if (listen(serverSocket, 1) == SOCKET_ERROR) {
            cerr << "listen failed with error: " << WSAGetLastError() << endl;
            closesocket(serverSocket);
            WSACleanup();
            return false;
        }

        cout << "Server listening on port " << port << "..." << endl;
        return true;
    }

    bool acceptConnection() {
        clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            cerr << "accept failed with error: " << WSAGetLastError() << endl;
            closesocket(serverSocket);
            WSACleanup();
            return false;
        }

        cout << "Client connected!" << endl;
        return true;
    }

    void startListening() {
        char recvbuf[512] = {0};
        int iResult;

        while (true) {
            iResult = recv(clientSocket, recvbuf, 512, 0);
            if (iResult > 0) {
                // Null-terminate the received string
                recvbuf[iResult] = '\0';
                lastCommand = string(recvbuf);
                cout << "Received command: " << lastCommand << endl;
            }
            else if (iResult == 0) {
                cout << "Client disconnected." << endl;
                break;
            }
            else {
                cerr << "recv failed with error: " << WSAGetLastError() << endl;
                break;
            }
        }
    }

    string getLastCommand() const {
        return lastCommand;
    }

    void cleanup() {
        if (clientSocket != INVALID_SOCKET)
            closesocket(clientSocket);
        if (serverSocket != INVALID_SOCKET)
            closesocket(serverSocket);
        WSACleanup();
    }

    ~SocketServer() {
        cleanup();
    }
};
