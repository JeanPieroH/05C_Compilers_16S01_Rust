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
  movq $1, %rax
  movq $32, %rdi
  call malloc@PLT
  movq %rax, %rbx
  movq $1, %rax
  movq %rax, 0(%rbx)
  movq $2, %rax
  movq %rax, 8(%rbx)
  movq $3, %rax
  movq %rax, 16(%rbx)
  movq $4, %rax
  movq %rax, 24(%rbx)
  movq %rbx, %rax
  movq %rax, -8(%rbp)
  movq $5, %rax
  movq $24, %rdi
  call malloc@PLT
  movq %rax, %rbx
  movq $5, %rax
  movq %rax, 0(%rbx)
  movq $6, %rax
  movq %rax, 8(%rbx)
  movq $7, %rax
  movq %rax, 16(%rbx)
  movq %rbx, %rax
# Advertencia: Declaración de tipo simple arr_b:i64 inicializada con un array. Se asume tipo array.
  movq %rax, -16(%rbp)
  movq $1, %rax
  movq $16, %rdi
  call malloc@PLT
  movq %rax, %rbx
  movq $1, %rax
  movq %rax, 0(%rbx)
  movq $0, %rax
  movq %rax, 8(%rbx)
  movq %rbx, %rax
  movq %rax, -24(%rbp)
  movq $10, %rax
  movq $16, %rdi
  call malloc@PLT
  movq %rax, %rbx
  movq $10, %rax
  movq %rax, 0(%rbx)
  movq $20, %rax
  movq %rax, 8(%rbx)
  movq %rbx, %rax
  movq %rax, -32(%rbp)
  movq $30, %rax
  movq $16, %rdi
  call malloc@PLT
  movq %rax, %rbx
  movq $30, %rax
  movq %rax, 0(%rbx)
  movq $40, %rax
  movq %rax, 8(%rbx)
  movq %rbx, %rax
  movq %rax, -32(%rbp)
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
