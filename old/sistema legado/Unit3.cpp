//---------------------------------------------------------------------------

#include <vcl.h>
#include <math.h>
#pragma hdrstop
#include <Math.h>
#include <math.hpp>


#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include "Unit3.h"
/*
#include "TS.h"
#include "TP.h"
#include "nsga.h"
#include "nsgaii.h"
#include "spea.h"
*/

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//Constantes
#define PTRANSF 133.32231202221 //Transforma mmHg em Pa
#define A1 0.0008 //A1 = 8.10^-4 Kg/(m.s2.k3,5)
#define PN 100000 //Pressao nominal 10^5 Kg/(m.s2)
#define A 2500000 //Constante da eq. (14) do calor latente A=2,5.10^6 J/Kg
#define B 2380 //Constante da eq. (14) do calor latente B=2,38.10^3 J/(Kg.K)
#define CP 1004 //Calor especifico à pressao constante cp=1004 J/(kg.K)
//#define ROM 1.2 //Massa específica média do ar úmido da eq. (2) kg/m3
#define DA 1 //Diferencial de área dA
#define ALFA 3500 //3500 s-1
#define Tast 273.15 //T* [ºK] eq 42
#define past 101325 //p* [kg/(m*s2) eq 42
#define C1 700000 //C1 [kg/(m3.s2)] pag 1593
#define RV 461 //Constante dos gases para vapor de água [J/(kg.K)] pag 1592
#define R 287 //J/(K.Kg)
#define G 9.81 //Aceleração da gravidade m/s2
#define K 0 //Considerado pag 1602
#define GAMA 1
#pragma link "Excel_2K_SRVR"
#include "sysvari.h"

FILE *streamLog;

TFPrincipal *FPrincipal;
//---------------------------------------------------------------------------
__fastcall TFPrincipal::TFPrincipal(TComponent* Owner)
        : TForm(Owner)
{
    linha=0;
    total_linha=4;
    tam_vet_erro=0;
    S1 = 0;
    S2=0;
    streamLog = fopen("log.txt", "wt");

}

//---------------------------------------------------------------------------
//----------------------------------------------------------
//procedimento para liberar a memoria
void TFPrincipal::libera_memoria()
{
//Desalocacao da memoria
    delete ToC;
    delete ur;
    delete pommHg;
    delete To;
    delete po;
    delete Td;
    delete ps;
    delete Ts;
    delete es;
    delete wo;
    delete wss;
    delete wst;
    delete teta;
    delete L;
    delete tetaE1;
    delete I;
    delete Nv;
    delete vbeta;
    delete c;
    delete Dast;
    delete Tt;
    delete pt;
    delete P;
    delete Zb;
    delete Dc;
    delete Nd;
    delete vp;
    delete teta17;
    delete teste;
    delete Pobs;
    delete X;
    delete Zt;
    delete Zt2;
    delete Zc;
    delete VIL;
    delete TopoDosEcos;
}

void TFPrincipal::alocacao_dinamica_vetores(int n)
{

//Alocação dinamica dos vetores
    dia = new int[n] ;
    hora = new int [n];
    ToC = new float[n]; //Teperatura em ºC
    ur = new float [n]; //Umidade relativa na superficie
    pommHg = new float[n]; //Pressao em mm Hg

    To = new double [n]; //Temperatura em ºK
    po = new float [n]; //Pressao em Pa
    Td = new double [n]; //Temp ponto de orvalho ºK
    ps = new double [n]; //Pressao na base da nuvem
    Ts = new double [n]; //Pressao na base da nuvem
    es = new double [n]; //Pressao de saturacao umida
    wo = new double [n]; //Razao de mistura inicial
    wss = new float [n] ; //Razao de mistura na base
    Tt = new float [n];
    pt = new float [n];
    wst = new double [n]; //Razao de mistura no topo
    teta = new double [n]; //temperatura potencial teta
    L = new float [n]; //Calor latente de condensação
    tetaE1 = new double [n];//temperatura potencial teta e
    I = new float [n]; //Entrada de umidade I
    Nv = new float [n]; //Adimensional de velocidade
    vbeta = new float [n]; //Velocidade v beta
    c = new float [n]; //Inverso do diâmetro médio
    Dast = new double [n]; //D* [equacao 42]
    P = new double [n]; //PWV
    obs = new double [n*2];
    erro = new double[n];
//S1 = new double;
//S2 = new double;
    Zb = new double [n]; //Zb
    Dc = new float [n];
    Nd = new float [n];
    vp = new float [n];
    pt1= new float [n];
    pt2= new float [n];
    plinha = new float [n];
    tslinha = new float [n];
    Tm = new float [n];
    v= new float [n];
    DELTAT= new float [n];
    X = new float [n];
    Xant = new float [n];
    Dx = new float [n];
    Ob1 = new float [n];
    Ob2 = new float [n];
    Ot = new float [n];
    Zt = new double [n];
    Zt2 = new double [n];
    Zc = new double [n];
    teta17 = new double [n];
    teste = new int [n];
    Tw = new float [n];
    Tw1 = new float [n];
    Pobs = new double [n]; //Precipitação observada na estação
    VIL = new float[n];
    TopoDosEcos = new float[n];


    DeltaVol = 0.0;
    Correl = 0.0;
}

//----- calcula pressao de saturacao do ambiente-------------------------inicio
float pressao_saturacao(float temp_externa)
{
    float pressao=0.0;

    pressao = 611*(exp((17.27*temp_externa)/(237.3 + temp_externa)));
    return pressao;
}
//----- calcula pressao de saturacao do ambiente--------------------------fim

//----- calcula temperatura ponto de orvalho-----------------------------inicio
float ponto_orvalho(AnsiString umido)
{
    Extended X;
    X = log(StrToFloat(umido));
    return X;
}
//----- calcula temperatura ponto de orvalho------------------------------fim

void __fastcall TFPrincipal::FormActivate(TObject *Sender)
{
    Application->Title = "Andrade & Gonçalves";
    SG_final->Cells[C_ANO][0] = "Ano";
    SG_final->Cells[C_DATA][0] = "Data";
    SG_final->Cells[C_TEMPO][0] = "Tempo(Hora/Min)";
    SG_final->Cells[C_TEMPERATURA][0] = "Temp. K"; //coluna * linha
    SG_final->Cells[C_UMIDADE][0] = "Umidade do Ar";
    SG_final->Cells[C_TEMPORV][0] = "Temp. Orvalho [K]";
    SG_final->Cells[C_PRECAC][0] = "Prec. Ac. em 7,5min [mm]";
    SG_final->Cells[C_POBS][0] = "Prec. Obs. em 7,5min [mm]";
    SG_final->Cells[C_X][0] = "massa X [kg/m2]";
    SG_final->Cells[C_ZC][0] = "Zc [km]";
    SG_final->Cells[C_ZT][0] = "Zt [km]";
    SG_final->Cells[C_ZT2][0] = "Topo dos Ecos [km]";
    SG_final->Cells[C_ZB][0] = "Zb [km]";
    SG_final->Cells[C_PRESSAT][0] = "Pressão Atm.";
    SG_final->Cells[C_ERROTETAE][0] = "Erro tetaE";
    SG_final->Cells[C_W0][0] = "w0";
    SG_final->Cells[C_WS][0] = "ws";
    SG_final->Cells[C_WT][0] = "wt";
    SG_final->Cells[C_PS][0] = "Ps [Pa]";
    SG_final->Cells[C_TS][0] = "Ts [K]";
    SG_final->Cells[C_TETA][0] = "Teta [K]";
    SG_final->Cells[C_V][0] = "v [m/s]";
    SG_final->Cells[C_PT][0] = "Pt [Pa]";
    SG_final->Cells[C_TT][0] = "Tt [K]";
    SG_final->Cells[C_TETAE][0] = "Teta E [K]";
    SG_final->Cells[C_OBGEO][0] = "Saída Ob Georgakakos";
    SG_final->Cells[C_OBRADAR][0] = "Saída Ob Radar";
    SG_final->Cells[C_OT][0] = "Saída Ot";
    SG_final->Cells[C_I][0] = "Umid. I";
    SG_final->Cells[C_NV][0] = "Adim. Nv";
    SG_final->Cells[C_ND][0] = "Adim. Nd";
    SG_final->Cells[C_VP][0] = "vp";
    SG_final->Cells[C_DC][0] = "Dc [mm]";
}

void TFPrincipal::Limpa_Dados()
{
    for(int i=1; i<=SG_final->RowCount; i++)
    {
        SG_final->Cells[C_ANO][i] = "";
        SG_final->Cells[C_DATA][i] = "";



        SG_final->Cells[C_TEMPO][i] = "";
        SG_final->Cells[C_TEMPERATURA][i] = ""; //coluna * linha
        SG_final->Cells[C_UMIDADE][i] = "";
        SG_final->Cells[C_TEMPORV][i] = "";
        SG_final->Cells[C_PRECAC][i] = "";
        SG_final->Cells[C_POBS][i] = "";
        SG_final->Cells[C_X][i] = "";
        SG_final->Cells[C_ZC][i] = "";
        SG_final->Cells[C_ZT][i] = "";
        SG_final->Cells[C_ZT2][i] = "";
        SG_final->Cells[C_ZB][i] = "";
        SG_final->Cells[C_PRESSAT][i] = "";
        SG_final->Cells[C_ERROTETAE][i] = "";
        SG_final->Cells[C_W0][i] = "";
        SG_final->Cells[C_WS][i] = "";
        SG_final->Cells[C_WT][i] = "";
        SG_final->Cells[C_PS][i] = "";
        SG_final->Cells[C_TS][i] = "";
        SG_final->Cells[C_TETA][i] = "";
        SG_final->Cells[C_V][i] = "";
        SG_final->Cells[C_PT][i] = "";
        SG_final->Cells[C_TT][i] = "";
        SG_final->Cells[C_TETAE][i] = "";
        SG_final->Cells[C_OBGEO][i] = "";
        SG_final->Cells[C_OBRADAR][i] = "";
        SG_final->Cells[C_OT][i] = "";
        SG_final->Cells[C_I][i] = "";
        SG_final->Cells[C_NV][i] = "";
        SG_final->Cells[C_ND][i] = "";
        SG_final->Cells[C_VP][i] = "";
        SG_final->Cells[C_DC][i] = "";
    }
}
//---------------------------------------------------------------------------*/
void TFPrincipal::insere_entrada(int ano,int data,int tempo,float temp_externa,float umidade_ar,float pressao_atm, int i)
{
    /* SG->Cells[0][i]= IntToStr(ano);
    SG->Cells[1][i]= IntToStr(data);
    SG->Cells[2][i]= IntToStr(tempo);
    SG->Cells[3][i] = FormatFloat("0.00",temp_externa); //Temperatura do ar no solo SG->Cells[4][i] = FormatFloat("0.00",umidade_ar); //Umidade do ar no solo SG->Cells[5][i] = FormatFloat("0.00",pressao_atm); //Pressão atmosférica */
}

void TFPrincipal::insere_calculos(float coluna6,float coluna7,float coluna8,float coluna9,float coluna10,float coluna11,float coluna12,float coluna13,float coluna14,float coluna15,float coluna16,float coluna17,float coluna18, int linha)
{
    /* SG->Cells[6][linha] = FormatFloat("0.00",coluna6);//calcula pressao de saturacao SG->Cells[7][linha] = FormatFloat("0.00",coluna7);//"Temp. Externa ºK"; SG->Cells[8][linha] = FormatFloat("0.00",coluna8);//"e úmido";
    SG->Cells[9][linha] = FormatFloat("0.00",coluna9);//"Temp. P. Orvalho"; SG->Cells[10][linha] = FormatFloat("0.00",coluna10);//"P. atm[mbar]"; SG->Cells[11][linha] = FormatFloat("0.",coluna11);//"patm";
    SG->Cells[12][linha] = FormatFloat("0.0000",coluna12);//"qv";
    SG->Cells[13][linha] = FormatFloat("0.0000",coluna13);//"P. atm[mbar]"; SG->Cells[14][linha] = FormatFloat("0.",coluna14);//"patm";
    SG->Cells[15][linha] = FormatFloat("0.00",coluna15);//"qv";
    SG->Cells[16][linha] = FormatFloat("0.00",coluna16);


    SG->Cells[17][linha] = FormatFloat("0.00",coluna17);
    SG->Cells[18][linha] = FormatFloat("0.00",coluna18); */
}


//------------------------------------------------------------------------------------------

void TFPrincipal::exibe_calculos(int ano,int data,int tempo,float temp_externa,float umidade_ar,float pressao_atm, int i)
{
    SG_final->Cells[C_ANO][i] = IntToStr(ano);
    SG_final->Cells[C_DATA][i] = IntToStr(data);
    SG_final->Cells[C_TEMPO][i] = IntToStr(tempo);
    SG_final->Cells[C_TEMPERATURA][i] = FormatFloat("0.000",To[i]); //coluna * linha SG_final->Cells[C_UMIDADE][i] = FormatFloat("0.000",umidade_ar);
    SG_final->Cells[C_TEMPORV][i] = FormatFloat("0.000",Td[i]); //coluna * linha SG_final->Cells[C_PRECAC][i] = FormatFloat("0.00000",P[i]);
    SG_final->Cells[C_POBS][i] = FormatFloat("0.00000",Pobs[i]);
    SG_final->Cells[C_PRESSAT][i] = FormatFloat("0.000",po[i]);
    SG_final->Cells[C_ERROTETAE][i] = FormatFloat("0.000",teta17[i]-tetaE1[i]);
    SG_final->Cells[C_W0][i] = FormatFloat("0.0000",wo[i]); //"Razão Ini."; SG_final->Cells[C_WS][i] = FormatFloat("0.0000",wss[i]); //"Razão Base"; SG_final->Cells[C_WT][i] = FormatFloat("0.0000",wst[i]); //"Razão Topo"; SG_final->Cells[C_PS][i] = FormatFloat("0",ps[i]); //"Ps"; SG_final->Cells[C_TS][i] = FormatFloat("0.00",Ts[i]); //"Ts"; SG_final->Cells[C_TETA][i] = FormatFloat("0.0000",teta[i]); //"T. Pot. E"; SG_final->Cells[C_V][i] = FormatFloat("0.00",v[i]);
    SG_final->Cells[C_PT][i] = FormatFloat("0.00",pt[i]);
    SG_final->Cells[C_TT][i] = FormatFloat("0.00",Tt[i]); //"Calor Lat."; SG_final->Cells[C_TETAE][i] = FormatFloat("0.00",tetaE1[i]); //"T. Pot.E1"; SG_final->Cells[C_OBGEO][i] = FormatFloat("0.000000",Ob1[i]); //"Saida Ob Georgakakos"; SG_final->Cells[C_OBRADAR][i] = FormatFloat("0.000000",Ob2[i]); //"Saida Ob Radar"; SG_final->Cells[C_OT][i] = FormatFloat("0.000000",Ot[i]); //"Saida Ot"; SG_final->Cells[C_I][i] = FormatFloat("0.000000",I[i]); //"Umid. I"; SG_final->Cells[C_NV][i] = FormatFloat("0.000",Nv[i]); //"Adim. Nv"; SG_final->Cells[C_ND][i] = FormatFloat("0.000",Nd[i]); //"Adim. Nd"; SG_final->Cells[C_X][i] = FormatFloat("0.000",X[i]);//;
    SG_final->Cells[C_DC][i] = FormatFloat("0.000",1000*Dc[i]);//"Dc"; SG_final->Cells[C_ZC][i] = FormatFloat("0.000",Zc[i]/1000);
    SG_final->Cells[C_ZT][i] = FormatFloat("0.000",Zt[i]/1000);
    SG_final->Cells[C_ZT2][i] = FormatFloat("0.000",Zt2[i]/1000);
    SG_final->Cells[C_ZB][i] = FormatFloat("0.000",Zb[i]/1000);
    SG_final->Cells[C_VP][i] = FormatFloat("0.000",vp[i]);

    FPrincipal->SG_final->RowCount++;
// FPrincipal->SG->RowCount++;
}

//------------------------------------------------------------------------------------------

void TFPrincipal::Calcula_Dados(int e_ano,int data,int tempo,float temp_externa, float umidade_ar, float pressao_atm, int linha,int total_linha)
{
    float temperatura_externa;
    float somatorio = 0.00;
    int linha_m;
    altura = StrToFloat(e_altura->Text);
    for(linha_m=1; linha_m<=total_intervalo; linha_m++) //total de linhas
// for(int linha_m=1;linha_m<2;linha_m++) //total de linhas
    {
        vet_resultados[0][linha_m] = e_ano; //coluna * linha



        vet_resultados[1][linha_m] = data;
        vet_resultados[2][linha_m] = tempo;
        vet_resultados[0][linha] = vet_resultados[0][linha_m];
        vet_resultados[1][linha] = vet_resultados[1][linha_m];
        vet_resultados[2][linha] = vet_resultados[2][linha_m];

        if (linha_m==1)
            vet_resultados[3][linha_m] = temp_externa;
        if (linha_m==2)
        {
            temperatura_externa =(temp_externa - StrToFloat(alfa->Text)*(vet_altura[linha_m-1]-0));
            vet_resultados[3][linha_m] = temperatura_externa;
            vet_resultados[3][linha] = temp_externa;
        }
        if (linha_m>2)
        {
            temperatura_externa =(temperatura_externa - StrToFloat(alfa->Text)*(vet_altura[linha_m-1] -
                                  vet_altura[linha_m-2]));
            vet_resultados[3][linha_m] = temperatura_externa;
            vet_resultados[3][linha] = temp_externa;
        }
        vet_resultados[4][linha_m] = umidade_ar;
        vet_resultados[5][linha_m] = pressao_atm;
        vet_resultados[6][linha_m] = (pressao_saturacao(temp_externa)); //pressao saturacao
        vet_resultados[7][linha_m] = vet_resultados[3][linha_m]+273.15;
        vet_resultados[4][linha] = vet_resultados[4][linha_m];
        vet_resultados[5][linha] = vet_resultados[5][linha_m];
        vet_resultados[6][linha] = vet_resultados[6][linha_m];
        vet_resultados[7][linha] = vet_resultados[7][linha_m];

        if (linha_m==1)
            vet_resultados[8][linha_m] = (vet_resultados[4][linha_m]/100)* vet_resultados[6][linha_m];
        else vet_resultados[8][linha_m] = vet_resultados[8][linha_m-1];
        vet_resultados[9][linha_m] = (15.957* ponto_orvalho(vet_resultados[8][linha_m]))-103.35;
        vet_resultados[10][linha_m] = ((vet_resultados[5][linha_m]*4)/3);
        vet_resultados[11][linha_m] = vet_resultados[10][linha_m] *100;
        vet_resultados[12][linha_m] = 0.662 * (vet_resultados[6][linha_m] / vet_resultados[11][linha_m]);
        vet_resultados[8][linha] = vet_resultados[8][linha_m];
        vet_resultados[9][linha] = vet_resultados[9][linha_m];
        vet_resultados[10][linha] = vet_resultados[10][linha_m];
        vet_resultados[11][linha] = vet_resultados[11][linha_m];
        vet_resultados[12][linha] = vet_resultados[12][linha_m];
        if (linha_m==1)
            vet_resultados[13][linha_m] = 0.00;
        else
        {
            vet_resultados[13][linha_m] = (vet_resultados[12][linha_m] + vet_resultados[12][linha_m-1])/linha_m;
            vet_resultados[13][linha] = vet_resultados[13][linha_m];
        }
        if (linha_m==1)
            vet_resultados[14][linha_m] = 287*(1+(0.6088*vet_resultados[12][linha_m]));
        else vet_resultados[14][linha_m] = vet_resultados[14][linha_m-1];
        vet_resultados[15][linha_m] =
            vet_resultados[11][linha_m]/(vet_resultados[14][linha_m]*vet_resultados[7][linha_m]);
        if (linha_m==1)
            vet_resultados[16][linha_m] = 0.00;
        else
        {
            vet_resultados[16][linha_m] = (vet_resultados[15][linha_m] + vet_resultados[15][linha_m-1])/2;
            vet_resultados[16][linha] = vet_resultados[16][linha_m];



        }
        vet_resultados[14][linha] = vet_resultados[14][linha_m];
        vet_resultados[15][linha] = vet_resultados[15][linha_m];
        vet_resultados[16][linha] = vet_resultados[16][linha_m];
        if (linha_m==1)
            vet_resultados[17][linha_m] = 0.00;
        else
        {
            vet_resultados[17][linha_m] = vet_resultados[13][linha_m]* vet_resultados[16][linha_m]*
                                          StrToFloat(area->Text)* (vet_altura[linha_m-2]-0)*pow(10,3);
            vet_resultados[17][linha] = vet_resultados[17][linha_m];
        }
        if (linha_m==1)
            vet_resultados[18][linha_m] = 0.00;
        else
        {
            vet_resultados[18][linha_m] = vet_resultados[17][linha_m] / (StrToFloat(densidade->Text)*StrToFloat(area->Text));
            vet_resultados[18][linha] = (0.00,vet_resultados[18][linha_m]);
            somatorio = somatorio + (vet_resultados[18][linha_m]);
        }
    }
    vet_resultados[19][linha]=(somatorio*1000)-20;
//exibe_calculos(linha);
}




void TFPrincipal::carrega_vetor_altura(float altura,int total_intervalo)
{
    FPrincipal->vet_altura[0] = altura;
    for(int j=1; j<total_intervalo-1; j++)
        FPrincipal->vet_altura[j] = (FPrincipal->vet_altura[j-1] + altura);
}

void TFPrincipal::limpa_vetor_altura(int altura, int total_intervalo)
{
    for(int j=0; j<total_intervalo; j++)
        FPrincipal->vet_altura[j] = 0;

}

//---------------------------------------------------------------------------
//funcao
//---------------------------------------------------------------------------
//funcao
float TFPrincipal::f(float T, float p, int i)
{
    float pt,p1,p2,p3;

    p1= pow((PN/p),0.286);
    p2= 0.622 * A1*(pow((T-223.15),3.5))/p;
    p3= (A - B*(T-273.15))*p2/(CP*T);

// tetaE1[i]=teta[i]*exp(L[i]*wss[i]/(CP*Ts[i]));
    float res=T*p1*exp(p3);
    return res;
}
//---------------------------------------------------------------------------
//derivada da funcao - encontra valor de T
/*dados de entrada
pt = valor de entrada - chute
Tt = raiz
*/
float TFPrincipal::fl(float T, float p, int i)
{
    float a1,a2,a3,b,c,d, res;

    a1 = 0.622*((A-B*(T-273.15))* (A1*(pow((T-223.15),3.5)))) / (CP*p*T);
    a2 = exp(a1);
    a3 = pow((PN/p),0.286)*a2;
    b = - 0.622*(B*A1*(pow((T-223.15),3.5))) / (CP*p*T);

    c = (2.1770*(A-B*(T-273.15))*A1*pow((T-223.15),2.5)) / (CP*p*T);

    d = (0.622*(A-B*(T-273.15))*A1*pow((T-223.15),3.5)) / (CP*p*pow(T,2));

    res = a3 + (T* pow((PN/p),0.286)) * (b+c-d)*a3;

    return res;
}

//---------------------------------------------------------
float TFPrincipal::NewtonRaphson(float T, float p, int atual)
{
    float x, x0, Epsilon, E, p1,p2, erro[2];
    int i=1;
    x0=T;
    Epsilon = 0.001 * tetaE1[atual];
    E=tetaE1[atual];
    do
    {
        p1=f(x0,p,atual);
        E = fabs(p1-tetaE1[atual]);
        erro[0]=E;
        x0=x0-0.1;
        p1=f(x0,p,atual); //funçao
// p2=fl(x0,p,atual); //derivada
        erro[1]=fabs(p1-tetaE1[atual]);
        i++;
// x0=x0-0.1;
// E=fabs(p1-tetaE1[atual]);
// erro[1]=E;
    }while ((E>Epsilon) && (erro[0]>erro[1]));
    return (x0);

}

//------------------------------------------------------------------------------

float TFPrincipal::calculaMedia(int numIntervalos, float *elementos)
{
    float somatorio = 0.0;
    for (int i = 1; i <= numIntervalos; i++)
        somatorio += elementos[i];

    return(somatorio/(float)numIntervalos);
}

//------------------------------------------------------------------------------

float TFPrincipal::calculaVariancia(int numIntervalos, float media, float *elementos)
{
    float variancia = 0.0;

    for(int i = 1; i <= numIntervalos; i++)
        variancia = variancia + pow(elementos[i] - media, 2);

    return(variancia / ((float)numIntervalos - 1.0));
}

//-----------------------------------------------------------------------------

float TFPrincipal::calculaCovariancia(int numIntervalos, float media, float *elementos)
{
    float media1 = 0.0,
          media2 = 0.0,
          somatorio = 0.0,
          covariancia = 0.0;

    int meLag = StrToInt(FPrincipal->me_lag->Text);

    for(int i = 1; i <= numIntervalos; i++)
        somatorio += elementos[i];

    for(int i = 1; i <= meLag; i++)
    {
        media1 = somatorio - elementos[i];
        media2 = somatorio - elementos[numIntervalos-i+1];
    }
    media1 = media1 / (numIntervalos - meLag);
    media2 = media2 / (numIntervalos - meLag);

    for(int i = 1; i <= (numIntervalos - meLag); i++)
        covariancia = covariancia + (elementos[i + meLag] - media1) * (elementos[i] - media2);

    covariancia = covariancia / (numIntervalos - meLag);

    return (covariancia);
}

//---------------------------------------------------------

float TFPrincipal::calculaAutoCorrelacao(float covariancia, float variancia)
{
    return (covariancia / variancia);
}

//---------------------------------------------------------

void TFPrincipal::Regressao(int e_ano,int data,int tempo,float temp_externa, float umidade_ar, float pressao_atm, int linha,int total_linha)
{

    float mediaT=0,mediaP=0,mediaU=0, mediaVIL = 0.0, mediaTopoDosEcos = 0.0,
          varT=0,varP=0,varU=0, varVIL = 0.0, varTopoDosEcos = 0.0,
          autocorT=0, autocorP=0, autocorU=0, autocorVIL = 0.0, autocorTopoDosEcos = 0.0,
          covarT=0, covarP=0, covarU=0, covarVIL = 0.0, covarTopoDosEcos = 0.0;

    int i;

    mediaT = calculaMedia(FPrincipal->linha, FPrincipal->ToC);
    mediaP = calculaMedia(FPrincipal->linha, FPrincipal->pommHg);
    mediaU = calculaMedia(FPrincipal->linha, FPrincipal->ur);

    varT = calculaVariancia(FPrincipal->linha, mediaT, FPrincipal->ToC);
    varP = calculaVariancia(FPrincipal->linha, mediaP, FPrincipal->pommHg);
    varU = calculaVariancia(FPrincipal->linha, mediaU, FPrincipal->ur);

    covarT = calculaCovariancia(FPrincipal->linha, mediaT, FPrincipal->ToC);
    covarP = calculaCovariancia(FPrincipal->linha, mediaP, FPrincipal->pommHg);
    covarU = calculaCovariancia(FPrincipal->linha, mediaU, FPrincipal->ur);

    autocorT = calculaAutoCorrelacao(covarT, varT);
    autocorP = calculaAutoCorrelacao(covarP, varP);
    autocorU = calculaAutoCorrelacao(covarU, varU);

    if ((FPrincipal->VIL[FPrincipal->linha] != -1) || (FPrincipal->TopoDosEcos[FPrincipal->linha] != -1))
    {
        mediaVIL = calculaMedia(FPrincipal->linha, FPrincipal->VIL);
        varVIL = calculaVariancia(FPrincipal->linha, mediaVIL, FPrincipal->VIL);
        covarVIL = calculaCovariancia(FPrincipal->linha, mediaVIL, FPrincipal->VIL);
        autocorVIL = calculaAutoCorrelacao(covarVIL, varVIL);

        mediaTopoDosEcos = calculaMedia(FPrincipal->linha, FPrincipal->TopoDosEcos);
        varTopoDosEcos = calculaVariancia(FPrincipal->linha, mediaTopoDosEcos, FPrincipal->TopoDosEcos);
        covarTopoDosEcos = calculaCovariancia(FPrincipal->linha, mediaTopoDosEcos, FPrincipal->TopoDosEcos);
        autocorTopoDosEcos = calculaAutoCorrelacao(covarTopoDosEcos, varTopoDosEcos);
    }


    int limite = linha+StrToInt(me_intervalo_previsao->Text);
    int inicio = linha+1;

    for (i=inicio; i<=limite; i++)
    {
        Randomize();
        ToC[i] = mediaT + autocorT * (ToC[i-1]-mediaT) + RandG(0,1) * sqrt(varT) * sqrt(1-pow(autocorT,2));
        pommHg[i] = mediaP + autocorP * (pommHg[i-1]-mediaP) + RandG(0,1) * sqrt(varP) * sqrt(1-pow(autocorP,2));
        ur[i] = mediaU + (autocorU * (ur[i-1]-mediaU)) + (RandG(0,1) * sqrt(varU) * sqrt(1-pow(autocorU,2)));
        if ((FPrincipal->VIL[FPrincipal->linha] != -1) || (FPrincipal->TopoDosEcos[FPrincipal->linha] != -1))
        {
            VIL[i] = mediaVIL + (autocorVIL * (VIL[i-1]-mediaVIL)) + (RandG(0,1) * sqrt(varVIL) * sqrt(1-pow(autocorVIL,2)));
            TopoDosEcos[i] = mediaTopoDosEcos + (autocorTopoDosEcos * (TopoDosEcos[i-1]-
                                                 mediaTopoDosEcos)) + (RandG(0,1) * sqrt(varTopoDosEcos) * sqrt(1-pow(autocorTopoDosEcos,2)));
        }


        if (ur[i] > 100.0)
            ur[i] = 100.0;
        FPrincipal->linha++;
//FPrincipal->Calcula_Georgakakos(e_ano,data,tempo,ToC[i], ur[i], pommHg[i], FPrincipal->linha, FPrincipal->total_linha);

        FPrincipal->Calcula_Georgakakos(e_ano,data,FPrincipal->linha,ToC[i], ur[i], pommHg[i], FPrincipal->linha, FPrincipal->total_linha);
    }
}

//------------------------------------------------------------------------------

void TFPrincipal::converte_dados(int data,int tempo, float temp_externa,float umidade_ar, float pressao_atm, int linha) //n por linha
{
    int i;
    float aux, aux_tm, aux_l, aux_w, tolerancia, tolerancia_v, v2,v1,res, pressao;

    i=linha; //n eh a linha atual
    dia[i]=data;
    hora[i]=tempo;
    ToC[i]=temp_externa; //temp ºC
    ur[i]=umidade_ar; //umid. relativa
    pommHg[i]=pressao_atm; //pressao mmHg
//Dados de entrada do modelo

    To[i]=(ToC[i]+273.15); //Temperatura em ºK
    po[i]=PTRANSF*pommHg[i]; //Pressao em Pa
//Pressao de Saturacao Umida [es umido] eq 4
    es[i]=(ur[i]/100)*(A1*(pow((To[i]-223.15),3.5)));
    Td[i]=(pow((es[i]/A1),(1/3.5)))+223.15; //Temp. Ponto de orvalho ºK

// Avaliar o grau de saturação do ar na superfície. Se w0 < wss o ar não
// está saturado e a ascensão é adiabática, caso contrário, desde a
// superfície a taxa de decaimento da temperatura é pseudo adiabática.

//Razao de mistura wo = ws(Td,po) eq 5
    wo[i]=0.622*(A1*(pow((Td[i]-223.15),3.5)))/po[i];

//Razao de mistura ws(To,po) eq 6
    wss[i]=0.622*(A1*(pow((To[i]-223.15),3.5)))/po[i];

//Teste
    if(wo[i]>=wss[i])
        teste[i]=1; // ascenção pseudo-adiabática
    else teste[i]=0; // ascenção adiabática ok!! ;)


//Pressao e temperatura na base eq 12 e 13
    ps[i]=po[i]/(pow((((To[i]-Td[i])/223.15)+1),3.5));
    Ts[i]=To[i]/(((To[i]-Td[i])/223.15)+1);

//Teta eq 7
    teta[i]=(pow((PN/ps[i]),0.286))*Ts[i];

//Calor latente de condensação eq 15
    L[i]=A-B*(Ts[i]-273.15);

//tetae equação 14 p1588
    tetaE1[i] = teta[i] * exp(L[i]*(0.622*(A1*(pow((Ts[i]-223.15),3.5)))/ps[i])/(CP*Ts[i]));

// Parametrizacao para encontrar v e pt.



    v1 = StrToFloat(v_inicial->Text);

    do
    {
// subst v1 na eq 18, encontra pt
        pt1[i] = ((StrToFloat(e2->Text)-StrToFloat(pl->Text) )/(1+StrToFloat(e3->Text)*v1))+StrToFloat(pl->Text);
// pt1[i]=1.1*ps[i];
// subst pt1 na eq 15 encontra p'
        plinha[i] = ps[i] - ((0.25)*(ps[i]-pt1[i]));
// encontra Tm eq.17 subst p' e 1a aprox Tm = Ts
        Tm[i]=NewtonRaphson(Ts[i],plinha[i],i);
// subst valor de pt na eq 16 e encontra tslinha
        aux = ((0.75)*ps[i])+((0.25)*pt1[i]);
        tslinha[i] = (To[i]/pow(po[i],0.286))* pow(aux,0.286); //encontra tslinha
// subst Tm e Ts' na eq 14 e encontra deltat
        DELTAT[i]= Tm[i] - tslinha[i];
// vai pra 13 e encontra v2
        v1 = StrToFloat(E1->Text)*pow((CP*DELTAT[i]),0.5);//eq 13
//calcula p novamente
        pt[i] = ((StrToFloat(e2->Text)-StrToFloat(pl->Text) )/(1+StrToFloat(e3->Text)*v1))+StrToFloat(pl->Text);
// compara os dois valores de p
        res = pt1[i]-pt[i];
        tolerancia_v = pt1[i]*2/100;
        pt1[i]=pt[i];

// compara os dois valores de v
// v2 = ((StrToFloat(e2->Text)-StrToFloat(pl->Text) )/(1+StrToFloat(e3->Text)*v1))+StrToFloat(pl->Text);
// res = v1-v2;
// tolerancia_v = v1*2/100;
// v1=v2;
    }while (fabs(res) > tolerancia_v);

    v[i] = v1;
    pt[i]=pt1[i];

//c eq 19 estudo de caso
    c[i]=1/(StrToFloat(E4->Text)*pow(v[i],K));

//ECONTRA TT: apaguei.
//tetae eq. 16
//erro*********

    Tt[i]=NewtonRaphson(Ts[i],pt[i],i);
    teta17[i]=f(Tt[i],pt[i],i); //funçao

    if (TopoDosEcos[i] == -1) //Se o topo dos Ecos nao foi informado
    {
        Zc[i]=(R*(Ts[i]+Tt[i])/(2*G))*log(po[i]/pt[i]);
        Zb[i]=(R*(Ts[i]+To[i])/(2*G))*log(po[i]/ps[i]);
        Zt[i] = Zc[i] + Zb[i];
        Zt2[i] = 0.0;
    }
    else
    {
        Zt2[i] = TopoDosEcos[i] * 1000;

        Zt[i] = (R*(Tt[i]+To[i])/(2*G))*log(po[i]/pt[i]);
        Zb[i]=(R*(Ts[i]+To[i])/(2*G))*log(po[i]/ps[i]);


        if (Zt2[i] < Zb[i])
            Zc[i] = 0;
        else
            Zc[i] = Zt2[i] - Zb[i];
    }

//Razao de mistura ws(Tt,pt)
    wst[i]=0.622*(A1*(pow((Tt[i]-223.15),3.5)))/pt[i];

//Umidade I eq 2
//I[i]=(wo[i]-wst[i])*ROM*v[i]*DA;
    I[i]=(wo[i]-wst[i])*((ps[i]/(R*Ts[i])+(pt[i]/(R*Tt[i])))/2)*v[i]*DA;

//D* eq 42
    Dast[i]= 2.11*pow(10,-5)*pow((To[i]/Tast),1.94)*(past/po[i]);


//Diametro crítico Dc eq 56
    Dc[i]=pow((4*(Dast[i]/(C1*RV))*Zb[i]*((A1*pow((To[i]-223.15),3.5)/To[i])-(A1*pow((Td[i]-
                                    223.15),3.5)/To[i]))),(0.3333));

//Adimensionais eq 58-60
    Nv[i]= v[i]*c[i]/ALFA;
    Nd[i]=c[i]*Dc[i];
    vp[i]=4*ALFA/c[i];

//DELTA eq.32
    DELTA=(0.333)*((1/GAMA)+(1/(GAMA*GAMA))+(1/(GAMA*GAMA*GAMA)));

    X[0]= StrToFloat(xinicial->Text);

//Ob e Ot em funcao da massa, ou seja, nao esta multiplicado por X[i]
    float F1, F2,F3;

    F1 = (1+(3/4)*Nv[i]+pow(Nv[i],2)*0.25);
    F2 = pow(Nv[i],3)*0.0416667;
    F3 = (F1+F2)/exp(Nv[i]);
    if (Zc[i] == 0)
        Ob1[i] = 0;
    else
        Ob1[i]= ((vp[i]*X[i-1])/(DELTA*Zc[i]))*F3;
    F1 = (1+((3/4)*(GAMA*Nv[i])) + pow((GAMA*Nv[i]),2)*0.25 + (pow(GAMA*Nv[i],3)*0.0416667)) / exp(GAMA*Nv[i]);
    F2 = (GAMA*Nv[i]*0.25);
    if (Zc[i] == 0)
        Ot[i] = 0;
    else
        Ot[i]=((vp[i]*X[i-1])/(DELTA*Zc[i]*pow(GAMA,5)))*( F1 + F2 - 1);

//calcula valor de x no intervalo seguinte
    X[0]= StrToFloat(xinicial->Text);
//X[i+1]=(I[i]-(Ob[i]+Ot[i])*X[i])*StrToFloat(dt->Text);
// Se nao houver dados de radar, X[i] usa os dados da estaçao
    if (VIL[i] == -1)
        X[i]=(I[i]-(Ob1[i]+Ot[i]))*StrToFloat(dt->Text) + X[i-1];
// Caso contrario, usa os dados de VIL
    else
    {
        X[i] = VIL[i];
        if (X[i-1] == 0)
        {
            Ob2[i] = 0.0;
        }
        else
        {
            Ob2[i] = I[i] - Ot[i] - ((X[i] - X[i-1])/StrToFloat(dt->Text));
        }
    }

//Precipitacao 63 - 64 resultado em unidade de massa

    if (Zc[i] == 0 || ((VIL[i] < 3.0) && (chkBoxVIL->Checked)))
        P[i] = 0;
    else
    {
        if(Nd[i]>=Nv[i])
        {
//double pa = (X[i+1]/(DELTA*Zc[i]))*vp[i];
            double pa = (X[i]/(DELTA*Zc[i]))*vp[i];
            double pb = (1-Nv[i]*0.25) * (1 + Nd[i] + (pow(Nd[i],2)*0.5));
            double pc = (pow(Nd[i],3)*0.125);
            double pd = exp(Nd[i]);
            double pe = (pb+pc)/pd;
            P[i] = pa*pe;
        }
        else
        {
            P[i]=(X[i]/(DELTA*Zc[i]))*vp[i]*(1+0.75*Nv[i]+pow(Nv[i],2)*0.25+pow(Nv[i],3)*0.0416667-pow(Nd[i],3)*0.0416667)/exp(Nv[i]);
        }
    }

    P[i] = P[i]*StrToFloat(dt->Text); //valor acumulado em 7,5 min
}

//---------------------------------------------------------

void __fastcall TFPrincipal::B_CalcularClick(TObject *Sender)
{
    float temp_externa, umidade_ar, pressao_atm;
    int ano, data,tempo;

    Limpa_Dados();
    linha++;
    total_linha=total_linha+1;
    total_intervalo = 500;//;StrToFloat(total_intervalos->Text)+1;
    altura = StrToInt(e_altura->Text);
    alocacao_dinamica_vetores(total_intervalo);
    converte_dados( data, tempo, temp_externa, umidade_ar, pressao_atm, total_intervalo);
    exibe_calculos( ano, data, tempo, temp_externa, umidade_ar, pressao_atm, linha);
    libera_memoria();
    /*
    //insere_entrada(ano,data,tempo,temp_externa,umidade_ar,pressao_atm,linha);
    Calcula_Dados(ano,data,tempo,temp_externa, umidade_ar, pressao_atm, linha, total_linha);
    //Limpa_Dados(linha, total_linha);
    limpa_vetor_altura(altura,total_intervalo); */
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void __fastcall TFPrincipal::btnSairClick(TObject *Sender)
{
    Application->Terminate();
}

//---------------------------------------------------------------------------

void TFPrincipal::calcula_vetor_erro()
{
    float valor = 0;
    for (int i=1; i<FPrincipal->tam_vet_erro-1; i++)
    {
        valor = StrToFloat(FPrincipal->SG_final->Cells[6][i]);
        FPrincipal->erro[i] = valor - FPrincipal->obs[i+1];
//ShowMessage(FloatToStr(FPrincipal->erro[i]));
    }
}
//---------------------------------------------------------------------------

void TFPrincipal::calcula_vetor_S1()
{
    int total = FPrincipal->tam_vet_erro-1;
    for (int i=1; i<FPrincipal->tam_vet_erro-1; i++)
        FPrincipal->S1 = FPrincipal->S1 + (FPrincipal->erro[i] * FPrincipal->erro[i]);
    FPrincipal->S1 = sqrt (FPrincipal->S1 / total);
    FPrincipal->d1->Text = (FPrincipal->S1);
// ShowMessage( "S1 --> " + FloatToStr(FPrincipal->S1));
}


//---------------------------------------------------------------------------
void TFPrincipal::calcula_vetor_S2()
{
    int choveu = 0;
    float valor = 0;
    for (int i=1; i<FPrincipal->tam_vet_erro-1; i++)
    {
        valor = StrToFloat(FPrincipal->SG_final->Cells[6][i]);
// if (FPrincipal->P[i]>= 0 )
        if ((valor != 0 ) || (FPrincipal->obs[i]!= 0))
        {
            FPrincipal->S2 = FPrincipal->S2 + (FPrincipal->erro[i-1] * FPrincipal->erro[i-1]);
            choveu++;
        }
    }
    FPrincipal->S2 = sqrt (FPrincipal->S2 / choveu);
    FPrincipal->d2->Text =(FPrincipal->S2);
// ShowMessage( "S2 --> " + FloatToStr(FPrincipal->S2));
}


//---------------------------------------------------------------------------

void TFPrincipal::Calcula_Georgakakos(int e_ano,int data,int tempo,float temp_externa, float umidade_ar, float pressao_atm, int linha,int total_linha)
{
    total_linha=total_linha+1;
    total_intervalo = 1;//;StrToFloat(total_intervalos->Text)+1;
    altura = StrToInt(e_altura->Text);
//alocacao_dinamica_vetores(linha);
    converte_dados( data, tempo, temp_externa, umidade_ar, pressao_atm, linha);

    exibe_calculos( e_ano, data, tempo, temp_externa, umidade_ar, pressao_atm, linha);

//libera_memoria();
}

//------------------------------------------------------------------------------

float TFPrincipal::CalculaDeltaVol(int numIntervalos, double *Pmod, double *Pobs)
{
    double somatorioPmod = 0.0,
           somatorioPobs = 0.0;

    for(int i = 1; i <= numIntervalos; i++)
    {
        somatorioPmod += Pmod[i];
        somatorioPobs += Pobs[i];
    }

    return (somatorioPmod / somatorioPobs);

}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------

void recupera_dados(char msg[],int &e_ano,int &data,int &tempo_f,float &temp,float &um, float &pressao_f, double &p_obs, int &linha_f, int &total_linha_f)
{
    int i=3, pos_atual;
    AnsiString ano="",dia="",tempo="";
    bool achou=true;
    AnsiString temperatura="",umidade="",pressao="", pobs = "";
    int p_ano ,p_dia,p_tempo;
    float p_temperatura,p_umidade,p_pressao;

    pos_atual=0;

//Pulando o campo 'numero da estacao'
    achou=true;
    for(i=pos_atual; i<pos_atual+10; i++)
        if( (msg[i] != ',') && (msg[i] != '-') && (achou) )
        {
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }

//Obtendo o campo 'ANO'
    achou=true;
    for(i = pos_atual+1; i<pos_atual+10; i++)
        if((msg[i] != ',') && (achou))
        {
            ano = ano + StrToInt(msg[i]);
            if (msg[i+1] == ',')
            {
                achou=false;
                pos_atual=i;
            }
        }

//Obtendo o campo 'DIA'
    achou=true;
    for(i=pos_atual+1; i<pos_atual+10; i++)
        if((msg[i]!=',') &&(achou))
        {
            dia = dia + StrToInt(msg[i]);
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }

//Obtendo o campo 'HORA'
    achou=true;
    for(i=pos_atual+1; i<pos_atual+10; i++)
        if((msg[i]!=',') &&(achou))
        {
            tempo = tempo + StrToInt(msg[i]);
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }

//Obtendo o campo 'TEMPERATURA'
    achou=true;
    for(i=pos_atual+1; i<pos_atual+10; i++)
        if((msg[i]!=',') &&(achou))
        {
            if (msg[i]=='.') msg[i]='.';
//if (msg[i]=='.') msg[i]=',';
            temperatura = temperatura + (msg[i]);
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }

//Obtendo o campo 'UMIDADE'
    achou=true;
    for(i=pos_atual+1; i<pos_atual+10; i++)
        if((msg[i]!=',') &&(achou))
        {
            if (msg[i]=='.') msg[i]='.';
//if (msg[i]=='.') msg[i]=',';
            umidade = umidade + (msg[i]);
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }




//Pulando o campo 'RADIOACAO'
    achou=true;
    for(i=pos_atual+1; i<pos_atual+10; i++)
        if((msg[i]!=',')&&(msg[i]!='-') &&(achou))
        {
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }

//Pulando o campo 'RADIOACAO REF'
    achou=true;
    for(i=pos_atual+1; i<pos_atual+10; i++)
        if((msg[i]!=',') &&(msg[i]!='-')&&(achou))
        {
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }

//Pulando o campo 'RADIOACAO LIQUIDA'
    achou=true;
    for(i=pos_atual+1; i<pos_atual+10; i++)
        if((msg[i]!=',') &&(msg[i]!='-')&&(achou))
        {
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }

//Obtendo o campo 'PRESSAO'
    achou=true;
    for(i=pos_atual+1; i<pos_atual+10; i++)
        if((msg[i]!=',')&&(msg[i]!='-') &&(achou))
        {
            if (msg[i]=='.') msg[i]='.';
// if (msg[i]=='.') msg[i]=',';
            pressao = pressao + (msg[i]);
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }

//Obtendo o campo 'Precipitação Observada'
    achou=true;
    for(i=pos_atual+1; i<pos_atual+10; i++)
        if((msg[i]!=',')&&(msg[i]!='-') &&(achou))
        {
            if (msg[i]=='.') msg[i]='.';
// if (msg[i]=='.') msg[i]=',';
            pobs = pobs + (msg[i]);
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }

    p_obs = StrToFloat(pobs);
    p_ano=StrToInt(ano);
    e_ano = p_ano;
    p_dia=StrToInt(dia);
    data = p_dia;
    p_tempo=StrToInt(tempo);
    p_temperatura=StrToFloat(temperatura);
    p_umidade=StrToFloat(umidade);
    tempo_f = p_tempo;
    temp = p_temperatura;
    um = p_umidade;
    if (p_umidade >= 100.0)
        p_umidade = 99.9;
    p_pressao=StrToFloat(pressao);
    FPrincipal->total_linha=FPrincipal->total_linha+1;
    pressao_f = p_pressao;
    total_linha_f = FPrincipal->total_linha+1;

    /*
    //procedimento para realizar os calculos
    if (p_tempo != FPrincipal->hora[FPrincipal->linha])
    { FPrincipal->linha++;
    linha_f = FPrincipal->linha;
    FPrincipal->Calcula_Georgakakos(p_ano,p_dia,p_tempo,p_temperatura, p_umidade, p_pressao, FPrincipal-
    >linha, FPrincipal->total_linha);
    }
    // else ShowMessage("erro");
    */
}

//---------------------------------------------------------------------------
void recupera_dados_radar(char msg[],int &e_ano,int &data,int &tempo_f, float &p_vil, float &p_topoDosEcos)
{
    int i=3, pos_atual;
    AnsiString ano="",dia="",tempo="";
    bool achou=true;
    AnsiString VIL = "", TopoDosEcos = "";
    int p_ano ,p_dia,p_tempo;

    pos_atual=0;

//Pulando o campo 'numero da estacao'
    achou=true;
    for(i=pos_atual; i<pos_atual+10; i++)
        if( (msg[i] != ',') && (msg[i] != '-') && (achou) )
        {
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }




//Obtendo o campo 'ANO'
    achou=true;
    for(i = pos_atual+1; i<pos_atual+10; i++)
        if((msg[i] != ',') && (achou))
        {
            ano = ano + StrToInt(msg[i]);
            if (msg[i+1] == ',')
            {
                achou=false;
                pos_atual=i;
            }
        }

//Obtendo o campo 'DIA'
    achou=true;
    for(i=pos_atual+1; i<pos_atual+10; i++)
        if((msg[i]!=',') &&(achou))
        {
            dia = dia + StrToInt(msg[i]);
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }

//Obtendo o campo 'HORA'
    achou=true;
    for(i=pos_atual+1; i<pos_atual+10; i++)
        if((msg[i]!=',') &&(achou))
        {
            tempo = tempo + StrToInt(msg[i]);
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }

//Obtendo o campo 'VIL'
    achou=true;
    for(i=pos_atual+1; i<pos_atual+10; i++)
        if((msg[i]!=',') &&(achou))
        {
            if (msg[i]=='.') msg[i]='.';
//if (msg[i]=='.') msg[i]=',';
            VIL = VIL + (msg[i]);
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }

//Obtendo o campo 'TOPO DOS ECOS'
    achou=true;
    for(i=pos_atual+1; i<pos_atual+10; i++)
        if((msg[i]!=',') &&(achou))
        {
            if (msg[i]=='.') msg[i]='.';


//if (msg[i]=='.') msg[i]=',';
            TopoDosEcos = TopoDosEcos + (msg[i]);
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }

// Removendo caracteres invalidos no final da linha
    for (i = TopoDosEcos.Length() -1; i > 0; i--)
        if ( (TopoDosEcos[i] >= '0') && (TopoDosEcos[i] <= '9'))
        {
            TopoDosEcos[i+1] = ' ';
            break;
        }

    p_ano=StrToInt(ano);
    e_ano = p_ano;
    p_dia=StrToInt(dia);
    data = p_dia;
    p_tempo=StrToInt(tempo);
    p_vil = StrToFloat(VIL);
    p_topoDosEcos = StrToFloat(TopoDosEcos);

    tempo_f = p_tempo;
}






//---------------------------------------------------------------------------
void limpa_vetores()
{

    for(int i=0; i<19; i++)
        for(int j=0; j<500; j++)
            FPrincipal->vet_resultados[i][j]= 0.00;


}

//---------------------------------------------------------------------------
void salva()
{
// FPrincipal->SG->Cells[1][1].c_str();
    std::FILE *outfile;
    char msg;

    if (FPrincipal->SV_Dados->Execute())
    {
        outfile = std::fopen(FPrincipal->SV_Dados->FileName.c_str(), "wt");
        if (outfile)
        {
            for (int i=1; i < FPrincipal->SG_final->RowCount; i++)
            {
                for (int j=0; j < FPrincipal->SG_final->ColCount; j++)
                    if (FPrincipal->SG_final->Cells[j][i]!= "NULL")
                        fprintf(outfile,"%s ",FPrincipal->SG_final->Cells[j][i].c_str());
                fprintf(outfile,"");
            }
        }
        fclose(outfile);
    }

}


//---------------------------------------------------------------------------
void soma_observado(char msg[], int pos_obs)
{
    int i=3,pos_atual;
    AnsiString lixo="";
    bool achou=true;
    double observado;
    AnsiString valor_enc="";

    pos_atual=0;
    int cont = 0; //Indica a quantidade de termos ja lidos do arquivo de entrada

    achou=true;
    i=0;
    while ((msg[i] != ' ' ) && (achou))
    {
// if((msg[i]!=',')&&(msg[i]!='-') &&(achou))
// {
        if (msg[i+1]==',')
        {
            cont++;
            i++;

            if (cont == 4)
            {
                while ((msg[i+1] != ',') && (msg[i+1] != ' ')&& (msg[i+1] != ' ') && (msg[i+1] != ' ' ))
                {
                    valor_enc = valor_enc + msg[i+1];
                    achou = false;
                    i++;
                }
            }
        }
        else
            i++;
    }
    FPrincipal->obs[pos_obs] = FPrincipal->obs[pos_obs] + StrToFloat(valor_enc);
}
//---------------------------------------------------------------------------
void lista()
{

    FILE *stream;
    long curpos, length;
    int pos_obs = 0, linha = 0;
    char msg[100];
    int i=0,e_ano=0,data=0,tempo=0;
    float temp=0, um=0, pressao=0, total_linha = 0.0;
    double pobs = 0.0;

    /* open a file for update */


    if(FPrincipal->OD_Dados->Execute())
    {
//nome = Form1->OD_Dados->FileName;//"F:\Olivete\DC\ArquivosEntrada\entradaFADISC.txt"; stream = fopen(FPrincipal->OD_Dados->FileName.c_str(), "rb+");
        if (stream == NULL)
            ShowMessage("Erro ao Abrir o Arquivo!!");
        else
        {
            FPrincipal->lblNomeArquivo->Caption = "Arquivo: " + FPrincipal->OD_Dados->FileName;
            while (!(feof(stream)))
            {
                curpos = ftell(stream);
                fseek(stream, 0L, SEEK_END);
                length = ftell(stream);
                fseek(stream, curpos, SEEK_SET);
                /* read a string from the file*/
                fgets(msg, length, stream);

                if (strlen(msg)>40)
                {
                    recupera_dados(msg,e_ano,data,tempo,temp, um, pressao, pobs, linha, total_linha);
//procedimento para realizar os calculos
                    if (tempo != FPrincipal->hora[FPrincipal->linha])
                    {
                        FPrincipal->linha++;
                        linha = FPrincipal->linha;
                        FPrincipal->Pobs[linha] = pobs;
                        FPrincipal->TopoDosEcos[linha] = -1;
                        FPrincipal->VIL[linha] = -1;
                        FPrincipal->Calcula_Georgakakos(e_ano,data,tempo,temp, um, pressao, linha, total_linha);
                    }
// else ShowMessage("erro");

                    FPrincipal->tam_vet_erro++;
                    pos_obs++;
                }
            }
            linha = FPrincipal->linha;

            FPrincipal->DeltaVol = FPrincipal->CalculaDeltaVol(linha, FPrincipal->P, FPrincipal->Pobs);
            FPrincipal->Correl = FPrincipal->CalculaCorrel(linha, FPrincipal->P, FPrincipal->Pobs);
            FPrincipal->NashSutcliffe = FPrincipal->calculaNashSutcliffe(linha, FPrincipal->P, FPrincipal->Pobs);
            FPrincipal->edtDeltaVol->Text = FormatFloat("0.0000", FPrincipal->DeltaVol);
            FPrincipal->edtCorrel->Text = FormatFloat("0.0000", FPrincipal->Correl);
            FPrincipal->edtNash->Text = FormatFloat("0.0000", FPrincipal->NashSutcliffe);
            FPrincipal->Regressao(e_ano,data,tempo,temp, um, pressao, FPrincipal->linha, total_linha);
            fclose(stream);
        }
    }
}
//------------------------------------------------------------------------------

float TFPrincipal::calculaNashSutcliffe(int numIntervalos, double *Pmod, double *Pobs)
{

    float somatorioNumerador = 0.0,
          somatorioDenominador = 0.0,
          somatorioPobs = 0.0,
          mediaPobs = 0.0;



    int i;

    for (i = 0; i < numIntervalos; i++)
    {
        somatorioPobs += Pobs[i];
    }
    mediaPobs = somatorioPobs / numIntervalos;

    for (i = 0; i < numIntervalos; i++)
    {
        somatorioNumerador += pow(Pobs[i] - Pmod[i], 2);
        somatorioDenominador += pow(Pobs[i] - mediaPobs, 2);
    }

    return (1 - (somatorioNumerador/somatorioDenominador));
}

//------------------------------------------------------------------------------
float TFPrincipal::CalculaCorrel(int numIntervalos, double *Pmod, double *Pobs)
{

    double somatorioPmod = 0.0,
           somatorioPobs = 0.0,
           mediaPmod = 0.0,
           mediaPobs = 0.0,
           dpPmod = 0.0,
           dpPobs = 0.0,
           numeradorCorrel = 0.0;

//Calcula a média de Pmod e Pobs
    for (int i = 1; i <= numIntervalos; i++)
    {
        somatorioPmod += Pmod[i];
        somatorioPobs += Pobs[i];
    }
    mediaPmod = somatorioPmod / numIntervalos;
    mediaPobs = somatorioPobs / numIntervalos;

//Calcula o desvio padrao de Pmod e Pobs
    for (int i = 1; i <= numIntervalos; i++)
    {
        dpPmod += pow((Pmod[i] - mediaPmod), 2);
        dpPobs += pow((Pobs[i] - mediaPobs), 2);
    }
    dpPmod = sqrt(dpPmod / (numIntervalos-1));
    dpPobs = sqrt(dpPobs / (numIntervalos-1));

//Calcula o coeficiente de Correlação
    for(int i = 1; i <= numIntervalos; i++)
    {
        numeradorCorrel += (Pmod[i] - mediaPmod) * (Pobs[i] - mediaPobs);
    }
    numeradorCorrel /= numIntervalos;

//Retorna o valor do coeficiente de Correlação
    return (numeradorCorrel / (dpPobs * dpPmod));
}

//------------------------------------------------------------------------------

void listaDadosRadar()
{

    FILE *stream;
    long curpos, length;
    int pos_obs = 0;


    char msg[100];
    int i=0,e_ano=0,data=0,tempo=0, linha=0;
    float temp=0, um=0, pressao=0, total_linha=0;
    float VIL = 0.0, TopoDosEcos = 0.0;
    double Pobs = 0.0;

    /* open a file for update */
    if(FPrincipal->OD_Dados->Execute())
    {
//nome = Form1->OD_Dados->FileName;//"F:\Olivete\DC\ArquivosEntrada\entradaFADISC.txt"; stream = fopen(FPrincipal->OD_Dados->FileName.c_str(), "rb+");
        if (stream == NULL)
            ShowMessage("Erro ao Abrir o Arquivo!!");
        else
        {
            FPrincipal->lblNomeArquivo->Caption = "Arquivo: " + FPrincipal->OD_Dados->FileName;
            while (!(feof(stream)))
            {
                curpos = ftell(stream);
                fseek(stream, 0L, SEEK_END);
                length = ftell(stream);
                fseek(stream, curpos, SEEK_SET);

//Le uma linha de cada vez do arquivo de entrada, guardando o valor em msg
                fgets(msg, length, stream);
                recupera_dados_radar(msg, e_ano, data, tempo, VIL, TopoDosEcos);
                curpos = ftell(stream);
                fseek(stream, 0L, SEEK_END);
                length = ftell(stream);
                fseek(stream, curpos, SEEK_SET);
//Le uma linha de cada vez do arquivo de entrada, guardando o valor em msg
                fgets(msg, length, stream);
                recupera_dados(msg, e_ano,data,tempo,temp, um, pressao, Pobs,linha, total_linha);



//procedimento para realizar os calculos
                if (tempo != FPrincipal->hora[FPrincipal->linha])
                {
                    FPrincipal->linha++;
                    linha = FPrincipal->linha;
                    FPrincipal->Pobs[linha] = Pobs;
                    FPrincipal->VIL[linha] = VIL;
                    FPrincipal->TopoDosEcos[linha] = TopoDosEcos;
                    FPrincipal->Calcula_Georgakakos(e_ano,data,tempo,temp, um, pressao, linha, total_linha);
                }
// else ShowMessage("erro");

                FPrincipal->tam_vet_erro++;
                pos_obs++;
            }
            linha = FPrincipal->linha;

            FPrincipal->DeltaVol = FPrincipal->CalculaDeltaVol(linha, FPrincipal->P, FPrincipal->Pobs);
            FPrincipal->Correl = FPrincipal->CalculaCorrel(linha, FPrincipal->P, FPrincipal->Pobs);
            FPrincipal->NashSutcliffe = FPrincipal->calculaNashSutcliffe(linha, FPrincipal->P, FPrincipal->Pobs);
            FPrincipal->edtDeltaVol->Text = FormatFloat("0.0000", FPrincipal->DeltaVol);
            FPrincipal->edtCorrel->Text = FormatFloat("0.0000", FPrincipal->Correl);
            FPrincipal->edtNash->Text = FormatFloat("0.0000", FPrincipal->NashSutcliffe);
            FPrincipal->Regressao(e_ano,data,tempo,temp, um, pressao,FPrincipal->linha, total_linha);
            fclose(stream);
        }
    }
}






//---------------------------------------------------------------------------


//para o micro esquerda

void recupera_dados_campus2(char msg[])
{
    int i=3,pos_atual;
    AnsiString ano="",dia="",tempo="";
    bool achou=true;
    AnsiString temperatura="",umidade="",pressao="";
    int p_ano ,p_dia,p_tempo;
    float p_temperatura,p_umidade,p_pressao;

    pos_atual=0;

    achou=true;
    for(i=pos_atual; i<pos_atual+10; i++)
        if((msg[i]!=',')&&(msg[i]!='-') &&(achou))
        {
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }
    achou=true;
    for(i=pos_atual+1; i<pos_atual+10; i++)
        if((msg[i]!=',') &&(achou))
        {
            ano = ano + StrToInt(msg[i]);
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }
    achou=true;
    for(i=pos_atual+1; i<pos_atual+10; i++)
        if((msg[i]!=',') &&(achou))
        {
            dia = dia + StrToInt(msg[i]);
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }
    achou=true;


    for(i=pos_atual+1; i<pos_atual+10; i++)
        if((msg[i]!=',') &&(achou))
        {
            tempo = tempo + StrToInt(msg[i]);
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }
    achou=true;
    for(i=pos_atual+1; i<pos_atual+10; i++)
        if((msg[i]!=',')&&(msg[i]!='-') &&(achou))
        {
            if (msg[i]=='.') msg[i]='.';
// if (msg[i]=='.') msg[i]=',';
            pressao = pressao + (msg[i]);
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }
    achou=true;
    for(i=pos_atual+1; i<pos_atual+10; i++)
        if((msg[i]!=',')&&(msg[i]!='-') &&(achou))
        {
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }
    achou=true;
    for(i=pos_atual+1; i<pos_atual+10; i++)
        if((msg[i]!=',') &&(achou))
        {
            if (msg[i]=='.') msg[i]='.';
//if (msg[i]=='.') msg[i]=',';
            temperatura = temperatura + (msg[i]);
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }
    achou=true;
    for(i=pos_atual+1; i<pos_atual+10; i++)
        if((msg[i]!=',') &&(achou))
        {
            if (msg[i]=='.') msg[i]='.';
//if (msg[i]=='.') msg[i]=',';
            umidade = umidade + (msg[i]);
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }
    achou=true;



    for(i=pos_atual+1; i<=strlen(msg); i++)
        if((msg[i]!=',') &&(msg[i]!='-')&&(achou))
        {
            if (msg[i+1]==',')
            {
                achou=false;
                pos_atual=i;
            }
        }

    p_ano=StrToInt(ano);
    p_dia=StrToInt(dia);
    p_tempo=StrToInt(tempo);
    p_temperatura=StrToFloat(temperatura);
    p_umidade=StrToFloat(umidade);
    if (p_umidade >= 100.0)
        p_umidade = 99.9;
    p_pressao=StrToFloat(pressao);
    FPrincipal->total_linha=FPrincipal->total_linha+1;
//procedimento para realizar os calculos
    if (p_tempo != FPrincipal->hora[FPrincipal->linha])
    {
        FPrincipal->linha++;
        FPrincipal->Calcula_Georgakakos(p_ano,p_dia,p_tempo,p_temperatura, p_umidade, p_pressao, FPrincipal->linha, FPrincipal->total_linha);
    }
// else ShowMessage("erro");

}
//---------------------------------------------------------------------------

void lista2()
{

    FILE *stream;
    long curpos, length;
    char msg[150];
    AnsiString anda="";

    /* open a file for update */
    if(FPrincipal->OD_Dados->Execute())
    {
//nome = Form1->OD_Dados->FileName;//"F:\Olivete\DC\ArquivosEntrada\entradaFADISC.txt"; stream = fopen(FPrincipal->OD_Dados->FileName.c_str(), "rb+");
        if (stream==NULL)
            ShowMessage("Erro ao Abrir o Arquivo!!");
        else
        {
            while (!(feof(stream)))
            {
                curpos = ftell(stream);
                fseek(stream, 0L, SEEK_END);
                length = ftell(stream);
                fseek(stream, curpos, SEEK_SET);
                /* read a string from the file*/
                fgets(msg, length, stream);
                if (strlen(msg)<120)
                {
                    recupera_dados_campus2(msg);
                }
                else


                {
                    for(int i=0; i<=strlen(msg); i++)
                        anda= anda + " ";
                }
            }
            fclose(stream);
//limpa_vetores();
//limpa_vetor_altura(FPrincipal->altura,FPrincipal->total_intervalo);
        }
    }
}

//---------------------------------------------------------------------------

void __fastcall TFPrincipal::btnEstacaoClick(TObject *Sender)
{
    total_intervalo = 100;
    altura = StrToFloat(e_altura->Text);
    alocacao_dinamica_vetores(2000);
// SG->RowCount = total_linha+6;

    if ( RG_leitura->ItemIndex==1)
        lista();
    else if ( RG_leitura->ItemIndex==0)
        lista2();

    libera_memoria();


}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void salva_excel()
{
    Variant book, Wb, Sheet;
    float strRowTemp;

    try
    {
        Variant V = Variant::CreateObject("Excel.Application");
        book = V.OlePropertyGet( "Workbooks" );
        book.OleFunction("Add");
        Wb = V.OlePropertyGet("ActiveWorkBook");
        Sheet = Wb.OlePropertyGet("ActiveSheet");//???????Sheet
        int linha = 0;
        Screen->Cursor = crHourGlass; // cursor de espera
        while (linha <= FPrincipal->SG_final->RowCount)
        {
            for (int j=0; j<FPrincipal->SG_final->ColCount; j++)

                Sheet.OlePropertyGet("Cells").OlePropertyGet("Item",linha+1,j+1).OlePropertySet("Value",FPrincipal->SG_final->Cells[j][linha].c_str());
            linha++;
        }
        V.OlePropertySet("Visible", true);
        V.OleFunction("Quit");
        V = book = Wb = Sheet = Unassigned;
        Screen->Cursor = crDefault;
    }



    catch(...)
    {
        Application->MessageBox ("Versão do Ms-Excel Incompatível","Erro",MB_OK+MB_ICONEXCLAMATION);
    }

}
//---------------------------------------------------------------------------


void __fastcall TFPrincipal::btnSalvarClick(TObject *Sender)
{
//salva();
    salva_excel();
}
//---------------------------------------------------------------------------

void __fastcall TFPrincipal::btnLimparClick(TObject *Sender)
{
    for (int i=1; i < FPrincipal->SG_final->RowCount; i++)
        for (int j=0; j < FPrincipal->SG_final->ColCount; j++)
        {
// FPrincipal->SG->Cells[j][i]=" ";
            FPrincipal->SG_final->Cells[j][i]=" ";
        }
//SG->RowCount=2;
    SG_final->RowCount=2;
    linha=0;
// libera_memoria();
}
//---------------------------------------------------------------------------

void __fastcall TFPrincipal::btnHelpClick(TObject *Sender)
{
    Application->HelpFile = "Sistema de Alerta.hlp";
    Application->HelpJump("");

}
//---------------------------------------------------------------------------


void __fastcall TFPrincipal::btnSCSClick(TObject *Sender)
{
    Application->CreateForm(__classid(TF_Principal), &F_Principal);
    F_Principal->ShowModal();
    F_Principal->Free();


}
//---------------------------------------------------------------------------


void __fastcall TFPrincipal::btnRadarClick(TObject *Sender)
{
    total_intervalo = 100;
    altura = StrToFloat(e_altura->Text);
    alocacao_dinamica_vetores(2000);
// SG->RowCount = total_linha+6;
    if (RG_leitura->ItemIndex == 1)
        listaDadosRadar();


    /* else
    if (RG_leitura->ItemIndex == 0)
    lista2(); */
    libera_memoria();

}
//---------------------------------------------------------------------------



