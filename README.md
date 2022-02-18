This directory contains a minimal working example to demonstrate a behaviour
observable when compiling the example code in [`minimal_example.c`][mwe]
with [xc16][] v1.70 (or v1.50, or v1.41) on a PIC33EP512GM604, or other EP/EV variant.
If compiled with optimization levels -O2 and -Os, the function works as expected.
If compiled using -O1, though, the result seems wrong.

[mwe]:	./mplabx/xc16_int32_indexed_array_access.X/minimal_example.c
[xc16]:	https://www.microchip.com/en-us/development-tools-tools-and-software/mplab-xc-compilers#tabs

**Edit: This problem was the subject of support case 00515171 at Microchip Technology,
opened in May, 2020. It was accepted as issue XC16-1595 in September, 2021;
it is fixed now in XC16 compiler v2.00 from January 2022.**

The function `moveright1` contained in the C source file mentioned above
is a completely stripped-down version of a running median filtering function;
it is a minimal example to demonstrate the behaviour.

Minimal code example, extracted from minimal_example.c:

```C
long mval[4] = {0xc0decafeL, 0x222200a5L, 0x12345678L, 0};

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

void
testmr1(void)
{
	moveright1(&mval[1], 2);
}
```

Compilation command line:

	/opt/microchip/xc16/v1.70/bin/xc16-gcc \
		-O1 \
		-Wall \
		-mcpu=33EP512GM604

The function takes an int32 array `a`,
and moves `n` array elements to the next higher position,
so that a new value could be stored into position a[0].
For an input array as defined above, and e.g. 'n=2',
the following resulting array content can be observed,
if compiled using -O2, and -Os:

(16-bit words in memory order)

            [0]         [1]         [2]         [3]
    before: cafe c0de | 00a5 2222 | 5678 1234 | 0000 0000
                        ----+----   ----+----
                             \           \
                              `----.      `----.
                                    \           \
    after:  cafe c0de | 00a5 2222 | 00a5 2222 | 5678 1234  (OK)
    
For -O1, though, its different, the **resulting array values are incorrect**:
    
            [0]         [1]         [2]         [3]
    before: cafe c0de | 00a5 2222 | 5678 1234 | 0000 0000
                 -----+----- -------+---
                       \             \
                        `---------.   `--------.
                                   \            \
    after:  cafe c0de | 00a5 2222 | c0de 00a5 | 2222 5678  (BAD)

This suggests that in the compiled function **the array index unintendedly gets shifted by two bytes towards a lower memory position**,
so that **words of two adjacent int32-values** --
high-word of a[i-1], low-word of a[i] --
are moved to a new int32 position,
even though the pointer explicitely is of a 32-bit integer type.
I would expect that the function produces the same outcome if compiled with -O1.
The "objdump -S -s" output of the O1-version is (my comments at the end of lines):

	Contents of section .data:
	 0000 fec4dec0 a5002222 78563412 00000000  ......""xV4.....
	
	Disassembly of section .text:
	
	          ; with n=2, w0=4, w1=2 at start of function
	00000000 <_moveright1>:
	                                                ; after:
	   0:	01 01 e9    	dec.w     w1, w2        ; w2=2-1=1
	   2:	0b 00 33    	bra       N, 0x1a <.L1>
	   4:	c2 11 dd    	sl.w      w2, #0x2, w3  ; w3=1<<2= 4
	   6:	83 01 40    	add.w     w0, w3, w3    ; w3=4+4 = 8
	   8:	81 00 e8    	inc.w     w1, w1        ; w1=2+1 = 3
	   a:	c2 08 dd    	sl.w      w1, #0x2, w1  ; w1=3<<2=12
	   c:	01 00 40    	add.w     w0, w1, w0    ; w0=4+12=16
	
	0000000e <.L3>:                  ; w3=8 here, but should be =10!
	   e:	a3 02 78    	mov.w     [w3--], w5	; w5=0x5678; w3=6
	  10:	23 02 78    	mov.w     [w3--], w4    ; w4=0x2222; w3=4
	  12:	04 a0 be    	mov.d     w4, [--w0]    ; w0=10
	  14:	02 01 e9    	dec.w     w2, w2
	  16:	e1 0f 41    	add.w     w2, #0x1, [w15]
	  18:	fa ff 3a    	bra       NZ, 0xe <.L3>
	
	0000001a <.L1>:
	  1a:	00 00 06    	return    
	
	0000001c <_testmr1>:
	  1c:	80 00 78    	mov.w     w0, w1
	  1e:	40 00 20    	mov.w     #0x4, w0
	  20:	ef ff 07    	rcall     0x0 <_moveright1>
	  22:	00 00 06    	return    


The following changes fix the behaviour for the
example function, as I have verified on the target:

At address 4, shift w1, not w2,
to increase the initial value of w3 by 4.

	-   4:	c2 11 dd    	sl.w      w2, #0x2, w3
	+   4:	c2 09 dd    	sl.w      w1, #0x2, w3

As a result, when entering the loop,
(for n=2) w3 is 12, not 8 as in the original version.
Then, in the loop, instead of post-decrementing w3,
the modified version pre-decrements it.
This reduces w3 by 2 before it is dereferenced,
so that actually w3=10 is used to access the first (high-)word to be copied:

	 0000000e <.L3>:
	-   e:	a3 02 78    	mov.w     [w3--], w5
	-  10:	23 02 78    	mov.w     [w3--], w4
	+   e:	c3 02 78    	mov.w     [--w3], w5
	+  10:	43 02 78    	mov.w     [--w3], w4
