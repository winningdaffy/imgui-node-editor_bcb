#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include "widgets.h"
#include "blue_print.h"
#include "setup.h"
#include "platform.h"
#include "renderer.h"

extern "C" {
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#define STBI_NO_SIMD
#include "stb_image.h"
}

#include "xml_interface.h"

namespace bp
{

namespace ed = ax::NodeEditor;

//* TNode

void TNode::set_type(TNodeType type)
{
    type_ = type;
}
void TNode::set_id(int32_t id)
{
    id_ = id;
}
void TNode::add_in_pin(const char *p_name, TPinType type)
{
    in_pin_list_.emplace_back(0, p_name, type);
    in_pin_list_.back().dir_ = TPinDir::Input;
}
void TNode::add_out_pin(const char *p_name, TPinType type)
{
    out_pin_list_.emplace_back(0, p_name, type);
    out_pin_list_.back().dir_ = TPinDir::Output;
}
void TNode::set_in_pin_id(const char *p_name, int32_t id)
{
    for (auto &it_pin : in_pin_list_)
    {
        if (it_pin.name_ == p_name)
        {
            it_pin.id_ = id;
            return;
        }
    }
}
void TNode::set_out_pin_id(const char *p_name, int32_t id)
{
    for (auto &it_pin : out_pin_list_)
    {
        if (it_pin.name_ == p_name)
        {
            it_pin.id_ = id;
            return;
        }
    }
}

//* TNodeLib
TNodeLib::TNodeLib()
{
}
TNodeLib::~TNodeLib()
{
    for (auto &it_node : node_list_)
    {
        TNode *p_node = it_node.second;
        if (p_node != NULL)
        {
            delete (p_node);
            p_node = NULL;
        }
    }
}
TNode *TNodeLib::add_node(const char *p_name,
                          TNodeType type,
                          ImColor color)
{
    TNode *p_node = new TNode(0, p_name, color);
    p_node->set_type(type);

    node_list_[p_name] = p_node;
    return p_node;
}

TNode *TNodeLib::get_node(const char *p_name)
{
    TNode *p_temp_node = nullptr;
    // 检查节点是否存在
    if (node_list_.find(p_name) != node_list_.end())
    {
        // 如果节点存在，执行相应的操作
        p_temp_node = node_list_[p_name];
    }
    return p_temp_node;
}

//* TBluePrint
TBluePrint::TBluePrint(const char *name)
    : m_Name(name), m_Platform(CreatePlatform()), m_Renderer(CreateRenderer())
{
    //* init node library
    p_node_lib_ = std::make_unique<TNodeLib>();

    m_Platform->ApplicationStart();
}

TBluePrint::~TBluePrint()
{
    m_Renderer->Destroy();

    m_Platform->ApplicationStop();

    if (m_Context)
    {
        ImGui::DestroyContext(m_Context);
        m_Context = nullptr;
    }
}

int32_t TBluePrint::get_next_id()
{
    return next_id_++;
}

bool TBluePrint::Create(int32_t is_child_wnd, void *parent_handle, int width /*= -1*/, int height /*= -1*/)
{
    m_Context = ImGui::CreateContext();
    ImGui::SetCurrentContext(m_Context);

    if (!m_Platform->OpenMainWindow("TBluePrint", is_child_wnd, parent_handle, width, height))
        return false;

    if (!m_Renderer->Create(*m_Platform))
        return false;

    m_IniFilename = m_Name + ".ini";

    ImGuiIO &io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.IniFilename = nullptr; //m_IniFilename.c_str();
    io.LogFilename = nullptr;

    // ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();
    ImGui::StyleColorsClassic();

    RecreateFontAtlas();

    //    m_Platform->AcknowledgeWindowScaleChanged();
    //    m_Platform->AcknowledgeFramebufferScaleChanged();

    editor_context_ = ed::CreateEditor(&config_);

    OnStart();

    Frame();

    return true;
}

int TBluePrint::Run()
{
    m_Platform->ShowMainWindow();

    while (m_Platform->ProcessMainWindowEvents())
    {
        if (!m_Platform->IsMainWindowVisible())
            continue;

        Frame();
    }

    ed::DestroyEditor(editor_context_);

    OnStop();

    return 0;
}

void TBluePrint::RecreateFontAtlas()
{
    ImGuiIO &io = ImGui::GetIO();

    IM_DELETE(io.Fonts);

    io.Fonts = IM_NEW(ImFontAtlas);

    ImFontConfig config;
    config.OversampleH = 4;
    config.OversampleV = 4;
    config.PixelSnapH = false;

    m_DefaultFont = io.Fonts->AddFontFromFileTTF("data/Play-Regular.ttf", 18.0f, &config);
    m_HeaderFont = io.Fonts->AddFontFromFileTTF("data/Cuprum-Bold.ttf", 20.0f, &config);

    io.Fonts->Build();
}

void TBluePrint::Frame()
{
    auto &io = ImGui::GetIO();

    //        if (m_Platform->HasWindowScaleChanged())
    //    		m_Platform->AcknowledgeWindowScaleChanged();
    //
    //    	if (m_Platform->HasFramebufferScaleChanged())
    //    	{
    //    		RecreateFontAtlas();
    //    		m_Platform->AcknowledgeFramebufferScaleChanged();
    //    	}
    //
    //    	const float windowScale      = m_Platform->GetWindowScale();
    //    	const float framebufferScale = m_Platform->GetFramebufferScale();
    //
    //    	if (io.WantSetMousePos)
    //    	{
    //    		io.MousePos.x *= windowScale;
    //    		io.MousePos.y *= windowScale;
    //    	}

    m_Platform->NewFrame();

    //        // Don't touch "uninitialized" mouse position
    //        if (io.MousePos.x > -FLT_MAX && io.MousePos.y > -FLT_MAX)
    //        {
    //            io.MousePos.x    /= windowScale;
    //            io.MousePos.y    /= windowScale;
    //        }
    //        io.DisplaySize.x /= windowScale;
    //        io.DisplaySize.y /= windowScale;
    //
    //        io.DisplayFramebufferScale.x = framebufferScale;
    //        io.DisplayFramebufferScale.y = framebufferScale;

    m_Renderer->NewFrame();

    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    const auto windowBorderSize = ImGui::GetStyle().WindowBorderSize;
    const auto windowRounding = ImGui::GetStyle().WindowRounding;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::Begin("Content", nullptr, GetWindowFlags());
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, windowBorderSize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, windowRounding);

    ed::SetCurrentEditor(editor_context_);

    show_controls();

    OnFrame(io.DeltaTime);

    ed::SetCurrentEditor(nullptr);

    ImGui::PopStyleVar(2);
    ImGui::End();
    ImGui::PopStyleVar(2);

    // Rendering
    m_Renderer->Clear(ImColor(32, 32, 32, 255));
    ImGui::Render();
    m_Renderer->RenderDrawData(ImGui::GetDrawData());

    m_Platform->FinishFrame();
}

void TBluePrint::show_controls(void)
{
    auto &io = ImGui::GetIO();

    static float leftPaneWidth = 200.0f;
    static float rightPaneWidth = 800.0f;
    Splitter(true, 4.0f, &leftPaneWidth, &rightPaneWidth, 50.0f, 50.0f);

    show_left_panel(leftPaneWidth - 4);

    ImGui::SameLine();
    show_tab_control(io.DeltaTime);
}

void TBluePrint::SetTitle(const char *title)
{
    m_Platform->SetMainWindowTitle(title);
}

bool TBluePrint::Close()
{
    return m_Platform->CloseMainWindow();
}

void TBluePrint::Quit()
{
    m_Platform->Quit();
}

const std::string &TBluePrint::GetName() const
{
    return m_Name;
}

ImFont *TBluePrint::DefaultFont() const
{
    return m_DefaultFont;
}

ImFont *TBluePrint::HeaderFont() const
{
    return m_HeaderFont;
}

ImTextureID TBluePrint::LoadTexture(const char *path)
{
    int width = 0, height = 0, component = 0;
    if (auto data = stbi_load(path, &width, &height, &component, 4))
    {
        auto texture = CreateTexture(data, width, height);
        stbi_image_free(data);
        return texture;
    }
    else
        return nullptr;
}

ImTextureID TBluePrint::CreateTexture(const void *data, int width, int height)
{
    return m_Renderer->CreateTexture(data, width, height);
}

void TBluePrint::DestroyTexture(ImTextureID texture)
{
    m_Renderer->DestroyTexture(texture);
}

int TBluePrint::GetTextureWidth(ImTextureID texture)
{
    return m_Renderer->GetTextureWidth(texture);
}

int TBluePrint::GetTextureHeight(ImTextureID texture)
{
    return m_Renderer->GetTextureHeight(texture);
}

ImGuiWindowFlags TBluePrint::GetWindowFlags() const
{
    return ImGuiWindowFlags_NoTitleBar |
           ImGuiWindowFlags_NoResize |
           //    ImGuiWindowFlags_AlwaysAutoResize |
           ImGuiWindowFlags_NoMove |
           ImGuiWindowFlags_NoScrollbar |
           ImGuiWindowFlags_NoScrollWithMouse |
           ImGuiWindowFlags_NoSavedSettings |
           ImGuiWindowFlags_NoBringToFrontOnFocus;
}

bool TBluePrint::Splitter(bool split_vertically,
                          float thickness,
                          float *size1,
                          float *size2,
                          float min_size1,
                          float min_size2,
                          float splitter_long_axis_size)
{
    ImGuiContext &g = *GImGui;
    if (editor_context_ != nullptr)
    {
        ImGuiWindow *window = g.CurrentWindow;
        ImGuiID id = window->GetID("##Splitter");
        ImRect bb;
        bb.Min = window->DC.CursorPos +
                 (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
        bb.Max = bb.Min + ImGui::CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
        return ImGui::SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
    }
    else
    {
        return false;
    }
}

void TBluePrint::show_left_panel(float panel_width)
{
    auto &io = ImGui::GetIO();

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    ImGui::BeginChild("Block Library", ImVec2(panel_width, 0), true, window_flags);

    // ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetCursorScreenPos(),
    //                                           ImGui::GetCursorScreenPos() + ImVec2(panel_width, ImGui::GetTextLineHeight()),
    //                                           ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]),
    //                                           ImGui::GetTextLineHeight() * 0.25f);
    // ImGui::Spacing();
    // ImGui::SameLine();
    // ImGui::TextUnformatted("Nodes");

    if (ImGui::TreeNodeEx("Block Library", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                               //    ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                               ImGuiTreeNodeFlags_SpanAvailWidth;
        static bool test_drag_and_drop = true;
        // 'selection_mask' is dumb representation of what may be user-side selection state.
        //  You may retain selection state inside or outside your objects in whatever format you see fit.
        // 'node_clicked' is temporary storage of what node we have clicked to process selection at the end
        /// of the loop. May be a pointer to your own node type, etc.
        static int selection_mask = (1 << 2);
        int node_clicked = -1;
        auto library_node_list = p_node_lib_->get_node_list();
        int32_t i = 0;
        for (auto &it_node : library_node_list)
        {
            TNode *p_temp_node = it_node.second;
            // Disable the default "open on single-click behavior" + set Selected flag according to our selection.
            ImGuiTreeNodeFlags node_flags = base_flags;
            const bool is_selected = (selection_mask & (1 << i)) != 0;
            // if (is_selected)
            //     node_flags |= ImGuiTreeNodeFlags_Selected;
            // if (i < 3)
            // {
            //     // Items 0..2 are Tree Node
            //     bool node_open = ImGui::TreeNodeEx((void *)(intptr_t)i, node_flags, "Selectable Node %d", i);
            //     if (ImGui::IsItemClicked())
            //         node_clicked = i;
            //     if (test_drag_and_drop && ImGui::BeginDragDropSource())
            //     {
            //         ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
            //         ImGui::Text("This is a drag and drop source");
            //         ImGui::EndDragDropSource();
            //     }
            //     if (node_open)
            //     {
            //         ImGui::BulletText("Blah blah\nBlah Blah");
            //         ImGui::TreePop();
            //     }
            // }
            // else
            {
                // Items 3..5 are Tree Leaves
                // The only reason we use TreeNode at all is to allow selection of the leaf. Otherwise we can
                // use BulletText() or advance the cursor by GetTreeNodeToLabelSpacing() and call Text().
                node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
                ImGui::TreeNodeEx((void *)(intptr_t)i, node_flags, "%s", p_temp_node->name_.c_str());
                if (ImGui::IsItemClicked())
                    node_clicked = i;
                if (test_drag_and_drop && ImGui::BeginDragDropSource())
                {
                    strcpy(drag_drop_block_name_, p_temp_node->name_.c_str());
                    ImGui::SetDragDropPayload("_BLOCK_LIBRARY", drag_drop_block_name_, strlen(drag_drop_block_name_));
                    ImGui::Text("This is a drag and drop source");
                    ImGui::EndDragDropSource();
                }
            }
            i++;
        }
        // if (node_clicked != -1)
        // {
        //     // Update selection state
        //     // (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
        //     if (ImGui::GetIO().KeyCtrl)
        //         selection_mask ^= (1 << node_clicked); // CTRL+click to toggle
        //     else                                       //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
        //         selection_mask = (1 << node_clicked);  // Click to single-select
        // }
        ImGui::TreePop();
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
}

void TBluePrint::show_editor(float delta_time, int32_t page_id)
{
    //* editor
    ed::Begin("My Editor", ImVec2(0.0, 0.0f));

    {
        //* display node
        for (auto &it_node : node_list_)
        {
            if (it_node.page_id_ == page_id)
            {
                draw_node(it_node);
            }
        }
    }

    {
        //* draw link
        for (auto &link : link_list_)
            ed::Link(link.id_, link.begin_pin_id_, link.end_pin_id_, link.color_, link.width_);
    }

    {
        //* link node
        if (!createNewNode)
        {
            if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f))
            {
                auto showLabel = [](const char *label, ImColor color) {
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
                    auto size = ImGui::CalcTextSize(label);

                    auto padding = ImGui::GetStyle().FramePadding;
                    auto spacing = ImGui::GetStyle().ItemSpacing;

                    ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

                    auto rectMin = ImGui::GetCursorScreenPos() - padding;
                    auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

                    auto drawList = ImGui::GetWindowDrawList();
                    drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
                    ImGui::TextUnformatted(label);
                };

                ed::PinId startPinId = 0, endPinId = 0;
                if (ed::QueryNewLink(&startPinId, &endPinId))
                {
                    auto startPin = FindPin(startPinId.Get());
                    auto endPin = FindPin(endPinId.Get());

                    newLinkPin = startPin ? startPin : endPin;

                    if (startPin && endPin)
                    {
                        if (startPin->dir_ == TPinDir::Input)
                        {
                            std::swap(startPin, endPin);
                            std::swap(startPinId, endPinId);
                        }

                        if (endPin == startPin)
                        {
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        }
                        else if (endPin->dir_ == startPin->dir_)
                        {
                            showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        }
                        else if (endPin->p_node_ == startPin->p_node_)
                        {
                            showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                        }
                        else if (endPin->type_ != startPin->type_)
                        {
                            showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                        }
                        else
                        {
                            showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                            if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
                            {
                                link_list_.emplace_back(TLink(get_next_id(),
                                                              startPinId.Get(),
                                                              endPinId.Get()));
                                link_list_.back().color_ = GetIconColor(startPin->type_);
                                link_list_.back().width_ = default_link_width_;
                            }
                        }
                    }
                }

                //                        ed::PinId pinId = 0;
                //                        if (ed::QueryNewNode(&pinId))
                //                        {
                //                            newLinkPin = FindPin(pinId.Get());
                //                            if (newLinkPin)
                //                                showLabel("+ Create Node", ImColor(32, 45, 32, 180));
                //
                //                            if (ed::AcceptNewItem())
                //                            {
                //                                createNewNode = true;
                //                                newNodeLinkPin = FindPin(pinId.Get());
                //                                newLinkPin = nullptr;
                //                                ed::Suspend();
                //                                ImGui::OpenPopup("Create New Node");
                //                                ed::Resume();
                //                            }
                //                        }
            }
            else
                newLinkPin = nullptr;

            ed::EndCreate();

            if (ed::BeginDelete())
            {
                ed::NodeId nodeId = 0;
                while (ed::QueryDeletedNode(&nodeId))
                {
                    if (ed::AcceptDeletedItem())
                    {
                        auto id = std::find_if(node_list_.begin(),
                                               node_list_.end(),
                                               [nodeId](auto &node) { return node.id_ == nodeId.Get(); });
                        if (id != node_list_.end())
                            node_list_.erase(id);
                    }
                }

                ed::LinkId linkId = 0;
                while (ed::QueryDeletedLink(&linkId))
                {
                    if (ed::AcceptDeletedItem())
                    {
                        auto id = std::find_if(link_list_.begin(),
                                               link_list_.end(),
                                               [linkId](auto &link) { return link.id_ == linkId.Get(); });
                        if (id != link_list_.end())
                            link_list_.erase(id);
                    }
                }
            }
            ed::EndDelete();
        }
    }

    ed::End();

    //* process dragdrop
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("_BLOCK_LIBRARY"))
        {
            //* 获取鼠标光标相对于当前窗口的位置
            auto drop_abs_position = ImGui::GetMousePos();
            ImVec2 new_node_pos = ed::ScreenToCanvas(drop_abs_position);

            char *p_block_name = (char *)payload->Data;

            //* create new node
            TNode *p_node = create_node(p_block_name);
            if (p_node != nullptr)
            {
                p_node->page_id_ = page_id;
                ed::SetNodePosition(p_node->id_, new_node_pos);
            }
        }
        ImGui::EndDragDropTarget();
    }
}

void TBluePrint::show_tab_control(float delta_time)
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    ImGui::BeginChild("Editor", ImVec2(0, 0), true, window_flags);

    auto &io = ImGui::GetIO();

    {
        //* tools bar
        {
            if (ImGui::Button("Zoom to Content"))
            {
                ed::NavigateToContent();
            }
            ImGui::SameLine();
            if (ImGui::Button("Save"))
            {
                save("test.txt");
            }
            ImGui::SameLine();
            if (ImGui::Button("Load"))
            {
                load("test.txt");
            }
            ImGui::SameLine();
            ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);
        }

        ImGui::Separator();

        //* tab
        {
            int32_t show_leading_button = false;
            int32_t show_trailing_button = true;
            // Expose some other flags which are useful to showcase how they interact with Leading/Trailing tabs
            static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_AutoSelectNewTabs |
                                                    ImGuiTabBarFlags_Reorderable |
                                                    ImGuiTabBarFlags_FittingPolicyResizeDown;
            // ImGui::CheckboxFlags("ImGuiTabBarFlags_TabListPopupButton", &tab_bar_flags, ImGuiTabBarFlags_TabListPopupButton);
            tab_bar_flags &= ~(ImGuiTabBarFlags_FittingPolicyMask_ ^ ImGuiTabBarFlags_FittingPolicyResizeDown);
            tab_bar_flags &= ~(ImGuiTabBarFlags_FittingPolicyMask_ ^ ImGuiTabBarFlags_FittingPolicyResizeDown);
            tab_bar_flags &= ~(ImGuiTabBarFlags_FittingPolicyMask_ ^ ImGuiTabBarFlags_FittingPolicyScroll);

            if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                // Demo a Leading TabItemButton(): click the "?" button to open a menu
                if (show_leading_button)
                    if (ImGui::TabItemButton("?", ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip))
                        ImGui::OpenPopup("MyHelpMenu");
                if (ImGui::BeginPopup("MyHelpMenu"))
                {
                    ImGui::Selectable("Hello!");
                    ImGui::EndPopup();
                }

                //* get next tab id
                int32_t next_tab_id_ = 1;
                if (tab_list_.size() == 0)
                {
                    next_tab_id_ = 1;
                }
                else
                {
                    auto it_last = tab_list_.rbegin();
                    if (it_last != tab_list_.rend())
                    {
                        next_tab_id_ = it_last->first + 1;
                    }
                }
                // Demo Trailing Tabs: click the "+" button to add a new tab.
                // (In your app you may want to use a font icon instead of the "+")
                // We submit it before the regular tabs, but thanks to the ImGuiTabItemFlags_Trailing flag it will always appear at the end.
                if (show_trailing_button)
                {
                    if (ImGui::TabItemButton("+",
                                             ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
                    {
                        TAB_INFO temp_tab_info = {""};
                        sprintf(temp_tab_info.name_, "model %d", next_tab_id_);
                        tab_list_[next_tab_id_] = temp_tab_info;
                        next_tab_id_++;
                    }
                }

                //* add default tab
                if (tab_list_.size() == 0)
                {
                    TAB_INFO temp_tab_info;
                    sprintf(temp_tab_info.name_, "model %d", next_tab_id_);
                    tab_list_[next_tab_id_] = temp_tab_info;
                    next_tab_id_++;
                }

                // Submit our regular tabs
                std::vector<int32_t> remove_tab_list;
                for (auto &it_tab_info : tab_list_)
                {
                    bool open = true;
                    char temp_tab_caption[64];
                    sprintf(temp_tab_caption, "%s###Tab%d", it_tab_info.second.name_, it_tab_info.first);
                    if (ImGui::BeginTabItem(temp_tab_caption, &open, ImGuiTabItemFlags_None))
                    {
                        //* context item, rename the tab name
                        if (ImGui::BeginPopupContextItem())
                        {
                            ImGui::Text("Edit name:");
                            ImGui::InputText("##edit", it_tab_info.second.name_, MAX_SIZE_OF_MODEL_NAME);
                            if (ImGui::Button("Close"))
                            {
                                ImGui::CloseCurrentPopup();
                            }
                            ImGui::EndPopup();
                        }

                        // ImGui::Text("This is the %s tab!", it_tab_info.second.name_);
                        show_editor(delta_time, it_tab_info.first);

                        ImGui::EndTabItem();
                    }

                    if (!open)
                    {
                        remove_tab_list.push_back(it_tab_info.first);
                    }
                }
                //*remove tab
                for (auto &it_tab : remove_tab_list)
                {
                    tab_list_.erase(it_tab);
                }

                ImGui::EndTabBar();
            }
        }
    }

    // ImGui::ShowMetricsWindow();

    ImGui::EndChild();
    ImGui::PopStyleVar();
}

TNode *TBluePrint::create_node(const char *p_name)
{
    TNode *p_rt = nullptr;

    TNode *p_lib_node = p_node_lib_->get_node(p_name);
    if (p_lib_node != nullptr)
    {
        node_list_.emplace_back(get_next_id(), p_lib_node->name_.c_str());
        node_list_.back().type_ = p_lib_node->type_;
        for (auto &it_pin : p_lib_node->in_pin_list_)
        {
            node_list_.back().in_pin_list_.emplace_back(get_next_id(),
                                                        it_pin.name_.c_str(),
                                                        it_pin.type_);
        }
        for (auto &it_pin : p_lib_node->out_pin_list_)
        {
            node_list_.back().out_pin_list_.emplace_back(get_next_id(),
                                                         it_pin.name_.c_str(),
                                                         it_pin.type_);
        }
        TNode *p_create_node = &node_list_.back();

        for (auto &it_pin : p_create_node->in_pin_list_)
        {
            it_pin.p_node_ = p_create_node;
            it_pin.dir_ = TPinDir::Input;
        }

        for (auto &it_pin : p_create_node->out_pin_list_)
        {
            it_pin.p_node_ = p_create_node;
            it_pin.dir_ = TPinDir::Output;
        }

        p_rt = p_create_node;
    }
    return p_rt;
}

void TBluePrint::draw_node(TNode &node)
{
    TPin *p_new_link_pin = nullptr;

    ed::PushStyleVar(ed::StyleVar::StyleVar_NodePadding, ImVec4(8, 4, 8, 8));
    {
        ed::BeginNode(node.id_);
        ImGui::PushID(node.id_);

        //* draw head
        ImGui::TextUnformatted(node.name_.c_str());
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(0, 28));

        //* draw input
        {
            ImGui::BeginGroup();
            for (auto &input : node.in_pin_list_)
            {
                auto alpha = ImGui::GetStyle().Alpha;
                if (p_new_link_pin &&
                    !CanCreateLink(p_new_link_pin, &input) &&
                    &input != p_new_link_pin)
                {
                    alpha = alpha * (48.0f / 255.0f);
                }
                ed::PushStyleVar(ed::StyleVar_PivotAlignment, ImVec2(0, 0.5f));
                ed::PushStyleVar(ed::StyleVar_PivotSize, ImVec2(0, 0));
                {
                    ed::BeginPin(input.id_, ax::NodeEditor::PinKind::Input);

                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                    DrawPinIcon(input, IsPinLinked(input.id_), (int)(alpha * 255));
                    if (!input.name_.empty())
                    {
                        ImGui::SameLine();
                        ImGui::TextUnformatted(input.name_.c_str());
                    }
                    ImGui::PopStyleVar();
                    ed::EndPin();
                }
                ed::PopStyleVar(2);
            }
            ImGui::EndGroup();
        }

        ImGui::SameLine();
        ImGui::BeginGroup();
        if (0)
        {
            // ImGui::Spring(1, 0);
            ImGui::TextUnformatted(node.name_.c_str());
            // ImGui::Spring(1, 0);
        }
        ImGui::EndGroup();

        ImGui::SameLine();
        //* draw output
        {
            ImGui::BeginGroup();
            float group_width = 120; //ImGui::GetContentRegionAvail().x;
            for (auto &output : node.out_pin_list_)
            {
                auto alpha = ImGui::GetStyle().Alpha;
                if (p_new_link_pin &&
                    !CanCreateLink(p_new_link_pin, &output) &&
                    &output != p_new_link_pin)
                {
                    alpha = alpha * (48.0f / 255.0f);
                }

                ed::PushStyleVar(ed::StyleVar_PivotAlignment, ImVec2(1.0f, 0.5f));
                ed::PushStyleVar(ed::StyleVar_PivotSize, ImVec2(0, 0));
                {
                    ed::BeginPin(output.id_, ax::NodeEditor::PinKind::Output);
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

                    //* calc name size
                    float name_size = ImGui::CalcTextSize(output.name_.c_str()).x;
                    ImGui::Dummy(ImVec2(group_width - name_size - size_of_pin_icon_, 0));
                    if (!output.name_.empty())
                    {
                        // ImGui::Spring(0);
                        ImGui::SameLine();
                        ImGui::TextUnformatted(output.name_.c_str());
                    }
                    // ImGui::Spring(0);
                    ImGui::SameLine();
                    DrawPinIcon(output, IsPinLinked(output.id_), (int)(alpha * 255));
                    ImGui::PopStyleVar();
                    ed::EndPin();
                }
                ed::PopStyleVar(2);
            }
            ImGui::EndGroup();
        }

        ed::EndNode();
    }
    ImGui::PopID();
    ed::PopStyleVar();
}

int32_t TBluePrint::CanCreateLink(TPin *a, TPin *b)
{
    int32_t rt = false;
    if (!a ||
        !b ||
        a == b ||
        a->dir_ == b->dir_ ||
        a->type_ != b->type_ ||
        a->p_node_ == b->p_node_)
    {
        rt = false;
    }
    else
    {
        rt = true;
    }

    return rt;
}

ImColor TBluePrint::GetIconColor(TPinType type)
{
    switch (type)
    {
        default:
        case TPinType::FLOW:
            return ImColor(255, 255, 255);
        case TPinType::VALUE_INT:
            return ImColor(220, 48, 48);
        case TPinType::VALUE_FLOAT:
            return ImColor(68, 201, 156);
        case TPinType::VALUE_INT64:
            return ImColor(147, 226, 74);
        case TPinType::CURVE_FLOAT:
            return ImColor(124, 21, 153);
        // case PinType::Object:
        //     return ImColor(51, 150, 215);
        // case PinType::Function:
        //     return ImColor(218, 0, 183);
        case TPinType::EVENT:
            return ImColor(255, 48, 48);
    }
};

void TBluePrint::DrawPinIcon(const TPin &pin,
                             bool connected,
                             int alpha)
{
    ax::Drawing::IconType iconType;
    ImColor color = GetIconColor(pin.type_);
    color.Value.w = alpha / 255.0f;
    switch (pin.type_)
    {
        case TPinType::FLOW:
            iconType = ax::Drawing::IconType::Flow;
            break;
        case TPinType::VALUE_INT:
        case TPinType::VALUE_FLOAT:
        case TPinType::VALUE_INT64:
        case TPinType::CURVE_FLOAT:
            iconType = ax::Drawing::IconType::Circle;
            break;
        case TPinType::EVENT:
            iconType = ax::Drawing::IconType::Square;
            break;
        default:
            return;
    }

    ax::Widgets::Icon(ImVec2(static_cast<float>(size_of_pin_icon_), static_cast<float>(size_of_pin_icon_)),
                      iconType,
                      connected,
                      color,
                      ImColor(32, 32, 32, alpha));
};

int32_t TBluePrint::IsPinLinked(int32_t id)
{
    int32_t rt = false;
    if (id == 0)
    {
        rt = false;
    }

    for (auto &link : link_list_)
    {
        if (link.begin_pin_id_ == id || link.end_pin_id_ == id)
        {
            rt = true;
            break;
        }
    }

    return rt;
}

TPin *TBluePrint::FindPin(int32_t id)
{
    if (!id)
        return nullptr;

    for (auto &node : node_list_)
    {
        for (auto &pin : node.in_pin_list_)
            if (pin.id_ == id)
                return &pin;

        for (auto &pin : node.out_pin_list_)
            if (pin.id_ == id)
                return &pin;
    }

    return nullptr;
}

TNode *TBluePrint::add_node(const char *p_name,
                            TNodeType type,
                            ImColor color)
{
    TNode *p_node = nullptr;
    if (p_node_lib_.get() != nullptr)
    {
        p_node = p_node_lib_->add_node(p_name, type, color);
    }
    return p_node;
}

void TBluePrint::set_window_size(int32_t x, int32_t y, int32_t width, int32_t height)
{
    m_Platform->set_window_size(x, y, width, height);
}

int32_t TBluePrint::save(const char *p_filename)
{
    int32_t rt = false;

    TiXmlElement *p_element_root = NULL;
    TiXmlElement *p_element_temp = NULL;
    TiXmlElement *p_element_model = NULL;
    TiXmlElement *p_element_node_list = NULL;
    TiXmlElement *p_element_node = NULL;
    TiXmlElement *p_element_pin = NULL;
    TiXmlElement *p_element_link_list = NULL;
    TiXmlElement *p_element_link = NULL;
    TiXmlElement *p_element_page_list = NULL;
    TiXmlElement *p_element_page = NULL;

    TiXmlDocument doc;
    TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "utf-8", "");
    doc.LinkEndChild(decl);

    p_element_root = new TiXmlElement("AnalysisModel");

    //* save page info
    p_element_page_list = new TiXmlElement("PageList");
    for (auto &it_tab : tab_list_)
    {
        p_element_page = new TiXmlElement("Page");
        set_attribute_data(p_element_page, "ID", "Value", it_tab.first);
        set_attribute_data(p_element_page, "Name", "Value", it_tab.second.name_);
        p_element_page_list->LinkEndChild(p_element_page);
    }
    p_element_root->LinkEndChild(p_element_page_list);

    //* save node_list_
    p_element_node_list = new TiXmlElement("NodeList");
    for (auto &it_node : node_list_)
    {
        p_element_node = new TiXmlElement("Node");
        set_attribute_data(p_element_node, "ID", "Value", it_node.id_);
        set_attribute_data(p_element_node, "Name", "Value", (char *)it_node.name_.c_str());
        set_attribute_data(p_element_node, "PageName", "Value", tab_list_[it_node.page_id_].name_);
        ImVec2 pos = ed::GetNodePosition(it_node.id_);
        set_attribute_data(p_element_node, "Left", "Value", pos.x);
        set_attribute_data(p_element_node, "Top", "Value", pos.y);
        //* save input
        for (auto &it_pin : it_node.in_pin_list_)
        {
            p_element_pin = new TiXmlElement("Input");
            set_attribute_data(p_element_pin, "ID", it_pin.id_);
            set_attribute_data(p_element_pin, "Name", (char *)it_pin.name_.c_str());
            p_element_node->LinkEndChild(p_element_pin);
        }
        //* save output
        for (auto &it_pin : it_node.out_pin_list_)
        {
            p_element_pin = new TiXmlElement("Output");
            set_attribute_data(p_element_pin, "ID", it_pin.id_);
            set_attribute_data(p_element_pin, "Name", (char *)it_pin.name_.c_str());
            p_element_node->LinkEndChild(p_element_pin);
        }
        p_element_node_list->LinkEndChild(p_element_node);
    }
    p_element_root->LinkEndChild(p_element_node_list);

    //* save link_list_
    p_element_link_list = new TiXmlElement("LinkList");
    for (auto &it_link : link_list_)
    {
        p_element_link = new TiXmlElement("Link");
        set_attribute_data(p_element_link, "ID", "Value", it_link.id_);
        set_attribute_data(p_element_link, "BeginPin", "Value", it_link.begin_pin_id_);
        set_attribute_data(p_element_link, "EndPin", "Value", it_link.end_pin_id_);
        set_attribute_data(p_element_link, "R", "Value", it_link.color_.Value.x);
        set_attribute_data(p_element_link, "G", "Value", it_link.color_.Value.y);
        set_attribute_data(p_element_link, "B", "Value", it_link.color_.Value.z);
        set_attribute_data(p_element_link, "Width", "Value", it_link.width_);
        p_element_link_list->LinkEndChild(p_element_link);
    }
    p_element_root->LinkEndChild(p_element_link_list);

    doc.LinkEndChild(p_element_root);
    doc.SaveFile(p_filename);
}

int32_t TBluePrint::load(const char *p_filename)
{
    int32_t rt = true;
    TiXmlDocument doc;
    TiXmlElement *p_element_temp = NULL;
    TiXmlElement *p_root = NULL;

    TiXmlNode *p_model_node = NULL;
    TiXmlNode *p_node_list_node = NULL;
    TiXmlNode *p_node = NULL;
    TiXmlNode *p_input_node = NULL;
    TiXmlNode *p_output_node = NULL;
    TiXmlNode *p_link_list_node = NULL;
    TiXmlNode *p_link = NULL;
    TiXmlNode *p_page_list_node = NULL;
    TiXmlNode *p_page = NULL;

    int32_t max_id = 0;

    rt = doc.LoadFile(p_filename, TIXML_ENCODING_LEGACY);
    p_root = doc.RootElement();
    if (p_root != NULL)
    {
        clear();

        //* read pages
        int32_t max_page_id = 0;
        p_page_list_node = p_root->FirstChild("PageList");
        for (p_page = p_page_list_node->FirstChild("Page"); p_page != NULL;)
        {
            int32_t page_id = 0;
            char temp_name[_MAX_PATH] = {0};

            get_child_data(p_page,
                           "ID",
                           "Value",
                           &page_id);
            get_child_data(p_page,
                           "Name",
                           "Value",
                           temp_name,
                           _MAX_PATH);
            TAB_INFO temp_tab_info;
            strcpy(temp_tab_info.name_, temp_name);
            tab_list_[page_id] = temp_tab_info;
            if (max_page_id < page_id)
            {
                max_page_id = page_id;
            }

            p_page = p_page->NextSibling("Page");
        }
        next_tab_id_ = max_page_id + 1;

        //* read node
        p_node_list_node = p_root->FirstChild("NodeList");
        for (p_node = p_node_list_node->FirstChild("Node"); p_node != NULL;)
        {
            int32_t node_id = 0;
            int32_t left = 0;
            int32_t top = 0;
            char temp_name[_MAX_PATH] = {0};
            char temp_page_name[_MAX_PATH] = {0};

            get_child_data(p_node,
                           "ID",
                           "Value",
                           &node_id);
            get_child_data(p_node,
                           "Name",
                           "Value",
                           temp_name,
                           _MAX_PATH);
            get_child_data(p_node,
                           "PageName",
                           "Value",
                           temp_page_name,
                           _MAX_PATH);
            get_child_data(p_node,
                           "Left",
                           "Value",
                           &left);
            get_child_data(p_node,
                           "Top",
                           "Value",
                           &top);
            TNode *p_temp_node = create_node(temp_name);
            if (p_temp_node != NULL)
            {
                p_temp_node->set_id(node_id);
                if (node_id > max_id)
                {
                    max_id = node_id;
                }

                //* get page id
                p_temp_node->page_id_ = get_page_id(temp_page_name);

                for (p_input_node = p_node->FirstChild("Input"); p_input_node != NULL;)
                {
                    int32_t pin_id = 0;
                    get_attribute_data(p_input_node,
                                       "ID",
                                       &pin_id);
                    get_attribute_data(p_input_node,
                                       "Name",
                                       temp_name,
                                       _MAX_PATH);

                    p_temp_node->set_in_pin_id(temp_name, pin_id);
                    if (pin_id > max_id)
                    {
                        max_id = pin_id;
                    }

                    p_input_node = p_input_node->NextSibling("Input");
                }

                for (p_output_node = p_node->FirstChild("Output"); p_output_node != NULL;)
                {
                    int32_t pin_id = 0;
                    get_attribute_data(p_output_node,
                                       "ID",
                                       &pin_id);
                    get_attribute_data(p_output_node,
                                       "Name",
                                       temp_name,
                                       _MAX_PATH);
                    p_temp_node->set_out_pin_id(temp_name, pin_id);
                    if (pin_id > max_id)
                    {
                        max_id = pin_id;
                    }

                    p_output_node = p_output_node->NextSibling("Output");
                }

                // node_list_.push_back(temp_node);

                ed::SetNodePosition(p_temp_node->id_,
                                    ImVec2(left, top));
            }
            else
            {
            }

            p_node = p_node->NextSibling("Node");
        }

        //* read link
        p_link_list_node = p_root->FirstChild("LinkList");
        for (p_link = p_link_list_node->FirstChild("Link"); p_link != NULL;)
        {
            int32_t link_id = 0;
            int32_t begin_pin_id = 0;
            int32_t end_pin_id = 0;
            float color_r = 0;
            float color_g = 0;
            float color_b = 0;
            float width = 0;
            get_child_data(p_link,
                           "ID",
                           "Value",
                           &link_id);
            get_child_data(p_link,
                           "BeginPin",
                           "Value",
                           &begin_pin_id);
            get_child_data(p_link,
                           "EndPin",
                           "Value",
                           &end_pin_id);
            get_child_data(p_link,
                           "R",
                           "Value",
                           &color_r);
            get_child_data(p_link,
                           "G",
                           "Value",
                           &color_g);
            get_child_data(p_link,
                           "B",
                           "Value",
                           &color_b);
            get_child_data(p_link,
                           "Width",
                           "Value",
                           &width);
            bp::TLink temp_link = bp::TLink(link_id,
                                            begin_pin_id,
                                            end_pin_id);
            temp_link.width_ = width;
            temp_link.color_ = ImColor(color_r, color_g, color_b);

            if (link_id > max_id)
            {
                max_id = link_id;
            }

            link_list_.push_back(temp_link);

            p_link = p_link->NextSibling("Link");
        }

        next_id_ = max_id + 1;
    }
}
void TBluePrint::clear(void)
{
    node_list_.clear();
    link_list_.clear();
}

int32_t TBluePrint::get_page_id(char *p_page_name)
{
    int32_t rt = 0;
    for (auto &it_tab : tab_list_)
    {
        if (strcmp(p_page_name, it_tab.second.name_) == 0)
        {
            rt = it_tab.first;
            break;
        }
    }
    return rt;
}

} // namespace bp