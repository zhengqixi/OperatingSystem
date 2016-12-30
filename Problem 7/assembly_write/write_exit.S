.global _start


.text
_start:
    movq $1, %rax
    movq $1, %rdi
    movq $ass, %rsi
    movq $6, %rdx
    syscall

    movq $60, %rax
    movq $3, %rdi
    syscall

.data
ass: 
    .ascii "sleep\n"
