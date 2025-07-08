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
  pushq %rax
  movq $0, %rax
  pushq %rax
  movq $0, %rax
  cmpq $0, %rax
  sete %al
  movzbl %al, %eax
  movq %rax, %rcx
  popq %rax
  cmpq $0,%rax
  jne L1
  movq %rcx,%rax
  jmp L2
L1:
  movq $1,%rax
L2:
  cmpq $0,%rax
  setne %al
  movzbl %al,%eax
  movq %rax, %rcx
  popq %rax
  cmpq $0,%rax
  je L3
  movq %rcx,%rax
  jmp L4
L3:
  movq $0,%rax
L4:
  cmpq $0,%rax
  setne %al
  movzbl %al,%eax
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
