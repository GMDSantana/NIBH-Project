#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "brain.h"

/// mudar #define para const int/float/double, etc..
/// checar quais valores podem assumir quais tipos de variaveis
/* CONSTANTES */
#define PTRANSF 133.32231202221 // Utilizado para transformar pressao mmHg em Pa
#define A1 0.0008 //A1 = 8.10^-4 Kg/(m.s2.k3,5)
#define PN 100000 //Pressao nominal 10^5 Kg/(m.s2)
#define A 2500000 //Constante da eq. (14) do calor latente A=2,5.10^6 J/Kg
#define B 2380 //Constante da eq. (14) do calor latente B=2,38.10^3 J/(Kg.K)
#define CP 1004 //Calor especifico à pressao constante cp=1004 J/(kg.K)
#define DA 1 //Diferencial de área dA
#define ALFA 3500 //3500 s-1
#define TAST 273.15 //T* [ºK] eq 42
#define PAST 101325 //p* [kg/(m*s2) eq 42
#define C1 700000 //C1 [kg/(m3.s2)] pag 1593
#define RV 461 //Constante dos gases para vapor de água [J/(kg.K)] pag 1592
#define R 287 //J/(K.Kg)
#define G 9.81 //Aceleração da gravidade m/s2
#define K 0 //Considerado pag 1602
#define GAMA 1
//abstraídos: ifs de tempo/hora // linha/total_linha // altura // parametros de entrada

typedef struct stationData stationData;

float f(float T, float p)
{
    float pt, p1, p2, p3, res;

    p1 = pow((PN / p), 0.286);
    p2 = 0.622 * A1 * (pow((T-223.15),3.5)) / p;
    p3 = (A - B * (T-273.15)) * p2 / (CP * T);

    res = T * p1 * exp(p3);
    return res;
}

float NewtonRaphson(double *ts, float *plinha, double *tetaE1)
{
    float x, x0, Epsilon, E, p, p1, p2, erro[2];
    int i = 1;
    x0 = *ts;
    p = *plinha;
    Epsilon = 0.001 * *tetaE1;
    E = *tetaE1;

    do{
        p1 = f(x0, p);
        E = fabs(p1 - *tetaE1);
        erro[0] = E;
        x0 = x0 - 0.1;
        p1 = f(x0, p);
        erro[1] = fabs(p1 - *tetaE1);
        i++;
    }while ((E>Epsilon) && erro[0] > erro[1]);
    return x0;
}

int SaveResults(stationData stat, double P)
{
    struct tm tm1 = { 0 };
    int hours, minutes;
    int isEmpty;
    char fileName[100];
    FILE *outFile;

    /* A variavel "date" contem apenas o dia do ano (0-365), mas o SWMM requer dia, mes e ano.
     * Entao a partir de "dia do ano" (date) + "ano" + time.h descobre dia, mes e ano. */
    tm1.tm_year = 2000 - 1900 + stat.year % 4;
    tm1.tm_mday = stat.date;
    tm1.tm_hour = 12;
    mktime(&tm1);
    tm1.tm_year = stat.year;
    tm1.tm_mon++;

    // Tambem eh necessario formatar o tempo, que vem em hhmm, para hh\tmm
    minutes = stat.time%100;
    hours = stat.time/100;

    if( (outFile = fopen("precOutSample.dat", "a")) == NULL)
    {
        printf("Erro ao abrir o arquivo de saida.\n");
        return 0;
    }
    // Checa se o arquivo estava vazio. Se sim, inclui o cabecalho
    fseek(outFile, 0, SEEK_END);
    isEmpty = ftell(outFile);
    if ( isEmpty == 0 )
      fprintf(outFile, ";StaID\tYear\tMonth\tDay\tHour\tMin\tPrecip\n");

    fprintf(outFile, "1\t%d\t%d\t%d\t%d\t%d\t%f\n", tm1.tm_year, tm1.tm_mon, tm1.tm_mday, hours, minutes, P);
    fclose(outFile);

    return 1;
    // Abre (ou cria se não houver) arquivo de saida em ASCII
    // Vai ao fim do arquivo
    // Adiciona dados formatados para o SWMM no final do arquivo
    // Fecha arquivo
    // Retorna sucesso
    // Main retorna sucesso
    // Brain roda swmm.
}

double Georgakakos(stationData stat, double pobs, int *iteracao)
{
    /* Funcao equivalente a: Calcula_Georgakakos + converte_dados
     * Declaracoes: estao na forma mais extensa pra poderem comentar o significado de cada uma */
    //Variaveis que eram globais
    //to, po
    double td; //temperatura no ponto de orvalho (em K)
    double ps; //pressao na base da nuvem
    double ts; //pressao na base da nuvem (de novo?)
    double es; //pressao de saturacao umida
    double wo; //razao de mistura inicial
    float wss; //razao de mistura na base
    float tt;
    float pt;
    double wst;
    double teta; //temperatura potencial teta
    float L; //Calor latente de condensação
    double tetaE1;//temperatura potencial teta e
    float umI; //Entrada de umidade I
    float nv; //Adimensional de velocidade
    float vbeta; //Velocidade v beta
    float c; //Inverso do diâmetro médio
    double dast; //D* [equacao 42]
    double P; //PWV (precipitacao acumulada?)
    double obs;
    double erro;
    double zb;
    float dc;
    float nd;
    float vp;
    float pt1;
    //pt2
    float plinha;
    float tslinha;
    float tm;
    float v;
    float deltaT;
    float X[2000];
    //Xant, Dx, ...
    float ob1;
    float ob2;
    float ot;
    float zt;
    float zt2;
    float zc;
    float teta17;
    //...
    //Variaveis que eram locais
    float aux, aux_tm, aux_l, aux_w, tolerancia, tolerancia_v, v2, res, F1, F2, F3;
    float ur = stat.umidade; //somente para facilitar a leitura de voces, desfacam se necessario (por memoria ou "elegancia")
    float temperatura = stat.temperatura; //idem
    float pressao = stat.pressao; //idem
    float topoDosEcos = stat.topoDosEcos; //idem
    float VIL = stat.VIL; //idem
    float DELTA;
    double pa, pb, pc, pd, pe;


    //Valores de entrada da interface: (na automatizacao deveriam ser carregados de um arquivo tipo "config.txt" por ex.)
    float v1 = 1.5; //V inicial, parametro de entrada
    float pl = 20000; //PL, parametro de entrada
    float dt = 450; //delta t dos dados (em segundos). 450 = 7.5 min de intervalo de calculos
    double e1 = 0.00403;
    double e2 = 62725.067;
    double e3 = 0.95496;
    double e4 = 0.000288;
    int limitVIL = 0; //se 1, desconsidera valores de VIL < 3 kg/m². [C99 nao suporta tipo boolean, mas limitVIL receberia 0 ou 1]

    X[0] = 1; //x inicial

    temperatura = temperatura + 273.15; //temperatura passada para Kelvin
    pressao = pressao * PTRANSF; //[pressao de saturacao umida [es umido] eq 4]
    es = ( ur / 100 ) * (A1 * ( pow ( temperatura - 223.15, 3.5) ) ); // pow(x,3.15) == x^3.15
    td = pow((es/A1),(1/3.5))+223.15;

 /* [Avaliar o grau de saturação do ar na superfície. Se w0 < wss o ar não
  * está saturado e a ascensão é adiabática, caso contrário, desde a
  * superfície a taxa de decaimento da temperatura é pseudo adiabática.] */

    wo = ( 0.622 * ( A1 * pow(td-223.15,3.5) ) / pressao ); //razao de mistura wo = ws(Td,po), eq 5
    wss = ( 0.622 * ( A1 * pow(temperatura-223.15,3.5) ) / pressao ); //razao de mistura ws(To, po), eq 6
    //se wo > wss, ascencao pseudo-adiabatica. Senao, ascencao adiabatica
    ps = pressao / ( pow((((temperatura - td) / 223.15)+1),3.5) );
    ts = temperatura / (((temperatura - td)/223.15)+1);
    teta = pow(PN / ps,0.286) * ts; //teta eq 7
    L = A - B * (ts-273.15); //calor latente de condensacao eq 15
    tetaE1 = teta * exp( L *(0.622*(A1*(pow((ts-223.15),3.5))) / ps)/(CP * ts)); //tetae equacao 14 p1588

    //Parametrizacao para encontrar v e pt. ++UTILIZACAO DOS PARAMETROS DE ENTRADA++
    do{
        pt1 = ( (e2 - pl) / (1 + e3 * v1) ) + pl; //subst v1 na eq 18, encontra pt
        plinha = ps - ( 0.25 * (ps - pt1) ); //pt1 = 1.1*ps / subst pt1 na eq 15, encontra p'
        tm = NewtonRaphson(&ts, &plinha, &tetaE1); //encontra Tm
        aux = ((0.75) * ps) + ((0.25) * pt1); //subst valor de pt na eq 16 e encontra tslinha
        tslinha = (temperatura/pow(pressao, 0.286)) * pow(aux, 0.286); //encontra tslinha
        deltaT = tm - tslinha; //subst tm e tslinha na eq 14 e encontra deltat
        v1 = e1 * pow((CP * deltaT), 0.5); //eq 13
        pt = ( (e2 - pl) / (1 + e3 * v1) ) + pl; //calcula p novamente
        res = pt1 - pt; //compara os dois valores de p
        tolerancia_v = pt1 * 2/100;
        pt1 = pt;
    }while (fabs(res) > tolerancia_v);

    v = v1;
    pt = pt1;
    c = 1 / (e4 * pow(v, K)); //c eq 19 estudo de caso
    tt = NewtonRaphson(&ts, &pt, &tetaE1);
    teta17 = f(tt, pt); //funcao

    if(topoDosEcos == -1) // se o topo dos ecos nao foi informado
    {
        zc = (R * (ts + tt) / (2*G)) * log( pressao / pt);
        zb = (R * (ts + temperatura) / (2*G)) * log( pressao / ps);
        zt = zc + zb;
        zt2 = 0.0;
    }
    else
    {
        zt2 = topoDosEcos * 1000;
        zt = (R * (tt + temperatura) / (2*G)) * log (pressao / pt);
        zb = (R * (ts + temperatura) / (2*G)) * log (pressao / ps);

        if (zt2 < zb)
            zc = 0;
        else
            zc = zt2 - zb;
    }
    wst = 0.622 * (A1 * (pow((tt - 223.15), 3.5))) / pt; //Razao de mistura ws(Tt, pt)
    umI = (wo - wst) * ((ps / (R * ts) + (pt / (R * tt))) / 2) * v * DA; //Umidade I eq 2; I[i] = (wo[i]-wst[i])*ROM*v[i]*DA;
    dast = 2.11 * pow(10,-5) * pow((temperatura / TAST), 1.94) * (PAST / pressao); //D* eq 42
    dc = pow((4 * (dast / (C1 * RV)) * zb * ((A1 * pow((temperatura - 223.15), 3.5) / temperatura) //Diametro critico Dc eq 56
                                              - (A1 * pow((td - 223.15), 3.5) / temperatura))),(0.3333));
    //Adimensionais eq 58-60
    nv = v * c / ALFA;
    nd = c * dc;
    vp = 4 * ALFA / c;

    DELTA = (0.333) * ((1/GAMA) + (1 / (GAMA*GAMA)) + (1/(GAMA*GAMA*GAMA))); //DELTA eq 32

    //Ob e Ot em funcao da massa, ou seja, nao esta multiplicado por X[i]
    F1 = (1 + (3/4) * nv + pow(nv,2) * 0.25);
    F2 = pow(nv,3) * 0.0416667;
    F3 = (F1+F2) / exp(nv);
    if (zc == 0)
        ob1 = 0;
    else
        ob1 = ((vp * X[*iteracao-1])/(DELTA * zc)) * F3;
    F1 = (1 + ((3/4) * (GAMA * nv)) + pow((GAMA * nv),2) * 0.25 + (pow(GAMA * nv,3) * 0.0416667)) / exp(GAMA * nv);
    F2 = (GAMA * nv * 0.25);
    if (zc == 0)
        ot = 0;
    else
        ot = ((vp * X[*iteracao-1]) / (DELTA * zc * pow(GAMA,5))) * (F1 + F2 - 1);

    /* X[iteração+1] = (umI - (Ob+Ot) * X[iteracao}) * dt;
     * Se nao houver dados de radar, X[iteracao] usa os dados da estacao */

    if(VIL == -1) //Se nao houver dados de radar, X[iteracao] usa os dados da estacao
        X[*iteracao] = (umI - (ob1 + ot)) * dt + X[*iteracao-1];
    else{ //Senao, usa os dados de VIL
        X[*iteracao] = VIL;
        if(X[*iteracao-1] == 0)
            ob2 = 0;
        else
            ob2 = umI - ot - ((X[*iteracao-1]) / dt);
    }

    //Precipitacao 63 - 64 resultado em unidade de massa
    if( zc == 0 || ((VIL < 3.0) && (limitVIL == 1)))
        P = 0;
    else
    {
        if(nd >= nv)
        {
            pa = (X[*iteracao] / (DELTA * zc)) * vp;
            pb = (1 - nv * 0.25) * (1 + nd + (pow(nd,2) * 0.5));
            pc = (pow(nd,3) * 0.125);
            pd = exp(nd);
            pe = (pb + pc) / pd;
            P = pa * pe;
        }
        else
            P = (X[*iteracao] / (DELTA * zc)) * vp * (1+0.75 * nv + pow(nv,2) * 0.25 + pow(nv,3) * 0.0416667 - pow(nd,3) * 0.0416667) / exp(nv);
    }
    P = P * dt; //valor acumulado em 7,5 min

    printf("\n\nTemp. (K): %f || Umid.: %f || T. orvalho: %f || Prec. Ac.: %f || Prec. Obs.: N/A || massa X: %f || Zc: %f || Zt: %f || Topo Dos Ecos: N/A || Zb: %f || Pressao (atm): %f || Erro tetaE: %f || w0: %f || ws: %f || wt: %f || ps: %f || ts: %f || teta: %f || v: %f || Pt [pa]: %f || Tt[K]: %f || Teta E[K]: %f || Sai. Ob Geo.: %f \n\n", temperatura, ur, td, P, X[*iteracao], zc/1000, zt/1000, zb/1000, pressao, teta17-tetaE1, wo, wss, wst, ps, ts, teta, v, pt, tt, tetaE1, ob1);
    return P;
    /*printf("\n v: %f // pt: %f // c: %f // tt: %f // teta17: %f // zc: %f // zb: %f // zt: %f // zt2: %f \n\n", *v, *pt, *c, *tt, *teta17, *zc, *zb, *zt, *zt2);
    ShowMessage("v[i]: " + AnsiString(v[i]) + " // pt[i]: " + AnsiString(pt[i]) + " // c[i]: " + AnsiString(c[i]) + " // Tt[i]: " + AnsiString(Tt[i]) + " // teta17[i]: " + AnsiString(teta17[i]) + " // Zc[i]: " + AnsiString(Zc[i]) + " // Zb[i]: " + AnsiString(Zb[i]) + " // Zt[i]: " + AnsiString(Zt[i]) + " // Zt2[i]: " + AnsiString(Zt2[i]));*/
}



int PrecForecast(stationData *stat, double *pobs, int *iteracao)
{
    double P;

    /* Calculos de previsao */
    P = Georgakakos(*stat, *pobs, iteracao);
    //pobs: regressao e etc

    /* Gera arquivo de saida para o SWMM */
    SaveResults(*stat, P);

    (*iteracao)++;

    return 0;
}
