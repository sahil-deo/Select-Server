#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define SERVER_LISTEN_BACKLOG 5
#define PORT 5555

void printMessage(int fd, std::string message)
{
    std::cout << "[" << fd << "]: " << message << std::endl;
}

int setupServer(const uint16_t port)
{
    // create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // set opt

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (server_fd < 0)
    {
        perror("socker() failure\n");
        exit(EXIT_FAILURE);
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    // bind socket
    if (bind(server_fd, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind() failure\n");
        exit(EXIT_FAILURE);
    }

    // listen socket
    if (listen(server_fd, SERVER_LISTEN_BACKLOG) < 0)
    {
        perror("listen() failure\n");
        exit(EXIT_FAILURE);
    }

    return server_fd;
}

int acceptConnection(int server_fd)
{
    int client_fd = accept(server_fd, nullptr, nullptr);
    if (client_fd < 0)
    {
        perror("accept() failure\n");
    }
    return client_fd;
}

bool handleConnection(int client_fd) // return true if client closed or error
{
    char buf[1024];
    ssize_t bytesRead = recv(client_fd, buf, sizeof(buf), 0);

    if (bytesRead <= 0) // client is closed
    {
        close(client_fd);
        printMessage(client_fd, "closed");
        return true;
    }

    printMessage(client_fd, std::string(buf, bytesRead));

    ssize_t totalBytesSent{};

    while (totalBytesSent < bytesRead)
    {
        ssize_t bytesSent = send(client_fd, buf + totalBytesSent, bytesRead - totalBytesSent, 0);
        if (bytesSent <= 0)
        {
            close(client_fd);
            printMessage(client_fd, "closed");
            return true;
        }
        totalBytesSent += bytesSent;
    }

    return false; // return false as client still open
}

int main()
{
    // set up server
    int server_fd = setupServer(PORT);

    // set up select
    fd_set currentClients, readyClients;
    FD_ZERO(&currentClients);

    // add server fd in set for accepting connections
    FD_SET(server_fd, &currentClients);

    while (true)
    {
        // use a temp set because select is destructive
        readyClients = currentClients;

        // select
        if (select(FD_SETSIZE, &readyClients, NULL, NULL, NULL) < 0)
        {
            perror("select() failure\n");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < FD_SETSIZE; i++)
        {
            // skip fd if not set
            if (!FD_ISSET(i, &readyClients))
                continue;
            if (i == server_fd)
            {
                // accept incoming connections
                int client_fd = acceptConnection(server_fd);

                if (client_fd < 0) // if client fd is less than 0, accept failed
                {
                    continue;
                }
                // add fd to fd set for select
                FD_SET(client_fd, &currentClients);
            }
            else
            {
                // handle existing connections
                if (handleConnection(i)) // returns true if client closed
                {
                    // remove client fd from select fd set
                    FD_CLR(i, &currentClients);
                }
            }
        }
    } // while

    close(server_fd);
}