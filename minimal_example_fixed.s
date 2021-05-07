; This assembly code, after being converted into an object file,
; provides a corrected version of the function's original compiled code.
;
; The changes make sure that w3, when entering the loop .L3,
; is 2 greater than w3 in the original object code.
.section .data
	.global _mval
_mval:
	.word 0xc4fe
	.word 0xc0de
	.word 0x00a5
	.word 0x2222
	.word 0x5678
	.word 0x1234
 	.word 0x0000
	.word 0x0000

.section .text
	.global	_moveright1
	.global	_testmr1

_moveright1:
	dec.w     w1, w2
	bra       N, .L1
	sl.w      w1, #0x2, w3	; using w1 here, instead of w2, so that w3 is greater by 4 initially
	add.w     w0, w3, w3
	inc.w     w1, w1
	sl.w      w1, #0x2, w1
	add.w     w0, w1, w0

.L3:
	mov.w     [--w3], w5	; pre-decrementing, instead of post-decrementing
	mov.w     [--w3], w4  
	mov.d     w4, [--w0]
	dec.w     w2, w2
	add.w     w2, #0x1, [w15]
	bra       NZ, .L3

.L1:
	return    

_testmr1:
	mov.w     #0x2, w1
	mov.w     #_mval+4, w0
	rcall     _moveright1
  	return
