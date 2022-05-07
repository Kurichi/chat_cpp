#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

class Client
{
private:
    const uint16_t PORT = 12345;
    const char *IP = "127.0.0.1";
    int sockfd;
    struct sockaddr_in addr = *(new sockaddr_in);
    bool isConnected = false;
    std::string username;
    bool isLoop = true;

public:
    Client();

    void setup();

    void start();

    void receive();

    std::string createCommand(std::string str);

    std::string encode(std::string str);

    void waitEnterMessage();

    void _send(std::string str);

    bool commandCheck(std::string str);
};