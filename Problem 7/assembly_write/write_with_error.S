.global _start


.text
_start:
    movq $1, %rax
    movq $1, %rdi
    movq $ass, %rsi
    movq $50, %rdx
    syscall

.data
ass: 
    .ascii "sleep\n"
