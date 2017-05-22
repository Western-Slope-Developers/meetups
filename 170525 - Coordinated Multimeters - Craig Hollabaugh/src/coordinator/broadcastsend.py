#!/usr/bin/env python
#
# Send UDP broadcast packets

PORT = 12345

import sys, time
from socket import *

s = socket(AF_INET, SOCK_DGRAM)
s.bind(('', 0))
s.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)

while 1:
    data = "%-10s %10.6f"%(gethostname(),time.time())
    s.sendto(data, ('<broadcast>', PORT))
    print data    
    time.sleep(0.1)
