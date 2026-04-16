#include "network/ChatClient.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

ChatClient::ChatClient() : sock(-1), isRunning(false){}

ChatClient::~ChatClient() {
    isRunning = false;
    if (sock != -1) close(sock);
    if (receiveThread.joinable()) receiveThread.join();
}

bool ChatClient::connectToServer(const std::string& ip, int port) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)return false;

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        return false;
    }

    isRunning =true;

    receiveThread = std::thread(&ChatClient::receiveLoop, this);
    return true;
}

void ChatClient::sendMessage(const std::string& message) const {
    if (isRunning && !message.empty()) {
        send(sock, message.c_str(), message.length(), 0);
    }
}

void ChatClient::receiveLoop() {
    char buffer[1024];
    while (isRunning) {
        memset(buffer, 0, 1024);
        int bytes = recv(sock, buffer, 1024, 0);

        if (bytes <= 0) {
            isRunning = false;
            break;
        }

        std::lock_guard<std::mutex> lock(msgMutex);
        messageHistory.push_back(std::string(buffer,  bytes));
    }
}

std::vector<std::string> ChatClient::getMessages() {
    std::lock_guard<std::mutex> lock(msgMutex);
    return messageHistory;
}

