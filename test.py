import os
import glob
import sys
import subprocess

def read_file_content(file_path):
    """Lee el contenido de un archivo y maneja errores."""
    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            return file.read().strip()
    except FileNotFoundError:
        return None
    except Exception as e:
        print(f"Error leyendo {file_path}: {e}")
        return None

def compare_files(file1_path, file2_path):
    """Compara el contenido de dos archivos."""
    content1 = read_file_content(file1_path)
    content2 = read_file_content(file2_path)
    
    if content1 is None or content2 is None:
        return False
    
    return content1 == content2

def run_make_script():
    """Ejecuta el archivo make.py"""
    base_dir = os.path.dirname(os.path.abspath(__file__))
    make_script = os.path.join(base_dir, "make.py")
    
    if not os.path.exists(make_script):
        print(f"Error: No existe el archivo {make_script}")
        return False
    
    print("=== EJECUTANDO MAKE.PY ===")
    try:
        result = subprocess.run([sys.executable, make_script], 
                              capture_output=True, text=True)
        
        if result.returncode == 0:
            print("✅ make.py ejecutado exitosamente")
            if result.stdout:
                print("Salida:", result.stdout)
            return True
        else:
            print("❌ Error ejecutando make.py")
            if result.stderr:
                print("Error:", result.stderr)
            return False
            
    except Exception as e:
        print(f"❌ Excepción ejecutando make.py: {e}")
        return False

def main():
    # Ejecutar make.py primero
    if not run_make_script():
        print("Abortando debido a errores en make.py")
        sys.exit(1)
    
    print("\n" + "="*50)
    print("=== INICIANDO COMPARACIÓN DE ARCHIVOS ===")
    print("="*50 + "\n")
    # Ruta base del script
    base_dir = os.path.dirname(os.path.abspath(__file__))
    
    # Carpetas de salidas generadas y resultados esperados
    outputs_dir = os.path.join(base_dir, "outputs")
    test_results_dir = os.path.join(base_dir, "test_results")
    
    # Verificar que las carpetas existen
    if not os.path.exists(outputs_dir):
        print(f"Error: No existe la carpeta {outputs_dir}")
        sys.exit(1)
    
    if not os.path.exists(test_results_dir):
        print(f"Error: No existe la carpeta {test_results_dir}")
        sys.exit(1)
    
    # Obtener todos los archivos de outputs
    output_files = glob.glob(os.path.join(outputs_dir, "*"))
    
    if not output_files:
        print("No se encontraron archivos en outputs")
        sys.exit(1)
    
    # Contadores para estadísticas
    total_tests = 0
    passed_tests = 0
    failed_tests = 0
    missing_results = 0
    
    print("=== COMPARACIÓN DE ARCHIVOS DE PRUEBA ===\n")
    
    # Procesar cada archivo de outputs
    for output_file in sorted(output_files):
        file_name = os.path.basename(output_file)
        expected_result_file = os.path.join(test_results_dir, file_name)
        
        total_tests += 1
        
        print(f"Procesando: {file_name}")
        
        # Verificar si existe el archivo de resultado esperado
        if not os.path.exists(expected_result_file):
            print(f"  ❌ FALTA: No existe {file_name} en test_results")
            missing_results += 1
            continue
        
        # Comparar contenidos
        if compare_files(output_file, expected_result_file):
            print(f"  ✅ PASS: Los contenidos coinciden")
            passed_tests += 1
        else:
            print(f"  ❌ FAIL: Los contenidos difieren")
            failed_tests += 1
            
            # Mostrar diferencias básicas
            content1 = read_file_content(output_file)
            content2 = read_file_content(expected_result_file)
            
            if content1 is not None and content2 is not None:
                print(f"    outputs/{file_name} tiene {len(content1)} caracteres")
                print(f"    test_results/{file_name} tiene {len(content2)} caracteres")
        
        print()
    
    # Mostrar resumen
    print("=== RESUMEN ===")
    print(f"Total de archivos procesados: {total_tests}")
    print(f"Pruebas exitosas: {passed_tests}")
    print(f"Pruebas fallidas: {failed_tests}")
    print(f"Archivos faltantes en test_results: {missing_results}")
    
    # Calcular porcentaje de éxito
    if total_tests > 0:
        success_rate = (passed_tests / total_tests) * 100
        print(f"Tasa de éxito: {success_rate:.1f}%")
    
    # Verificar archivos extra en test_results
    result_files = set(os.path.basename(f) for f in glob.glob(os.path.join(test_results_dir, "*")))
    output_files_set = set(os.path.basename(f) for f in output_files)
    extra_results = result_files - output_files_set
    
    if extra_results:
        print(f"\nArchivos extra en test_results: {len(extra_results)}")
        for extra_file in sorted(extra_results):
            print(f"  - {extra_file}")

if __name__ == "__main__":
    main()