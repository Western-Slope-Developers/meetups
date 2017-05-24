import sys, time, struct, socket, json, fcntl, datetime


def udp_mcast_listen(port=12345, group_address='239.0.0.57'):
    tmulticast_group = (group_address, port)
    rserver_address = ('', port)

    tsocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    tsocket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    # set the time-to-live for messages to 1 so they do not go past the
    # local network segment.
    ttl = struct.pack('b', 1)
    tsocket.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, ttl)

    rsocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    rsocket.bind(rserver_address)
    rgroup = socket.inet_aton(group_address)
    mreq = struct.pack('4sL', rgroup, socket.INADDR_ANY)
    rsocket.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

    while True: yield rsocket.recvfrom(1024)
