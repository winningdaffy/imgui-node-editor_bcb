//---------------------------------------------------------------------------

#ifndef main_formH
#define main_formH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>

#include "blue_print.h"
//---------------------------------------------------------------------------
class TForm149 : public TForm
{
__published:
	TButton *Button1;
    void __fastcall FormCreate(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);

private: // User declarations
public:  // User declarations
    bp::TBluePrint *p_blue_print_ = NULL;
    __fastcall TForm149(TComponent *Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm149 *Form149;
//---------------------------------------------------------------------------
#endif
