from cs50 import get_string
import re

while True:
    text = get_string("Text:")
    if len(text) > 0:
        break

letters = 0

for letter in text:
    if letter.isalpha() == True:
        letters += 1

words = len(text.split(" "))
sentences = len(re.split('\. |\? |\! ', text))
# print(re.split('\. |\? |\! ', text))

# print(f"{letters} letter(s)")
# print(f"{words} word(s)")
# print(f"{sentences} sentence(s)")

L = float(letters) / (float(words) / 100)
S = float(sentences) / (float(words) / 100)
index = round(0.0588 * L - 0.296 * S - 15.8)

# print(L)
# print(S)
# print(index)


if index < 1:
    print("Before Grade 1")
elif index >= 16:
    print("Grade 16+")
else:
    print(f"Grade {index}")