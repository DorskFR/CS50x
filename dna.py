import csv
from sys import argv

# storing here the patterns found in the database
# list of the count for each pattern (same ordering as the patterns list)
patterns = []
profile = []

# loop to get just the patterns from the database
with open(argv[1], newline='') as csvfile:
    database = csv.reader(csvfile, delimiter=',')
    row1 = next(database)
    for col in row1[1:]:
        patterns.append(col)


# loop to populate the profile dictionary with the pattern matches
with open(argv[2], "r") as textfile:
    sequence = textfile.read()
    for pattern in patterns:
        count = max([i for i in range(len(sequence)) if sequence.find(pattern * i) != -1])
        profile.append(str(count))

# loop over the csv database to match the name that corresponds to the profile we have built
with open(argv[1], newline='') as csvfile:
    database = csv.reader(csvfile, delimiter=',')
    # skipping the first line which is the column names
    next(database)
    match = False
    for row in database:
        if row[1:] == profile:
            print(f"{row[0]}")
            match = True
            break
    if match != True:
        print("No match")

# open .csv
# load patterns we are looking for in a list.
# create a dictionary with name and nested pattern + count
# open .txt
# recursion read the txt
# match for sequences (or use the substring of the pattern to split the string and get the length of the substrings)
# count the streaks and record the highest streak for that pattern
# compare the found streat records and compare with the database
# return the name if found a match or nothing if not