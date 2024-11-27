#ifndef ENJAMBRE_H
#define ENJAMBRE_H

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

#include "Cultivacion.h"
#include "Luciernaga.h"

class Enjambre {
   public:
    int numLuciernagas = 100;
    double alfa = 0.05;
    double beta0 = 1;
    double gamma = 3.0;
    vector<Luciernaga> luciernagas;
    vector<double> valoresObjetivo;

    Enjambre(int numLuciernagas, int dimension) : numLuciernagas(numLuciernagas) {
        for (int i = 0; i < numLuciernagas; ++i) {
            luciernagas.emplace_back(dimension);
        }
        valoresObjetivo.resize(numLuciernagas, 0.0);
    }

    double calcularAguaTotalRequerida(const Luciernaga& luciernaga, int numeroCultivos, int mes, double areaTotalDisponible,
                                      const vector<double>& requerimientoAgua) const {
        double aguaTotalRequerida = 0.0;
        for (int cultivo = 0; cultivo < numeroCultivos; ++cultivo) {
            int indice = cultivo + numeroCultivos * mes;
            double areaAsignada = luciernaga.valores[indice];
            if (areaAsignada > 0) {
                aguaTotalRequerida += requerimientoAgua[cultivo] * areaAsignada * areaTotalDisponible;
            }
        }
        return aguaTotalRequerida;
    }

    double calcularCoeficienteAgua(double aguaTotalRequerida, double aguaTotalDisponible) const {
        return aguaTotalRequerida > 0 ? min(1.0, max(0.0, aguaTotalDisponible / aguaTotalRequerida)) : 1.0;
    }

    double calcularCosechaCultivo(const Luciernaga& luciernaga, int numeroCultivos, int mes, double areaTotalDisponible,
                                  double coeficienteAgua, double conductividadElectrica,
                                  const vector<int>& mesesCultivo, const vector<double>& maxCosechaPorArea,
                                  const vector<double>& susceptibilidadAgua, const vector<double>& reduccionRendimiento,
                                  const vector<double>& salinidadCritica) const {
        double cosechaMensual = 0.0;
        for (int cultivo = 0; cultivo < numeroCultivos; ++cultivo) {
            int indice = cultivo + numeroCultivos * mes;
            double areaAsignada = luciernaga.valores[indice];

            if (areaAsignada <= 0) continue;

            double cosechaEsperada = (maxCosechaPorArea[cultivo] * areaAsignada) / mesesCultivo[cultivo];
            double factorExponente = (coeficienteAgua * susceptibilidadAgua[cultivo]) / areaAsignada;
            double efectoAgua = 1 - exp(-factorExponente);
            double impactoSalinidad = reduccionRendimiento[cultivo] * (conductividadElectrica - salinidadCritica[cultivo]);
            double efectoSalinidad = min(1.0, max(0.0, 1.0 - impactoSalinidad / 100.0));
            double cosechaReal = cosechaEsperada * efectoAgua * efectoSalinidad;
            cosechaMensual += cosechaReal;
        }
        return cosechaMensual;
    }

    double actualizarSalinidad(const Luciernaga& luciernaga, int numeroCultivos, int mes, double areaTotalDisponible,
                               const vector<double>& cambioSalinidadPorArea) const {
        double cambioTotalSalinidad = 0.0;
        for (int cultivo = 0; cultivo < numeroCultivos; ++cultivo) {
            int indice = cultivo + numeroCultivos * mes;
            double areaAsignada = luciernaga.valores[indice];
            cambioTotalSalinidad += cambioSalinidadPorArea[cultivo] * (areaAsignada * areaTotalDisponible);
        }
        return cambioTotalSalinidad;
    }

    void trasladarAgua(vector<double>& aguaDisponible, int mes, double aguaTotalRequerida) const {
        if (mes < aguaDisponible.size() - 1) {
            aguaDisponible[mes + 1] += max(0.0, aguaDisponible[mes] - aguaTotalRequerida);
        }
    }

    double funcionObjetivo(const Luciernaga& luciernaga, int numeroCultivos, int meses,
                           Cultivacion& cultivacion) const {
        double cosechaTotal = 0.0;
        double conductividadElectrica = cultivacion.conductividadElectrica;
        vector<double> aguaDisponible = cultivacion.aguaInicialDisponible;

        for (int mes = 0; mes < meses; ++mes) {
            double aguaTotalRequerida = calcularAguaTotalRequerida(luciernaga, numeroCultivos, mes, cultivacion.areaTotalDisponible, cultivacion.requerimientoAgua);
            double coeficienteAgua = calcularCoeficienteAgua(aguaTotalRequerida, aguaDisponible[mes]);
            double cosechaMensual = calcularCosechaCultivo(luciernaga, numeroCultivos, mes, cultivacion.areaTotalDisponible,
                                                           coeficienteAgua, conductividadElectrica, cultivacion.mesesCultivo,
                                                           cultivacion.maxCosechaPorArea, cultivacion.susceptibilidadAgua, cultivacion.reduccionRendimiento, cultivacion.salinidadCritica);

            if (mes < meses - 1) {
                double cambioSalinidad = actualizarSalinidad(luciernaga, numeroCultivos, mes, cultivacion.areaTotalDisponible, cultivacion.cambioSalinidadPorArea);
                conductividadElectrica += cambioSalinidad;
            }
            // Pasar el agua restante de un mes al mes siguiente
            trasladarAgua(aguaDisponible, mes, aguaTotalRequerida);
            cosechaTotal += cosechaMensual;
        }

        return cosechaTotal;
    }

    void actualizarValorObjetivo(size_t indice, int numeroCultivos, int meses, Cultivacion& cultivacion) {
        double valor = funcionObjetivo(luciernagas[indice], numeroCultivos, meses, cultivacion);
        luciernagas[indice].valorObjetivo = valor;
        valoresObjetivo[indice] = valor;
    }

    void inicializarValoresObjetivo(int numeroCultivos, int meses, Cultivacion& cultivacion) {
        valoresObjetivo.resize(luciernagas.size());
        for (size_t i = 0; i < luciernagas.size(); ++i) {
            actualizarValorObjetivo(i, numeroCultivos, meses, cultivacion);
        }
    }

    double calcularAtractivo(double distancia) const {
        return beta0 * exp(-gamma * pow(distancia, 2));
    }

    double calcularDistancia(const Luciernaga& luciernaga1, const Luciernaga& luciernaga2) const {
        double suma = 0.0;
        for (size_t i = 0; i < luciernaga1.valores.size(); ++i) {
            suma += pow(luciernaga2.valores[i] - luciernaga1.valores[i], 2);
        }
        return sqrt(suma);
    }

    vector<int> identificarCultivosValidos(int mes, int numeroCultivos, const vector<int>& mesesCultivo, const vector<int>& cultivable) const {
        vector<int> cultivosValidos;
        for (int cultivo = 0; cultivo < numeroCultivos; ++cultivo) {
            if (Luciernaga::esCultivable(cultivable, cultivo, mes, mesesCultivo[cultivo], numeroCultivos)) {
                cultivosValidos.push_back(cultivo);
            }
        }
        return cultivosValidos;
    }

    double calcularAreaMesActual(const Luciernaga& luciernaga, int numeroCultivos, int mes) const {
        double areaMesActual = 0.0;
        for (int cultivo = 0; cultivo < numeroCultivos; ++cultivo) {
            areaMesActual += luciernaga.valores[cultivo + numeroCultivos * mes];
        }
        return areaMesActual;
    }

    double aplicarIncremento(double valorActual, double incremento) const {
        return max(0.0, min(1.0, valorActual + incremento));
    }

    bool verificarDisponibilidadAreaMesesSiguientes(const Luciernaga& luciernaga, int cultivoSeleccionado, int numeroCultivos,
                                                    int mes, int meses, int periodoCrecimiento, double incremento) const {
        for (int m = 1; m < periodoCrecimiento && (mes + m) < meses; ++m) {
            double areaTotalMes = 0.0;
            for (int cultivo = 0; cultivo < numeroCultivos; ++cultivo) {
                int indice = cultivo + numeroCultivos * (mes + m);
                if (cultivo == cultivoSeleccionado) {
                    areaTotalMes += luciernaga.valores[indice] + incremento;
                } else {
                    areaTotalMes += luciernaga.valores[indice];
                }
            }
            if (areaTotalMes > 1.0 || areaTotalMes < 0.0) {
                return false;
            }
        }
        return true;
    }

    void actualizarAreasMesesSiguientes(Luciernaga& luciernaga, int cultivoSeleccionado, int numeroCultivos,
                                        int mes, int meses, int periodoCrecimiento, double incremento) const {
        for (int m = 0; m < periodoCrecimiento && (mes + m) < meses; ++m) {
            int indice = cultivoSeleccionado + numeroCultivos * (mes + m);
            luciernaga.valores[indice] = aplicarIncremento(luciernaga.valores[indice], incremento);
        }
    }

    void movimientoAleatorio(Luciernaga& luciernaga, int numeroCultivos, int meses, Cultivacion& cultivacion) const {
        for (int mes = 0; mes < meses; ++mes) {
            vector<int> cultivosValidos = identificarCultivosValidos(mes, numeroCultivos,
                                                                     cultivacion.mesesCultivo,
                                                                     cultivacion.cultivable);
            if (cultivosValidos.empty()) continue;

            int cultivoSeleccionado = cultivosValidos[rand() % cultivosValidos.size()];
            int indice = cultivoSeleccionado + numeroCultivos * mes;

            double areaMesActual = calcularAreaMesActual(luciernaga, numeroCultivos, mes);

            double incremento = alfa * (static_cast<double>(rand()) / RAND_MAX - 0.5);
            double nuevoValor = aplicarIncremento(luciernaga.valores[indice], incremento);

            if (areaMesActual - luciernaga.valores[indice] + nuevoValor > 1.0) continue;

            if (verificarDisponibilidadAreaMesesSiguientes(luciernaga, cultivoSeleccionado, numeroCultivos, mes, meses,
                                                           cultivacion.mesesCultivo[cultivoSeleccionado], incremento))
                actualizarAreasMesesSiguientes(luciernaga, cultivoSeleccionado, numeroCultivos, mes, meses,
                                               cultivacion.mesesCultivo[cultivoSeleccionado], incremento);
        }
    }

    void moverLuciernaga(Luciernaga& luciernaga, const Luciernaga& mejorLuciernaga, double beta, int numeroCultivos, int meses,
                         Cultivacion& cultivacion) {
        for (size_t i = 0; i < luciernaga.valores.size(); ++i) {
            luciernaga.valores[i] += beta * (mejorLuciernaga.valores[i] - luciernaga.valores[i]);
            luciernaga.valores[i] = max(0.0, min(1.0, luciernaga.valores[i]));
        }
        movimientoAleatorio(luciernaga, numeroCultivos, meses, cultivacion);
    }

    void inicializarLuciernagas(int numeroCultivos, int meses, Cultivacion& cultivacion) {
        int dimension = numeroCultivos * meses;

        for (int k = 0; k < numLuciernagas; ++k) {
            Luciernaga nuevaLuciernaga = Luciernaga::inicializar(dimension, numeroCultivos, meses,
                                                                 cultivacion.mesesCultivo,
                                                                 cultivacion.requerimientoAgua,
                                                                 cultivacion.cultivable,
                                                                 cultivacion.aguaInicialDisponible,
                                                                 cultivacion.areaTotalDisponible,
                                                                 alfa);
            luciernagas.push_back(nuevaLuciernaga);
        }
    }

    void imprimirLuciernagas(int numeroCultivos) const {
        for (size_t i = 0; i < luciernagas.size(); ++i) {
            cout << "Luciernaga " << i + 1 << ": ";
            luciernagas[i].imprimirLuciernaga(numeroCultivos);
        }
    }

    Luciernaga encontrarMejorLuciernaga() const {
        Luciernaga mejorLuciernaga = luciernagas[0];
        double mejorValor = valoresObjetivo[0];

        for (size_t i = 1; i < luciernagas.size(); ++i) {
            if (valoresObjetivo[i] > mejorValor) {
                mejorLuciernaga = luciernagas[i];
                mejorValor = valoresObjetivo[i];
            }
        }

        return mejorLuciernaga;
    }
};

#endif /* ENJAMBRE_H */
