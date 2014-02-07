/*  swmm_interfacing.h.
    Arquivo de cabecalho para as funcoes de interfaceamento com o SWMM 5.
    #include este arquivo em qualquer modulo C que referencie as funcoes contidas em swmm_interfacing.c. */

#ifndef SWMM_INTERFACING_H
#define SWMM_INTERFACING_H

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "swmm5.h"

int    SWMM_Nperiods;   /* Numero de periodos relatados. */
int    SWMM_FlowUnits;  /* Codigo das unidades de fluxo. */
int    SWMM_Nsubcatch;  /* Numero de subcatchments. */
int    SWMM_Nnodes;     /* Numero de nos de sistema de drenagem. */
int    SWMM_Nlinks;     /* Numero de ligacoes de sistema de drenagem. */
int    SWMM_Npolluts;   /* Numero de poluentes rastreados. */
double SWMM_StartDate;  /* Data inicial da simulacao. */
int    SWMM_ReportStep; /* Instante de tempo do relatorio. */


/* Utilizada para rodar o SWMM pelo swmm5.exe. */
int    RunSwmmExe(char *cmdLine);

/* Utilizada para rodar o SWMM pelo swmm5.dll (compativel apenas com Windows). */
int    RunSwmmDll(char *inpFile, char *rptFile, char *outFile);

/* Abre dados do arquivo de saida do SWMM para exibicao. */
int    OpenSwmmOutFile(char *outFile);

/* Recupera resultado do SWMM. */
int    GetSwmmResult(int iType, int iIndex, int vIndex, int period, float *value);

/* Fecha arquivo de saida do SWMM. */
void   CloseSwmmOutFile(void);

int UpdateInitFlow(char *inpFile, char *link, double value);

int UpdateInitDepth(char *inpFile, char *link, double value);

#endif /* SWMM_INTERFACING_H */
