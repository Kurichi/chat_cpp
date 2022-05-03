#include "server.hpp"

Server::Server()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Error socket: " << errno;
        exit(1);
    }

    struct sockaddr_in addr = *(new sockaddr_in);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "Error bind: " << errno << std::endl;
        close(sockfd);
        exit(1);
    }

    if (listen(sockfd, SOMAXCONN) < 0)
    {
        std::cerr << "Error listen: " << errno << std::endl;
        close(sockfd);
        exit(1);
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
        struct sockaddr_in *get_addr = new sockaddr_in;
        socklen_t len = sizeof(struct sockaddr_in);
        int connect = accept(sockfd, (struct sockaddr *)get_addr, &len);
        std::cout << connect << std::endl;

        if (connect < 0)
        {
            std::cerr << "Error accept: " << errno << std::endl;
            exit(1);
        }

        connList.push_back(connect);
        std::thread thd(&Server::waitReceive, this, connect);
        thd.join();
    }
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
