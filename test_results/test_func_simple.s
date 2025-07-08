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
fun_foo:
  pushq %rbp
  movq %rsp, %rbp
  movq $1, %rax
  movq %rax, -8(%rbp)
end_fun_0:
  leave
  ret
main:
  pushq %rbp
  movq %rsp, %rbp
  # No se encontró una función 'main' definida por el usuario.
  movl $0, %eax
  leave
  ret
.section .note.GNU-stack,"",@progbits
