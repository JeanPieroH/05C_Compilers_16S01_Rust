// Programa 8: Muestra los elementos de un array y sus índices sin usar .len()

fn main() {
    let numeros: [f64; 4] = [10.5, 20.0, 30.2, 40.8];

    for i in 0..4 {
        println!("Elemento {}: {}", i, numeros[i]);
    }
}
