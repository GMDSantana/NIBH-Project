#include "swmm_interfacing.h"

/* *******************************
 *      CHAMADA PARA O SWMM      *
 *********************************/
int SwmmCall()
{
    // Nomes dos arquivos setados "manualmente", assim como na previsao.c em SalvaResultados.
    // Isso deve ser acertado durante a fase de sincronizacao.
    char swmmPath[100] = "SWMM\\EPA_SWMM_5.0\\swmm5.exe "; // caminho para o executavel do swmm
    char inpFName[50] = "FrutalTeste.inp "; //nome do arquivo de entrada
    char rptFName[50] = "FrutalTesteOut1.rpt "; // nome do arquivo report saida
    char outFName[50] = "FrutalTesteOut2.out\""; // nome do arquivo binario saida
    char swmmCallCat[300] = {0};
    int res;

    strcat(swmmCallCat, swmmPath);
    strcat(swmmCallCat, inpFName);
    strcat(swmmCallCat, rptFName);
    strcat(swmmCallCat, outFName);

    if( ( system(swmmCallCat) ) != 0)
        return 1; // erro

    return 0;
}
