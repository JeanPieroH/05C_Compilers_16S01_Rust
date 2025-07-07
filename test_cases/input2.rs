// Programa 2: Operaciones aritméticas y lógicas con tipos explícitos

fn main() {
    let a: i32 = 10;
    let b: i32 = 5;

    let suma: i32 = a + b;
    let resta: i32 = a - b;
    let producto: i32 = a * b;
    let division: i32 = a / b;
    let modulo: i32 = a % b;

    let logico: bool = a > b && b < 10;

    println!("{}, {}, {}, {}, {}, {}", suma, resta, producto, division, modulo, logico);
}
