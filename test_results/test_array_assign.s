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
  movq $0, %rax
  movq $32, %rdi
  call malloc@PLT
  movq %rax, %rbx
  movq $0, %rax
  movq %rax, 0(%rbx)
  movq $0, %rax
  movq %rax, 8(%rbx)
  movq $0, %rax
  movq %rax, 16(%rbx)
  movq $0, %rax
  movq %rax, 24(%rbx)
  movq %rbx, %rax
# Advertencia: Declaración de tipo simple data:i64 inicializada con un array. Se asume tipo array.
  movq %rax, -8(%rbp)
  movq -8(%rbp), %rbx
  movq $0, %rax
  movq %rax, %rcx
  movq $10, %rax
  movq $8, %rdi
  imulq %rdi, %rcx
  addq %rbx, %rcx
  movq %rax, (%rcx)
  movq -8(%rbp), %rbx
  movq $1, %rax
  movq %rax, %rcx
  movq $20, %rax
  pushq %rax
  movq $5, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq $8, %rdi
  imulq %rdi, %rcx
  addq %rbx, %rcx
  movq %rax, (%rcx)
  movq $2, %rax
  movq %rax, -16(%rbp)
  movq -8(%rbp), %rbx
  movq -16(%rbp), %rax
  movq %rax, %rcx
  movq $30, %rax
  movq $8, %rdi
  imulq %rdi, %rcx
  addq %rbx, %rcx
  movq %rax, (%rcx)
  movq -8(%rbp), %rbx
  movq -16(%rbp), %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, %rcx
  movq -8(%rbp), %rbx
  movq $0, %rax
  movq %rax, %rcx
  movq $8, %rdi
  imulq %rdi, %rcx
  addq %rbx, %rcx
  movq (%rcx), %rax
  pushq %rax
  movq $2, %rax
  movq %rax, %rcx
  popq %rax
  imulq %rcx, %rax
  movq $8, %rdi
  imulq %rdi, %rcx
  addq %rbx, %rcx
  movq %rax, (%rcx)
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
