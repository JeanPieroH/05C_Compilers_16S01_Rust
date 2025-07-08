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
.data
  .align 8
L1: .double 10.00000000000000000
.text
  movsd L1(%rip), %xmm0
  movsd %xmm0, -8(%rbp)
.data
  .align 8
L2: .double 20.50000000000000000
.text
  movsd L2(%rip), %xmm0
  movsd %xmm0, -16(%rbp)
  movsd -8(%rbp), %xmm0
  subq $8, %rsp
  movsd %xmm0, (%rsp)
  movsd -16(%rbp), %xmm0
  movsd %xmm0, %xmm1
  movsd (%rsp), %xmm0
  addq $8, %rsp
  comisd %xmm1, %xmm0
  seta %al
  movzbl %al, %eax
  movq %rax, -24(%rbp)
  movsd -8(%rbp), %xmm0
  subq $8, %rsp
  movsd %xmm0, (%rsp)
  movsd -16(%rbp), %xmm0
  movsd %xmm0, %xmm1
  movsd (%rsp), %xmm0
  addq $8, %rsp
  comisd %xmm1, %xmm0
  setb %al
  movzbl %al, %eax
  movq %rax, -32(%rbp)
  movsd -8(%rbp), %xmm0
  subq $8, %rsp
  movsd %xmm0, (%rsp)
.data
  .align 8
L3: .double 10.00000000000000000
.text
  movsd L3(%rip), %xmm0
  movsd %xmm0, %xmm1
  movsd (%rsp), %xmm0
  addq $8, %rsp
  comisd %xmm1, %xmm0
  setae %al
  movzbl %al, %eax
  movq %rax, -40(%rbp)
  movsd -16(%rbp), %xmm0
  subq $8, %rsp
  movsd %xmm0, (%rsp)
.data
  .align 8
L4: .double 20.50000000000000000
.text
  movsd L4(%rip), %xmm0
  movsd %xmm0, %xmm1
  movsd (%rsp), %xmm0
  addq $8, %rsp
  comisd %xmm1, %xmm0
  setbe %al
  movzbl %al, %eax
  movq %rax, -48(%rbp)
  movsd -8(%rbp), %xmm0
  subq $8, %rsp
  movsd %xmm0, (%rsp)
  movsd -16(%rbp), %xmm0
  movsd %xmm0, %xmm1
  movsd (%rsp), %xmm0
  addq $8, %rsp
  comisd %xmm1, %xmm0
  sete %al
  movzbl %al, %eax
  movq %rax, -56(%rbp)
  movsd -8(%rbp), %xmm0
  subq $8, %rsp
  movsd %xmm0, (%rsp)
.data
  .align 8
L5: .double 10.00000000000000000
.text
  movsd L5(%rip), %xmm0
  movsd %xmm0, %xmm1
  movsd (%rsp), %xmm0
  addq $8, %rsp
  comisd %xmm1, %xmm0
  setne %al
  movzbl %al, %eax
  movq %rax, -64(%rbp)
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
