#include <string.h>

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
    std::thread thdcomm(&Server::waitCommand, this);
    thdconn.join();
    thdrecv.join();
    thdcomm.join();
}

void Server::waitConnection()
{
    while (isLoop)
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
    while (isLoop)
    {
        sleep(0.5);
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

                if (*str == '/')
                    userCommand(str);
                else
                    for (int conn : connList)
                        send(conn, str, 1024, 0);
            }
        }
    }
}

void Server::waitCommand()
{
    while (isLoop)
    {
        std::string str;
        std::cin >> str;
        command(str);
    }
}

void Server::userCommand(char *str)
{
    if (!strncmp(str, "/join ", 6))
    {
        for (int i = 0; i < sizeof("/join "); i++)
            str++;
        strcat(str, "joined.");

        for (int conn : connList)
            send(conn, str, 1024, 0);
    }
}

void Server::command(std::string str)
{
    if (str == "/stop" || str == "/quit")
    {
        for (int conn : connList)
            close(conn);
        isLoop = false;
        close(sockfd);
    }
    else if (str == "/list")
    {
        for (int conn : connList)
            std::cout << conn << std::endl;
    }
    else if (str == "/kill")
    {
        std::cin >> str;
        if (str == "all")
        {
            for (int conn : connList)
                close(conn);
            std::cout << "kill all" << std::endl;
        }
        else
        {
            int connect = atoi(str.c_str());
            for (int conn : connList)
            {
                if (connect == conn)
                {
                    close(conn);
                    std::cout << "kill " << connect << std::endl;
                    return;
                }
            }
            std::cout << "Error" << std::endl;
        }
    }
}

void Server::command(char *str)
{
    std::string s;
    while (*str != '\0')
        s.push_back(*str++);
    command(s);
}
