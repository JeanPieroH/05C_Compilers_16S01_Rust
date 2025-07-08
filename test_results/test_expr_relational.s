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
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  setg %al
  movzbl %al, %eax
  pushq %rax
  movq $10, %rax
  pushq %rax
  movq $20, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  setle %al
  movzbl %al, %eax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  sete %al
  movzbl %al, %eax
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
