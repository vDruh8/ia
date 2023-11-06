#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>  // Para rand y srand
#include <ctime>    // Para time
#include <algorithm> // Para shuffle
#include <random> 

using namespace std;

// Abstracción del problema
struct RepresentacionALSP {
    string file_name;
    float **S;
    vector<int> Ei;
    vector<int> Ti;
    vector<int> Li;
    vector<float> gi;
    vector<float> hi;
    unsigned int N;
};

struct SolucionCandidata {
    vector<int> avion;  // Aviones en orden
    vector<int> tiempo;  // Tiempos de aterrizaje correspondientes
};


SolucionCandidata GenerarSolucionInicialGreedy(const RepresentacionALSP& ALSP) {
    unsigned int N = ALSP.N;

    // Inicializar listas de aviones y tiempos
    vector<int> aviones(N);
    vector<int> tiempos(N);

    for (unsigned int i = 0; i < N; i++) {
        aviones[i] = i;
    }

    // Ordenar los aviones por sus tiempos ideales de aterrizaje
    sort(aviones.begin(), aviones.end(), [&](int a, int b) {
        return ALSP.Ti[a] < ALSP.Ti[b];
    });

    // Asignar tiempos de aterrizaje de manera determinista
    tiempos[0] = ALSP.Ti[aviones[0]];
    for (unsigned int i = 1; i < N; i++) {
        int avion_i = aviones[i];
        int avion_prev = aviones[i - 1];
        int separacion_minima = ALSP.S[avion_i][avion_prev];
        tiempos[i] = max(tiempos[i - 1] + separacion_minima, ALSP.Ti[avion_i]);
    }

    // Crear una solución candidata factible
    SolucionCandidata solucion;
    solucion.avion = aviones;
    solucion.tiempo = tiempos;

    return solucion;
}



int evaluarCosto(RepresentacionALSP ALSP, vector<SolucionCandidata> solucion_candidata, int mejor_solucion, int N) {
    int costo_candidato = 0;

    for (int i = 0; i < N; i++) {
        int avion = solucion_candidata[0].avion[i];  // Acceder al primer elemento del vector
        // Accede a los valores correspondientes en los vectores Ti, gi y hi
        int tiempo_aterrizaje = solucion_candidata[0].tiempo[i] - ALSP.Ti[avion];

        if (tiempo_aterrizaje <= 0) {
            costo_candidato += -tiempo_aterrizaje * ALSP.gi.at(avion);
        } else if (tiempo_aterrizaje > 0) {
            costo_candidato += tiempo_aterrizaje * ALSP.hi.at(avion);
        }
    }
    
    if (costo_candidato < mejor_solucion) {
        return costo_candidato;
    } else {
        return -1;
    }
}


int main() {
    // Nombre del archivo de entrada
    string inputFileName = "airland1.txt";

    ifstream inputFile(inputFileName);
    if (!inputFile.is_open()) {
        cerr << "No se pudo abrir el archivo 'airland1.txt'." << endl;
        return 1;
    }

    int N;
    inputFile >> N;

    // Instanciacion de variables para leer
    vector<int> Ei(N);
    vector<int> Ti(N);
    vector<int> Li(N);
    vector<float> gi(N);
    vector<float> hi(N);
    float** S = new float*[N];

    // Se leen las variables desde el archivo
    for (int i = 0; i < N; i++) {
        inputFile >> Ei[i];
        inputFile >> Ti[i];
        inputFile >> Li[i];
        inputFile >> gi[i];
        inputFile >> hi[i];
        S[i] = new float[N];
        for (int j = 0; j < N; j++) {
            inputFile >> S[i][j];
        }
    }

    // Asignacion variables del problema
    RepresentacionALSP ALSP;
    ALSP.N = N;
    ALSP.Ei = Ei;
    ALSP.Ti = Ti;
    ALSP.Li = Li;
    ALSP.gi = gi;
    ALSP.hi = hi;
    ALSP.S = S;
    ALSP.file_name = inputFileName;


    // Imprime algunos datos para verificar.
    
    cout << "Numero de aviones: " << ALSP.N << endl;
    cout << "Tiempos mas tempranos de aterrizaje: ";
    for (unsigned int i = 0; i < ALSP.N; i++) {
        cout << ALSP.Ei[i] << " ";
    }
    cout << endl;

    cout << "Tiempos ideales de aterrizaje: ";
    for (unsigned int i = 0; i < ALSP.N; i++) {
        cout << ALSP.Ti[i] << " ";
    }
    cout << endl;

    cout << "Tiempos mas tardio de aterrizaje: ";
    for (unsigned int i = 0; i < ALSP.N; i++) {
        cout << ALSP.Li[i] << " ";
    }
    cout << endl;

    cout << "Penalizacion aterrizar antes: ";
    for (unsigned int i = 0; i < ALSP.N; i++) {
        cout << ALSP.gi[i] << " ";
    }
    cout << endl;

    cout << "Penalizacion aterrizar despues: ";
    for (unsigned int i = 0; i < ALSP.N; i++) {
        cout << ALSP.hi[i] << " ";
    }
    cout << endl;

    cout << "Matriz de separacion:" << endl;
    for (unsigned int i = 0; i < ALSP.N; i++) {
        for (unsigned int j = 0; j < ALSP.N; j++) {
            cout << ALSP.S[i][j] << " ";
        }
        cout << endl;
    }
    
    SolucionCandidata solucionInicial = GenerarSolucionInicialGreedy(ALSP);

    // Imprimir la solución inicial
    cout << "Solucion Inicial:\n ";
    cout << "Aviones: ";
    for (int avion : solucionInicial.avion) {
        cout << avion + 1 << " ";  // Sumar 1 para mostrar aviones desde 1 en lugar de 0
    }
    cout << endl;

    cout << "Tiempos: ";
    for (int tiempo : solucionInicial.tiempo) {
        cout << tiempo << " ";
    }
    cout << endl;


    // Llama a evaluarCosto y almacena el resultado
    int mejor_solucion = 100000000;  
    vector<SolucionCandidata> solucionesIniciales;
    solucionesIniciales.push_back(solucionInicial);

    int costo = evaluarCosto(ALSP, solucionesIniciales, mejor_solucion, N);

    // Imprime el resultado
    if (costo >= 0) {
        std::cout << "El costo de la solucion es: " << costo << std::endl;
    } else {
        std::cout << "La solución no es factible." << costo << std::endl;
    }

    // Cierra el archivo después de usarlo.
    inputFile.close();

    return 0;
}
