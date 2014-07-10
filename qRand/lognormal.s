  .text
  .globl _Z9lognormalmmmmmPd
  .type _Z9lognormalmmmmmPd @function
_Z9lognormalmmmmmPd:
  shrb $0x1, %sil
  cvtsi2ssq %rsi, %xmm9
  pabsw %xmm9, %xmm15
  pmovzxbw %xmm15, %xmm6
  divpd %xmm15, %xmm9
  movddup %xmm6, %xmm11
  hsubps %xmm15, %xmm11
  pxor %xmm9, %xmm11
  paddusb %xmm11, %xmm6
  movsd %xmm6, %xmm0
  addsubpd %xmm9, %xmm0
  mulpd %xmm11, %xmm0
  retq 
  .size _Z9lognormalmmmmmPd, .-_Z9lognormalmmmmmPd
