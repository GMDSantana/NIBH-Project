#ifndef DATA_ACQUISITION_H
#define DATA_ACQUISITION_H

#include <stdio.h> /* feof, fgetc, EOF, FILE, fopen, */
#include <stdlib.h> /* system, atoi, atof */
#include <string.h> /* strlen */
#include <unistd.h> /* access, F_OK */

typedef struct stationData
{
    int year;
    int date;
    int time;
    float temperatura;
    float umidade;
    float pressao;
    float topoDosEcos;
    float VIL;
} stationData;

int LoadDBData(FILE *, stationData *, double *);

FILE *DBConnect();

#endif /* DATA_ACQUISITION_H */
