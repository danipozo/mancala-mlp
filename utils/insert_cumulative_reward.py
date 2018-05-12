import sys
import sqlite3
import csv
import glob

db_name = sys.argv[1]
data_directory = sys.argv[2]

files = glob.glob(data_directory+"/*[0-9]")

conn = sqlite3.connect(db_name)
cursor = conn.cursor()

alpha = 0.35

for data_file in files:
    with open(data_file) as csvfile:
        reader = csv.reader(csvfile, delimiter=',')
    
        for row in reader:
            cursor.execute(
                    "INSERT OR REPLACE INTO state_reward VALUES (:state, COALESCE((SELECT occurrences FROM state_reward WHERE state=:state)+1, 1), COALESCE((SELECT cumulative_reward*(1.0-:alpha) + :alpha*:reward   FROM state_reward WHERE state=:state),:reward))", {"state" : row[0], "reward" : float(row[1]), "alpha" : alpha})
    
conn.commit()
