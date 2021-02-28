from cs50 import SQL
from sys import argv, exit

db = SQL("sqlite:///students.db")

if (len(argv) < 2):
    print("Usage: python roster.py [Gryffindor, Slytherin, Hufflepuff, Ravenclaw]")
    exit(1)

query = "SELECT * FROM students WHERE house = %s ORDER BY last, first"
result = db.execute(query, argv[1])

for r in result:
    print(f"{r['first']} ", end='')
    if (r['middle'] != None):
        print(f"{r['middle']} ", end='')
    print(f"{r['last']}", end='')
    print(", born ", end='')
    print(f"{r['birth']}")

# python roster.py Gryffindor
# check command-line arguments
# query databse for all students in house
# print-out each student's full name and birth year
# students should be sorted alphabetically by last name, then first name