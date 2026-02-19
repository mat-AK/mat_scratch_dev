#include <iostream>
#include "operators.h"
#include "operators_test.h"

using namespace std;

int main() {
    cout << "=== Fundamentals Project - Operators ===\n" << endl;

    // run tests
    testOperators();

    // simple interactive example
    cout << "--- Simple Example ---" << endl;

    Value a, b;
    initNum(&a, 15);
    initNum(&b, 4);

    char buffer[MAX_STR_LEN];
    cout << "a = " << toStr(&a, buffer) << ", b = " << toStr(&b, buffer) << endl;

    Value res;
    res = add(&a, &b);
    cout << "a + b = " << toStr(&res, buffer) << endl;

    res = subtract(&a, &b);
    cout << "a - b = " << toStr(&res, buffer) << endl;

    res = multiply(&a, &b);
    cout << "a * b = " << toStr(&res, buffer) << endl;

    res = divide(&a, &b);
    cout << "a / b = " << toStr(&res, buffer) << endl;

    res = modulo(&a, &b);
    cout << "a % b = " << toStr(&res, buffer) << endl;

    res = greaterThan(&a, &b);
    cout << "a > b ? " << (res.boolVal ? "true" : "false") << endl;

    cout << "\nProgram finished." << endl;

    return 0;
}