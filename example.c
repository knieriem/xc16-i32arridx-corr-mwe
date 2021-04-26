#include "example.h"

// Minimal working/reproducible example showing the behaviour
// described at:
//    https://dl.t9rsys.com/xc16_int32_indexed_array_access.html
// and
//  Microchip Case 00515171
//
// Compile with optimization option -O1
// Btw, the memory corruption occurs when compiling for
// dsPIC33EP512GM604, but NOT for dsPIC33CH512MP506.

long mval[8] = {
    0x11110000L,
    0x33332222L,
    0x55554444L,
    0x77776666L,

    0x10000000L,
    0x30002000L,
    0x30002000L,
    0x50004000L,
};

void
moveright1(long *a, int n)
{
	int i;

	for (i=n-1; i>=0; i--) {
        /* the a[i] expression will yield the wrong value,
         * even if a's array boundary is NOT exceeded
         */
		a[i+1] = a[i];
	}
}


/* Function testmr1 only exists to make sure
 * that the minimal working example, which is
 * needed so that the behaviour can be observed,
 * builds as expected.
 * 
 * Moveright1 is called with n=2 in this example,
 * thus taking care NOT to exceed the upper boundary of mval.
 * No element behind mval[3] will be accessed.
 * 
 * But the memory gets corrupted anyway.
 * See "File Registers" @1000..100E, when debugging
 * in the simulator. 
 */
void
testmr1(void)
{
	moveright1(&mval[1], 2);
}
