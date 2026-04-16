#ifndef SOCKETSYNC_CHAT_CHATCLIENT_H
#define SOCKETSYNC_CHAT_CHATCLIENT_H

#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>

class ChatClient {
private:
    int sock;
    std::vector<std::string> messageHistory;
    std::mutex msgMutex;
    std::thread receiveThread;
    std::atomic<bool> isRunning;

    void receiveLoop();

public:
    ChatClient();
    ~ChatClient();

    bool connectToServer(const std::string&ip, int port);
    void sendMessage(const std::string& message) const;

    std::vector<std::string> getMessages();
    bool isConnected() const {return isRunning;}
};

#endif

