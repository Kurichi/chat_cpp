#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <thread>
#include <map>

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

    void userCommand(int connect, std::string str);

    void command(std::string str);
    void command(char *str);

    void _send(int connect, std::string str);
};