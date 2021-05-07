#include "minimal_example.h"

// Minimal working/reproducible example showing the behaviour
// described at:
//    https://github.com/knieriem/xc16-i32arridx-corr-mwe
// and
//  Microchip Case 00515171
//
// To observe the behaviour, it is necessary to compile with
// optimization option -O1; other optimization levels won't
// show the behaviour.
//
// Btw, the array index corruption, and thus, memory corruption, occurs
// when compiling for dsPIC33EP512GM604 or other EP/EV MCU variants,
// but NOT for dsPIC33CH512MP506 or FJ MCUs.

long mval[5] = {
	0xc0dec4feL,
	0x222200a5L,
	0x12345678L,
	0,

	/* Memory behind this position will NOT be overwritten
	 * by the minimal example code.
	 */
	0xeeeeffff,
};

void
moveright1(long *a, int n)
{
	int i;

	for (i=n-1; i>=0; i--) {
		/* The loop pointer used in the compiled assembly code
		 * gets corrupted BEFORE the first array access;
		 * it is then not 32-bit aligned anymore, but shifted by 16 bit.
		 * Thus, a[i] will contain a wrong value composed of parts of
		 * two adjacing 32-bit array elements.
		 * The upper array boundary is NOT exceeded at any time.
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
