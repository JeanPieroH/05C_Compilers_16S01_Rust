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
fun_multiply:
  pushq %rbp
  movq %rsp, %rbp
  movsd %xmm0, -8(%rbp)
  movsd %xmm1, -16(%rbp)
  movsd -8(%rbp), %xmm0
  subq $8, %rsp
  movsd %xmm0, (%rsp)
  movsd -16(%rbp), %xmm0
  movsd %xmm0, %xmm1
  movsd (%rsp), %xmm0
  addq $8, %rsp
  mulsd %xmm1, %xmm0
  jmp end_fun_0
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
