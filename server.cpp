#include "server.h"

bool Server::start()
{
    if (!isListen)
    {
        std::cerr << "Error start: not listenable" << std::endl;
        return false;
    }

    while (true)
    {
        struct sockaddr_in get_addr;
        socklen_t len = sizeof(struct sockaddr_in);
        int connect = accept(sockfd, (struct sockaddr *)&get_addr, &len);

        if (connect < 0)
        {
            std::cerr << "Error accept: " << errno;
            exit(1);
        }

        connList.push_back(connect);
        std::thread th(waitReceive, connect);
        th.join();
    };
}

void Server::waitReceive(const int connect)
{
    while (true)
    {
        char str[1024];
        recv(connect, str, 1024, 0);

        for (int conn : connList)
            send(conn, str, 1024, 0);
    }
}

int main()
{

    return 0;
}