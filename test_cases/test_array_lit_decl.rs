fn main() {
    // Declaración e inicialización con un literal de array.
    // El tipo del array se infiere del tipo de la expresión (el literal).
    // La gramática de DeclaracionVariable es:
    // let [mut] <Identificador> [: <Tipo> | [: [<Tipo>; <LiteralEntero>]]] [= <CExp>];
    // Si no se especifica :<Tipo> o :[<Tipo>; <LitEnt>], el tipo se deriva de <CExp>.
    let arr_a = [1, 2, 3, 4];      // El parser debería crear un DecStament sin type_name explícito.
    let arr_b: [i64; 3] = [5, 6, 7]; // Tipo explícito y literal.
    let arr_c = [true, false];

    let mut arr_d = [10, 20];
    arr_d = [30, 40]; // Esto es una asignación (Identificador = CExp)
}
