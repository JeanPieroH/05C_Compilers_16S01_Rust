fn busqueda_binaria(arr: [i32; 5], objetivo: i32) -> i32 {
    let mut izquierda: i32 = 0;
    let mut derecha: i32 = 4;

    while izquierda <= derecha {
        let medio: i32 = (izquierda + derecha) / 2;
        let valor: i32 = arr[medio];

        if valor == objetivo {
            return medio;
        } else if valor < objetivo {
            izquierda = medio + 1;
        } else {
            derecha = medio - 1;
        }
    }

    return -1;
}

fn main() {
    let datos: [i32; 5] = [5, 10, 15, 20, 25];
    let valor_a_buscar: i32 = 15;
    let resultado: i32 = busqueda_binaria(datos, valor_a_buscar);

    if resultado != -1 {
        println!("Encontrado en posicion {}", resultado);
    } else {
        println!("No encontrado");
    }
}
0