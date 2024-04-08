# imgui-node-editor for C++ builder

Integrate the [imgui-node-editor](https://github.com/thedmd/imgui-node-editor.git) to c++ builder, include command line application and VCL application. Add some functions as below:
* Node library
* Multi pages
* Save and load from .xml file

## Using
Open the **test_node_editor_bcb.groupproj** and compile.

### New class and add node to library
```c++
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
```

### Show the window in TForm
```c++
if (p_blue_print_->Create(true, this->Handle, 1200, 800))
{
    p_blue_print_->Run();
}
```
