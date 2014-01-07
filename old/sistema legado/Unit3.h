//---------------------------------------------------------------------------

#ifndef Unit3H
#define Unit3H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------
class TFPrincipal : public TForm
{
__published:	// IDE-managed Components
        TOpenDialog *OD_Dados;
        TStringGrid *SG_final;
        TSaveDialog *SV_Dados;
private:	// User declarations
public:
        int linha, total_linha, tam_vet_erro, S1, S2;

        float *ToC;
        int *dia;
        int *hora;
        float *ur;
        float *pommHg;
        double *To;
        float *po;
        double *Td;
        double *ps;
        double *Ts;
        double *es;
        double *wo;
        float *wss;
        float *Tt;
        float *pt;
        double *wst;
        double *teta;
        float *L;
        double *tetaE1;
        float *I;
        float *Nv;
        float *vbeta;
        float *c;
        double *Dast;
        double *P;
        double *obs;
        double *erro;
        double *Zb;
        float *Dc;
        float *Nd;
        float *vp;
        float *pt1;
        float *pt2;
        float *plinha;
        float *tslinha;
        float *Tm;
        float *v;
        float *DELTAT;
        float *X;
        float *Xant;
        float *Dx;
        float *Ob1;
        float *Ob2;
        float *Ot;
        double *Zt;
        double *Zt2;
        double *Zc;
        double *teta17;
        int *teste;
        float *Tw;
        float *Tw1;
        double *Pobs;
        float *VIL;
        float *TopoDosEcos;
        float DeltaVol;
        float Correl;

        float NashSutcliffe;

        int C_ANO;
        int C_DATA;
        int C_TEMPO;
        int C_TEMPERATURA;
        int C_UMIDADE;
        int C_TEMPORV;
        int C_PRECAC;
        int C_POBS;
        int C_X;
        int C_ZC;
        int C_ZT;
        int C_ZT2;
        int C_ZB;
        int C_PRESSAT;
        int C_ERROTETAE;
        int C_W0;
        int C_WS;
        int C_WT;
        int C_PS;
        int C_TS;
        int C_TETA;
        int C_V;
        int C_PT;
        int C_TT;
        int C_TETAE;
        int C_OBGEO;
        int C_OBRADAR;
        int C_OT;
        int C_I;
        int C_NV;
        int C_ND;
        int C_VP;
        int C_DC;

        int altura, total_intervalo, DELTA;
        TFPrincipal *e_altura, *alfa, *area, *densidade, *me_lag, *me_intervalo_previsao, *v_inicial, *e2, *pl, *e3, *E1, *E4, *xinicial, *dt, *chkBoxVIL, *Checked, *d1, *d2, *lblNomeArquivo, *edtDeltaVol, *edtCorrel, *edtNash;
        int **vet_resultados;
        int *vet_altura;

        __fastcall TFPrincipal(TComponent* Owner);
        void libera_memoria();
        void alocacao_dinamica_vetores(int);
        void __fastcall TFPrincipal::FormActivate(TObject *Sender);
        void Limpa_Dados();
        void insere_entrada(int,int,int,float,float,float,int);
        void insere_calculos(float,float,float,float,float,float,float,float,float,float,float,float,float,int);
        void exibe_calculos(int,int,int,float,float,float,int);
        void Calcula_Dados(int,int,int,float,float,float,int,int);
        void carrega_vetor_altura(float,int); //não era TFPrincipal:: (alguns não marcados)
        void limpa_vetor_altura(int,int);     //não era TFPrincipal::
        float f(float,float,int);
        float fl(float,float,int);
        float NewtonRaphson(float,float,int);
        float calculaMedia(int, float*);       //não era TFPrincipal::
        float calculaVariancia(int,float,float*); //não era TFPrincipal::
        float calculaCovariancia(int,float,float*); //não era TFPrincipal::
        float calculaAutoCorrelacao(float,float); //não era TFPrincipal::
        void Regressao(int,int,int,float,float,float,int,int);
        void converte_dados(int,int,float,float,float,int);
        void __fastcall B_CalcularClick(TObject *Sender);
        void __fastcall btnSairClick(TObject *Sender);
        void calcula_vetor_erro(); //não era TFPrincipal::
        void calcula_vetor_S1();   //não era TFPrincipal::
        void calcula_vetor_S2();
        void Calcula_Georgakakos(int,int,int,float,float,float,int,int);
        float CalculaDeltaVol(int, double*, double*);
        //gap
        float calculaNashSutcliffe(int,double*,double*);
        float CalculaCorrel(int, double*, double*);
        //gap
        void __fastcall btnEstacaoClick(TObject *Sender);
        //gap
        void __fastcall btnSalvarClick(TObject *Sender);
        void __fastcall btnLimparClick(TObject *Sender);
        void __fastcall btnHelpClick(TObject *Sender);
        void __fastcall btnSCSClick(TObject *Sender);
        void __fastcall btnRadarClick(TObject *Sender);


};
//---------------------------------------------------------------------------
extern PACKAGE TFPrincipal *FPrincipal;
//---------------------------------------------------------------------------
#endif

