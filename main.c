#include <stdio.h> /* FILE, printf */

#include "data_acquisition.h" /* stationData, DBConnect, LoadDBData */
#include "forecast.h" /* PrecForecast */
#include "swmm_interfacing.h" /* RunSwmmDll */

/* *******************************
 *             MAIN              *
 *********************************/
int main()
{
    int i; /* Iterador para for. */
    int r; /* Retorno das funcoes (representa codigo de erro ou sucesso). */
    float totalRainfall; /* Recebe do arquivo out do SWMM a precipitacao total. */
    float totalRunoff; /* Recebe do arquivo out do SWMM a escorrencia total. */
    float totalOutflow; /* Recebe do arquivo out do SWMM a vazao total de saida */
    char inpFile[] = "data/2010.inp"; /* Arquivo de entrada do SWMM. */
    char rptFile[] = "data/2010.rpt"; /* Arquivo de relatorio de execucao do SWMM. */
    char outFile[] = "data/2010.out"; /* Arquivo de saida do SWMM. */
    FILE *DBConn; // Arquivo .dat de entrada com dados da estacao
    int iteracao = 1; // = "linha" do original. Iteracao = 1 -> realiza os calculos em cima de uma linha de dados. Iteracao = 2 -> realiza calculos 2a vez...
    double pobs; // Precipitacao observada
    stationData stat; // Struct stationData com os dados da estacao

    // --- ESTABELECE CONEXAO COM O "DB" (arquivo)
    DBConn = DBConnect();
    if (DBConn == NULL) // Erro
    {
        printf("\nErro no retorno da conexao com <banco>\n");
        return 1;
    }

    // --- CARREGA DADOS DO BANCO (arquivo) NAS VARIAVEIS E CHAMA PREVISAO DE PRECIPITACAO
    while ( ( LoadDBData(DBConn, &stat, &pobs) ) == 0 )
    {
        PrecForecast(&stat, &pobs, &iteracao);
    }

    // --- CHECAGEM E UTILIZACAO DE DADOS VGI
    // VGICheck();

    /* Execucao do SWMM. */
    r = RunSwmmDll(inpFile, rptFile, outFile);
    if (r > 0)
    {
        printf("\nA execucao do SWMM nao obteve sucesso; codigo do erro = %d\n", r);
    }
    else
    {
        /* Abre outfile como um arquivo de saida do SWMM. */
        r = OpenSwmmOutFile(outFile);
        if (r == 1)
        {
            printf("\nResultados invalidos no arquivo de saida do SWMM.\n");
        }
        else if (r == 2)
            {
                printf("\nO arquivo nao eh um arquivo de saida do SWMM.\n");
            }
            else
            {
                printf("\nPeriodo    Precipitacao   Escorrencia   Vazao");
                printf("\nde Tempo   Total          Total         Total");
                printf("\n==============================================");
                for (i = 1; i <= SWMM_Nperiods; i++)
                {
                    GetSwmmResult(3, 0, 1, i, &totalRainfall);
                    GetSwmmResult(3, 0, 4, i, &totalRunoff);
                    GetSwmmResult(3, 0, 11, i, &totalOutflow);
                    printf("\n%6d    %8.2f       %8.2f    %8.2f", i, totalRainfall, totalRunoff, totalOutflow);
                }
                CloseSwmmOutFile();
            }
    }
    remove(rptFile);
    remove(outFile);
    remove("data/precOutSample.dat");

    return 0;
}
