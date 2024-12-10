.global _start
_start:
cld
cli
movl $010000,%eax
mov %ax,%ds
mov %ax,%es
mov %ax,%fs
mov %ax,%gs

push $0
popf


inf: jmp inf
