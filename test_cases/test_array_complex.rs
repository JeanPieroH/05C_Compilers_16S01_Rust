// Combinación de declaraciones, literales, acceso y asignaciones.
// También uso en estructuras de control si es posible.
fn main() {
    let mut numbers: [i64; 5] = [1, 2, 3, 4, 5];
    let temp_idx: i64 = 2;

    numbers[0] = numbers[1] + numbers[temp_idx]; // numbers[0] = 2 + 3 = 5
    numbers[4] = numbers[3] - numbers[2];     // numbers[4] = 4 - 3 = 1 (original val)
                                                // numbers[4] = 4 - numbers[temp_idx] (que es 3) = 1

    if numbers[0] > numbers[4] { // 5 > 1
        let temp_arr: [i64; 2] = [numbers[0], numbers[4]];
        numbers[1] = temp_arr[0] + temp_arr[1]; // numbers[1] = 5 + 1 = 6
    }

    // println!("Array: [{}, {}, {}, {}, {}]", numbers[0], numbers[1], numbers[2], numbers[3], numbers[4]);
    // Esperado: Array: [5, 6, 3, 4, 1]
}
