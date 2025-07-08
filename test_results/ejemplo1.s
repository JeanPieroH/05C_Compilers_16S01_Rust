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
  movl $0, -4(%rbp)
  movl $0, -8(%rbp)
  movq $0, -16(%rbp)
  movq $1, %rax
  movl %eax, -4(%rbp)
  movq $10, %rax
  movl %eax, -8(%rbp)
  movq $1000000, %rax
  movq %rax, -16(%rbp)
  movq $20, %rax
  movl %eax, -4(%rbp)
  movl -4(%rbp), %eax
  movslq %eax, %rsi
  leaq print_i32_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  leaq print_newline(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  movl -8(%rbp), %eax
  movslq %eax, %rsi
  leaq print_i32_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  leaq print_newline(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  movq -16(%rbp), %rax
  movq %rax, %rsi
  leaq print_int_fmt(%rip), %rdi
  movl $0, %eax
  call printf@PLT
  leaq print_newline(%rip), %rdi
  movl $0, %eax
  call printf@PLT
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
