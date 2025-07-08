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
  movq $40, %rdi
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
  movq $5, %rax
  movq %rax, 32(%rbx)
  movq %rbx, %rax
# Advertencia: Declaración de tipo simple numbers:i64 inicializada con un array. Se asume tipo array.
  movq %rax, -8(%rbp)
  movq $2, %rax
  movq %rax, -16(%rbp)
  movq -8(%rbp), %rbx
  movq $0, %rax
  movq %rax, %rcx
  movq -8(%rbp), %rbx
  movq $1, %rax
  movq %rax, %rcx
  movq $8, %rdi
  imulq %rdi, %rcx
  addq %rbx, %rcx
  movq (%rcx), %rax
  pushq %rax
  movq -8(%rbp), %rbx
  movq -16(%rbp), %rax
  movq %rax, %rcx
  movq $8, %rdi
  imulq %rdi, %rcx
  addq %rbx, %rcx
  movq (%rcx), %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq $8, %rdi
  imulq %rdi, %rcx
  addq %rbx, %rcx
  movq %rax, (%rcx)
  movq -8(%rbp), %rbx
  movq $4, %rax
  movq %rax, %rcx
  movq -8(%rbp), %rbx
  movq $3, %rax
  movq %rax, %rcx
  movq $8, %rdi
  imulq %rdi, %rcx
  addq %rbx, %rcx
  movq (%rcx), %rax
  pushq %rax
  movq -8(%rbp), %rbx
  movq $2, %rax
  movq %rax, %rcx
  movq $8, %rdi
  imulq %rdi, %rcx
  addq %rbx, %rcx
  movq (%rcx), %rax
  movq %rax, %rcx
  popq %rax
  subq %rcx, %rax
  movq $8, %rdi
  imulq %rdi, %rcx
  addq %rbx, %rcx
  movq %rax, (%rcx)
  movq -8(%rbp), %rbx
  movq $0, %rax
  movq %rax, %rcx
  movq $8, %rdi
  imulq %rdi, %rcx
  addq %rbx, %rcx
  movq (%rcx), %rax
  pushq %rax
  movq -8(%rbp), %rbx
  movq $4, %rax
  movq %rax, %rcx
  movq $8, %rdi
  imulq %rdi, %rcx
  addq %rbx, %rcx
  movq (%rcx), %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  setg %al
  movzbl %al, %eax
  cmpl $0, %eax
  je else_1
  movq -8(%rbp), %rbx
  movq $0, %rax
  movq %rax, %rcx
  movq $8, %rdi
  imulq %rdi, %rcx
  addq %rbx, %rcx
  movq (%rcx), %rax
  movq $16, %rdi
  call malloc@PLT
  movq %rax, %rbx
  movq -8(%rbp), %rbx
  movq $0, %rax
  movq %rax, %rcx
  movq $8, %rdi
  imulq %rdi, %rcx
  addq %rbx, %rcx
  movq (%rcx), %rax
  movq %rax, 0(%rbx)
  movq -8(%rbp), %rbx
  movq $4, %rax
  movq %rax, %rcx
  movq $8, %rdi
  imulq %rdi, %rcx
  addq %rbx, %rcx
  movq (%rcx), %rax
  movq %rax, 8(%rbx)
  movq %rbx, %rax
# Advertencia: Declaración de tipo simple temp_arr:i64 inicializada con un array. Se asume tipo array.
  movq %rax, -24(%rbp)
  movq -8(%rbp), %rbx
  movq $1, %rax
  movq %rax, %rcx
  movq -24(%rbp), %rbx
  movq $0, %rax
  movq %rax, %rcx
  movq $8, %rdi
  imulq %rdi, %rcx
  addq %rbx, %rcx
  movq (%rcx), %rax
  pushq %rax
  movq -24(%rbp), %rbx
  movq $1, %rax
  movq %rax, %rcx
  movq $8, %rdi
  imulq %rdi, %rcx
  addq %rbx, %rcx
  movq (%rcx), %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq $8, %rdi
  imulq %rdi, %rcx
  addq %rbx, %rcx
  movq %rax, (%rcx)
  jmp endif_2
else_1:
endif_2:
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
