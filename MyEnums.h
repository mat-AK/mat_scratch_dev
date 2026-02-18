//
// Created by MSI on 2/18/2026.
//

#ifndef UNTITLED74_MYENUMS_H
#define UNTITLED74_MYENUMS_H




// ============= ثابت‌ها =============
#define MAX_BLOCKS 1000
#define MAX_STACK_SIZE 100
#define MAX_VARIABLES 10
#define MAX_NAME_LEN 20
#define MAX_CONDITION_LEN 256

// ============= انواع بلوک‌ها =============
enum BlockType {
    BLOCK_MOVE,         // حرکت
    BLOCK_TURN,         // چرخش
    BLOCK_WAIT,         // صبر کن
    BLOCK_REPEAT,       // تکرار
    BLOCK_FOREVER,      // برای همیشه
    BLOCK_IF,           // اگر
    BLOCK_IF_ELSE,      // اگر-وگرنه
    BLOCK_WAIT_UNTIL,   // صبر کن تا
    BLOCK_STOP_ALL,     // توقف همه
    BLOCK_REPEAT_UNTIL, // تکرار کن تا
    BLOCK_END_IF,       // پایان اگر
    BLOCK_END_REPEAT,   // پایان تکرار
    BLOCK_END_FOREVER,  // پایان برای همیشه
    BLOCK_SIMPLE        // بلوک ساده
};


#endif //UNTITLED74_MYENUMS_H
