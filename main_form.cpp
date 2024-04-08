//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "main_form.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#pragma comment(lib, "d3d11")       // Automatically link with d3dcompiler.lib as we are using D3DCompile() below.
#pragma comment(lib, "d3dcompiler") // Automatically link with d3dcompiler.lib as we are using D3DCompile() below.
#pragma comment(lib, "xinput")

TForm149 *Form149;
//---------------------------------------------------------------------------
__fastcall TForm149::TForm149(TComponent *Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm149::FormCreate(TObject *Sender)
{
    p_blue_print_ = new bp::TBluePrint("demo");
    bp::TNode *p_node = nullptr;
    p_node = p_blue_print_->add_node("source", bp::TNodeType::PORT);
    if (p_node != nullptr)
    {
        p_node->add_out_pin("out", bp::TPinType::VALUE_FLOAT);
    }

    p_node = p_blue_print_->add_node("pressure_analysis");
    if (p_node != nullptr)
    {
        p_node->add_in_pin("pcyl", bp::TPinType::VALUE_FLOAT);
        p_node->add_out_pin("pmax", bp::TPinType::VALUE_FLOAT);
        p_node->add_out_pin("pmin", bp::TPinType::VALUE_FLOAT);
    }

    p_node = p_blue_print_->add_node("imep");
    if (p_node != nullptr)
    {
        p_node->add_in_pin("pcyl", bp::TPinType::VALUE_FLOAT);
        p_node->add_in_pin("volume", bp::TPinType::VALUE_FLOAT);
        p_node->add_out_pin("imeph", bp::TPinType::VALUE_FLOAT);
        p_node->add_out_pin("imepn", bp::TPinType::VALUE_FLOAT);
        p_node->add_out_pin("imepl", bp::TPinType::VALUE_FLOAT);
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm149::Button1Click(TObject *Sender)
{
    if (p_blue_print_->Create(true, this->Handle, 1200, 800))
    // if (p_blue_print_->Create(false))
    {
        p_blue_print_->Run();
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm149::FormResize(TObject *Sender)
{
    p_blue_print_->set_window_size(0, 0, ClientWidth, ClientHeight);
}
//---------------------------------------------------------------------------
