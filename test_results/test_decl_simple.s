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
fun_main:
  pushq %rbp
  movq %rsp, %rbp
  movq $5, %rax
  movq %rax, -8(%rbp)
  movq $1, %rax
  movq %rax, -16(%rbp)
.data
  .align 8
L1: .double 3.14000000000000012
.text
  movsd L1(%rip), %xmm0
  movsd %xmm0, -24(%rbp)
end_fun_0:
  leave
  ret
main:
  pushq %rbp
  movq %rsp, %rbp
  call fun_main
  movl $0, %eax
  leave
  ret
.section .note.GNU-stack,"",@progbits
