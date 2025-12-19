#ifndef HASH_RING_HPP
#define HASH_RING_HPP

#include <cstdint>
#include <netinet/in.h>

#include <vector>
#include <unordered_map>


using RingId = std::uint64_t;

struct Endpoint {
    in_addr_t address;
    in_port_t port;

    Endpoint();
    Endpoint(in_addr_t address, in_port_t port);

    bool operator==(const Endpoint &other) const;
};

namespace std {
    template <>
    struct hash<Endpoint> {
        std::size_t operator()(const Endpoint &endpoint) const;
    };
}

struct HashNode {
    RingId id;
    std::size_t label_hash;

    HashNode(RingId id, std::size_t label_hash);
};

class HashRing {
private:
    int limit;
    int n;

    std::unordered_map<RingId, Endpoint> endpoints;
    std::vector<HashNode> nodes;

public:
    HashRing();
    HashRing(int limit);

private:
    int find_smallest_label_hash_bigger(std::size_t id_hash);

public:
    int add_endpoint(Endpoint &server_endpoint);
    int remove_endpoint(Endpoint &server_endpoint);

    const Endpoint &distribute(Endpoint &client_endpoint);

    std::vector<Endpoint> get_endpoints();
};

#endif /* hash_ring.hpp */
