#ifndef CALCULATION_H
#define CALCULATION_H

#include <math.h> /* sqrt */
#include <stdlib.h> /* rand */

/*  Calcula a media aritmetica de um vetor de valores double.
    Parametro 1: vetor tipo double.
    Parametro 2: tamanho do vetor. */
double ArithmeticMean(double *, int);

/*  Calcula a variancia de um vetor de valores double.
    Parametro 1: vetor tipo double.
    Parametro 2: tamanho do vetor. */
double Variance(double *, int);

/*  Calcula o desvio padrao de um vetor de valores double.
    Parametro 1: vetor tipo double.
    Parametro 2: tamanho do vetor. */
double StandardDeviation(double *, int);

/*  Calcula o coeficiente de autocorrelacao de lag 1.
    Equacao retirada de:
    Goncalves, MF (2009, MT).
    Previsao de chuva com auxilio de radar de tempo visando a um sistema de alerta antecipado de cheias em areas urbanas.
    Equacao 4.13.
    Parametro 1: vetor tipo double com o valor da variavel vec para cada intervalo de tempo.
    Parametro 2: tamanho do vetor. */
double AutocorrelationLag1(double *, int);

/*  Calcula a regressao uni-variavel de lag 1.
    Equacao retirada de:
    Goncalves, MF (2009, MT).
    Previsao de chuva com auxilio de radar de tempo visando a um sistema de alerta antecipado de cheias em areas urbanas.
    Equacao 4.12.
    Parametro 1: vetor tipo double com o valor da variavel vec para cada intervalo de tempo.
    Parametro 2: tamanho do vetor. */
double Regression(double *, int);

#endif /* CALCULATION_H */
