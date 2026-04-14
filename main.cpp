#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <vector>

void handle_client(int client_socket) {
    char buffer[1024] = {0};

    std::string welcome = "Welcome to chat. Write something,\n";
    send(client_socket, welcome.c_str(), welcome.length(),0);

    int valread = recv(client_socket, buffer, 1024, 0);
    if (valread > 0) {
        std::cout << "Client say: "<<buffer<<std::endl;
    }

    std::cout << "Closing connection to client.\n";
    close(client_socket);
}

int main() {

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0 ) {
        perror("Error creating socket");
        return 1;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)& address, sizeof(address)) < 0) {
        perror("Error binding socket");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 3) < 0) {
        perror("Error listening socket");
        close(server_fd);
        return 1;
    }

    std::cout << "Server started. Waiting for connection at port 8080\n";

    while (true) {
        sockaddr_in client_address{};
        socklen_t addrlen = sizeof(client_address);

        int client_socket = accept(server_fd, (struct sockaddr *)& client_address, &addrlen);

        if (client_socket < 0) {
            perror("Error accepting connection");
            continue;
        }

        std::cout << "New client connected: " << client_socket << std::endl;

        std::thread (handle_client, client_socket).detach();
    }

    return 0;
}
