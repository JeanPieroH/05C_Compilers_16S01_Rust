// Programa 1: Declaración explícita de tipos y mutación de una variable booleana

fn main() {
    let x: i64 = 42;         // Entero de 32 bits con signo
    let y: f64 = 3.14;       // Número flotante de 64 bits
    let mut z: bool = true;  // Booleano mutable

    println!("x = {}, y = {}, z = {}", x, y, z);

    z = false; // Cambio del valor booleano
    println!("z now is {}", z);
}
