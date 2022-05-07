#include "client.hpp"
#include <sys/select.h>
#include <ctime>
#include <string.h>
#include <thread>

Client::Client()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Error socket: " << errno << std::endl;
        exit(1);
    }

    struct sockaddr_in addr = *(new sockaddr_in);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP);

    connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
}

void Client::setup()
{
    std::cout << "Your name: ";
    std::cin >> username;
}

void Client::start()
{
    _send(std::string("/join ") + username);
    std::thread thd(&Client::receive, this);
    std::thread thd2(&Client::waitEnterMessage, this);
    thd.join();
    thd2.join();
}

void Client::receive()
{
    while (isLoop)
    {
        char length[4];
        recv(sockfd, length, 4, 0);
        int size = 0;
        for (int i = 3; i >= 0; i--)
        {
            size *= 16;
            if (length[i] < '0' || '9' < length[i])
                size += length[i] - 'a';
            else
                size += length[i] - '0';
        }
        char *str = (char *)malloc(size);
        recv(sockfd, str, size, 0);
        if (commandCheck(std::string(str)))
            std::cout << str << std::endl;
    }
}

std::string Client::createCommand(std::string str)
{
    if (str == "/quit")
    {
        str = str + " " + username;
        _send(str);
        close(sockfd);
        str = "";
        isLoop = false;
    }

    return str;
}

bool Client::commandCheck(std::string str)
{
    if (str[0] != '/')
        return true;

    if (str == "/close")
    {
        isLoop = false;
        close(sockfd);
    }

    return false;
}

std::string Client::encode(std::string str)
{
    if (str.size() == 0)
        return "";
    if (str[0] == '/')
        return createCommand(str);
    str = username + " > " + str;
    return str;
}

void Client::waitEnterMessage()
{
    while (isLoop)
    {
        std::string str;
        std::getline(std::cin, str);
        _send(encode(str));
    }
}

void Client::_send(std::string str)
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
    send(sockfd, length, 4, 0);
    send(sockfd, str.c_str(), str.size(), 0);
}