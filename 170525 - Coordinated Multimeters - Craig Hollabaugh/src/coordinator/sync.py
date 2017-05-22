#!/usr/bin/env python
#
# Send UDP broadcast packets
# references
#   https://pymotw.com/2/socket/multicast.html
#   https://en.wikipedia.org/wiki/Precision_Time_Protocol
#   https://www.perle.com/supportfiles/precision-time-protocol.shtml

import sys, time, struct, socket, json, fcntl, datetime

PORT         = 12345
GROUPADDRESS = '239.0.0.57'

def getIPAddress(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return socket.inet_ntoa(fcntl.ioctl(
        s.fileno(),
        0x8915,  # SIOCGIFADDR
        struct.pack('256s', ifname[:15])
    )[20:24])

tmulticast_group = (GROUPADDRESS,PORT)
rserver_address = ('',PORT)

tsocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#t.settimeout(2)
tsocket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
# Set the time-to-live for messages to 1 so they do not go past the
# local network segment.
ttl = struct.pack('b', 1)
tsocket.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, ttl)

rsocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
rsocket.bind(rserver_address)
rgroup = socket.inet_aton(GROUPADDRESS)
mreq = struct.pack('4sL', rgroup, socket.INADDR_ANY)
rsocket.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)


def microsecondsThisMinute():
    now = datetime.datetime.time(datetime.datetime.now())
    return 60000000 * now.minute + 1000000 * now.second + now.microsecond

def getTimestamp():
     dt = datetime.datetime.now()
     return (dt.hour * 3600000 + dt.minute * 60000 + dt.second * 1000 + dt.microsecond / 1000)

def txrx(message,members,timeout=2.0):
    responses = {}

    datastr, address = rsocket.recvfrom(1024)

    print message

    messagestr = json.dumps(message)
    sent = tsocket.sendto(messagestr, tmulticast_group)

    rsocket.settimeout(timeout)

    endtime = time.time() + timeout
    while True:
        try:
            datastr, address = rsocket.recvfrom(1024)
        except socket.timeout:
            print >>sys.stderr, 'timed out, no more responses'
            break
        else:
            # print >>sys.stderr, 'received %s bytes from %s' % (len(data), address)
            # print >>sys.stderr, data
            # print datastr,address,getIPAddress('enp4s4')
            data = json.loads(datastr)
            src = data['src']
            if src in members:
                responses[src] = data
                members.remove(src)
            if len(members) == 0:
                return responses
        if time.time() > endtime:
            break
    return None


members = ['plate']
timestamp = getTimestamp()
print txrx({"type":"sync","src":"coordinator"},members,2.0)
time.sleep(1.0)
members = ['plate']
print txrx({"type":"followup","src":"coordinator","timestamp":timestamp},members,2.0)

# waiting for delayrequest
members = ['plate']
endtime = time.time() + 2.0
while True:
    try:
        datastr, address = rsocket.recvfrom(1024)
    except socket.timeout:
        print >>sys.stderr, 'timed out, no more responses'
        break
    else:
        data = json.loads(datastr)
        if data['type'] == 'delayrequest':
            src = data['src']
            print "delayrequest from",src
            timestamp = getTimestamp()
            message = "{\"type\":\"delayreply\",\"src\":\"coordinator\",\"dst\":\"%s\",\"timestamp\":%d}"%(src,timestamp)
            sent = tsocket.sendto(message, tmulticast_group)
            if src in members:
                members.remove(src)
            if len(members) == 0:
                break
    if time.time() > endtime:
        break
