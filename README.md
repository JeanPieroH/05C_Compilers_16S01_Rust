# Rust-Compiler
Este proyecto compila e interpreta archivos `.rs` escritos en un lenguaje de programación simple (tipo IMP). Utiliza un compilador e intérprete escritos en C++.

## 📄 Link Utiles
Solo temporal para acceder rapidamente
- [Ver informe en Overleaf](https://es.overleaf.com/project/6841dea025afa05f6e19e493)
- [Compilador de Rust online](https://onecompiler.com/rust)
  
## 📁 Estructura del proyecto

```plaintext
📦 rust/
├── 📁 compiler/             # Archivos fuente del compilador (.cpp y .hh)
├── 📁 test_cases/           # Casos de prueba escritos en Rust (.rs)
├── 📁 logs/                 # Registros completos de ejecuciones del programa (.log)
├── 📁 outputs/              # Resultados de salida del intérprete (.s)
├── 📄 make.py               # Script principal para compilar y correr los casos de prueba
└── 📄 README.md             # Documentación del proyecto (este archivo)
```
## 🚀 Cómo ejecutar

Sigue estos sencillos pasos para compilar y probar tus programas:

1.  Asegúrate de que todos los archivos fuente de C++ estén en la carpeta `compiler/` y tus casos de prueba en `.rs` en `test_cases/`.
2.  Abre tu terminal o línea de comandos en la raíz del proyecto y ejecuta el script de Python:

```bash
python make.py
```

Este script se encargará de compilar todos los archivos `.cpp` en tu directorio `compiler/` y luego ejecutará el binario resultante sobre cada archivo `.rs` encontrado en `test_cases/`.

---

## 📤 Salidas

El script generará varias salidas para que puedas seguir el proceso de compilación e interpretación:

* **Salida completa del programa**: La información detallada del scanner, parser, visitor e intérprete se guarda en archivos con la siguiente ruta:
    ```bash
    logs/<nombre_archivo>.log
    ```

* **Salida del intérprete**: El código ensamblador generado (o la salida final del intérprete, si no genera ASM) se guarda en:
    ```php-template
    outputs/<nombre_archivo>.s
    ```

---