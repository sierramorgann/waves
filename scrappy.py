import csv

with open('waves_data.csv') as csvfile:
	
    readCSV = csv.reader(csvfile, delimiter=',')

    for row in readCSV:
       print(row)

