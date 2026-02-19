//
// Created by MSI on 2/19/2026.
//

#ifndef UNTITLED75_OPERATORS_TEST_H
#define UNTITLED75_OPERATORS_TEST_H

#include "operators.h"

void testOperators() {
    cout << "\n========== OPERATORS TEST ==========\n" << endl;

    // test numbers
    Value num1, num2, num3, zero;
    initNum(&num1, 10);
    initNum(&num2, 3);
    initNum(&num3, 5.5);
    initNum(&zero, 0);

    // test strings
    Value str1, str2, str3;
    initStr(&str1, "Hello");
    initStr(&str2, " World");
    initStr(&str3, "ABC");

    // test booleans
    Value boolTrue, boolFalse;
    initBool(&boolTrue, true);
    initBool(&boolFalse, false);

    // ============= Math operators =============
    cout << "--- Math Operators ---" << endl;
    Value res;

    res = add(&num1, &num2);
    printOperation("+", &num1, &num2, &res);

    res = subtract(&num1, &num2);
    printOperation("-", &num1, &num2, &res);

    res = multiply(&num1, &num2);
    printOperation("*", &num1, &num2, &res);

    res = divide(&num1, &num2);
    printOperation("/", &num1, &num2, &res);

    res = divide(&num1, &zero);
    cout << endl;

    // ============= Comparison operators =============
    cout << "--- Comparison Operators ---" << endl;
    res = equals(&num1, &num2);
    printOperation("==", &num1, &num2, &res);

    res = greaterThan(&num1, &num2);
    printOperation(">", &num1, &num2, &res);

    res = lessThan(&num1, &num2);
    printOperation("<", &num1, &num2, &res);
    cout << endl;

    // ============= Logical operators =============
    cout << "--- Logical Operators ---" << endl;
    res = logicalNot(&boolTrue);
    printOperation("NOT", &boolTrue, &res);

    res = logicalAnd(&boolTrue, &boolFalse);
    printOperation("AND", &boolTrue, &boolFalse, &res);

    res = logicalOr(&boolTrue, &boolFalse);
    printOperation("OR", &boolTrue, &boolFalse, &res);

    res = logicalXor(&boolTrue, &boolFalse);
    printOperation("XOR", &boolTrue, &boolFalse, &res);
    cout << endl;

    // ============= String operators =============
    cout << "--- String Operators ---" << endl;
    res = concat(&str1, &str2);
    printOperation("concat", &str1, &str2, &res);

    res = stringLength(&str1);
    char buffer[MAX_STR_LEN];
    cout << "length of \"" << toStr(&str1, buffer) << "\" = " << toStr(&res, buffer) << endl;

    Value index;
    initNum(&index, 2);
    res = charAt(&str1, &index);
    cout << "char 2 of \"" << toStr(&str1, buffer) << "\" = \"" << toStr(&res, buffer) << "\"" << endl;

    initNum(&index, 10);
    res = charAt(&str1, &index);
    cout << endl;

    // ============= Advanced math (bonus) =============
    cout << "--- Advanced Math ---" << endl;
    Value negNum;
    initNum(&negNum, -10);

    res = absolute(&negNum);
    printOperation("abs", &negNum, &res);

    Value sqrtNum;
    initNum(&sqrtNum, 16);
    res = squareRoot(&sqrtNum);
    printOperation("sqrt", &sqrtNum, &res);

    res = squareRoot(&negNum);

    Value floatNum;
    initNum(&floatNum, 5.7);
    res = floor(&floatNum);
    printOperation("floor", &floatNum, &res);

    res = ceil(&floatNum);
    printOperation("ceil", &floatNum, &res);

    res = modulo(&num1, &num2);
    printOperation("%", &num1, &num2, &res);

    res = modulo(&num1, &zero);
    cout << endl;

    // ============= Mixed types =============
    cout << "--- Mixed Types ---" << endl;
    res = add(&num1, &str1);
    printOperation("+", &num1, &str1, &res);

    res = add(&str1, &num1);
    printOperation("+", &str1, &num1, &res);

    Value numTen;
    initNum(&numTen, 10);
    res = equals(&num1, &numTen);
    printOperation("==", &num1, &numTen, &res);

    cout << "\n========== TEST END ==========\n" << endl;
}

#endif //UNTITLED75_OPERATORS_TEST_H
