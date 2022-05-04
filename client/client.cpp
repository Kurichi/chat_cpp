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
    int fd = 0;
    fd_set rfds;
    struct timeval tv;
    int retval;

    while (true)
    {
        char str[1024];
        recv(sockfd, str, 1024, 0);
        std::cout << str << std::endl;
        //     FD_ZERO(&rfds);
        // FD_SET(fd, &rfds);
        // tv.tv_sec = 0.5;
        // tv.tv_usec = 500000;
        // retval = select(fd + 1, &rfds, NULL, NULL, &tv);

        // if (retval < 0)
        // {
        // std::cerr << "Error select: " << errno << std::endl;
        // }
        // else if (retval > 0)
        // {
        // char str[1024];
        // recv(sockfd, str, 1024, 0);
        // std::cout << str << std::endl;
        // }
        // sleep(0.1);
    }
}

char *Client::encode(std::string t, std::string msg)
{
    std::string message;
    message = "{\"username\":\"" + username + "\",";
    message += "\"command-type\":\"" + t + "\",";
    message += "\"message\":\"" + msg + "\"}\0";

    return (message.c_str());
}

std::string Client::decode(char *msg)
{
    std::string name, comType, message;
}

void Client::_send()
{
    int fd = 0;
    fd_set rfds;
    struct timeval tv;
    int retval;
    while (true)
    {
        // FD_ZERO(&rfds);
        // FD_SET(fd, &rfds);
        // tv.tv_sec = 0.5;
        // tv.tv_usec = 50000;
        // retval = select(fd + 1, &rfds, NULL, NULL, &tv);
        // if (retval < 0)
        //{
        // std::cerr << "Error select: " << errno << std::endl;
        //}
        // else if (retval > 0)
        //{
        char str[1024];
        std::cin >> str;
        std::cout << str << std::endl;
        send(sockfd, str, 1024, 0);
        //}
    }
}