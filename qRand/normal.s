  .text
  .globl _Z6normalmmmmmPd
  .type _Z6normalmmmmmPd @function
_Z6normalmmmmmPd:
  rclb $0x10, %dl
  notb %dl
  movq %rdx, %xmm0
  cmpw $0x40, %di
  pmovsxbw %xmm0, %xmm5
  paddusw %xmm0, %xmm0
  cvtdq2pd %xmm5, %xmm15
  sarl $0x10, %ecx
  paddd %xmm15, %xmm0
  subpd %xmm15, %xmm0
  movq %rsi, %r8
  xchgb %r8b, %sil
  retq 
  .size _Z6normalmmmmmPd, .-_Z6normalmmmmmPd
