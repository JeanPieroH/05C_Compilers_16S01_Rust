// Usando float en condiciones de if
fn check_val(v: f64) {
    if v > 0.0 {
        let is_positive: bool = true;
    } else {
        if v < 0.0 {
            let is_negative: bool = true;
        } else {
            let is_zero: bool = true;
        }
    }
}

fn main() {
    check_val(3.14);
    check_val(-1.0);
    check_val(0.0);
}
