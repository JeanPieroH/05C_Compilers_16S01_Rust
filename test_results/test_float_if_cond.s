.data
print_str_fmt: .string "%s"
print_int_fmt: .string "%ld"
print_i32_fmt: .string "%d"
print_float_fmt: .string "%.17g"
print_f32_fmt: .string "%.7g"
print_newline: .string "\n"
.LC_array_placeholder: .string "[Array]"
.text
.globl main
fun_check_val:
  pushq %rbp
  movq %rsp, %rbp
  movsd %xmm0, -8(%rbp)
  movsd -8(%rbp), %xmm0
  subq $8, %rsp
  movsd %xmm0, (%rsp)
.data
  .align 8
L3: .double 0.00000000000000000
.text
  movsd L3(%rip), %xmm0
  movsd %xmm0, %xmm1
  movsd (%rsp), %xmm0
  addq $8, %rsp
  comisd %xmm1, %xmm0
  seta %al
  movzbl %al, %eax
  cmpl $0, %eax
  je else_1
  movq $1, %rax
  movq %rax, -16(%rbp)
  jmp endif_2
else_1:
  movsd -8(%rbp), %xmm0
  subq $8, %rsp
  movsd %xmm0, (%rsp)
.data
  .align 8
L6: .double 0.00000000000000000
.text
  movsd L6(%rip), %xmm0
  movsd %xmm0, %xmm1
  movsd (%rsp), %xmm0
  addq $8, %rsp
  comisd %xmm1, %xmm0
  setb %al
  movzbl %al, %eax
  cmpl $0, %eax
  je else_4
  movq $1, %rax
  movq %rax, -24(%rbp)
  jmp endif_5
else_4:
  movq $1, %rax
  movq %rax, -32(%rbp)
endif_5:
endif_2:
end_fun_0:
  leave
  ret
main:
  pushq %rbp
  movq %rsp, %rbp
  # No se encontró una función 'main' definida por el usuario.
  movl $0, %eax
  leave
  ret
.section .note.GNU-stack,"",@progbits
