//
// Created by MSI on 2/19/2026.
//

#ifndef UNTITLED75_OPERATORS_H
#define UNTITLED75_OPERATORS_H

#include <iostream>
#include <cmath>
#include <cstring>
#include <string>
#include <sstream>
using namespace std;


#define MAX_STR_LEN 256

enum ValueType { VAL_NUM, VAL_STR, VAL_BOOL };

struct Value {
    ValueType type;
    double numVal;
    bool boolVal;
    char strVal[MAX_STR_LEN];
};

void initNum(Value* v, double n) {
    v->type = VAL_NUM;
    v->numVal = n;
}

void initBool(Value* v, bool b) {
    v->type = VAL_BOOL;
    v->boolVal = b;
}

void initStr(Value* v, const char* s) {
    v->type = VAL_STR;
    strcpy(v->strVal, s);
}

double toNum(const Value* v) {
    if (v->type == VAL_NUM) return v->numVal;
    if (v->type == VAL_BOOL) return v->boolVal ? 1 : 0;
    // string to number
    return atof(v->strVal);
}

bool toBool(const Value* v) {
    if (v->type == VAL_BOOL) return v->boolVal;
    if (v->type == VAL_NUM) return v->numVal != 0;
    // empty string = false
    return v->strVal[0] != '\0';
}

const char* toStr(const Value* v, char* buffer) {
    if (v->type == VAL_NUM) {
        if (v->numVal == (int)v->numVal) {
            sprintf(buffer, "%d", (int)v->numVal);
        } else {
            sprintf(buffer, "%g", v->numVal);
        }
        return buffer;
    }
    if (v->type == VAL_BOOL) {
        return v->boolVal ? "true" : "false";
    }
    return v->strVal;
}

Value add(const Value* a, const Value* b) {
    Value result;

    // if both are numbers
    if (a->type == VAL_NUM && b->type == VAL_NUM) {
        initNum(&result, a->numVal + b->numVal);
        return result;
    }

    // if one is string, concatenate
    if (a->type == VAL_STR || b->type == VAL_STR) {
        char buffer[MAX_STR_LEN * 2];
        char tempA[MAX_STR_LEN];
        char tempB[MAX_STR_LEN];
        strcpy(buffer, toStr(a, tempA));
        strcat(buffer, toStr(b, tempB));
        initStr(&result, buffer);
        return result;
    }

    // default to number
    initNum(&result, toNum(a) + toNum(b));
    return result;
}

Value subtract(const Value* a, const Value* b) {
    Value result;
    initNum(&result, toNum(a) - toNum(b));
    return result;
}

Value multiply(const Value* a, const Value* b) {
    Value result;
    initNum(&result, toNum(a) * toNum(b));
    return result;
}

Value divide(const Value* a, const Value* b) {
    Value result;
    double divisor = toNum(b);

    if (divisor == 0) {
        cout << "Error: division by zero!" << endl;
        initNum(&result, 0);
        return result;
    }

    initNum(&result, toNum(a) / divisor);
    return result;
}

// ============= Comparison operators =============
Value equals(const Value* a, const Value* b) {
    Value result;

    if (a->type == VAL_STR || b->type == VAL_STR) {
        char tempA[MAX_STR_LEN];
        char tempB[MAX_STR_LEN];
        initBool(&result, strcmp(toStr(a, tempA), toStr(b, tempB)) == 0);
        return result;
    }

    initBool(&result, toNum(a) == toNum(b));
    return result;
}

Value greaterThan(const Value* a, const Value* b) {
    Value result;
    initBool(&result, toNum(a) > toNum(b));
    return result;
}

Value lessThan(const Value* a, const Value* b) {
    Value result;
    initBool(&result, toNum(a) < toNum(b));
    return result;
}

Value greaterEqual(const Value* a, const Value* b) {
    Value result;
    initBool(&result, toNum(a) >= toNum(b));
    return result;
}

Value lessEqual(const Value* a, const Value* b) {
    Value result;
    initBool(&result, toNum(a) <= toNum(b));
    return result;
}

// ============= Logical operators =============
Value logicalNot(const Value* a) {
    Value result;
    initBool(&result, !toBool(a));
    return result;
}

Value logicalAnd(const Value* a, const Value* b) {
    Value result;
    initBool(&result, toBool(a) && toBool(b));
    return result;
}

Value logicalOr(const Value* a, const Value* b) {
    Value result;
    initBool(&result, toBool(a) || toBool(b));
    return result;
}

Value logicalXor(const Value* a, const Value* b) {
    Value result;
    initBool(&result, toBool(a) != toBool(b));
    return result;
}

// ============= String operators =============
Value stringLength(const Value* s) {
    Value result;
    char temp[MAX_STR_LEN];
    initNum(&result, strlen(toStr(s, temp)));
    return result;
}

Value charAt(const Value* s, const Value* index) {
    Value result;
    char str[MAX_STR_LEN];
    strcpy(str, toStr(s, str));

    int idx = (int)toNum(index) - 1;  // index starts from 1

    if (idx < 0 || idx >= (int)strlen(str)) {
        cout << "Error: invalid index! (string length: " << strlen(str) << ")" << endl;
        initStr(&result, "");
        return result;
    }

    char ch[2] = {str[idx], '\0'};
    initStr(&result, ch);
    return result;
}

Value concat(const Value* a, const Value* b) {
    Value result;
    char buffer[MAX_STR_LEN * 2];
    char tempA[MAX_STR_LEN];
    char tempB[MAX_STR_LEN];

    strcpy(buffer, toStr(a, tempA));
    strcat(buffer, toStr(b, tempB));

    initStr(&result, buffer);
    return result;
}

// ============= Advanced math (bonus) =============
Value absolute(const Value* a) {
    Value result;
    initNum(&result, fabs(toNum(a)));
    return result;
}

Value squareRoot(const Value* a) {
    Value result;
    double val = toNum(a);

    if (val < 0) {
        cout << "Error: square root of negative number!" << endl;
        initNum(&result, 0);
        return result;
    }

    initNum(&result, sqrt(val));
    return result;
}

Value floor(const Value* a) {
    Value result;
    initNum(&result, (double)std::floor(toNum(a)));
    return result;
}

Value ceil(const Value* a) {
    Value result;
    initNum(&result, (double)std::ceil(toNum(a)));
    return result;
}

Value sine(const Value* a) {
    Value result;
    initNum(&result, std::sin(toNum(a)));
    return result;
}

Value cosine(const Value* a) {
    Value result;
    initNum(&result, std::cos(toNum(a)));
    return result;
}

Value modulo(const Value* a, const Value* b) {
    Value result;
    double divisor = toNum(b);

    if (divisor == 0) {
        cout << "Error: modulo by zero!" << endl;
        initNum(&result, 0);
        return result;
    }

    double res = fmod(toNum(a), divisor);
    if (res < 0) res += divisor;

    initNum(&result, res);
    return result;
}

// ============= Print helpers =============
void printValue(const Value* v) {
    char buffer[MAX_STR_LEN];
    cout << toStr(v, buffer);
}

void printOperation(const char* op, const Value* a, const Value* b, const Value* result) {
    char bufferA[MAX_STR_LEN];
    char bufferB[MAX_STR_LEN];
    char bufferR[MAX_STR_LEN];

    cout << toStr(a, bufferA) << " " << op << " " << toStr(b, bufferB)
         << " = " << toStr(result, bufferR) << endl;
}

void printOperation(const char* op, const Value* a, const Value* result) {
    char bufferA[MAX_STR_LEN];
    char bufferR[MAX_STR_LEN];

    cout << op << " " << toStr(a, bufferA) << " = " << toStr(result, bufferR) << endl;
}



#endif //UNTITLED75_OPERATORS_H
