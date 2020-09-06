from cs50 import get_float
import math

while True:
    cash = get_float("Change owed: ")
    if cash > 0:
        break

coins = [0.25, 0.10, 0.05, 0.01]
change = 0

while cash >= 0.01:
    for coin in range(len(coins)):
        while cash - coins[coin] >= 0:
            cash = round(cash - coins[coin], 2)
            change += 1

print(change)
