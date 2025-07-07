fn main() {
    let arr1: [i64; 3]; // Declaración de array de 3 i64 sin inicialización.
    // arr1 = [1, 2, 3]; // Esto debería ser una asignación válida si 'arr1' es un identificador
                       // y '[1,2,3]' es una CExp. El parser debería manejarlo.

    let arr2: [i64; 3] = [10, 20, 30]; // Inicialización directa con literal.

    let mut arr3: [bool; 2];
    // arr3 = [true, false]; // Similar a arr1, esto es una asignación.
                           // Para probar la declaración con tipo y tamaño, la inicialización
                           // es la forma más directa según la gramática de DeclaracionVariable.
                           // Dejaré estas asignaciones para test_array_assign.rs o test_array_lit_decl.rs

    let arr4: [f64; 2] = [1.2, 3.4];
}
