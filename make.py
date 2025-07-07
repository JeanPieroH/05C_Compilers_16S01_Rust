import subprocess
import os
import glob
import sys

# Ruta base del script
base_dir = os.path.dirname(os.path.abspath(__file__))

# Carpeta con archivos de entrada
input_dir = os.path.join(base_dir, "test_cases")

# Carpeta para guardar salidas
output_dir = os.path.join(base_dir, "outputs")
os.makedirs(output_dir, exist_ok=True)

# Archivos fuente (.cpp)
source_files = glob.glob(os.path.join(base_dir, "compiler", "*.cpp"))

if not source_files:
    print("No se encontraron archivos .cpp")
    sys.exit(1)

# Ejecutable
exe_name = "compiler_rust.exe" if os.name == "nt" else "compiler_rust.out"

# Compilar
print("Compilando...")
compile_cmd = ["g++"] + source_files + ["-o", exe_name]
result = subprocess.run(compile_cmd)

if result.returncode != 0:
    print("Error de compilación.")
    sys.exit(1)

print("Compilación exitosa.\n")

# Buscar archivos .rs de entrada
input_files = sorted(glob.glob(os.path.join(input_dir, "*.rs")))

if not input_files:
    print("No se encontraron archivos de entrada.")
    sys.exit(1)

# Opcional: Limpiar directorio de salidas antiguas
print(f"Limpiando directorio de salidas antiguas: {output_dir}")
for old_output_file in glob.glob(os.path.join(output_dir, "*.rs")):
    try:
        os.remove(old_output_file)
    except OSError as e:
        print(f"Error eliminando archivo {old_output_file}: {e.strerror}")
print("Directorio de salidas limpiado.\n")

# Ejecutar sobre cada input y guardar output
for input_file_path in input_files:
    input_name = os.path.basename(input_file_path)
    # Usar una extensión como .rs.out para los archivos de salida
    output_file_path = os.path.join(output_dir, f"{input_name}.out")

    print(f"Ejecutando compilador con: {input_name} -> Guardando salida en: {output_file_path}")

    try:
        # Ejecutar el compilador y capturar su salida
        result = subprocess.run(
            [f"./{exe_name}", input_file_path],
            capture_output=True,
            text=True,
            check=False # No lanzar excepción automáticamente si el compilador falla
        )

        # Guardar stdout a archivo de salida
        with open(output_file_path, "w", encoding="utf-8") as f_out:
            f_out.write(f"--- Ejecutando con: {input_name} ---\n")
            f_out.write(f"--- Salida Estándar (stdout) ---\n")
            f_out.write(result.stdout)
            f_out.write("\n") # Asegurar una nueva línea al final del stdout

            if result.stderr:
                f_out.write(f"--- Salida de Error (stderr) ---\n")
                f_out.write(result.stderr)
                f_out.write("\n") # Asegurar una nueva línea al final del stderr

            if result.returncode != 0:
                f_out.write(f"--- El compilador terminó con código de error: {result.returncode} ---\n")
                print(f"ATENCIÓN: El compilador falló para {input_name} (código: {result.returncode}). Ver {output_file_path} para detalles.")
            else:
                print(f"Ejecución de {input_name} completada exitosamente.")

    except Exception as e:
        print(f"Error ejecutando el compilador para {input_name}: {e}")
        # También guardar este error en el archivo de salida si es posible
        with open(output_file_path, "w", encoding="utf-8") as f_out:
            f_out.write(f"--- Error al ejecutar el script de prueba para: {input_name} ---\n")
            f_out.write(str(e))

    print("-" * 30)

print("\nTodas las ejecuciones de test cases completadas.")
print(f"Las salidas se encuentran en el directorio: {output_dir}")
