#include <bits/stdc++.h>
#include "ControlFlow.h"


using namespace std;

int main() {



    ExecutionContext ctx;
    initContext(&ctx);



    setVariable(&ctx, "x", 10);

    int repeatIdx = addBlock(&ctx, BLOCK_REPEAT);
    setRepeatCount(&ctx, repeatIdx, 3);

        addBlock(&ctx, BLOCK_MOVE);


    int ifIdx = addBlock(&ctx, BLOCK_IF);
    setCondition(&ctx, ifIdx, "x > 5");

    addBlock(&ctx, BLOCK_TURN);

    addBlock(&ctx, BLOCK_END_IF);


    addBlock(&ctx, BLOCK_END_REPEAT);

    cout << "number of blocks " << ctx.blockCount << "\n" << endl;

    printProgram(&ctx);

    preProcessProgram(&ctx);

    runProgram(&ctx);

    return 0;
}