#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <thread>

class Client
{
private:
    int sockfd;
    bool isConnected = false;
    std::string username;

public:
    Client();

    void setup();

    void start();

    void receive();

    char *encode(std::string t, std::string msg);

    std::string decode(char *msg);

    void _send();
};