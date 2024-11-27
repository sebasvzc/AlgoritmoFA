#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

#include "Enjambre.h"

int main() {
    srand(static_cast<unsigned int>(time(0)));

    int numLuciernagas = 100;  // Numero de luciernagas
    int iteraciones = 100;     // Numero de iteraciones

    int meses = 8;                           // Numero de meses
    int numeroCultivos = 5;                  // Numero de cultivos
    int dimension = numeroCultivos * meses;  // Dimension total

    Cultivacion cultivacion(meses, numeroCultivos);
    Enjambre enjambre(numLuciernagas, dimension);

    enjambre.inicializarLuciernagas(numeroCultivos, meses, cultivacion);
    enjambre.inicializarValoresObjetivo(numeroCultivos, meses, cultivacion);

    Luciernaga mejorLuciernaga = enjambre.encontrarMejorLuciernaga();
    double mejorValor = mejorLuciernaga.valorObjetivo;

    for (int iter = 0; iter < iteraciones; ++iter) {
        for (size_t i = 0; i < enjambre.luciernagas.size(); ++i) {
            for (size_t j = 0; j < enjambre.luciernagas.size(); ++j) {
                if (i == j) {
                    // Guardar la posicion actual para movimiento aleatorio
                    Luciernaga luciernagaOriginal = enjambre.luciernagas[i];

                    enjambre.movimientoAleatorio(enjambre.luciernagas[i],
                                                 numeroCultivos, meses,
                                                 cultivacion);
                    double nuevoValor = enjambre.funcionObjetivo(enjambre.luciernagas[i],
                                                                 numeroCultivos, meses,
                                                                 cultivacion);
                    // Revertir si la nueva posicion es peor
                    if (nuevoValor > enjambre.valoresObjetivo[i])
                        enjambre.luciernagas[i] = luciernagaOriginal;
                    else  // Actualizar el valor objetivo de lo contrario
                        enjambre.valoresObjetivo[i] = nuevoValor;

                } else {
                    if (enjambre.valoresObjetivo[j] < enjambre.valoresObjetivo[i]) {
                        double distancia = enjambre.calcularDistancia(enjambre.luciernagas[i],
                                                                      enjambre.luciernagas[j]);
                        double beta = enjambre.calcularAtractivo(distancia);

                        enjambre.moverLuciernaga(enjambre.luciernagas[i], enjambre.luciernagas[j],
                                                 beta, numeroCultivos, meses, cultivacion);
                        enjambre.actualizarValorObjetivo(i, numeroCultivos, meses, cultivacion);
                    }
                }
            }
        }
        Luciernaga luciernagaActualMejor = enjambre.encontrarMejorLuciernaga();
        if (luciernagaActualMejor.valorObjetivo < mejorValor) {
            mejorLuciernaga = luciernagaActualMejor;
            mejorValor = luciernagaActualMejor.valorObjetivo;
        }
    }
    mejorLuciernaga.imprimirDetallesLuciernaga(numeroCultivos, meses,
                                               cultivacion.areaTotalDisponible,
                                               cultivacion.requerimientoAgua,
                                               cultivacion.mesesCultivo,
                                               cultivacion.maxCosechaPorArea);
    return 0;
}
