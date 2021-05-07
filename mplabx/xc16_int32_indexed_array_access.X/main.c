#include <stdio.h>
#include <string.h>

#include "minimal_example.h"

#define nelem(x) (sizeof(x)/sizeof((x)[0]))

static const long expected_result[nelem(mval)] = {
    0xc0dec4feL,
    0x222200a5L,
    0x222200a5L,
    0x12345678L,

    0xeeeeffff,
};

void compare_result(void)
{
    int i;
    const char *warn;

    if (memcmp(expected_result, mval, sizeof expected_result)==0) {
        fprintf(stderr, "Test passed.\n");
        return;
    }
    fprintf(stderr, "Test failed.\n");
    fprintf(stderr, "  expected:       got:\n");
    for (i=0; i<nelem(expected_result); i++) {
        warn = "";
        if (expected_result[i] != mval[i]) {
            warn = "\t!";
        }
        fprintf(stderr, "%d 0x%08lx  0x%08lx%s\n", i, expected_result[i], mval[i], warn);
    }
}


int main(void) {
    /* run the function to be tested: */
    testmr1();

    compare_result();
    return 0;
}


