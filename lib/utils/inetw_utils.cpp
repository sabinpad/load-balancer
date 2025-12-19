#include <unistd.h>

#include <string.h>

#include "utils/inetw_utils.hpp"


int listening_inet_socket(in_addr_t inet_address, in_port_t inet_port, int n)
{
    int rc;
    int fd = -1;
    struct sockaddr_in addr_info;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd == -1)
        return -1;

    memset(&addr_info, 0, sizeof(addr_info));
    addr_info.sin_family = AF_INET;
    addr_info.sin_addr.s_addr = htonl(inet_address);
    addr_info.sin_port = htons(inet_port);

    rc = bind(fd, (const struct sockaddr *)&addr_info, sizeof(addr_info));

    if (rc == -1) {
        close(fd);

        return -1;
    }

    rc = listen(fd, n);

    if (rc == -1) {
        close(fd);

        return -1;
    }

    return fd;
}
