// Test para el acceso a elementos de array
// <AccesoArray> ::= <Identificador> [ <CExp> ]
// Esto se usa en <Factor> y por tanto puede ser parte de una <CExp>
// o en el lado izquierdo de una <Asignacion>
fn main() {
    let my_array: [i64; 3] = [10, 20, 30];
    let first_val: i64 = my_array[0];
    let second_val: i64 = my_array[1];
    let third_val: i64 = my_array[2];

    // Acceso dentro de una expresión más compleja
    let sum_vals: i64 = my_array[0] + my_array[1] + my_array[2];

    // let complex_access: i64 = my_array[1 + 1 - 1]; // Acceso con CExp como índice
                                                 // Esto es my_array[my_array[0]/10] si my_array[0] es 10.
    let idx: i64 = 1;
    let indexed_access: i64 = my_array[idx];

    // println!("Val: {}", my_array[0]); // Uso en println!
}
