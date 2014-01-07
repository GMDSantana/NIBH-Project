#ifndef BRAIN_H_INCLUDED
#define BRAIN_H_INCLUDED

struct stationData
{
    int ano;
    int data;
    int tempo;
    float temperatura;
    float umidade;
    float pressao;
    float topoDosEcos;
    float VIL;
};

typedef struct stationData stationData;

#endif // BRAIN_H_INCLUDED
