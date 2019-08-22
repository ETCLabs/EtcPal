# Network Interface Indexes                                {#interface_indexes}

Some EtcPal APIs make use of an index value to identify network interfaces.
Almost all underlying network stacks have the notion of a network interface
index, which is standardized for IPv6 by
[RFC 3493](https://tools.ietf.org/html/rfc3493).

Interface indexes _may_ also be used for specifying the _zone index_ or
_scope ID_ of an IPv6 address (see
[RFC 4007](https://tools.ietf.org/html/rfc4007)). However, depending on the
address scope, this doesn't always apply. It also seems to currently be a
lightly-specified area of the IPv6 protocol, so it should not be treated as a
guarantee.

In our protocols, we use the interface index mostly for multicasting purposes.
When used this way, it is used to specify an interface on which to send or
receive multicast traffic for a given socket. The @ref etcpal_socket API uses
them to specify network interfaces for the @ref ETCPAL_MCAST_JOIN_GROUP,
@ref ETCPAL_MCAST_LEAVE_GROUP, and @ref ETCPAL_IP_MULTICAST_IF socket options.

The interface index is of type `unsigned int`; 0 is an invalid value for
specifying a specific interface, but can be used as a wildcard (signaling the
kernel to pick the "best" or most appropriate interface). Interface indexes for
each connected interface can be obtained from the @ref etcpal_netint API (in
the @ref EtcPalNetintInfo structure returned from etcpal_netint calls), or from
the relevant OS APIs on OSes that support them (many conform to the standard
interface specified by RFC 3493).

Some embedded stacks have limited or nonstandard ways to obtain a network
interface index. For clarity, the exact way that interface indexes are obtained
or derived in each stack is listed below.

| OS/Stack | Method |
|----------|--------|
| macOS    | Standard / RFC 3493, can also be inferred from data in `ifconfig` and `netstat -rn` |
| Microsoft Windows | Indicated in [IP_ADAPTER_ADDRESSES](https://docs.microsoft.com/en-us/windows/win32/api/iptypes/ns-iptypes-_ip_adapter_addresses_lh) structure, also visible via `route print` at command line |
| Linux    | Standard / RFC 3493, visible via `ip link` on newer/systemd systems, or can be inferred from data in `ifconfig` |
| MQX RTOS | The index is derived from the RTCS _device identifier_ (an index starting from 0 into a linked list of network interfaces) plus 1. This is translated internally for relevant socket calls. |
