#!/usr/bin/python
import socket
import re
from bs4 import BeautifulSoup
import requests
import json
import csv
import datetime
import os

TCP_IP = '127.0.0.1'
TCP_PORT = 6258 
BUFFER_SIZE = 1024

var = requests.get('http://www.ndbc.noaa.gov/data/realtime2/46229.data_spec').content
another = BeautifulSoup(var, "html.parser")


for line in another:
	p = re.sub(r'[\(]\d\.\d\d\d[\)]|[9]\.[9][9][9]|(?<![-.])\b[0-9]+\b(?!\.[0-9])', ",", line)

# s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# s.connect((TCP_IP, TCP_PORT))
# s.send(p)
# data = s.recv(BUFFER_SIZE)
# s.close()		

def timeStamped(fname, fmt='%Y-%m-%d-%H-%M-%S_{fname}'):
    return datetime.datetime.now().strftime(fmt).format(fname=fname)	

with open('/Users/sierramorgan/Desktop/waves/waves_data.csv','w') as data_out:
    writer=csv.writer(data_out)    
    data_out.write(p)