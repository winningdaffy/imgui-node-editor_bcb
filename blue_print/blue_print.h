#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_node_editor.h>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <memory>

struct Platform;
struct Renderer;

namespace bp
{
enum class TPinType
{
    FLOW,
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_INT64,
    CURVE_FLOAT,
    EVENT, //* Delegate
};

enum class TPinDir
{
    Output,
    Input
};

enum class TNodeType
{
    PORT,
    Blueprint,
    Simple,
    Tree,
    Comment,
    Houdini
};

class TNode;
class TPin
{
private:
public:
    int32_t id_ = 0;
    std::string name_;
    std::string value_;

    TPinType type_ = TPinType::VALUE_FLOAT;
    TPinDir dir_ = TPinDir::Input;

    TNode *p_node_ = nullptr;

    TPin(int id, const char *name, TPinType type)
        : id_(id), p_node_(nullptr), name_(name), type_(type), dir_(TPinDir::Input)
    {
    }
    ~TPin()
    {
    }
};

class TNode
{
private:
public:
    int32_t id_ = 0;
    std::string name_;
    std::vector<TPin> in_pin_list_;
    std::vector<TPin> out_pin_list_;
    ImColor color_;
    TNodeType type_ = TNodeType::Blueprint;
    ImVec2 size_ = ImVec2(50, 50);

    std::string State;
    std::string SavedState;

    int32_t page_id_ = 0;

    TNode(int32_t id, const char *name, ImColor color = ImColor(255, 255, 255))
        : id_(id), name_(name), color_(color), type_(TNodeType::Blueprint), size_(0, 0)
    {
    }
    ~TNode()
    {
    }
    void set_type(TNodeType type);
    void set_id(int32_t id);
    void add_in_pin(const char *p_name, TPinType type);
    void add_out_pin(const char *p_name, TPinType type);
    void set_in_pin_id(const char *p_name, int32_t id);
    void set_out_pin_id(const char *p_name, int32_t id);
};

class TLink
{
private:
public:
    int32_t id_ = 0;

    int32_t begin_pin_id_ = 0;
    int32_t end_pin_id_ = 0;

    ImColor color_;
    float width_;

    TLink(int32_t id, int32_t begin_id, int32_t end_id)
        : id_(id), begin_pin_id_(begin_id), end_pin_id_(end_id), color_(255, 255, 255)
    {
    }
    ~TLink()
    {
    }
};

class TNodeLib
{
private:
    std::map<std::string, TNode *> node_list_;

public:
    TNodeLib();
    ~TNodeLib();

    const std::map<std::string, TNode *> &get_node_list() const
    {
        return node_list_;
    }
    TNode *add_node(const char *p_name,
                    TNodeType type,
                    ImColor color);
    TNode *get_node(const char *p_name);
};

struct NodeIdLess
{
    bool operator()(const ax::NodeEditor::NodeId &lhs, const ax::NodeEditor::NodeId &rhs) const
    {
        return lhs.AsPointer() < rhs.AsPointer();
    }
};

#define MAX_SIZE_OF_MODEL_NAME 64
typedef struct
{
    char name_[MAX_SIZE_OF_MODEL_NAME];
} TAB_INFO;

class TBluePrint
{
private:
    std::unique_ptr<TNodeLib> p_node_lib_;

    int next_id_ = 1;
    const int size_of_pin_icon_ = 24;
    std::vector<TNode> node_list_;
    std::vector<TLink> link_list_;
    ImTextureID header_background_ = nullptr;
    ImTextureID save_icon_ = nullptr;
    ImTextureID restore_icon_ = nullptr;
    const float touch_time_ = 1.0f;
    std::map<int32_t, float, NodeIdLess> node_touch_time_;
    bool m_ShowOrdinals = false;

    ax::NodeEditor::Config config_;
    ax::NodeEditor::EditorContext *editor_context_ = nullptr;

    std::string m_Name;
    std::string m_IniFilename;
    std::unique_ptr<Platform> m_Platform;
    std::unique_ptr<Renderer> m_Renderer;
    ImGuiContext *m_Context = nullptr;
    ImFont *m_DefaultFont = nullptr;
    ImFont *m_HeaderFont = nullptr;

    char drag_drop_block_name_[_MAX_PATH] = {0};
    int32_t createNewNode = false;

    TPin *newNodeLinkPin = nullptr;
    TPin *newLinkPin = nullptr;

    float default_link_width_ = 2;

    //* tab related
    std::map<int, TAB_INFO> tab_list_;
    int next_tab_id_ = 0;

    void RecreateFontAtlas();

    void Frame();

public:
    TBluePrint(const char *name);
    ~TBluePrint();

    //* init relate
    bool Create(int32_t is_child_wnd = false, void *parent_handle = NULL, int width = -1, int height = -1);
    int Run();
    bool Close();
    void Quit();
    virtual bool CanClose()
    {
        return true;
    }

    void SetTitle(const char *title);
    const std::string &GetName() const;
    virtual ImGuiWindowFlags GetWindowFlags() const;

    ImFont *DefaultFont() const;
    ImFont *HeaderFont() const;

    ImTextureID LoadTexture(const char *path);
    ImTextureID CreateTexture(const void *data, int width, int height);
    void DestroyTexture(ImTextureID texture);
    int GetTextureWidth(ImTextureID texture);
    int GetTextureHeight(ImTextureID texture);

    //* virtual realte
    virtual void OnStart()
    {
    }
    virtual void OnStop()
    {
    }
    virtual void OnFrame(float deltaTime)
    {
    }

    //* block library relate
    TNode *add_node(const char *p_name,
                    TNodeType type = TNodeType::Blueprint,
                    ImColor color = ImColor(255, 255, 255));

    //* display relate
    int32_t get_next_id();
    void show_controls(void);
    void show_left_panel(float panel_width);
    void show_tab_control(float delta_time);
    void show_editor(float delta_time, int32_t page_id);
    bool Splitter(bool split_vertically,
                  float thickness,
                  float *size1,
                  float *size2,
                  float min_size1,
                  float min_size2,
                  float splitter_long_axis_size = -1.0f);
    TNode *create_node(const char *p_name);
    void draw_node(TNode &node);
    int32_t CanCreateLink(TPin *a, TPin *b);
    int32_t IsPinLinked(int32_t id);
    void DrawPinIcon(const TPin &pin,
                     bool connected,
                     int alpha);
    ImColor GetIconColor(TPinType type);
    TPin *FindPin(int32_t id);
    void set_window_size(int32_t x, int32_t y, int32_t width, int32_t height);

    //* save and load
    int32_t save(const char *p_filename);
    int32_t load(const char *p_filename);

    //* node and link relate
    void clear(void);

    //* page relate
    int32_t get_page_id(char *p_page_name);
};
} // namespace bp