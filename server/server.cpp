#include <string.h>
#include <sstream>
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
    fd_set rfds;
    struct timeval tv;
    int retval;
    while (isLoop)
    {
        FD_ZERO(&rfds);
        FD_SET(sockfd, &rfds);
        tv.tv_sec = 1;
        tv.tv_usec = 100000;

        retval = select(sockfd + 1, &rfds, NULL, NULL, &tv);

        if (retval < 0)
        {
            std::cerr << "Error select: " << errno << std::endl;
        }
        else if (retval > 0)
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
            _send(connect, "You connected to server");
        }
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
                unsigned char length[4];
                recv(connect, length, 4, 0);
                int size = 0;
                for (int i = 3; i >= 0; i--)
                {
                    size *= 16;
                    if (length[i] < '0' || '9' < length[i])
                        size += length[i] - 'a';
                    else
                        size += length[i] - '0';
                }
                char *tmp = (char *)malloc(size);
                recv(connect, tmp, size, 0);
                std::string str(tmp);

                if (str[0] == '/')
                    userCommand(connect, str);
                else
                    for (int conn : connList)
                        _send(conn, str);
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

void Server::userCommand(int connect, std::string str)
{
    std::vector<std::string> v;
    std::string s;
    std::stringstream ss{str};
    while (getline(ss, s, ' '))
        v.push_back(s);
    if (v[0] == "/join")
    {
        str = v[1] + " joined.";

        for (int conn : connList)
            _send(conn, str);
    }
    else if (v[0] == "/quit")
    {
        for (int i = 0; i < (int)connList.size(); i++)
        {
            if (connect == connList[i])
            {
                connList.erase(connList.begin() + i);
                break;
            }
        }
        close(connect);
        str = v[1] + " quited...";

        for (int conn : connList)
            _send(conn, str);
    }
}

void Server::command(std::string str)
{
    if (str == "/stop" || str == "/quit")
    {
        for (int conn : connList)
        {
            _send(conn, "/close");
            close(conn);
        }
        close(sockfd);
        isLoop = false;
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
            {
                _send(conn, "/close");
                close(conn);
            }
            std::cout << "kill all" << std::endl;
        }
        else
        {
            int connect = atoi(str.c_str());
            for (int conn : connList)
            {
                if (connect == conn)
                {
                    _send(conn, "/close");
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

void Server::_send(int connect, std::string str)
{
    int tmp = str.size();
    if (tmp == 0)
        return;
    unsigned char length[4];
    for (int i = 0; i < 4; i++)
    {
        int val = tmp % 16;
        if (val < 10)
            length[i] = '0' + val;
        else
            length[i] = 'a' + val;
        tmp /= 16;
    }
    send(connect, length, 4, 0);
    send(connect, str.c_str(), str.size(), 0);
}