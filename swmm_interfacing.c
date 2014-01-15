#include "swmm_interfacing.h"

static const int SUBCATCH = 0;   /* Subcatch. */
static const int NODE     = 1;   /* No. */
static const int LINK     = 2;   /* Ligacao. */
static const int SYS      = 3;   /* Sistema. */
static const int RECORDSIZE = 4; /* Numero de butes por gravacao em arquivo. */

static int SubcatchVars; /* Numero de variaveis de relatorio de subcatch. */
static int NodeVars;     /* Numero de variaveis de relatorio de no. */
static int LinkVars;     /* Numero de variaveis de relatorio de ligacao. */
static int SysVars;      /* Numero de variaveis de relatorio de sistema. */

static FILE*  Fout;                  /* Manuseio do arquivo. */
static int    StartPos;              /* Posicao do arquivo onde os resultados comecam. */
static int    BytesPerPeriod;        /* Bytes usados para resultados em cada periodo. */
static void   ProcessMessages(void); /* Mensagens de processo. */

int RunSwmmExe(char *cmdLine)
{
    int exitCode;
    STARTUPINFO si;
    PROCESS_INFORMATION  pi;

    /* Inicializa estruturas de dados. */
    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));
    si.cb = sizeof(si);
    si.wShowWindow = SW_SHOWNORMAL;

    /* Carrega swmm5.exe. */
    exitCode = CreateProcess(NULL, cmdLine, NULL, NULL, 0, 0, NULL, NULL, &si, &pi);

    /* Espera o programa terminar. */
    exitCode = WaitForSingleObject(pi.hProcess, INFINITE);

    /* Recupera o codigo de erro produzido pelo programa. */
    GetExitCodeProcess(pi.hProcess, &exitCode);

    /* Libera hadles. */
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return exitCode;
}

int RunSwmmDll(char *inpFile, char *rptFile, char *outFile)
{
    int err;
    double elapsedTime;

    /* Abre um projeto SWMM. */
    err = swmm_open(inpFile, rptFile, outFile);
    if (!err)
    {
        /* Inicializa todos os sistemas em processamento. */
        err = swmm_start(1);
        if (err == 0)
        {
            /* Passa pela simulacao. */
            do
            {
                /* Permite ao Windows processar qualquer evento pendente. */
                ProcessMessages();

                /* Extende a simulacao em um passo de tempo de roteamento. */
                err = swmm_step(&elapsedTime);

                /*
                *************************************************
                * Chame funcoes de relatorio de progresso aqui, *
                * utilizando elapsedTime como um argumento.     *
                *************************************************
                */

            } while (elapsedTime > 0.0 && err == 0);

            /* Fecha todos os sistemas em processamento. */
            swmm_end();
        }
    }

    /* Fecha o projeto. */
    swmm_close();
    return err;
}

void ProcessMessages(void)
{
    /****  Apenas use esta funcao com uma aplicacao Win32. *****
    MSG msg;
    while (TRUE)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) break;
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else break;
    }
    ***********************************************************/

}

int OpenSwmmOutFile(char *outFile)
{
    int magic1, magic2, errCode, offset, offset0, version;
    int err;

    /* Abre arquivo de saida. */
    Fout = fopen(outFile, "rb");
    if (Fout == NULL) return 2;

    /* Checa se o arquivo contem pelo menos 14 gravacoes. */
    fseek(Fout, 0L, SEEK_END);
    if (ftell(Fout) < 14 * RECORDSIZE)
    {
        fclose(Fout);
        return 1;
    }

    /* Le parametros do fim do arquivo. */
    fseek(Fout, -5 * RECORDSIZE, SEEK_END);
    fread(&offset0, RECORDSIZE, 1, Fout);
    fread(&StartPos, RECORDSIZE, 1, Fout);
    fread(&SWMM_Nperiods, RECORDSIZE, 1, Fout);
    fread(&errCode, RECORDSIZE, 1, Fout);
    fread(&magic2, RECORDSIZE, 1, Fout);

    /* Le numero magico do comeco do arquivo. */
    fseek(Fout, 0L, SEEK_SET);
    fread(&magic1, RECORDSIZE, 1, Fout);

    /* Desempenha checagem de erro. */
    if (magic1 != magic2) err = 1;
    else if (errCode != 0) err = 1;
    else if (SWMM_Nperiods == 0) err = 1;
    else err = 0;

    /* Sai se nao encontrou erros. */
    if (err > 0 )
    {
        fclose(Fout);
        Fout = NULL;
        return err;
    }

    /* Senao, le parametros adicionais do comeco do arquivo. */
    fread(&version, RECORDSIZE, 1, Fout);
    fread(&SWMM_FlowUnits, RECORDSIZE, 1, Fout);
    fread(&SWMM_Nsubcatch, RECORDSIZE, 1, Fout);
    fread(&SWMM_Nnodes, RECORDSIZE, 1, Fout);
    fread(&SWMM_Nlinks, RECORDSIZE, 1, Fout);
    fread(&SWMM_Npolluts, RECORDSIZE, 1, Fout);

    /* Pula os valores de entrada de subcatch/nos/ligacoes salvos. */
    offset = (SWMM_Nsubcatch + 2) * RECORDSIZE /* Area de subcatchment */
            + (3*SWMM_Nnodes + 4) * RECORDSIZE /* Tipo do no, inverte e profundidade maxima. */
            + (5*SWMM_Nlinks + 6) * RECORDSIZE; /* Tipo do link, z1, z2, profundidade maxima e comprimento. */
    offset = offset0 + offset;
    fseek(Fout, offset, SEEK_SET);

    /* Le numeros e codigos de variaveis computadas. */
    fread(&SubcatchVars, RECORDSIZE, 1, Fout); /* # Variaveis de subcatch. */
    fseek(Fout, SubcatchVars*RECORDSIZE, SEEK_CUR);
    fread(&NodeVars, RECORDSIZE, 1, Fout);     /* # Variaveis de no. */
    fseek(Fout, NodeVars*RECORDSIZE, SEEK_CUR);
    fread(&LinkVars, RECORDSIZE, 1, Fout);     /* # Variaveis de ligacao. */
    fseek(Fout, LinkVars*RECORDSIZE, SEEK_CUR);
    fread(&SysVars, RECORDSIZE, 1, Fout);      /* # Variaveis de sistema. */

    /* Le dados logo antes do comeco dos resultados de saida. */
    offset = StartPos - 3 * RECORDSIZE;
    fseek(Fout, offset, SEEK_SET);
    fread(&SWMM_StartDate, sizeof(double), 1, Fout);
    fread(&SWMM_ReportStep, RECORDSIZE, 1, Fout);

    /* Computa numero de bytes de resultados de valores usados por periodo de tempo. */
    BytesPerPeriod = 2 * RECORDSIZE + /* Valor de data (um double). */
                    (SWMM_Nsubcatch * SubcatchVars +
                    SWMM_Nnodes * NodeVars +
                    SWMM_Nlinks * LinkVars +
                    SysVars) * RECORDSIZE;

    /* Retorna com arquivo deixado aberto. */
    return err;
}

int GetSwmmResult(int iType, int iIndex, int vIndex, int period, float *value)
{
    int offset;

    /* Computa offset no arquivo de saida. */
    *value = 0.0;
    offset = StartPos + (period - 1) * BytesPerPeriod + 2 * RECORDSIZE;
    if ( iType == SUBCATCH )
    {
        offset += RECORDSIZE * (iIndex * SubcatchVars + vIndex);
    }
    else if (iType == NODE)
    {
        offset += RECORDSIZE * (SWMM_Nsubcatch * SubcatchVars +
                iIndex * NodeVars + vIndex);
    }
    else if (iType == LINK)
    {
        offset += RECORDSIZE * (SWMM_Nsubcatch * SubcatchVars +
                SWMM_Nnodes * NodeVars +
                iIndex * LinkVars + vIndex);
    }
    else if (iType == SYS)
    {
        offset += RECORDSIZE * (SWMM_Nsubcatch * SubcatchVars +
                SWMM_Nnodes * NodeVars +
                SWMM_Nlinks * LinkVars + vIndex);
    }
    else return 0;

    /* Re-posiciona o arquivo e le o resultado. */
    fseek(Fout, offset, SEEK_SET);
    fread(value, RECORDSIZE, 1, Fout);
    return 1;
}

void CloseSwmmOutFile(void)
{
    if (Fout != NULL)
    {
        fclose(Fout);
        Fout = NULL;
    }
}
