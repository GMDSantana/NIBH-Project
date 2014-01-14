#include <stdio.h> /* FILE, printf */

#include "data_acquisition.h" /* stationData, DBConnect, LoadDBData */
#include "forecast.h" /* PrecForecast */
#include "swmm_interfacing.h" /* SwmmCall */

/* *******************************
 *             MAIN              *
 *********************************/
int main()
{
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

    // --- SWMM
    if ( (SwmmCall() ) != 0)
        return 1; // Erro

    return 0;
}
