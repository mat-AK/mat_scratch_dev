//
// Created by MSI on 2/18/2026.
//
// control_flow.cpp
// control_flow.cpp
#include "ControlFlow.h"
#include <iostream>
#include <cstring>
#include <cstdio>
using namespace std;

// ============= پیاده‌سازی توابع پشته =============
void initStack(Stack* s) {
    s->top = -1;
}

void push(Stack* s, int value) {
    if (s->top < MAX_STACK_SIZE - 1) {
        s->top++;
        s->items[s->top] = value;
    } else {
        cout << "خطا: پشته پر است!" << endl;
    }
}

int pop(Stack* s) {
    if (s->top >= 0) {
        int value = s->items[s->top];
        s->top--;
        return value;
    }
    cout << "خطا: پشته خالی است!" << endl;
    return -1;
}

int peek(Stack* s) {
    if (s->top >= 0) {
        return s->items[s->top];
    }
    cout << "خطا: پشته خالی است!" << endl;
    return -1;
}

bool isEmpty(Stack* s) {
    return (s->top == -1);
}

// ============= توابع مقداردهی اولیه =============
void initContext(ExecutionContext* ctx) {
    ctx->blockCount = 0;
    ctx->isRunning = false;
    ctx->isPaused = false;
    ctx->currentLine = 0;

    initStack(&ctx->repeatCounters);
    initStack(&ctx->repeatStartLines);

    for (int i = 0; i < MAX_VARIABLES; i++) {
        ctx->variables[i] = 0.0;
        ctx->varNames[i][0] = '\0';
    }

    strcpy(ctx->varNames[0], "x");
    strcpy(ctx->varNames[1], "y");

    cout << "زمینه اجرا مقداردهی اولیه شد." << endl;
}

// ============= توابع مدیریت بلوک‌ها =============
int addBlock(ExecutionContext* ctx, int type) {
    if (ctx->blockCount < MAX_BLOCKS) {
        int index = ctx->blockCount;
        ctx->program[index].type = type;
        ctx->program[index].lineNumber = index;
        ctx->program[index].jumpAddress = -1;
        ctx->program[index].data.repeat.count = 0;
        ctx->program[index].data.wait.seconds = 0.0;
        ctx->program[index].data.cond.condition[0] = '\0';
        ctx->program[index].data.cond.elseAddress = -1;
        ctx->blockCount++;

        cout << "بلوک " << index << " با نوع " << type << " اضافه شد." << endl;
        return index;
    }
    cout << "خطا: حافظه بلوک‌ها پر است!" << endl;
    return -1;
}

void setRepeatCount(ExecutionContext* ctx, int blockIndex, int count) {
    if (blockIndex >= 0 && blockIndex < ctx->blockCount) {
        ctx->program[blockIndex].data.repeat.count = count;
        cout << "تعداد تکرار بلوک " << blockIndex << " برابر " << count << " تنظیم شد." << endl;
    }
}

void setWaitTime(ExecutionContext* ctx, int blockIndex, double seconds) {
    if (blockIndex >= 0 && blockIndex < ctx->blockCount) {
        ctx->program[blockIndex].data.wait.seconds = seconds;
    }
}

void setCondition(ExecutionContext* ctx, int blockIndex, const char* condition) {
    if (blockIndex >= 0 && blockIndex < ctx->blockCount) {
        strcpy(ctx->program[blockIndex].data.cond.condition, condition);
        cout << "شرط بلوک " << blockIndex << " برابر " << condition << " تنظیم شد." << endl;
    }
}

void setElseAddress(ExecutionContext* ctx, int blockIndex, int elseAddress) {
    if (blockIndex >= 0 && blockIndex < ctx->blockCount) {
        ctx->program[blockIndex].data.cond.elseAddress = elseAddress;
    }
}

// ============= توابع مدیریت متغیرها =============
double getVariableValue(ExecutionContext* ctx, const char* name) {
    for (int i = 0; i < MAX_VARIABLES; i++) {
        if (strcmp(ctx->varNames[i], name) == 0) {
            return ctx->variables[i];
        }
    }
    cout << "خطا: متغیر " << name << " پیدا نشد!" << endl;
    return 0.0;
}

void setVariable(ExecutionContext* ctx, const char* name, double value) {
    for (int i = 0; i < MAX_VARIABLES; i++) {
        if (strcmp(ctx->varNames[i], name) == 0) {
            ctx->variables[i] = value;
            cout << "متغیر " << name << " = " << value << " تنظیم شد." << endl;
            return;
        }
    }
    cout << "خطا: متغیر " << name << " پیدا نشد!" << endl;
}

void printAllVariables(ExecutionContext* ctx) {
    cout << "\n=== مقادیر متغیرها ===" << endl;
    for (int i = 0; i < MAX_VARIABLES; i++) {
        if (ctx->varNames[i][0] != '\0') {
            cout << ctx->varNames[i] << " = " << ctx->variables[i] << endl;
        }
    }
    cout << "====================\n" << endl;
}

// ============= توابع ارزیابی =============
bool evaluateCondition(ExecutionContext* ctx, const char* condition) {
    char varName[20] = "";
    char op[3] = "";
    double value = 0.0;

    sscanf(condition, "%s %s %lf", varName, op, &value);
    double varValue = getVariableValue(ctx, varName);

    cout << "ارزیابی شرط: " << varName << "(" << varValue << ") " << op << " " << value << endl;

    if (strcmp(op, ">") == 0) return (varValue > value);
    if (strcmp(op, "<") == 0) return (varValue < value);
    if (strcmp(op, "==") == 0) return (varValue == value);
    if (strcmp(op, ">=") == 0) return (varValue >= value);
    if (strcmp(op, "<=") == 0) return (varValue <= value);
    if (strcmp(op, "!=") == 0) return (varValue != value);

    cout << "خطا: عملگر نامعتبر " << op << endl;
    return false;
}

// ============= پیش‌پردازش برنامه =============
void preProcessProgram(ExecutionContext* ctx) {
    cout << "\n=== شروع پیش‌پردازش برنامه ===" << endl;

    int ifStack[MAX_STACK_SIZE];
    int ifStackTop = -1;
    int repeatStack[MAX_STACK_SIZE];
    int repeatStackTop = -1;

    for (int i = 0; i < ctx->blockCount; i++) {
        Block* current = &ctx->program[i];

        switch (current->type) {
            case BLOCK_IF:
            case BLOCK_IF_ELSE:
                ifStack[++ifStackTop] = i;
                cout << "بلوک IF " << i << " به پشته اضافه شد." << endl;
                break;

            case BLOCK_REPEAT:
            case BLOCK_FOREVER:
                repeatStack[++repeatStackTop] = i;
                cout << "بلوک REPEAT/FOREVER " << i << " به پشته اضافه شد." << endl;
                break;

            case BLOCK_END_IF:
                if (ifStackTop >= 0) {
                    int startIf = ifStack[ifStackTop--];
                    ctx->program[startIf].jumpAddress = i;
                    cout << "آدرس END_IF برای IF " << startIf << " = " << i << endl;
                }
                break;

            case BLOCK_END_REPEAT:
            case BLOCK_END_FOREVER:
                if (repeatStackTop >= 0) {
                    int startRepeat = repeatStack[repeatStackTop--];
                    ctx->program[i].jumpAddress = startRepeat;
                    ctx->program[startRepeat].jumpAddress = i;
                    cout << "آدرس END_REPEAT برای REPEAT " << startRepeat << " = " << i << endl;
                }
                break;
        }
    }
    cout << "=== پایان پیش‌پردازش ===\n" << endl;
}

// ============= اجرای یک بلوک =============
void executeBlock(ExecutionContext* ctx, int blockIndex) {
    Block* block = &ctx->program[blockIndex];

    cout << "اجرای خط " << blockIndex << ": ";

    switch (block->type) {
        case BLOCK_WAIT:
            cout << "WAIT " << block->data.wait.seconds << " ثانیه" << endl;
            // در برنامه واقعی: SDL_Delay(block->data.wait.seconds * 1000);
            ctx->currentLine++;
            break;

        case BLOCK_REPEAT:
            cout << "REPEAT " << block->data.repeat.count << " بار" << endl;
            push(&ctx->repeatCounters, block->data.repeat.count);
            push(&ctx->repeatStartLines, blockIndex);
            ctx->currentLine++;
            break;

        case BLOCK_END_REPEAT: {
            int counter = peek(&ctx->repeatCounters);
            counter--;
            pop(&ctx->repeatCounters);
            push(&ctx->repeatCounters, counter);

            cout << "END_REPEAT - باقیمانده: " << counter << endl;

            if (counter > 0) {
                ctx->currentLine = peek(&ctx->repeatStartLines) + 1;
                cout << "برگشت به خط " << ctx->currentLine << endl;
            } else {
                pop(&ctx->repeatCounters);
                pop(&ctx->repeatStartLines);
                ctx->currentLine++;
            }
            break;
        }

        case BLOCK_FOREVER:
            cout << "FOREVER" << endl;
            push(&ctx->repeatStartLines, blockIndex);
            ctx->currentLine++;
            break;

        case BLOCK_END_FOREVER:
            cout << "END_FOREVER" << endl;
            ctx->currentLine = peek(&ctx->repeatStartLines) + 1;
            cout << "برگشت به خط " << ctx->currentLine << endl;
            break;

        case BLOCK_IF:
            cout << "IF " << block->data.cond.condition << endl;
            if (evaluateCondition(ctx, block->data.cond.condition)) {
                cout << "شرط درست - ادامه" << endl;
                ctx->currentLine++;
            } else {
                cout << "شرط غلط - پرش به خط " << block->jumpAddress + 1 << endl;
                ctx->currentLine = block->jumpAddress + 1;
            }
            break;

        case BLOCK_STOP_ALL:
            cout << "STOP_ALL" << endl;
            ctx->isRunning = false;
            break;

        case BLOCK_MOVE:
            cout << "MOVE - x = " << getVariableValue(ctx, "x") << endl;
            ctx->currentLine++;
            break;

        case BLOCK_TURN:
            cout << "TURN" << endl;
            ctx->currentLine++;
            break;

        default:
            cout << "بلوک ناشناخته" << endl;
            ctx->currentLine++;
            break;
    }
}

// ============= اجرای کل برنامه =============
void runProgram(ExecutionContext* ctx) {
    ctx->isRunning = true;
    ctx->currentLine = 0;

    cout << "\n*** شروع اجرای برنامه ***" << endl;
    printAllVariables(ctx);

    int stepCount = 0;
    const int MAX_STEPS = 1000; // برای جلوگیری از حلقه بی‌نهایت

    while (ctx->isRunning && !ctx->isPaused && stepCount < MAX_STEPS) {
        if (ctx->currentLine < 0 || ctx->currentLine >= ctx->blockCount) {
            cout << "پایان برنامه" << endl;
            ctx->isRunning = false;
            break;
        }

        executeBlock(ctx, ctx->currentLine);
        stepCount++;
    }

    if (stepCount >= MAX_STEPS) {
        cout << "خطا: تعداد اجرا بیش از حد مجاز (حلقه بی‌نهایت)" << endl;
    }

    cout << "*** پایان اجرای برنامه ***\n" << endl;
    printAllVariables(ctx);
}

// ============= توابع کمکی =============
void printBlockInfo(ExecutionContext* ctx, int blockIndex) {
    if (blockIndex < 0 || blockIndex >= ctx->blockCount) return;

    Block* b = &ctx->program[blockIndex];
    cout << "بلوک " << blockIndex << ": نوع=" << b->type
         << ", jump=" << b->jumpAddress << endl;
}

void printProgram(ExecutionContext* ctx) {
    cout << "\n=== برنامه ===" << endl;
    for (int i = 0; i < ctx->blockCount; i++) {
        cout << i << ": ";
        switch (ctx->program[i].type) {
            case BLOCK_REPEAT: cout << "REPEAT " << ctx->program[i].data.repeat.count; break;
            case BLOCK_END_REPEAT: cout << "END_REPEAT"; break;
            case BLOCK_IF: cout << "IF " << ctx->program[i].data.cond.condition; break;
            case BLOCK_END_IF: cout << "END_IF"; break;
            case BLOCK_MOVE: cout << "MOVE"; break;
            case BLOCK_TURN: cout << "TURN"; break;
            default: cout << "نوع " << ctx->program[i].type;
        }
        cout << " (jump=" << ctx->program[i].jumpAddress << ")" << endl;
    }
    cout << "=============\n" << endl;
}
//rest od the funcs