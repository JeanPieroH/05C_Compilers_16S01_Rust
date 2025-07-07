fn modify(v: &mut i64, k: i64) {
    let x:i64 = k; // No podemos hacer *v = k con la gramática actual
                  // pero parsear la firma es lo importante aquí.
}

fn main() {
    let mut x: i64 = 10;
    modify(&mut x, 5);
}
