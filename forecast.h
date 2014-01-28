#ifndef FORECAST_H
#define FORECAST_H

#include <math.h> /* pow, fabs, exp */
#include <stdio.h> /* FILE, fopen, printf, fprintf, fclose, fseek, ftell, SEEK_END */
#include <stdlib.h> /* system */
#include <time.h> /* mktime, time, tm */

#include "calculation.h"
#include "data_acquisition.h" /* stationData */

/// mudar #define para const int/float/double, etc..
/// checar quais valores podem assumir quais tipos de variaveis
/* CONSTANTES */
#define PTRANSF 133.32231202221 /* Utilizado para transformar pressao mmHg em Pa */
#define A1 0.0008 /* A1 = 8.10^-4 Kg/(m.s2.k3,5) */
#define PN 100000 /* Pressao nominal 10^5 Kg/(m.s2) */
#define A 2500000 /* Constante da eq. (14) do calor latente A=2,5.10^6 J/Kg */
#define B 2380 /* Constante da eq. (14) do calor latente B=2,38.10^3 J/(Kg.K) */
#define CP 1004 /* Calor especifico à pressao constante cp=1004 J/(kg.K) */
#define DA 1 /* Diferencial de área dA */
#define ALFA 3500 /* 3500 s-1 */
#define TAST 273.15 /* T* [ºK] eq 42 */
#define PAST 101325 /* p* [kg/(m*s2) eq 42 */
#define C1 700000 /* C1 [kg/(m3.s2)] pag 1593 */
#define RV 461 /* Constante dos gases para vapor de água [J/(kg.K)] pag 1592 */
#define R 287 /* J/(K.Kg) */
#define G 9.81 /* Aceleração da gravidade m/s2 */
#define K 0 /* Considerado pag 1602 */
#define GAMA 1
/* abstraídos: ifs de tempo/hora // linha/total_linha // altura // parametros de entrada */

/* Funcao auxiliar.
   Retorna o resultado da operacao. */
float f(float, float);

/* Implementa o metodo de Newton-Raphson para estimar as raizes de uma funcao.
   Retorna a raiz estimada. */
float NewtonRaphson(double *, float *, double *);

/* Funcao utilizada para gravar os resultados nos arquivos de saida para o SWMM.
   Retorna 1 em caso de sucesso. */
int SaveResults(stationData, double);

/* Implementa o modelo de previsao hidrometeorologica desenvolvido por Georgakakos e Bras.
   Retorna o resultado da previsao. */
double Georgakakos(stationData stat, double pobs, int *iteracao);

/* Executa calculos de previsao hidrometeorologica e gera arquivo de saida para o SWMM.
   Retorna 0 em caso de sucesso. */
int PrecForecast(stationData *, double *, int *);

#endif /* FORECAST_H */
