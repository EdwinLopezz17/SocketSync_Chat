#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#include "network/ChatClient.hpp"

int main() {
    ChatClient client;

    if (!client.connectToServer("127.0.0.1", 8080)) {
        std::cerr << "Error: Cannot connect to the server.\n";
        return 1;
    }

    std::cout << "Connect to serve. Write /exit to leave.\n";

    std::thread displayThread([&client]() {
        size_t lastCount = 0;
        while (client.isConnected()) {
            auto msgs = client.getMessages();
            if (msgs.size() > lastCount) {
                for (size_t i = lastCount; i < msgs.size(); i++) {
                    std::cout << "\n" << msgs[i] << "\n" << std::flush;
                }
                lastCount = msgs.size();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    std::string input;
    while (client.isConnected()) {
        std::getline(std::cin, input);
        if (input == "/exit") break;
        client.sendMessage(input);
    }

    displayThread.join();
    return 0;
}
