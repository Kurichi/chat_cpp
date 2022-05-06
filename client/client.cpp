#include "client.hpp"
#include <sys/select.h>
#include <ctime>
#include <string.h>

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
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
}

void Client::setup()
{
    std::cout << "Your name: ";
    std::cin >> username;
    char str[1024] = "/join ";
    strcat(str, username.c_str());
    send(sockfd, str, 1024, 0);
}

void Client::start()
{
    std::thread thd(&Client::receive, this);
    std::thread thd2(&Client::_send, this);
    thd.join();
    thd2.join();
}

void Client::receive()
{
    while (true)
    {
        char str[1024];
        recv(sockfd, str, 1024, 0);
        std::cout << str << std::endl;
    }
}

char *Client::createCommand(char *msg)
{
    char *str = new char[1024];
    if (!strcmp(msg, "/quit"))
    {
        strcpy(str, "/quit ");
        strcat(str, username.c_str());
    }

    return str;
}

char *Client::encode(char *msg)
{
    if (*msg == '/')
        return createCommand(msg);
    char *str = new char[1024];
    strcpy(str, username.c_str());
    strcat(str, " > ");
    strcat(str, msg);
    return str;
}

void Client::_send()
{
    while (true)
    {
        char str[1024];
        std::cin.getline(str, sizeof(str));
        send(sockfd, encode(str), 1024, 0);
    }
}