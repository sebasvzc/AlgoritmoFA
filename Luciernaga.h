#ifndef LUCIERNAGA_H
#define LUCIERNAGA_H

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

class Luciernaga {
   public:
    vector<double> valores;
    double valorObjetivo;

    Luciernaga(int dimension) : valores(dimension, 0.0), valorObjetivo(0.0) {}

    static bool esCultivable(const vector<int>& cultivable, int cultivo, int mes, int periodoCrecimiento, int numeroCultivos) {
        for (int m = 0; m < periodoCrecimiento && (mes + m) < cultivable.size() / numeroCultivos; ++m) {
            if (cultivable[(cultivo) + numeroCultivos * (mes + m)] == 0) {
                return false;
            }
        }
        return true;
    }

    static bool debeEntrarEnBucleInicializacion(double areaDisponible) {
        double resultado = -0.7 * exp(-6 * areaDisponible + 5.25) + 107;
        return resultado > (rand() % 100);
    }

    static bool esAguaSuficiente(const vector<double>& aguaDisponible, const vector<double>& requerimientoAgua, int cultivo, int mes, int periodoCrecimiento, double areaUsada, double areaTotalDisponible) {
        double areaEnHectareas = areaUsada * areaTotalDisponible;
        random_device rd;
        mt19937 gen(rd());

        for (int m = 0; m < periodoCrecimiento && (mes + m) < aguaDisponible.size(); ++m) {
            double aguaRequerida = requerimientoAgua[cultivo] * areaEnHectareas;
            double disponible = aguaDisponible[mes + m];

            if (disponible < aguaRequerida) {
                double porcentajeEscasez = (aguaRequerida - disponible) / aguaRequerida;
                double probabilidadContinuar = 1.0 - porcentajeEscasez;

                uniform_real_distribution<> dis(0.0, 1.0);
                if (dis(gen) > probabilidadContinuar) {
                    return false;
                }
            }
        }
        return true;
    }

    static Luciernaga inicializar(int dimension, int numeroCultivos, int meses, const vector<int>& mesesCultivo,
                                  const vector<double>& requerimientoAgua, const vector<int>& cultivable,
                                  const vector<double>& aguaInicialDisponible, double areaTotalDisponible, double alfa) {
        Luciernaga luciernaga(dimension);
        vector<double> areaDisponible(meses, 1.0);
        vector<double> aguaDisponible = aguaInicialDisponible;

        random_device rd;
        mt19937 gen(rd());
        chi_squared_distribution<> dist(5);

        for (int mes = 0; mes < meses; ++mes) {
            while (debeEntrarEnBucleInicializacion(areaDisponible[mes])) {
                int cultivo = rand() % numeroCultivos;
                int periodoCrecimiento = mesesCultivo[cultivo];

                if (!esCultivable(cultivable, cultivo, mes, periodoCrecimiento, numeroCultivos)) {
                    continue;
                }

                double prcAreaUsada = 8 * dist(gen) / 100.0;
                double areaUsada = (prcAreaUsada > 1 ? 0.0 : prcAreaUsada) * areaDisponible[mes];

                if (!esAguaSuficiente(aguaDisponible, requerimientoAgua, cultivo, mes, periodoCrecimiento, areaUsada, areaTotalDisponible)) {
                    continue;
                }

                for (int m = 0; m < periodoCrecimiento && (mes + m) < meses; ++m) {
                    int indice = cultivo + numeroCultivos * (mes + m);
                    luciernaga.valores[indice] += areaUsada;
                    areaDisponible[mes + m] -= areaUsada;

                    double areaEnHectareas = areaUsada * areaTotalDisponible;
                    double aguaDescontar = requerimientoAgua[cultivo] * areaEnHectareas;

                    if (aguaDisponible[mes + m] < aguaDescontar) {
                        aguaDescontar = aguaDisponible[mes + m];
                    }
                    aguaDisponible[mes + m] -= aguaDescontar;
                }
            }

            if (mes < meses - 1) {
                aguaDisponible[mes + 1] += aguaDisponible[mes];
            }
        }

        return luciernaga;
    }

    void imprimirLuciernaga(int numeroCultivos) const {
        cout << fixed << setprecision(2);
        cout << "(";
        for (size_t j = 0; j < valores.size(); ++j) {
            cout << valores[j];
            if (j != valores.size() - 1) {
                cout << ", ";
            }
            if ((j + 1) % numeroCultivos == 0 && j != valores.size() - 1) {
                cout << "| ";
            }
        }
        cout << ")" << endl;
    }

    void imprimirDetallesLuciernaga(int numeroCultivos, int meses, double areaTotalDisponible,
                                    const vector<double>& requerimientoAgua, const vector<int>& mesesCultivo,
                                    const vector<double>& maxCosechaPorArea) const {
        cout << "Informe detallado de la luciernaga:" << endl;

        double totalAguaUsadaMes = 0.0;
        double cosechaTotal = 0.0;
        vector<double> cosechaPorCultivo(numeroCultivos, 0.0);

        for (int mes = 0; mes < meses; ++mes) {
            cout << "Mes " << mes + 1 << ":" << endl;
            totalAguaUsadaMes = 0.0;

            for (int cultivo = 0; cultivo < numeroCultivos; ++cultivo) {
                int indice = cultivo + numeroCultivos * mes;
                double areaAsignadaPorcentaje = valores[indice];
                double areaRealAsignada = areaAsignadaPorcentaje * areaTotalDisponible;

                if (areaAsignadaPorcentaje <= 0) continue;

                double aguaUsada = requerimientoAgua[cultivo] * areaRealAsignada;
                totalAguaUsadaMes += aguaUsada;

                double cosechaEsperada = (maxCosechaPorArea[cultivo] * areaRealAsignada) / mesesCultivo[cultivo];
                cosechaTotal += cosechaEsperada;
                cosechaPorCultivo[cultivo] += cosechaEsperada;

                cout << "  Cultivo " << cultivo + 1 << ":" << endl;
                cout << "    Area Asignada (porcentaje): " << areaAsignadaPorcentaje * 100 << "%" << endl;
                cout << "    Area Real Asignada (hectareas): " << areaRealAsignada << " hectareas" << endl;
                cout << "    Agua Usada (metros cubicos): " << aguaUsada << " metros cubicos" << endl;
            }

            cout << "Agua total usada en el mes " << mes + 1 << ": " << totalAguaUsadaMes << " metros cubicos" << endl;
            cout << endl;
        }

        cout << "Cosecha total por cultivo:" << endl;
        for (int cultivo = 0; cultivo < numeroCultivos; ++cultivo) {
            cout << "  Cultivo " << cultivo + 1 << ": " << cosechaPorCultivo[cultivo] << " toneladas" << endl;
        }

        cout << "Cosecha total de todos los cultivos: " << cosechaTotal << " toneladas" << endl;
    }
};

#endif /* LUCIERNAGA_H */
