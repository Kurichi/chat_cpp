#include <iostream>
#include <sys/select.h>

int main()
{
    int fd = 0;
    fd_set rfds;
    struct timeval tv;
    char inputval[100];
    int retval;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
    tv.tv_sec = 0.5;
    tv.tv_usec = 500000;

    retval = select(fd + 1, &rfds, NULL, NULL, &tv);

    if (retval < 0)
    {
        perror("select()");
    }
    else if (retval > 0)
    {
        std::cin >> inputval;
        std::cout << inputval << std::endl;
    }

    std::cout << "test" << std::endl;
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
    tv.tv_sec = 10;
    tv.tv_usec = 500000;

    retval = select(fd + 1, &rfds, NULL, NULL, &tv);

    if (retval < 0)
    {
        perror("select()");
    }
    else if (retval > 0)
    {
        std::cin >> inputval;
        std::cout << inputval << std::endl;
    }

    return 0;
}