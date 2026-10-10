def add(a, b):
    return a + b

def subtract(a, b):
    return a - b

def multiply(a, b):
    return a * b

def find_remainder(a, b):
    if b == 0:
        return "Undefined (cannot divide by zero)"
    return a % b

num1 = float(input("Enter the first number: "))
num2 = float(input("Enter the second number: "))

print(f"Sum: {add(num1, num2)}")
print(f"Difference: {subtract(num1, num2)}")
print(f"Product: {multiply(num1, num2)}")
print(f"Remainder: {find_remainder(num1, num2)}")
