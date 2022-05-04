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
    addr.sin_port = htons(12345);
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

void Server::start()
{
    if (!isListen)
    {
        std::cerr << "Error start: not listenable" << std::endl;
        return;
    }

    std::cout << "Run server!" << std::endl;

    std::thread thdconn(&Server::waitConnection, this);
    std::thread thdrecv(&Server::waitReceive, this);
    thdconn.join();
    thdrecv.join();
}

void Server::waitConnection()
{
    while (true)
    {
        struct sockaddr_in *get_addr = new sockaddr_in;
        socklen_t len = sizeof(struct sockaddr_in);
        int connect = accept(sockfd, (struct sockaddr *)get_addr, &len);

        std::cout << "Connection: " << connect << std::endl;

        if (connect < 0)
        {
            std::cerr << "Error accept: " << errno << std::endl;
            exit(1);
        }

        connList.push_back(connect);
        char message[] = "You connected to server";
        send(connect, message, sizeof(message), 0);
    }
}

void Server::waitReceive()
{

    fd_set rfds;
    struct timeval tv;
    int retval;
    while (true)
    {
        for (int connect : connList)
        {
            FD_ZERO(&rfds);
            FD_SET(connect, &rfds);
            tv.tv_sec = 0.1;
            tv.tv_usec = 10000;

            retval = select(connect + 1, &rfds, NULL, NULL, &tv);

            if (retval < 0)
            {
                std::cerr << "Error select: " << errno << std::endl;
            }
            else if (retval > 0)
            {
                char str[1024];
                recv(connect, str, 1024, 0);
                std::cout << str << std::endl;

                for (int conn : connList)
                    send(conn, str, 1024, 0);
            }
        }
    }
}
