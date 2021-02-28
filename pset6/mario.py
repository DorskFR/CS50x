from cs50 import get_int

while True:
    height = get_int("height:")
    if height in range(1, 9):
        break

for c in range(height):
    c += 1
    print(" " * (height - c), end="")
    print("#" * c, end="")
    print("  ", end="")
    print("#" * c, end="")
    print("")
