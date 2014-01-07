#include <vcl.h>
#pragma hdrstop

#include "math.h";
#include "UPrincipal.h"
#include "principal.h"
#include "UHUT.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TF_Principal *F_Principal;
int total=0;

//---------------------------------------------------------------------------
__fastcall TF_Principal::TF_Principal(TComponent* Owner) : TForm(Owner)
{
    qtde_hut = 1;
}
//---------------------------------------------------------------------------
void __fastcall TF_Principal::FormActivate(TObject *Sender)
{
    Application->Title = "...SCS..." ;

    SG->RowCount = FPrincipal->SG_final->RowCount;

    total = FPrincipal->SG_final->RowCount - 1;
    for(int i=0; i<=FPrincipal->SG_final->RowCount; i++)
    {
        if (FPrincipal->SG_final->Cells[C_PRECAC][i+1]== "")
            SG->Cells[C_PMOD_SCS][i] = "0.00";
        else
            SG->Cells[C_PMOD_SCS][i] = ("0.00",FPrincipal->SG_final->Cells[C_PRECAC][i+1]); //col * lin if (SG->Cells[C_PMOD_SCS][i] == "")
        SG->Cells[C_PMOD_SCS][i] = "0.00";

        SG->Cells[C_SPMOD_SCS][i] = "0.00";
        SG->Cells[C_SPE_SCS][i] = "0.00";



        SG->Cells[C_PE_SCS][i] = "0.00";
        SG->Cells[C_TEMPO_SCS][i] = FPrincipal->SG_final->Cells[C_TEMPO][i+1];
    }

}
//---------------------------------------------------------------------------
void __fastcall TF_Principal::btnCalcularClick(TObject *Sender)
{
    float SPe = 0.0,
    p1,
    p2,
    Ia = 0.0;

    Ia = StrToFloat(edtIa->Text);

    for(int i=0;i<=FPrincipal->SG_final->RowCount+8;i++)
    {
        if (SG->Cells[C_PMOD_SCS][i] == "")
        SG->Cells[C_PMOD_SCS][i] = "0.00" ;
    }

    //Acumula as precipitacoes modeladas
    for(int i = 0; i <= FPrincipal->SG_final->RowCount; i++) {
        if ((i==0) || (SG->Cells[C_PMOD_SCS][i] == ""))
            SG->Cells[C_SPMOD_SCS][i] = (SG->Cells[C_PMOD_SCS][0]) ;
        else {
            if (SG->Cells[C_PMOD_SCS][i] == "0")
                SG->Cells[C_SPMOD_SCS][i] = "0";
            else {
                p1 = StrToFloat(SG->Cells[C_SPMOD_SCS][i-1]);
                p2 = StrToFloat(SG->Cells[C_PMOD_SCS][i]);
                SG->Cells[C_SPMOD_SCS][i] = p1 + p2;
                SG->Cells[C_SPMOD_SCS][i] = FormatFloat("0.0", StrToFloat(SG->Cells[C_SPMOD_SCS][i])); qtde_hut++;
            }
        }

        //calcula o armazemaento S [mm] -- SPmod
        S = (25400 / StrToFloat(edtCN->Text)) - 254; //Valores em milimetros

        //calcula Precipitacao efetiva acumulada (SPe)
        if (StrToFloat(SG->Cells[C_SPMOD_SCS][i]) <= Ia)
        SG->Cells[C_SPE_SCS][i] = "0" ;
        else {
            SPe = pow((StrToFloat(SG->Cells[C_SPMOD_SCS][i]) - Ia), 2) / (StrToFloat(SG->Cells[C_SPMOD_SCS][i]) + S - Ia);
            SG->Cells[C_SPE_SCS][i] = (SPe);
            SG->Cells[C_SPE_SCS][i] =FormatFloat("0.0", StrToFloat(SG->Cells[C_SPE_SCS][i]));
        }

        //desacumula SPe
        if (i == 0)
        SG->Cells[C_PE_SCS][0] = (SG->Cells[C_SPE_SCS][0]);
        else {
            p1 = StrToFloat(SG->Cells[C_SPE_SCS][i]);
            p2 = StrToFloat(SG->Cells[C_SPE_SCS][i-1]);
            SG->Cells[C_PE_SCS][i] = p1 - p2;
            SG->Cells[C_PE_SCS][i] =FormatFloat("0.0", StrToFloat(SG->Cells[C_PE_SCS][i]));
            if (SG->Cells[C_SPE_SCS][i] == "0")
                SG->Cells[C_PE_SCS][i] = "0";
        }
    }

    for (int i=total-1; i<=SG->RowCount; i++) {
        SG->Cells[C_SPMOD_SCS][i] = "0.00";
        SG->Cells[C_SPE_SCS][i] = "0.00";
        SG->Cells[C_PE_SCS][i] = "0.00";
    }

}
//---------------------------------------------------------------------------
void __fastcall TF_Principal::btnSairClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TF_Principal::btnLimparClick(TObject *Sender)
{
    for(int i=0;i<=SG->RowCount;i++) {
        SG->Cells[C_PMOD_SCS][i] = "0"; //coluna linha
        SG->Cells[C_SPMOD_SCS][i] = "0";
        SG->Cells[C_SPE_SCS][i] = "0";
        SG->Cells[C_PE_SCS][i] = "0";
    }
}
//---------------------------------------------------------------------------
void __fastcall TF_Principal::btnHUTClick(TObject *Sender)
{
    Application->CreateForm(__classid(TF_HUT), &F_HUT);
    F_HUT->ShowModal();
    F_HUT->Free();
}
//---------------------------------------------------------------------------

void __fastcall TF_Principal::edtCNExit(TObject *Sender)
{
    S = (25400 / StrToFloat(edtCN->Text)) - 254;
}
//---------------------------------------------------------------------------
