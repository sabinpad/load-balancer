#ifndef CONN_ENGINE_HPP
#define CONN_ENGINE_HPP

#include <netinet/in.h>

#include <unordered_set>
#include <unordered_map>

#include "hash_ring.hpp"


class ConnectionEngine {
private:
    int epoll_fd_;
    int listen_unix_socket_;
    int listen_inet_socket_;

    std::unordered_set<int> open_sockets_;
    std::unordered_map<int, int> forward_;

    int servers_limit_;

    HashRing hash_ring_;

public:
    ConnectionEngine();

private:
    void handle_ctl();
    void handle_connect();
    void handle_disconnect(int in_sock);
    void handle_traffic(int in_sock);

public:
    int setup(in_addr_t inet_address, in_port_t inet_port, int servers_limit);
    void run();
    void cleanup();
};

#endif /* conn_engine.hpp */
