#ifndef ENV
#define ENV

#include <list>
#include <string>
#include <unordered_map>
#include <vector>

#include <iostream>  // Para std::ostream
#include <cstdlib>   // Para exit

using namespace std;

template <typename T>
class Environment {
private:
    vector<unordered_map<string, T> > ribs; // Pila de tablas de símbolos (niveles de scope).

    // Busca una variable en los scopes, desde el más interno al más externo.
    int search_rib(string var) {
        int idx = ribs.size() - 1;
        while (idx >= 0) {
            typename std::unordered_map<std::string, T>::const_iterator it =
                ribs[idx].find(var);
            if (it != ribs[idx].end())
                return idx; // Variable encontrada en este scope.
            idx--;
        }
        return -1; // Variable no encontrada.
    }

    std::ostream& out; // Referencia al stream de salida.

public:
    // Constructor que recibe un std::ostream para la salida de mensajes.
    Environment(std::ostream& os) : out(os) {
        add_level(); // Añade el scope global al inicio.
    }

    // Limpia todos los scopes.
    void clear() { ribs.clear(); }

    // Añade un nuevo nivel de scope.
    void add_level() {
        unordered_map<string, T> r;
        ribs.push_back(r);
    }

    // Añade una variable con un valor al scope actual.
    void add_var(string var, T value) {
        if (ribs.empty()) {
            out << "Error de entorno: No hay scope para añadir variables." << endl;
            exit(1); // Error fatal, no se puede continuar.
        }
        ribs.back()[var] = value;
    }

    // Añade una variable sin valor inicial (asumiendo que T puede ser inicializado a 0).
    void add_var(string var) { 
        if (ribs.empty()) {
            out << "Error de entorno: No hay scope para añadir variables." << endl;
            exit(1); // Error fatal.
        }
        ribs.back()[var] = T(); // Inicializa T con su constructor por defecto (para tipos numéricos sería 0).
    }
    void add_var_ref(string var, T ref_value) {
        if (ribs.empty()) {
            out << "Error de entorno: No hay scope para añadir variables por referencia." << endl;
            exit(1);
        }
        ribs.back()[var] = ref_value;
    }
    // Elimina el nivel de scope actual.
    bool remove_level() {
        if (!ribs.empty()) {
            ribs.pop_back();
            return true;
        }
        return false; // No hay niveles para eliminar.
    }

    // Actualiza el valor de una variable existente en cualquier scope.
    bool update(string x, T v) {
        int idx = search_rib(x);
        if (idx < 0)
            return false; // Variable no encontrada para actualizar.
        ribs[idx][x] = v;
        return true;
    }

    // Verifica si una variable existe en cualquier scope.
    bool check(string x) {
        int idx = search_rib(x);
        return (idx >= 0);
    }

    // Busca y devuelve el valor de una variable. Si no la encuentra, reporta error y sale.
    T lookup(string x) {
        int idx = search_rib(x);
        if (idx < 0) {
            out << "Error de entorno: Variable '" << x << "' no encontrada." << endl;
            exit(1); // Error fatal, la variable no existe.
        } else {
            return ribs[idx][x];
        }
    }

    T& lookup_ref(const string& x) {
        int idx = search_rib(x);
        if (idx < 0) {
            out << "Error de entorno: Variable '" << x << "' no encontrada para referencia." << endl;
            exit(1);
        } else {
            return ribs[idx][x]; 
        }
    }

    // Devuelve una REFERENCIA constante al valor de una variable.
    const T& lookup_const_ref(const string& x) const {
        int idx = search_rib(x);
        if (idx < 0) {
            out << "Error de entorno: Variable '" << x << "' no encontrada para referencia constante." << endl;
            exit(1);
        } else {
            return ribs[idx].at(x); 
        }
    }
    
    // Busca el valor de una variable y lo guarda en 'v'.
    // Devuelve true si la encuentra, false si no.
    bool lookup(string x, T& v) {
        int idx = search_rib(x);
        if (idx < 0)
            return false;
        v = ribs[idx][x];
        return true;
    }
};

#endif