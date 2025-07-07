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

# Ejecutar sobre cada input y guardar output
for input_file in input_files:
    input_name = os.path.basename(input_file)
    output_file = os.path.join(output_dir, f"{input_name}")

    print(f"Ejecutando con {input_name}")
    subprocess.run([f"./{exe_name}", input_file])
