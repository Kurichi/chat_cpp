#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <thread>

class Server
{
private:
    int sockfd;
    struct sockaddr_in *addr;
    bool isListen = false;
    std::vector<int> connList;

public:
    Server()
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
            std::cerr << "Error socket: " << errno;
            exit(1);
        }

        addr = new sockaddr_in;
        addr->sin_family = AF_INET;
        addr->sin_port = htons(1234);
        addr->sin_addr.s_addr = inet_addr("127.0.0.1");

        if (bind(sockfd, (struct sockaddr *)addr, sizeof(addr)) < 0)
        {
            std::cerr << "Error bind: " << errno;
            close(sockfd);
            exit(1);
        }

        if (listen(sockfd, SOMAXCONN) < 0)
        {
            std::cerr << "Error listen: " << errno;
            close(sockfd);
            exit(1);
        }

        isListen = true;
    }

    bool start();

    void waitReceive(const int connect);
};