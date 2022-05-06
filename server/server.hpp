#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <thread>

class Server
{
private:
    int sockfd;
    bool isListen = false;
    std::vector<int> connList;
    bool isLoop = true;

public:
    Server();

    void start();

    void waitConnection();

    void waitReceive();

    void waitCommand();

    void command(std::string str);
    void command(char *str);
};