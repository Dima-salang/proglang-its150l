#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


// const definitions
const int CONTINUE = 1;
const int EXIT = 2;
const double LOLLIPOP_COST = 5.00;
const double VAT = 0.12;


// prototypes
void printHeader();
void printContinueChoices();
int parseChoice();
bool isValidQuantity(double quantity, bool *err);
bool isValidPayment(double payment, double amountDue, bool *err);
void printTransactionSummary(int quantity, double amountDue, double payment);
int getChoice();
double getQuantity(bool *err);
double getPayment(bool *err);


int main() {
    int choice;
    double quantity;
    double amountDue;
    double custPayment;

    // err flag
    bool err;

    // we print the header first
    printHeader();

    // start a do while loop to continue buying or exit and make the program more interactive
    do {
        printf("Sweet and minty Lollipops! How many would you want to buy?: ");

        // get and validate quantity
        // if the quantity is not valid, we continuously prompt the user
        do {
            quantity = getQuantity(&err);
            if (err) {
                continue;
            }
        } while (!isValidQuantity(quantity, &err));

        printf("\n");

        amountDue = (quantity * LOLLIPOP_COST);
        printf("You have bought %d Sweet and minty Lollipops! Please pay Php %.2f\n", (int)quantity, amountDue);

        // get and validate payment
        // same process as quantity
        do {
            custPayment = getPayment(&err);
            if (err) {
                continue;
            }
        } while (!isValidPayment(custPayment, amountDue, &err));


        // we print the transaction summary receipt
        printTransactionSummary(quantity, amountDue, custPayment);

        // finally, we print the continue choices and then parse it
        printContinueChoices();
        choice = parseChoice();
    } while (choice != EXIT);
    printf("\nThank you and enjoy our lollipops!\n");
    return 0;
}


// ----- FUNCTION HELPERS -----


void printHeader() {
    printf("\nWELCOME TO LOLLIPOPS FACTORY!\n");
    printf("-------------------------------------\n");
}

void printContinueChoices() { 
    printf("\n[1] Continue buying \n");
    printf("[2] Exit\n");
}


// parses the user input and validates it
int parseChoice() {
    int choice;
    choice = getChoice();

    while (choice != CONTINUE && choice != EXIT) {
        printContinueChoices();
        printf("Invalid choice. Please enter 1 or 2.\n");

        choice = getChoice();
    }
    return choice;
}

// gets the user input and validates it
int getChoice() {
    int choice;
    char input[50];
    char *validptr;
    printf("Enter your choice: ");

    // if the user enters something other than an int,
    // clear the input buffer because when the scanner scans again,
    // the buffer will still have the invalid input
    if (!fgets(input, sizeof(input), stdin)) {
        return -1;
    }

    input[strcspn(input, "\n")] = '\0';
    choice = (int)strtol(input, &validptr, 10);

    if (*validptr != '\0') {
        return -1;
    }

    return choice;
}

// gets quantity input from the user
double getQuantity(bool *err) {
    double quantity;
    char input[100];
    char *validptr;
    printf("Enter the quantity: ");

    // same process as getChoice()
    if (!fgets(input, sizeof(input), stdin)) {
        return 0;
    }

    // remove the newline character and convert the input to a double
    input[strcspn(input, "\n")] = '\0';
    quantity = (double)strtod(input, &validptr);

    // if the input is not a valid double
    if (*validptr != '\0') {
        *err = true;
        return 0;
    }

    *err = false;
    return quantity;
}

// gets the payment input from the user
double getPayment(bool *err) {
    double payment;
    char input[100];
    char *validptr;
    printf("Enter your payment: ");

    // same process as getChoice()
    if (!fgets(input, sizeof(input), stdin)) {
        return 0;
    }

    // same as the getQuantity() function
    input[strcspn(input, "\n")] = '\0';
    payment = (double)strtod(input, &validptr);

    if (*validptr != '\0') {
        *err = true;
        return 0;
    }
    *err = false;
    return payment;
}



// validates the quantity input
// will return a bool depending on the state of validity
bool isValidQuantity(double quantity, bool *err) {
    int intQuantity;

    // check if the err flag is true
    if (*err) {
        printf("Invalid quantity. Please enter a valid number.\n");
        return false;
    }
    // quantity should be int without a fractional part
    if (quantity == (int)quantity) {
        intQuantity = (int)quantity;
    } else {
        printf("Invalid quantity. Please enter an integer.\n");
        return false;
    }
    // if quantity is less than 0
    if (intQuantity < 0) {
        printf("Invalid quantity. Please enter a positive number.\n");
        return false;
    } else if (intQuantity == 0) {
        printf("Please buy something...\n");
        return false;
    }

    return true;
}

// validates the payment input
// will also return a bool
bool isValidPayment(double payment, double amountDue, bool *err) {
    if (*err) {
        printf("Invalid payment. Please enter a valid number.\n");
        return false;
    }
    // if payment is less than or equal to 0,
    // use should buy something in the store
    if (payment <= 0 ) {
        printf("Invalid quantity. Please enter a positive number...\n");
        return false;
    // check if the user enters something other than a double or int
    } else if (payment != (double)payment) {
        printf("Enter a valid amount...\n");
        return false;
    // if the payment is less than the amount due
    } else if (payment < amountDue) {
        printf("Payment is less than the amount due. Please enter a valid payment...\n");
        return false;
    }
    return true;
}


// prints the transaction summary receipt
void printTransactionSummary(int quantity, double amountDue, double payment) {
    // calculate the vat, vatable amount, and change.
    double vat = amountDue * (VAT / (1 + VAT));
    double vatAmount = amountDue / (1 + VAT);
    double change = payment - amountDue;

    printf("\n");
    printf("Please examine the transaction details below:\n\n");
    
    printf("TRANSACTION SUMMARY\n");
    printf("----------------------------------------------\n");
    printf("%-30s PhP %10.2f\n", "Amount Due (VAT inclusive):", amountDue);
    printf("%-30s PhP %10.2f\n", "VAT(12%):", vat);
    printf("%-30s PhP %10.2f\n", "VATable Amount:", vatAmount);
    printf("%-30s PhP %10.2f\n", "Amount of Cash:", payment);
    printf("%-30s PhP %10.2f\n", "Change:", change);
    printf("----------------------------------------------\n");
}