#include <unistd.h>

#include <string.h>

#include "utils/ipc_utils.hpp"


int listening_unix_socket(const char *unix_address, int n)
{
    int rc;
    int fd = -1;
    struct sockaddr_un addr_info;

    fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (fd == -1)
        return -1;

    memset(&addr_info, 0, sizeof(addr_info));
    addr_info.sun_family = AF_UNIX;
    addr_info.sun_path[0] = '\0';
    strcpy(addr_info.sun_path + 1, unix_address);
    
    rc = bind(fd, (const struct sockaddr *)&addr_info, sizeof(addr_info.sun_family) + 1 + strlen(unix_address));

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
