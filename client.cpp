#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <string>

void receive_messages(int client_socket) {
    char buffer[1024];

    while (true) {
        int bytes_received = recv(client_socket, buffer, 1024, 0);
        if (bytes_received <= 0) {
            std::cout <<"\n[System] Losing connection.\n";
            break;
        }
        std::cout << "\n" << std::string(buffer, bytes_received) <<std::endl;
        std::cout << "> " << std::flush;
    }
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error to connect");
        return 1;
    }

    std::cout << "Connect to server. Write your messages.\n";

    std::thread(receive_messages, sock).detach();

    std::string message;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, message);

        if (message == "/exit") break;

        send(sock, message.c_str(), message.length(), 0);
    }
    close(sock);
    return 0;
}