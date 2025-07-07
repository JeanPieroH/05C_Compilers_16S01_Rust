// Test para la asignación a elementos de array
// <Asignacion> ::= <AccesoArray> = <CExp>
fn main() {
    let mut data: [i64; 4] = [0, 0, 0, 0];
    data[0] = 10;
    data[1] = 20 + 5;

    let idx: i64 = 2;
    data[idx] = 30;
    data[idx + 1] = data[0] * 2; // data[3] = 10 * 2 = 20
}
