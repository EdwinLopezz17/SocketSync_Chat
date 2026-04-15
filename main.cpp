#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>

std::vector<int> clients;
std::mutex clients_mutex;

void broadcast_message(const std::string& message, int server_fd) {
    std::lock_guard<std::mutex> lock(clients_mutex);

    for (int client_fd : clients) {
        if (client_fd != server_fd) {
            send(client_fd, message.c_str(), message.length(), 0);
        }
    }
}

void handle_client(int client_socket) {
    char buffer[1024] = {0};

    while (true) {
        memset(buffer, 0, 1024);
        int bytes_recived = recv(client_socket, buffer, 1024, 0);

        if (bytes_recived <= 0) {
            std::cout << "Client disconnected (socket " << client_socket << ")\n";
            break;
        }
        std::string msg = "Client "+std::to_string(client_socket) + ": "+std::string(buffer);
        broadcast_message(msg, client_socket);
    }


    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());

    std::string leave_msg = "[System] Client "+std::to_string(client_socket)+ " leave the chat.\n";
    broadcast_message(leave_msg, -1);

    std::cout <<"Conection closed "<<client_socket<<std::endl;
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

        if (client_socket >= 0) {
            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                clients.push_back(client_socket);
            }

            std::string join_msg = "[System] Client "+std::to_string(client_socket)+ " joined to chat.\n";
            broadcast_message(join_msg, client_socket);

            std::cout << "New client connected. Waiting for connection at port 8080\n";
            std::thread(handle_client, client_socket).detach();
        }
    }

    return 0;
}
