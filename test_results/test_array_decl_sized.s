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
  movq $0, -8(%rbp)
  movq $10, %rax
  movq $24, %rdi
  call malloc@PLT
  movq %rax, %rbx
  movq $10, %rax
  movq %rax, 0(%rbx)
  movq $20, %rax
  movq %rax, 8(%rbx)
  movq $30, %rax
  movq %rax, 16(%rbx)
  movq %rbx, %rax
# Advertencia: Declaración de tipo simple arr2:i64 inicializada con un array. Se asume tipo array.
  movq %rax, -16(%rbp)
  movq $0, -24(%rbp)
.data
  .align 8
L1: .double 1.19999999999999996
.text
  movsd L1(%rip), %xmm0
  movq $16, %rdi
  call malloc@PLT
  movq %rax, %rbx
.data
  .align 8
L2: .double 1.19999999999999996
.text
  movsd L2(%rip), %xmm0
  movsd %xmm0, 0(%rbx)
.data
  .align 8
L3: .double 3.39999999999999991
.text
  movsd L3(%rip), %xmm0
  movsd %xmm0, 8(%rbx)
  movq %rbx, %rax
# Advertencia: Declaración de tipo simple arr4:f64 inicializada con un array. Se asume tipo array.
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
