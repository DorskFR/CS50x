from cs50 import get_string

while True:
    card = get_string("Number: ")
    if int(card) > 0:
        break

reverse_card = card[::-1]
total = 0

for digit in card[::-1][1::2]:
    digit = int(digit)
    digit *= 2
    if digit > 9:
        digit = 1 + digit % 10
    total += digit

for digit in card[::-1][::2]:
    total += int(digit)

if total % 10 == 0:
    if (len(card) in [13, 16] and card[0] == "4"):
        print("VISA")
    elif (len(card) == 16 and card[0:2] in str(range(51, 55))):
        print("MASTERCARD")
    elif (len(card) == 15 and card[0:2] in ["34", "37"]):
        print("AMEX")
    else:
        print("INVALID")
else:
    print("INVALID")