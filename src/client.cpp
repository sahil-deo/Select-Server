#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5555
#define HOST "127.0.0.1"

int setupClient()
{
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0)
    {
        perror("socker() failure");
        exit(EXIT_FAILURE);
    }

    // set up addr
    sockaddr_in addr{};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, HOST, &addr.sin_addr) != 1) // load host address into addr
    {
        perror("invalid address\n");
        exit(EXIT_FAILURE);
    }

    if (connect(client_fd, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect() failure");
        exit(EXIT_FAILURE);
    }

    return client_fd;
}

bool handleSend(int client_fd)
{
    std::string message{};

    std::cout << "> ";
    std::getline(std::cin, message);

    if (message == "exit")
        return true;

    send(client_fd, message.c_str(), message.size(), 0);
    return false;
}

bool handleRecv(int client_fd)
{
    char buf[1024];

    // recv
    ssize_t bytesRead = recv(client_fd, buf, sizeof(buf), 0);

    // if bytes read is 0 then close connection
    if (bytesRead <= 0)
        return true;

    std::cout << "SERVER: " << std::string(buf, bytesRead) << std::endl;

    return false; // return false on no errors
}

int main()
{
    int client_fd = setupClient();
    while (true)
    {
        if (handleSend(client_fd)) // returns true if connection closed
            break;

        if (handleRecv(client_fd)) // returns true if connection closed
            break;
    } // while

    close(client_fd);
}