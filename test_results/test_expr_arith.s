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
  pushq %rax
  movq $3, %rax
  pushq %rax
  movq $2, %rax
  movq %rax, %rcx
  popq %rax
  imulq %rcx, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  pushq %rax
  movq $8, %rax
  pushq %rax
  movq $4, %rax
  movq %rax, %rcx
  popq %rax
  cqto
  idivq %rcx
  movq %rax, %rcx
  popq %rax
  subq %rcx, %rax
  movq %rax, -8(%rbp)
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
