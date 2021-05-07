#include <stdio.h>
#include <string.h>

#include "example.h"

#define nelem(x) (sizeof(x)/sizeof((x)[0]))

static const long expected_result[nelem(mval)] = {
    0x11110000L,
    0x33332222L,
    0x33332222L,
    0x55554444L,

    0x10000000L,
    0x30002000L,
    0x30002000L,
    0x50004000L,
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
        fprintf(stderr, "%d 0x%04lx  0x%04lx%s\n", i, expected_result[i], mval[i], warn);
    }
}


int main(void) {
    /* run the function to be tested: */
    testmr1();

    compare_result();
    return 0;
}


