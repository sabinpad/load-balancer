#ifndef IPC_UTILS_HPP
#define IPC_UTILS_HPP

#include <string>

#include <ipc.h>


int listening_unix_socket(const char *unix_address, int n);

#endif /* ipc_utils.hpp */
