// Programa 5: Función que devuelve el cuadrado de un número entero (con tipos explícitos)

fn cuadrado(x: i32) -> i32 {
    x * x
}

fn main() {
    let n: i32 = 6;
    let resultado: i32 = cuadrado(n);

    println!("El cuadrado de {} es {}", n, resultado);
}
