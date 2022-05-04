#include "client.hpp"
#include <sys/select.h>
#include <ctime>

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

const char *Client::encode(std::string t, std::string msg)
{
    std::string message;
    if (t == "command")
        message = username + " " + msg + "...";
    else
        message = username + " : " + msg;
    return message.c_str();
}

void Client::_send()
{
    while (true)
    {
        std::string str;
        std::cin >> str;
        const char *msg;
        if (str[0] == '/')
            msg = encode("command", str);
        else
            msg = encode("text", str);
        send(sockfd, msg, 1024, 0);
    }
}