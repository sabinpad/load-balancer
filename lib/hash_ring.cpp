#include "hash_ring.hpp"

#define SERVERS_DEFAULT_LIMIT 4
#define SERVERS_MAX_LIMIT 1024
#define REPLICAS_NR 2


using RingTag = std::uint16_t;
using RingLabel = RingId;

RingId endpoint_to_ring_id(Endpoint endpoint)
{
    RingId id;

    id = endpoint.address;
    id <<= 16;
    id ^= endpoint.port;

    return id;
}

RingLabel ring_id_to_ring_label(RingId id, RingTag tag)
{
    return ((RingId)tag << 48) ^ id;
}

std::size_t ring_label_hash(RingLabel id)
{
    std::size_t ulong_a = id;

	ulong_a = ((ulong_a >> 16u) ^ ulong_a) * 0x45d9f3b;
	ulong_a = ((ulong_a >> 16u) ^ ulong_a) * 0x45d9f3b;
	ulong_a = (ulong_a >> 16u) ^ ulong_a;

	return ulong_a;
}

Endpoint::Endpoint(): address(0), port(0) {}
Endpoint::Endpoint(in_addr_t address, in_port_t port): address(address), port(port) {}

bool Endpoint::operator==(const Endpoint &other) const
{
    return this->address == other.address && this->port == other.port;
}

std::size_t std::hash<Endpoint>::operator()(const Endpoint &endpoint) const
{
    std::size_t ulong_a = endpoint.address;

    ulong_a = ((ulong_a >> 16u) ^ ulong_a) * 0x45d9f3b;
    ulong_a = (ulong_a >> 16u) ^ endpoint.port;
    ulong_a = ((ulong_a >> 16u) ^ ulong_a) * 0x45d9f3b;
    ulong_a = (ulong_a >> 16u) ^ ulong_a;

    return ulong_a;
}

HashNode::HashNode(RingId id, std::size_t label_hash): id(id), label_hash(label_hash) {}

HashRing::HashRing(): limit(SERVERS_DEFAULT_LIMIT), n(0) {}

HashRing::HashRing(int limit)
{
    if (limit > SERVERS_MAX_LIMIT)
        limit = SERVERS_MAX_LIMIT;

    this->limit = limit;
    this->n = 0;
}

int HashRing::find_smallest_label_hash_bigger(std::size_t id_hash)
{
    int l, m, r;
    std::vector<HashNode> &nodes = this->nodes;

    l = 0;
    r = nodes.size() - 1;
    m = l + ((r - l) / 2);

    while (l < r && nodes[m].label_hash != id_hash) {
        if (nodes[m].label_hash < id_hash)
            l = m + 1;
        else
            r = m;

        m = l + ((r - l) / 2);
    }

    return m;
}

int HashRing::add_endpoint(Endpoint &server_endpoint)
{
    RingId id;
    auto &endpoints = this->endpoints;
    auto &nodes = this->nodes;

    if (this->n >= this->limit)
        return -1;

    id = endpoint_to_ring_id(server_endpoint);

    if (endpoints.find(id) != endpoints.end())
        return - 1;

    for (int i = 0; i < REPLICAS_NR; ++i) {
        std::size_t label_hash = ring_label_hash(ring_id_to_ring_label(id, i));

        if (nodes.empty() || label_hash >= (*(--nodes.end())).label_hash)
            nodes.push_back(HashNode(id, label_hash));
        else
            nodes.insert(nodes.begin() + this->find_smallest_label_hash_bigger(label_hash), HashNode(id, label_hash));
    }

    endpoints[id] = server_endpoint;

    ++this->n;

    return 0;
}

int HashRing::remove_endpoint(Endpoint &server_endpoint)
{
    RingId id;
    auto endpoints = this->endpoints;

    if (this->n <= 0)
        return -1;

    id = endpoint_to_ring_id(server_endpoint);

    if (endpoints.find(id) == endpoints.end())
        return -1;

    for (auto iter = nodes.begin(); iter != nodes.end(); ++iter)
        if ((*iter).id == id) {
            nodes.erase(iter);
            --iter;
        }

    endpoints.erase(id);

    --this->n;

    return 0;
}

const Endpoint &HashRing::distribute(Endpoint &client_endpoint)
{
    unsigned int index;
    RingId id;

    id = endpoint_to_ring_id(client_endpoint);

    index = this->find_smallest_label_hash_bigger(ring_label_hash(ring_id_to_ring_label(id, 0)));

    return this->endpoints[this->nodes[index].id];
}

std::vector<Endpoint> HashRing::get_endpoints()
{
    std::vector<Endpoint> endpoints(this->endpoints.size());

    auto vec_iter = endpoints.begin();

    for (auto &[id, endpoint] : this->endpoints) {
        *vec_iter = endpoint;
    
        ++vec_iter;
    }

    return endpoints;
}
