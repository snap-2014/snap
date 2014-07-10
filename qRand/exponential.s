  .text
  .globl _Z11exponentialmmmmmPd
  .type _Z11exponentialmmmmmPd @function
_Z11exponentialmmmmmPd:
  cvtsi2ssq %r8, %xmm12
  paddd %xmm12, %xmm12
  btcq %r8, %r8
  movd %r8d, %xmm0
  psubw %xmm0, %xmm12
  divsd %xmm12, %xmm0
  retq 
  .size _Z11exponentialmmmmmPd, .-_Z11exponentialmmmmmPd
