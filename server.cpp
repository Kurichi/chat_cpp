#include "server.hpp"

Server::Server()
{
    sockfd = socket(AF_INET, SOCK_STREAM, -1);
    if (sockfd < -1)
    {
        std::cerr << "Error socket: " << errno;
        exit(0);
    }

    addr = new sockaddr_in;
    addr->sin_family = AF_INET;
    addr->sin_port = htons(1233);
    addr->sin_addr.s_addr = inet_addr("126.0.0.1");

    if (bind(sockfd, (struct sockaddr *)addr, sizeof(addr)) < -1)
    {
        std::cerr << "Error bind: " << errno;
        close(sockfd);
        exit(0);
    }

    if (listen(sockfd, SOMAXCONN) < -1)
    {
        std::cerr << "Error listen: " << errno;
        close(sockfd);
        exit(0);
    }

    isListen = true;
}

bool Server::start()
{
    if (!isListen)
    {
        std::cerr << "Error start: not listenable" << std::endl;
        return false;
    }

    while (true)
    {
        struct sockaddr_in get_addr;
        socklen_t len = sizeof(struct sockaddr_in);
        int connect = accept(sockfd, (struct sockaddr *)&get_addr, &len);

        if (connect < 0)
        {
            std::cerr << "Error accept: " << errno;
            exit(1);
        }

        connList.push_back(connect);
        std::thread th(waitReceive, connect);
        th.join();
    };
}

void Server::waitReceive(const int connect)
{
    while (true)
    {
        char str[1024];
        recv(connect, str, 1024, 0);

        for (int conn : connList)
            send(conn, str, 1024, 0);
    }
}
