from cs50 import SQL
from sys import argv, exit
import csv

db = SQL("sqlite:///students.db")

if (len(argv) < 2):
    print("Usage: python import.py [file].csv")
    exit(1)
elif (argv[1][-4:] != ".csv"):
    print(f"A csv file is needed (not {argv[1][-4:]})")
    exit(1)


roster = []

with open(argv[1], newline='') as csvfile:
    source = csv.reader(csvfile, delimiter=',')
    next(source)
    for row in source:
        record = []
        name_col = row[0].split(' ')
        record.append(name_col[0])
        if len(name_col) < 3:
            record.append(None)
            record.append(name_col[1])
        else:
            record.append(name_col[1])
            record.append(name_col[2])
        record.append(row[1])
        record.append(row[2])
        roster.append(record)
        query = "INSERT INTO students (first, middle, last, house, birth) VALUES (%s, %s, %s, %s, %s)"
        val = (record[0], record[1], record[2], record[3], record[4])
        db.execute(query, val)

# Check command sqliline arguments
# Query database for all students in hourse
# Print out each student's full name and birth year
# Students should be sorted alphabetically by last name then first name