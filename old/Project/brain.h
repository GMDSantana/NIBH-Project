#ifndef BRAIN_H_INCLUDED
#define BRAIN_H_INCLUDED

struct stationData
{
    int year;
    int date;
    int time;
    float temperatura;
    float umidade;
    float pressao;
    float topoDosEcos;
    float VIL;
};

typedef struct stationData stationData;

#endif // BRAIN_H_INCLUDED
