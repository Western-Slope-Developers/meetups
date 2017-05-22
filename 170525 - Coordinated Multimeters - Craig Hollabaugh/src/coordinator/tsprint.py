#!/usr/bin/env python
import datetime

def getTimestamp():
     dt = datetime.datetime.now()
     return (dt.hour * 3600000 + dt.minute * 60000 + dt.second * 1000 + dt.microsecond / 1000)

lasttsm = 0

while True:
    ts = getTimestamp()
    tsm = ts/10
    if tsm != lasttsm:
        print ts
        lasttsm = tsm
    
