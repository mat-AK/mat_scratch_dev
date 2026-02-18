//
// Created by MSI on 2/18/2026.
//

#ifndef UNTITLED74_CONTROLFLOW_H
#define UNTITLED74_CONTROLFLOW_H


#include <iostream>
#include <cstring>
using namespace std;

// ============= ثابت‌ها =============
#define MAX_BLOCKS 1000
#define MAX_STACK_SIZE 100
#define MAX_VARIABLES 10
#define MAX_NAME_LEN 20
#define MAX_CONDITION_LEN 256

enum BlockType {
    BLOCK_MOVE,         // 0: حرکت
    BLOCK_TURN,         // 1: چرخش
    BLOCK_WAIT,         // 2: صبر کن
    BLOCK_REPEAT,       // 3: تکرار
    BLOCK_FOREVER,      // 4: برای همیشه
    BLOCK_IF,           // 5: اگر
    BLOCK_IF_ELSE,      // 6: اگر-وگرنه
    BLOCK_WAIT_UNTIL,   // 7: صبر کن تا
    BLOCK_STOP_ALL,     // 8: توقف همه
    BLOCK_REPEAT_UNTIL, // 9: تکرار کن تا
    BLOCK_END_IF,       // 10: پایان اگر
    BLOCK_END_REPEAT,   // 11: پایان تکرار
    BLOCK_END_FOREVER,  // 12: پایان برای همیشه
    BLOCK_SIMPLE        // 13: بلوک ساده
};

// ============= ساختارها =============
struct Block {
    int type;
    int lineNumber;
    int jumpAddress;

    union {
        struct { double seconds; } wait;
        struct { int count; } repeat;
        struct {
            char condition[MAX_CONDITION_LEN];
            int elseAddress;
        } cond;
    } data;
};

struct Stack {
    int items[MAX_STACK_SIZE];
    int top;

    Stack() { top = -1; }

    void push(int value) {
        if (top < MAX_STACK_SIZE - 1) {
            items[++top] = value;
        }
    }

    int pop() {
        if (top >= 0) return items[top--];
        return -1;
    }

    int peek() {
        if (top >= 0) return items[top];
        return -1;
    }

    bool isEmpty() {
        return (top == -1);
    }
};

struct ExecutionContext {
    Block program[MAX_BLOCKS];
    int blockCount;
    bool isRunning;
    bool isPaused;
    int currentLine;
    Stack repeatCounters;
    Stack repeatStartLines;
    double variables[MAX_VARIABLES];
    char varNames[MAX_VARIABLES][MAX_NAME_LEN];

    ExecutionContext() {
        blockCount = 0;
        isRunning = false;
        isPaused = false;
        currentLine = 0;

        for (int i = 0; i < MAX_VARIABLES; i++) {
            variables[i] = 0;
            varNames[i][0] = '\0';
        }
        strcpy(varNames[0], "x");
        strcpy(varNames[1], "y");
    }
};

// ============= توابع =============

void initContext(ExecutionContext* ctx) {
    ctx->blockCount = 0;
    ctx->isRunning = false;
    ctx->isPaused = false;
    ctx->currentLine = 0;

    cout << "✅ Context آماده شد" << endl;
}

int addBlock(ExecutionContext* ctx, int type) {
    if (ctx->blockCount < MAX_BLOCKS) {
        int idx = ctx->blockCount;
        ctx->program[idx].type = type;
        ctx->program[idx].lineNumber = idx;
        ctx->program[idx].jumpAddress = -1;
        ctx->program[idx].data.repeat.count = 0;
        ctx->program[idx].data.wait.seconds = 0;
        ctx->program[idx].data.cond.condition[0] = '\0';
        ctx->program[idx].data.cond.elseAddress = -1;
        ctx->blockCount++;
        return idx;
    }
    return -1;
}

void setRepeatCount(ExecutionContext* ctx, int idx, int count) {
    if (idx >= 0 && idx < ctx->blockCount) {
        ctx->program[idx].data.repeat.count = count;
    }
}

void setCondition(ExecutionContext* ctx, int idx, const char* cond) {
    if (idx >= 0 && idx < ctx->blockCount) {
        strcpy(ctx->program[idx].data.cond.condition, cond);
    }
}

void setVariable(ExecutionContext* ctx, const char* name, double value) {
    for (int i = 0; i < MAX_VARIABLES; i++) {
        if (strcmp(ctx->varNames[i], name) == 0) {
            ctx->variables[i] = value;
            return;
        }
    }
}

double getVariable(ExecutionContext* ctx, const char* name) {
    for (int i = 0; i < MAX_VARIABLES; i++) {
        if (strcmp(ctx->varNames[i], name) == 0) return ctx->variables[i];
    }
    return 0;
}

bool evaluateCondition(ExecutionContext* ctx, const char* cond) {
    char var[20], op[3];
    double val;
    sscanf(cond, "%s %s %lf", var, op, &val);
    double v = getVariable(ctx, var);

    if (strcmp(op, ">") == 0) return v > val;
    if (strcmp(op, "<") == 0) return v < val;
    if (strcmp(op, "==") == 0) return v == val;
    if (strcmp(op, ">=") == 0) return v >= val;
    if (strcmp(op, "<=") == 0) return v <= val;
    return false;
}

void preProcessProgram(ExecutionContext* ctx) {
    int ifStack[MAX_STACK_SIZE], ifTop = -1;
    int repStack[MAX_STACK_SIZE], repTop = -1;

    for (int i = 0; i < ctx->blockCount; i++) {
        switch (ctx->program[i].type) {
            case BLOCK_IF:
                ifStack[++ifTop] = i;
                break;
            case BLOCK_REPEAT:
                repStack[++repTop] = i;
                break;
            case BLOCK_END_IF:
                if (ifTop >= 0) {
                    int start = ifStack[ifTop--];
                    ctx->program[start].jumpAddress = i;
                }
                break;
            case BLOCK_END_REPEAT:
                if (repTop >= 0) {
                    int start = repStack[repTop--];
                    ctx->program[i].jumpAddress = start;
                    ctx->program[start].jumpAddress = i;
                }
                break;
        }
    }
}

void executeBlock(ExecutionContext* ctx, int idx) {
    Block* b = &ctx->program[idx];

    switch (b->type) {
        case BLOCK_REPEAT:
            ctx->repeatCounters.push(b->data.repeat.count);
            ctx->repeatStartLines.push(idx);
            ctx->currentLine++;
            break;

        case BLOCK_END_REPEAT: {
            int counter = ctx->repeatCounters.peek() - 1;
            ctx->repeatCounters.pop();
            ctx->repeatCounters.push(counter);

            if (counter > 0) {
                ctx->currentLine = ctx->repeatStartLines.peek() + 1;
            } else {
                ctx->repeatCounters.pop();
                ctx->repeatStartLines.pop();
                ctx->currentLine++;
            }
            break;
        }

        case BLOCK_IF:
            if (evaluateCondition(ctx, b->data.cond.condition)) {
                ctx->currentLine++;
            } else {
                ctx->currentLine = b->jumpAddress + 1;
            }
            break;

        case BLOCK_END_IF:
            ctx->currentLine++;
            break;

        case BLOCK_MOVE:
            ctx->currentLine++;
            break;

        case BLOCK_TURN:
            ctx->currentLine++;
            break;

        default:
            ctx->currentLine++;
            break;
    }
}

void runProgram(ExecutionContext* ctx) {
    ctx->isRunning = true;
    ctx->currentLine = 0;

    int steps = 0;
    while (ctx->isRunning && steps < 1000) {
        if (ctx->currentLine >= ctx->blockCount) break;
        executeBlock(ctx, ctx->currentLine);
        steps++;
    }
}

void printProgram(ExecutionContext* ctx) {
    cout << "\n📋 برنامه:" << endl;
    for (int i = 0; i < ctx->blockCount; i++) {
        cout << i << ": ";
        switch (ctx->program[i].type) {
            case BLOCK_REPEAT: cout << "REPEAT " << ctx->program[i].data.repeat.count; break;
            case BLOCK_END_REPEAT: cout << "END_REPEAT"; break;
            case BLOCK_IF: cout << "IF " << ctx->program[i].data.cond.condition; break;
            case BLOCK_END_IF: cout << "END_IF"; break;
            case BLOCK_MOVE: cout << "MOVE"; break;
            case BLOCK_TURN: cout << "TURN"; break;
            default: cout << "type=" << ctx->program[i].type;
        }
        if (ctx->program[i].jumpAddress != -1)
            cout << " (→" << ctx->program[i].jumpAddress << ")";
        cout << endl;
    }
    cout << "================\n" << endl;
}


#endif //UNTITLED74_CONTROLFLOW_H
