#include <stdio.h>
#include <unistd.h>
#include "forecast.c"
#include "brain.h"


#define MAX_LSIZE 300 // max line size
#define MAX_WSIZE 30  // max word size


/* *******************************
 *    ANALISE DE RESULTADOS      *
 *********************************/
 int AnResult(float od[], float dqo[], float ft[], float st[], float coltot[])
 {
    FILE *rptF;
    char tableKey[MAX_LSIZE] = "  Subcatchment Washoff Summary\n";
    char buffer[MAX_LSIZE];
    char header1[MAX_LSIZE]; // 1a linha do header
    char header2[MAX_LSIZE]; // 2a linha do header
    char columnData[MAX_LSIZE];
    char *rowPBuff;
    int rowPos;
    int dataPos;
    int i;
    int idx;
    float od2[58];
    float dqo2[58];
    float dbo2[58];
    float ft2[58];
    float st2[58];
    int flagout = 1;
    char opt;
    int ibacia;


    //printf("IDX: %d", *idx);
    // Abrir arquivo: se falhar
    if( (rptF = fopen("SaidaSwmm1.rpt", "r")) == NULL)
    {
        printf("ERRO: Nao foi possivel abrir o arquivo .rpt. Abortando!\n");
        fclose(rptF);
        return 1; // Erro repassado a main
    }

    // Abrir arquivo: se sucesso
    else{
        do{ // encontra a linha [Subcatchment Washoff Summary]
            fgets(buffer, MAX_LSIZE, rptF);
        }while( (strcmp(tableKey,buffer)) != 0 ); // repete enquanto essa 1a palavra nao for a desejada
        fgets(buffer, MAX_LSIZE, rptF);
        fgets(buffer, MAX_LSIZE, rptF);
        fgets(buffer, MAX_LSIZE, rptF);
        fgets(buffer, MAX_LSIZE, rptF);
        fgets(buffer, MAX_LSIZE, rptF);
        fgets(buffer, MAX_LSIZE, rptF);

        for ( i = 1; i <= 57; i++){
            fscanf(rptF, "%d\t%f\t%f\t%f\t%f\t%f", &idx, &od2[i], &dqo2[i], &dbo2[i], &ft2[i], &st2[i]);
            //printf("\n%d\t%f\t%f\t%f\t%f\t%f", idx, od2[i], dqo2[i], dbo2[i], ft2[i], st2[i]);
            }
        while(flagout)
        {
            system("cls");
            printf("Numero da sub-bacia a analisar ou 0 para sair: ");
            scanf("%d", &ibacia);
            if(ibacia == 0)
                return 0;
            if(ibacia < 0 || ibacia > 57)
                printf("Range invalida.\n");
            else{
                printf("\n***********************************\n");
                printf("      [CARGAS] SUB-BACIA %d", ibacia);
                printf("\n***********************************\n");
                printf("-------------------------------------------------------\n");
                printf("\t\tInicial\t\tFinal\n");
                printf("OD\t\t%.3f\t\t%.3f\n", od[ibacia], od2[ibacia]);
                printf("DQO\t\t%.3f\t\t%.3f\n", dqo[ibacia], dqo2[ibacia]);
                printf("FT\t\t%.3f\t\t%.3f\n", ft[ibacia], ft2[ibacia]);
                printf("ST\t\t%.3f\t\t%.3f\n", st[ibacia], st2[ibacia]);
                system("pause");
            }

        }


        fclose(rptF);
    }
    printf("\nEstado inicial armazenado.\n");

    return 0;
 }

/* *******************************
 *      CHAMADA PARA O SWMM      *
 *********************************/
int SwmmCall(char inpFName[])
{
    // Nomes dos arquivos setados "manualmente", assim como na previsao.c em SalvaResultados.
    // Isso deve ser acertado durante a fase de sincronizacao.
    char swmmPath[100] = "\"..\\..\\EPA_SWMM_5.0\\swmm5.exe ";
    char whitespace[3] = {' '};
    char rptFName[50] = "SaidaSWMM1.rpt ";
    char outFName[50] = "SaidaSWMM2.out\"";
    char swmmCallCat[300] = {0};
    int res;

    printf("\nChamando SWMM para %s...\n", inpFName);
    strcat(inpFName, whitespace);

    strcat(swmmCallCat, swmmPath);
    strcat(swmmCallCat, inpFName);
    strcat(swmmCallCat, rptFName);
    strcat(swmmCallCat, outFName);

    if( ( system(swmmCallCat) ) != 0)
        return 1; // erro

    printf("\n\nSWMM finalizado. Pode ser necessario limpar os arquivos de saida do SWMM no diretorio.\n");
    printf("\nArquivos de saida:\n");
    printf("%s => Arquivo binario do SWMM\n", outFName);
    printf("%s => Arquivo texto do SWMM\n", rptFName);
    return 0;
}

/* *******************************
 * CHECK/UTILIZACAO DE DADOS VGI *
 *********************************/
int VGICheck(char inpFName[])
{
    FILE *inpF;
    FILE *vgiF;
    char tableKey[30] = "[CONDUITS]";
    char buffer[MAX_LSIZE];
    int nodeVGI;
    float flowVGI;
    int nodeInp = 0;
    float inNode, outNode, leng, manN, inOff, outOff, flowInp, maxFlow;
    int pos1, pos2;

    printf("\nChecagem VGI...\n");

    // Se ha dados vgi, modifica .inp com novos niveis
    if( access("VGIData.dat", F_OK ) != -1 ) // Checa existencia do arquivo da estacao
    {
        printf("...Dados VGI encontrados, substituindo no .INP em %s...", tableKey);

        // Abrindo arquivo com dados VGI para carregar dados
        if( (vgiF = fopen ("VGIData.dat", "r")) == NULL)
        {
            return 1;
        }
        else
        {
            // Abrindo .inp para atualizar dados
            if( (inpF = fopen(inpFName, "r+")) == NULL)
            {
                return 1;
            }
            else
            {
                do{ // encontra a linha [CONDUITS]
                    fscanf(inpF, "%s ", buffer); // le 1a palavra de cada linha
                }while( (strcmp(tableKey,buffer)) != 0 ); // repete enquanto essa 1a palavra nao for a desejada
                printf("Buff: %s", buffer);
                fgets(buffer, MAX_LSIZE, inpF);
                fgets(buffer, MAX_LSIZE, inpF);
                fgets(buffer, MAX_LSIZE, inpF);

                fgets(buffer, MAX_LSIZE, vgiF);

                fscanf(vgiF, "%d\t%f", &nodeVGI, &flowVGI);

                    printf("\nNode scaneado: %d, flowVGI: %f\n", nodeVGI, flowVGI);
                    system("pause");
                    while(nodeVGI != nodeInp){
                        system("pause");
                        pos1 = ftell(inpF);
                        printf("\nPos b4 inpf scanf: %d\n", pos1);
                        fscanf(inpF, "%d%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f", &nodeInp, &inNode, &outNode, &leng, &manN, &inOff, &outOff, &flowInp, &maxFlow);
                        printf("Inp scaneado: %d // %f // %f // %f // %f / %f // %f // %f // %f\n", nodeInp, inNode, outNode, leng, manN, inOff, outOff, flowInp, maxFlow);
                        pos2 = ftell(inpF);
                        printf("Pos after inpf scanf: %d\n", pos2);
                    }
                    fseek(inpF, pos1, SEEK_SET);
                    fprintf(inpF, "\n%d\t\t %.0f\t\t  %.0f\t\t   %.1f      %.3f\t%.1f\t    %.0f\t       %.3f\t  %.0f                                                                 ", nodeInp, inNode, outNode, leng, manN, inOff, outOff, flowVGI, maxFlow);
                    printf("\nPrinting: %d\t\t%.1f\t\t%.1f\t\t%.1f\t\t%.3f\t%.1f\t\t%.1f\t%.3f\t\t%.2f\n", nodeInp, inNode, outNode, leng, manN, inOff, outOff, flowVGI, maxFlow);

            }
            fclose(inpF);
        }
        fclose(vgiF);
    }
    else
    {
        printf("...Dados nao existem.\n");
    }


    // Senao, prossegue
    return 0;
}

/* *******************************
 *    COLETA DE DADOS DO BANCO   *
 *********************************/
int CarregaDadosDB(FILE *inFile, stationData *stat_F, double *pobs_F)
{
    /* Por enquanto, na verdade, carregando os dados do arquivo.
     * Quando (se) um dia os dados chegarem pelo banco, refazer a funcao. */

    int dadoSendoLido = 1; //identifica o dado a ser lido do arquivo. 1 = no. da estacao, 2 = ano, ...
    char c = ""; //caractere sendo lido
    int linhaInvalida = 0;
    char ano[10] = ""; //variaveis locais, receberao os dados do arquivo
    char data[10] = "";
    char tempo[10] = "";
    char temperatura[10] = "";
    char umidade[10] = "";
    char pressao[10] = "";
    char pobs[10] = "";
    int horas, minutos, resto;

    if ( feof(inFile) ) // Se a leitura anterior foi feita sobre a ultima linha, entao chegou ao fim do arquivo
        return 1;      // E a main deve ser avisada com o -1 para parar a leitura/calculos.

    while( c != '\n' && c != EOF ){ //le caractere por caractere ate o final da linha, armazenando os valores nas devidas variaveis
        c = fgetc(inFile);
        if(c != ','){
            switch(dadoSendoLido){
                case 1: //primeiro dado a ser lido: numero da estacao
                    if(c == '2') //se for a estacao 2, a flag linhaInvalida eh acionada porque a linha nao contem os dados completos
                        linhaInvalida = 1; //depois do switch case ela retornara 0 para "avisar" a main
                    break;
                case 2:
                    ano[strlen(ano)] = c;
                    break;
                case 3:
                    data[strlen(data)] = c;
                    break;
                case 4:
                    tempo[strlen(tempo)] = c;
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
        CarregaDadosDB(inFile, stat_F, pobs_F);
        return 0;
    }
    else
    {
    /* Cada variavel local (ex.: "ano") recebeu os dados direto do arquivo (em formato de string).
     * Agora elas retornam os valores para a main atraves das variaveis passadas por referencia
     * (ex.: "*ano_I").*/
    stat_F->ano = atoi(ano);
    stat_F->data = atoi(data);
    stat_F->tempo = atoi(tempo);
    stat_F->temperatura = atof(temperatura); //retornado como float
    stat_F->umidade = atof(umidade);
    stat_F->pressao = atof(pressao);
    *pobs_F = atof(pobs);

    //radar, ifs..
    stat_F->topoDosEcos = -1;
    stat_F->VIL = -1;
    }
    return 0;          // Se chegou aqui, tudo esta correto e a main pode chamar os calculos pra linha lida.
}


/* *******************************
 * CONEXAO COM O DB (ou arquivo) *
 *********************************/
int DBConnect(char estFname[])
{
    /* Codigo para o caso dos dados virem em arquivos. *
     * Quando (se) for por DB, colocar a conexao aqui. */

    FILE *estFile; // ponteiro para arquivo da estacao

    if( access(estFname, F_OK ) != -1 ) // Checa existencia do arquivo da estacao
    {
        if( (estFile = fopen(estFname, "r")) == NULL)
        {
            printf("Nao foi possivel abrir o arquivo de entrada da estacao.\n");
            return 1; // Erro repassado a main
        }
        else
        {// Se for possivel abrir o arqivo, retorna ponteiro a main
            printf("\nConexao com eventos realizada com sucesso!\n\n", estFname);
            return estFile;
        }
    }
    else
    {
        printf("ERRO: Arquivo de entrada nao encontrado, abortando!\n");
        return 1; // Erro repassado a main
    }
    return 0; // anti warning
}


/* *******************************
 * ARMAZENAR DADOS DO .INP          *
 *********************************/
int ArmazenaDadosSWMM(char inpFName[], int *idx, float od[], float dqo[], float ft[], float st[], float coltot[])
{
    FILE *FInp;
    char tableKey[MAX_WSIZE] = "[LOADINGS]";
    char buffer[MAX_LSIZE];
    char bufferc[MAX_LSIZE];
    char header1[MAX_LSIZE]; // 1a linha do header
    char header2[MAX_LSIZE]; // 2a linha do header ou separador ---
    char header3[MAX_LSIZE]; // 3a linha do header ou separador ---
    char columnKey[MAX_WSIZE] = "Source"; // (DATA) source
    char columnData[MAX_LSIZE];
    char whatever[300];
    char *rowPBuff;
    int rowPos;
    int dataPos;
    int i;

    //printf("IDX: %d", *idx);
    // Abrir arquivo: se falhar
    if( (FInp = fopen(inpFName, "r")) == NULL)
    {
        printf("ERRO: Nao foi possivel abrir o arquivo .inp. Abortando!\n");
        fclose(FInp);
        return 1; // Erro repassado a main
    }

    // Abrir arquivo: se sucesso
    else{
        printf("\nAbrindo %s para armazenamento do estado inicial...", inpFName);
        do{ // encontra a linha [LOADINGS]
            fscanf(FInp, "%s ", buffer); // le 1a palavra de cada linha
        }while( (strcmp(tableKey,buffer)) != 0 ); // repete enquanto essa 1a palavra nao for a desejada
        // Leitura dos headers
        fgets(header1, MAX_LSIZE, FInp);
        fgets(header2, MAX_LSIZE, FInp);

        for ( i = 1; i <= 57; i++){
            if(i == 40){
                fscanf(FInp, "%d", idx);
                fscanf(FInp, "%c", &bufferc);
                fscanf(FInp, "\t%s\t%f", &buffer, &od[*idx]);

                fscanf(FInp, "%d", idx);
                fscanf(FInp, "%c", &bufferc);
                fscanf(FInp, "\t%s\t%f", &buffer, &dqo[*idx]);

                fscanf(FInp, "%d", idx);
                fscanf(FInp, "%c", &bufferc);
                fscanf(FInp, "\t%s\t%f", &buffer, &ft[*idx]);

                fscanf(FInp, "%d", idx);
                fscanf(FInp, "%c", &bufferc);
                fscanf(FInp, "\t%s\t%f", &buffer, &st[*idx]);

                fscanf(FInp, "%d", idx);
                fscanf(FInp, "%c", &bufferc);
                fscanf(FInp, "\t%s\t%f", &buffer, &od[*idx]);

                fscanf(FInp, "%d", idx);
                fscanf(FInp, "%c", &bufferc);
                fscanf(FInp, "\t%s\t%f", &buffer, &dqo[*idx]);

                fscanf(FInp, "%d", idx);
                fscanf(FInp, "%c", &bufferc);
                fscanf(FInp, "\t%s\t%f", &buffer, &ft[*idx]);

                fscanf(FInp, "%d", idx);
                fscanf(FInp, "%c", &bufferc);
                fscanf(FInp, "\t%s\t%f", &buffer, &st[*idx]);
            }
            else{
                fscanf(FInp, "%d", idx);
                fscanf(FInp, "\t%s\t%f", &buffer, &od[*idx]);

                fscanf(FInp, "%d", idx);
                fscanf(FInp, "\t%s\t%f", &buffer, &dqo[*idx]);

                fscanf(FInp, "%d", idx);
                fscanf(FInp, "\t%s\t%f", &buffer, &ft[*idx]);

                fscanf(FInp, "%d", idx);
                fscanf(FInp, "\t%s\t%f", &buffer, &st[*idx]);
            }
        }
        fclose(FInp);
    }
    printf("\n...Estado inicial armazenado!\n");
    return 0;
}

/* *******************************
 *             MAIN              *
 *********************************/
int main()
{
    FILE *datInF = NULL; // Arquivo dat de entrada com dados da estacao
    int iteracao = 1; // = "linha" do original. Iteracao = 1 -> realiza os calculos em cima de uma linha de dados. Iteracao = 2 -> realiza calculos 2a vez...
    double pobs; // Precipitacao observada
    stationData stat; // Struct stationData com os dados da estacao
    char arqeventos[50];
    char inpFName[50];
    int idx;
    float od[58];
    float dqo[58];
    float ft[58];
    float st[58];
    float coltot[58];
    char opt = 'a';
    double Pmod;
    int precCalc = 1;


    printf("Insira o nome do arquivo de eventos (.dat): ");
    scanf("%s", arqeventos);

    printf("Insira o arquivo do modelo do SWMM (.inp): ");
    scanf("%s", inpFName);
    printf("Arq do modelo: %s\n", inpFName);

    od[1] = 3;
    if ( ArmazenaDadosSWMM(inpFName, &idx, &od, &dqo, &ft, &st, &coltot)  != 0 )
        return 1;
    printf("APOS ARMAZ DADOS: %s\n", inpFName);

    // --- ESTABELECE CONEXAO COM O "DB" (arquivo)
    datInF = DBConnect(arqeventos);
    if (datInF == 1) // Erro
    {
        printf("ERRO no retorno da conexao com 'banco'. Abortando!\n");
        return 1;
    }

    printf("Carregando dados para as variaveis...\n");
    printf("...Dados carregados.\n\nCalculando previsao de precipitacao...\n");
    printf("...Previsao calculada.\n");

    printf("\nComparacao de precipitacao modelada e observada: \n\n");
    printf("INSTANTE\tPREC. MODELADA\tPREC. OBSERVADA\n");
    printf("----------------------------------------------------------\n");
    // --- CARREGA DADOS DO BANCO (arquivo) NAS VARIAVEIS E CHAMA PREVISAO DE PRECIPITACAO
    while ( ( CarregaDadosDB(datInF, &stat, &pobs) ) == NULL ){
        Pmod = precipitacao(&stat, &pobs, &iteracao);
        if (precCalc <= 10)
        {
            printf("t%d\t\t%f\t%f\n", precCalc, Pmod, pobs);
            precCalc++;
        }
    }

    // --- CHECAGEM E UTILIZACAO DE DADOS VGI
    VGICheck(inpFName);

    // --- SWMM
    if ( (SwmmCall(inpFName) ) != NULL)
        return 1; // Erro

    // --- ANALISE DOS RESULTADOS
    while(opt != 's' && opt != 'n')
    {
        fflush(stdin);
        printf("\nDeseja iniciar analise dos resultados? (s/n): ");
        scanf("%c", &opt);
    }

    if(opt == 's')
    {
        printf("\nIniciando analise dos resultados...");
        if ( (AnResult(&od, &dqo, &ft, &st, &coltot) != 0) )
            return 1;
    }

    system("pause");

    return 0;
}

///swmmcall inside while
