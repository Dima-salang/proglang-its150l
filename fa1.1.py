
def factorial(number):
        if number == 1 or number == 0:
            return 1
        elif number < 0:
            raise ValueError("Number must be non-negative")
        elif number != int(number):
            raise ValueError("Number must be an integer")
        else:
            return number * factorial(number - 1)

def main():
    try:
        number = int(input("Enter a number: "))
        print("Factorial of", number, "is", factorial(number))
    except ValueError as e:
        print(e)
    except RecursionError as e: 
        print("Recursive depth reached... Try again with a smaller number")
    except Exception as e:
        print("An error occurred:", e)

if __name__ == "__main__":
    main()