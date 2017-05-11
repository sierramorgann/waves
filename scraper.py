#!/usr/bin/python
import serial
import re
import json
import csv
import datetime
import urllib
import OSC
from OSC import OSCClient, OSCMessage

#umpqua off shore bouey wave height in feet and the period of the wave in seconds

url = 'http://www.ndbc.noaa.gov/data/latest_obs/46229.rss'
source = urllib.urlopen(url)
content = source.read()
source.close()

waveHeight = re.findall(r'.\bWave Height\b.*', str(content))
wavePeriod = re.findall(r'.\bWave Period\b.*', str(content))
ft = re.findall(r'(?:\d*\.)?\d+', str(waveHeight))
sec = re.findall(r'(?:\d*\.)?\d+', str(wavePeriod))
feet = ''.join(ft)
seconds = ''.join(sec)
data = ','.join(ft + sec)

ftData = float(feet)
secData = int(float(seconds))

print(ftData)
print(secData)

	

def timeStamped(fname, fmt='%Y-%m-%d-%H-%M-%S_{fname}'):
    return datetime.datetime.now().strftime(fmt).format(fname=fname)	

    #/Users/sierramorgan/Desktop/waves/

with open('/Users/sierramorgan/Desktop/waves/waves_data.csv','w') as data_out:
    writer=csv.writer(data_out)    
    data_out.write(data)

client = OSCClient()
client.connect( ('10.0.1.3', 4380) )
msg = OSCMessage("/send")
msg.append(ftData)
msg.append(secData)
client.send(msg)