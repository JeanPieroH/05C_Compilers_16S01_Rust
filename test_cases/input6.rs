// Programa 6: Contador con bucle while sin usar +=

fn main() {
    let mut i: i32 = 0;

    while i < 5 {
        println!("i = {}", i);
        i = i + 1; // en lugar de i += 1
    }
}
