#include "data_acquisition.h"

/* *******************************
 *    COLETA DE DADOS DO BANCO   *
 *********************************/
int LoadDBData(FILE *inFile, stationData *stat_F, double *pobs_F)
{
    /* Por enquanto, na verdade, carregando os dados do arquivo.
     * Quando (se) um dia os dados chegarem pelo banco, refazer a funcao. */

    int dadoSendoLido = 1; //identifica o dado a ser lido do arquivo. 1 = no. da estacao, 2 = year, ...
    char c = NULL; //caractere sendo lido
    int linhaInvalida = 0;
    char year[10] = ""; //variaveis locais, receberao os dados do arquivo
    char date[10] = "";
    char time[10] = "";
    char temperatura[10] = "";
    char umidade[10] = "";
    char pressao[10] = "";
    char pobs[10] = "";
    int horas, minutos, resto;

    if ( feof(inFile) ) // Se a leitura anterior foi feita sobre a ultima linha, entao chegou ao fim do arquivo
        return 1;      // E a main deve ser avisada com o 1 para parar a leitura/calculos.

    while( c != '\n' && c != EOF ){ //le caractere por caractere ate o final da linha, armazenando os valores nas devidas variaveis
        c = fgetc(inFile);
        if(c != ',' && c!= '\n' && linhaInvalida != 1){
            switch(dadoSendoLido){
                case 1: //primeiro dado a ser lido: numero da estacao
                    if(c == '2') //se for a estacao 2, a flag linhaInvalida eh acionada porque a linha nao contem os dados completos
                        linhaInvalida = 1; //depois do switch case ela retornara 0 para "avisar" a main
                    break;
                case 2:
                    year[strlen(year)] = c;
                    break;
                case 3:
                    date[strlen(date)] = c;
                    break;
                case 4:
                    time[strlen(time)] = c;
                    break;
                case 5:
                    temperatura[strlen(temperatura)] = c;
                    break;
                case 6:
                    umidade[strlen(umidade)] = c;
                    break;
                case 7:
                    //radioacao
                    break;
                case 8:
                    //radioacaoref
                    break;
                case 9:
                    //radioacaoliq
                    break;
                case 10:
                    pressao[strlen(pressao)] = c;
                    break;
                case 11:
                    pobs[strlen(pobs)] = c;
                    break;
                default:
                    break;
            }
        }
        else{
            dadoSendoLido++;
        }
    }
    if(linhaInvalida == 1){
        LoadDBData(inFile, stat_F, pobs_F);
        return 0; //sucesso
    }
    else
    {
    /* Cada variavel local (ex.: "year") recebeu os dados direto do arquivo (em formato de string).
     * Agora elas retornam os valores para a main atraves das variaveis passadas por referencia
     * (ex.: "*year_I").*/
    stat_F->year = atoi(year);
    stat_F->date = atoi(date);
    stat_F->time = atoi(time);
    stat_F->temperatura = atof(temperatura); //retornado como float
    stat_F->umidade = atof(umidade);
    stat_F->pressao = atof(pressao);
    *pobs_F = atof(pobs);
    // Printf apresentando os valores corretamente recebidos pelas variaveis.
    printf("\nYear: %d, Date: %d, Time: %d, Temperatura: %.1f, Umidade: %.1f, Pressao: %.1f, Pobs: %.0f\n", stat_F->year, stat_F->date, stat_F->time, stat_F->temperatura, stat_F->umidade, stat_F->pressao, *pobs_F);
    system("pause");
    //radar, ifs..
    stat_F->topoDosEcos = -1;
    stat_F->VIL = -1;
    }
    return 0;          // Se chegou aqui, tudo esta correto e a main pode chamar os calculos pra linha lida.
}


/* *******************************
 * CONEXAO COM O DB (ou arquivo) *
 *********************************/
FILE *DBConnect()
{
    /* Codigo para o caso dos dados virem em arquivos. *
     * Quando (se) for por DB, colocar a conexao aqui. */

    FILE *estFile = NULL; // ponteiro para arquivo da estacao
    char estFname[100] = "DATA085_275a277.dat"; // nome do arquivo da estacao

    if( access(estFname, F_OK ) != -1 ) // Checa existencia do arquivo da estacao
    {
        if( (estFile = fopen(estFname, "r")) == NULL)
        {
            printf("Nao foi possivel abrir o arquivo de entrada da estacao.\n");
            return estFile; // Erro repassado a main
        }
        else{ // Se for possivel abrir o arqivo, retorna ponteiro a main
                //printf("%d", estFile);
           // printf("\n%p", estFile);
            return estFile;
        }
    }
    else
    {
        printf("Arquivo de entrada nao encontrado.\n");
        return estFile; // Erro repassado a main
    }
}
