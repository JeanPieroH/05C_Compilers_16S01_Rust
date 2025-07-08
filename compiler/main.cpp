#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

#include "scanner.h"
#include "parser.h"
#include "visitor.h"
#include "imp_interpreter.hh"
#include "gencode.hh"
#include "imp_type.hh"

namespace fs = std::filesystem;

using namespace std;

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        cout << "Numero incorrecto de argumentos. Uso: " << argv[0] << " <archivo_de_entrada>" << endl;
        exit(1);
    }

    // === Rutas relativas a donde está el ejecutable ===
    fs::path exec_dir = fs::canonical(argv[0]).parent_path();  // Ruta del ejecutable

    fs::path base_dir = exec_dir;                // Un nivel arriba

    fs::path logs_dir = base_dir / "logs";
    fs::path outputs_dir = base_dir / "outputs";

    // Crear directorios si no existen
    fs::create_directories(logs_dir);
    fs::create_directories(outputs_dir);
    // === Extraer nombre base del archivo de entrada ===
    fs::path input_path = argv[1];
    string base_name = input_path.stem().string();  // sin extensión

    // === Abrir archivos de salida ===
    ofstream log_file(logs_dir / (base_name + ".log"));
    ofstream output_file(outputs_dir / (base_name + ".s"));

    if (!log_file.is_open() || !output_file.is_open()) {
        cout << "No se pudieron abrir archivos de salida" << endl;
        return 1;
    }

    // === Leer archivo de entrada ===
    ifstream infile(input_path);
    if (!infile.is_open()) {
        log_file << "No se pudo abrir el archivo: " << argv[1] << endl;
        return 1;
    }

    string input;
    string line;
    while (getline(infile, line)) {
        input += line + '\n';
    }
    infile.close();

    // === Ejecutar compilación ===
    Scanner scanner(input.c_str(),log_file);

    string input_copy = input;
    Scanner scanner_test(input_copy.c_str(),log_file);
    test_scanner(&scanner_test,log_file);
    log_file << "Scanner exitoso" << endl;
    log_file << endl;

    log_file << "Iniciando parsing:" << endl;
    Parser parser(&scanner,log_file);

    try {
        Program* program = parser.parseProgram();

        log_file << "Parsing exitoso" << endl << endl;

        log_file << "Iniciando Visitor:" << endl;

        PrintVisitor printVisitor(log_file);
        printVisitor.imprimir(program); // Optional: Keep for debugging AST if needed

        ImpCODE interpreter(output_file); // Uncommented

        log_file << endl;
        log_file << "IMPRIMIR AST (para debugging si es necesario):" << endl;
        // printVisitor.imprimir(program); // Uncomment if you want to log the AST structure
        log_file << endl;

        log_file << endl << "Generando codigo ensamblador:" << endl;

        cout << "Generando codigo ensamblador en outputs/" << base_name << ".s" << endl;
        interpreter.interpret(program);              // ejecutar // Uncommented

        log_file << "Generacion de codigo ensamblador finalizada." << endl;
        log_file.close();
        output_file.close();

        delete program;

    } catch (const exception& e) {
        log_file << "Error durante la ejecución: " << e.what() << endl;
        return 1;
    }

    return 0;
}
