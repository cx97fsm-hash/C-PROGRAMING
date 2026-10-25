num = int(input("Enter an integer: "))

for _ in [num]:
    if num % 2 == 0:
        print(num, "is Even")
    else:
        print(num, "is Odd")