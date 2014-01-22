#include "calculation.h"

/*  Calcula a media aritmetica de um vetor de valores double.
    Parametro 1: vetor tipo double.
    Parametro 2: tamanho do vetor. */
double ArithmeticMean(double *vec, int length)
{
    int i;
	double sum = 0;

	for (i = 0; i < length; i++)
		sum += vec[i];

	return sum / length;
}

/*  Calcula a variancia de um vetor de valores double.
    Parametro 1: vetor tipo double.
    Parametro 2: tamanho do vetor. */
double Variance(double *vec, int length)
{
    int i;
    double vec_mean = 0, sum = 0;

    vec_mean = ArithmeticMean(vec, length);
    for(i = 0; i < length; i++)
        sum = sum + ((vec[i] - vec_mean) * (vec[i] - vec_mean));

    return sum / length;
}

/*  Calcula o desvio padrao de um vetor de valores double.
    Parametro 1: vetor tipo double.
    Parametro 2: tamanho do vetor. */
double StandardDeviation(double *vec, int length)
{
    return sqrt(Variance(vec, length));
}

/*  Calcula o coeficiente de autocorrelacao de lag 1.
    Equacao retirada de:
    Goncalves, MF (2009, MT).
    Previsao de chuva com auxilio de radar de tempo visando a um sistema de alerta antecipado de cheias em areas urbanas.
    Equacao 4.13.
    Parametro 1: vetor tipo double com o valor da variavel vec para cada intervalo de tempo.
    Parametro 2: tamanho do vetor. */
double AutocorrelationLag1(double *vec, int length)
{
    int i;
    double vec_mean, denom = 0, coeff = 0;

    vec_mean = ArithmeticMean(vec, length);
    for(i = 0; i < length - 1; i++)
    {
        coeff += (vec[i] - vec_mean) * (vec[i + 1] - vec_mean);
        denom += (vec[i] - vec_mean) * (vec[i] - vec_mean);
    }
    denom += (vec[i] - vec_mean) * (vec[i] - vec_mean);

    return coeff /= denom;
}

/*  Calcula a regressao uni-variavel de lag 1.
    Equacao retirada de:
    Goncalves, MF (2009, MT).
    Previsao de chuva com auxilio de radar de tempo visando a um sistema de alerta antecipado de cheias em areas urbanas.
    Equacao 4.12.
    Parametro 1: vetor tipo double com o valor da variavel vec para cada intervalo de tempo.
    Parametro 2: tamanho do vetor. */
double Regression(double *vec, int length)
{
    double vec_mean, autocor_coeff, std_deviation, rand_num;

    vec_mean = ArithmeticMean(vec, length);
    autocor_coeff = AutocorrelationLag1(vec, length);
    std_deviation = StandardDeviation(vec, length);
    rand_num = (rand() % 10000); /* Gera numero aleatorio entre 0 e 9999. */
    rand_num /= 9999; /* Normaliza entre 0.0000 e 1.0000 o numero aleatorio. */

    return vec_mean + ((vec[length - 1] - vec_mean) * autocor_coeff) + (rand_num * std_deviation * sqrt(1 - (autocor_coeff * autocor_coeff)));
}
