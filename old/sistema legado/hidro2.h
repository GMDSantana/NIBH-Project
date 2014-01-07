//---------------------------------------------------------------------------

#ifndef hidro2H
#define hidro2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TFPrincipal : public TForm
{
__published:	// IDE-managed Components
private:	// User declarations
public:
        float *ToC;		// User declarations
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
        __fastcall TFPrincipal(TComponent* Owner);
        void libera_memoria();
        void alocacao_dinamica_vetores(int);
        void __fastcall TFPrincipal::FormActivate(TObject *Sender);
};

//---------------------------------------------------------------------------
extern PACKAGE TFPrincipal *FPrincipal;
//---------------------------------------------------------------------------
#endif
