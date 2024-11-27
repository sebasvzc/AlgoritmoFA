#ifndef CULTIVACION_H
#define CULTIVACION_H

#include <vector>

using namespace std;

class Cultivacion {
   public:
    vector<int> mesesCultivo = {4, 5, 3, 3, 4};                                       // Periodos de crecimiento de los cultivos
    vector<double> requerimientoAgua = {1.0, 1.0, 1.4, 1.3, 1.1};                     // Requerimiento de agua para cada cultivo
    vector<double> aguaInicialDisponible = {120, 110, 130, 100, 150, 140, 125, 115};  // Agua inicial disponible
    vector<int> cultivable = {1, 1, 1, 1, 1,
                              1, 1, 1, 1, 1,
                              1, 0, 1, 1, 1,  // No se puede cosechar el cultivo 2 en el mes 3
                              1, 1, 1, 1, 1,
                              1, 1, 1, 1, 1,
                              1, 1, 1, 1, 1,
                              1, 1, 1, 1, 1,
                              1, 1, 1, 1, 1};                                   // Periodos validos de crecimiento
    vector<double> reduccionRendimiento = {4, 7, 10, 1, 10};                    // Reducción del rendimiento por unidad de salinidad
    vector<double> salinidadCritica = {2, 1, 1, 3, 4};                          // Niveles criticos de salinidad
    vector<double> maxCosechaPorArea = {1.2, 1.3, 0.8, 0.9, 0.9};               // Maxima cosecha por area
    vector<double> cambioSalinidadPorArea = {0.03, 0.03, -0.03, -0.03, -0.03};  // Cambio de salinidad por area
    vector<double> susceptibilidadAgua = {2.0, 3.1, 4.1, 4.6, 3.3};             // Susceptibilidad al agua por cultivo
    double areaTotalDisponible = 100.0;                                         // Area total disponible
    double conductividadElectrica = 0.8;                                        // Conductividad electrica inicial

    // Constructor vacío
    Cultivacion() {};

    Cultivacion(int meses, int numeroCultivos) {}

    // Constructor
    Cultivacion(int meses, int numeroCultivos,
                vector<int> mesesCultivo,
                vector<double> requerimientoAgua,
                vector<double> aguaInicialDisponible,
                vector<int> cultivable,
                vector<double> reduccionRendimiento,
                vector<double> salinidadCritica,
                vector<double> maxCosechaPorArea,
                vector<double> cambioSalinidadPorArea,
                vector<double> susceptibilidadAgua,
                double areaTotalDisponible,
                double conductividadElectrica)
        : mesesCultivo(mesesCultivo),
          requerimientoAgua(requerimientoAgua),
          aguaInicialDisponible(aguaInicialDisponible),
          cultivable(cultivable),
          reduccionRendimiento(reduccionRendimiento),
          salinidadCritica(salinidadCritica),
          maxCosechaPorArea(maxCosechaPorArea),
          cambioSalinidadPorArea(cambioSalinidadPorArea),
          susceptibilidadAgua(susceptibilidadAgua),
          areaTotalDisponible(areaTotalDisponible),
          conductividadElectrica(conductividadElectrica) {}
};

#endif /* CULTIVACION_H */
