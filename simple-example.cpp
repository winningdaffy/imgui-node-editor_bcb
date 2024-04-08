#include <imgui.h>
#include <imgui_node_editor.h>
#include <blue_print.h>

#pragma comment(lib, "d3d11")       // Automatically link with d3dcompiler.lib as we are using D3DCompile() below.
#pragma comment(lib, "d3dcompiler") // Automatically link with d3dcompiler.lib as we are using D3DCompile() below.
#pragma comment(lib, "xinput")

int main(int argc, char **argv)
{
    bp::TBluePrint blue_print("demo");
    bp::TNode *p_node = nullptr;
    p_node = blue_print.add_node("source", bp::TNodeType::PORT);
    if (p_node != nullptr)
    {
        p_node->add_out_pin("out", bp::TPinType::VALUE_FLOAT);
    }

    p_node = blue_print.add_node("pressure_analysis");
    if (p_node != nullptr)
    {
        p_node->add_in_pin("pcyl", bp::TPinType::VALUE_FLOAT);
        p_node->add_out_pin("pmax", bp::TPinType::VALUE_FLOAT);
        p_node->add_out_pin("pmin", bp::TPinType::VALUE_FLOAT);
    }

    p_node = blue_print.add_node("imep");
    if (p_node != nullptr)
    {
        p_node->add_in_pin("pcyl", bp::TPinType::VALUE_FLOAT);
        p_node->add_in_pin("volume", bp::TPinType::VALUE_FLOAT);
        p_node->add_out_pin("imeph", bp::TPinType::VALUE_FLOAT);
        p_node->add_out_pin("imepn", bp::TPinType::VALUE_FLOAT);
        p_node->add_out_pin("imepl", bp::TPinType::VALUE_FLOAT);
    }

    if (blue_print.Create())
    {
        return blue_print.Run();
    }

    return 0;
}