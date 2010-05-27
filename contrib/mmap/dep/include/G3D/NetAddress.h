#ifndef G3D_NETADDRESS_H
#define G3D_NETADDRESS_H

#include "G3D/platform.h"
#include "G3D/Table.h"

/** These control the version of Winsock used by G3D.
    Version 2.0 is standard for G3D 6.09 and later.
    Version 1.1 is standard for G3D 6.08 and earlier.
 */
#define G3D_WINSOCK_MAJOR_VERSION 2
#define G3D_WINSOCK_MINOR_VERSION 0

#ifdef G3D_WIN32
#   if (G3D_WINSOCK_MAJOR_VERSION == 2)
#       include <winsock2.h>
#   elif (G3D_WINSOCK_MAJOR_VERSION == 1)
#       include <winsock.h>
#   endif
#else
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <netinet/in.h>
#   ifndef SOCKADDR_IN
#       define SOCKADDR_IN struct sockaddr_in
#   endif
#   define SOCKET int
#endif

#include "G3D/g3dmath.h"

namespace G3D {

class NetAddress {
private:
    friend class NetworkDevice;
    friend class LightweightConduit;
    friend class ReliableConduit;

    /** Host byte order */
    void init(uint32 host, uint16 port);
    void init(const std::string& hostname, uint16 port);
    NetAddress(const SOCKADDR_IN& a);
    NetAddress(const struct in_addr& addr, uint16 port = 0);

    SOCKADDR_IN                 addr;

public:
    /**
     In host byte order
     */
    NetAddress(uint32 host, uint16 port = 0);

    /**
     Port is in host byte order.
     */
    NetAddress(const std::string& hostname, uint16 port);

    /**
    String must be in the form "hostname:port"
     */
    NetAddress(const std::string& hostnameAndPort);

    /**
     For use with a lightweight conduit.
     */
    static NetAddress broadcastAddress(uint16 port);

    NetAddress();

    void serialize(class BinaryOutput& b) const;
    void deserialize(class BinaryInput& b);

    /** Returns true if this is not an illegal address. */
    bool ok() const;

    /** Returns a value in host format */
    inline uint32 ip() const {
        return ntohl(addr.sin_addr.s_addr);
        //return ntohl(addr.sin_addr.S_un.S_addr);
    }

    inline uint16 port() const {
        return ntohs(addr.sin_port);
    }

    std::string ipString() const;
    std::string toString() const;

};

std::ostream& operator<<(std::ostream& os, const NetAddress&);

} // namespace G3D

template <>
struct GHashCode<G3D::NetAddress>
{
    size_t operator()(const G3D::NetAddress& key) const 
    { 
        return static_cast<size_t>(key.ip() + (static_cast<G3D::uint32>(key.port()) << 16));
    }
};

namespace G3D {

/**
 Two addresses may point to the same computer but be != because
 they have different IP's.
 */
inline bool operator==(const NetAddress& a, const NetAddress& b) {
	return (a.ip() == b.ip()) && (a.port() == b.port());
}


inline bool operator!=(const NetAddress& a, const NetAddress& b) {
    return !(a == b);
}

} // namespace G3D

#endif
