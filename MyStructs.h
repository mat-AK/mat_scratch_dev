//
// Created by MSI on 2/18/2026.
//

#ifndef UNTITLED74_MYSTRUCTS_H
#define UNTITLED74_MYSTRUCTS_H


#include "MyEnums.h"

// ============= ساختار بلوک =============
struct Block {
    int type;                // نوع بلوک
    int lineNumber;          // شماره خط
    int jumpAddress;         // آدرس پرش

    // داده‌های اضافی بر اساس نوع بلوک
    union {
        struct { double seconds; } wait;           // برای WAIT
        struct { int count; } repeat;               // برای REPEAT
        struct {
            char condition[MAX_CONDITION_LEN];     // شرط
            int elseAddress;                        // آدرس ELSE (برای IF-ELSE)
        } cond;
    } data;
};

// ============= ساختار پشته (Stack) =============
struct Stack {
    int items[MAX_STACK_SIZE];
    int top;

    // سازنده
    Stack() {
        top = -1;
    }
};

// ============= ساختار زمینه اجرا =============
struct ExecutionContext {
    Block program[MAX_BLOCKS];      // آرایه بلوک‌های برنامه
    int blockCount;                  // تعداد بلوک‌ها
    bool isRunning;                  // در حال اجراست؟
    bool isPaused;                   // متوقف شده؟
    int currentLine;                 // خط فعلی
    Stack repeatCounters;            // پشته شمارنده‌های تکرار
    Stack repeatStartLines;          // پشته خطوط شروع تکرار
    double variables[MAX_VARIABLES]; // مقادیر متغیرها
    char varNames[MAX_VARIABLES][MAX_NAME_LEN]; // نام متغیرها

    // سازنده
    ExecutionContext() {
        blockCount = 0;
        isRunning = false;
        isPaused = false;
        currentLine = 0;

        for (int i = 0; i < MAX_VARIABLES; i++) {
            variables[i] = 0.0;
            varNames[i][0] = '\0';
        }

        // متغیرهای پیش‌فرض
        strcpy(varNames[0], "x");
        strcpy(varNames[1], "y");
    }
};




#endif //UNTITLED74_MYSTRUCTS_H
