#include "client.cpp"

int main()
{
    Client *client = new Client;
    client->setup();
    client->start();

    return 0;
}