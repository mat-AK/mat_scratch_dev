//
// Created by IRAN Poshtiban(NX1) on 2/19/2026.
//

#ifndef SP_VARIABLES_VARIABLES_H
#define SP_VARIABLES_VARIABLES_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cstdio>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

using namespace std;

enum class ValueType{NUMBER,STRING,BOOLEAN2,UNDEFINED};

struct Value{
    ValueType type;
    union {
        double num;
        bool boolean;
    };
    string str;

//     Constructors
    Value() : type(ValueType::UNDEFINED) {}

    Value(double d) : type(ValueType::NUMBER)
    {num = d;}

    Value(int i) : type(ValueType::NUMBER)
    {num = i;}

    Value(const char* s) : type(ValueType::STRING)
    {str = string (s);}

    Value(const string& s) : type(ValueType::STRING)
    {str = s;}

    Value(bool b) : type(ValueType::BOOLEAN2)
    {boolean = b;}

//     Copy constructor
    Value(const Value& other)
    {
        type = other.type;
        if (type == ValueType::NUMBER)
        {num = other.num;}
        else if (type == ValueType::STRING)
        {
            new (&str) string(other.str);
        }
        else if (type == ValueType::BOOLEAN2)
        {
            boolean = other.boolean;
        }
    }

//     Assignment operator
    Value& operator=(const Value &other){
        if (this == &other) return *this;

        if (type == ValueType::STRING)
        {
            str.~basic_string();
        }

        type = other.type;
        if (type == ValueType::NUMBER)
        {num = other.num;}
        else if (type == ValueType::STRING)
        {
            new (&str) string(other.str);
        }
        else if (type == ValueType::BOOLEAN2)
        {boolean = other.boolean;}
        return *this;
    }

//     Destructor
    ~Value()
    {
        if (type == ValueType::STRING)
        {str.~basic_string();}
    }

//     Convert to string for display
    string toString() const
    {
        switch (type)
        {
            case ValueType::NUMBER:
            {
                char st_buffer[64];
                snprintf(st_buffer, sizeof(st_buffer), "%g", num);
                return {st_buffer};
            }
            case ValueType::STRING:
            {return str;}
            case ValueType::BOOLEAN2:
            {return boolean ? "true" : "false";}
            default:
            {return "undefined";}
        }
    }

    double toNumber() const
    {
        switch (type)
        {
            case ValueType::NUMBER:
            {return num;}
            case ValueType::STRING:
            {return stod(str);}
            case ValueType::BOOLEAN2:
            {return boolean ? 1.000 : 0.000;}
            default:
            {return 0.000;}
        }
    }

    bool toBoolean() const
    {
        switch (type)
        {
            case ValueType::BOOLEAN2:
                return boolean;
            case ValueType::NUMBER:
                return num != 0;
            case ValueType::STRING:
                return !str.empty() && str != "false" && str != "0";
            default:
                return false;
        }
    }
};

enum class VariableBlockType { SET,CHANGE,SHOW,HIDE,GET };

struct VariableBlock {
    VariableBlockType type;
    string varName;
    Value value;
    int blockId;
    SDL_Rect rect;
    bool isSelected;

    VariableBlock() : blockId(0), isSelected(false)
    { rect = {0, 0, 0, 0}; }

    VariableBlock(VariableBlockType t, const string& name, const Value& val = Value(), int id = 0)
            : type(t), varName(name), value(val), blockId(id), isSelected(false)
            { rect = {0, 0, 0, 0}; }
};

// Variable definition
struct Variable {
    string name;
    Value value;
    bool isGlobal;
    SDL_Rect displayRect;
    bool isVisible;

    Variable() : name(""), value(Value()), isGlobal(true), isVisible(true)
    {displayRect = {0, 0, 0, 0};}

    Variable(const string &n, const Value &v, bool global = true)
            : name(n), value(v), isGlobal(global), isVisible(true)
            {displayRect = {0, 0, 0, 0};}
};

// Variable manager
struct VariableManager {
    unordered_map<string, Variable> variables;
    vector<VariableBlock> variableBlocks;

    int nextBlockId;
    bool showVariablesPanel;
    bool showBlocksPanel;
    bool showCreateDialog;
    bool showTableView;

    SDL_Rect panelRect;
    SDL_Rect blocksPanelRect;
    SDL_Rect tableViewRect;
    SDL_Rect createDialogRect;

    SDL_Color panelBGColor;
    SDL_Color titleBarColor;
    SDL_Color textColor;
    SDL_Color valueColor;
    SDL_Color blockColor;
    SDL_Color blockSelectedColor;
    SDL_Color buttonColor;
    SDL_Color buttonHoverColor;

    int lineHeight;
    TTF_Font* font;
    TTF_Font* titleFont;

    bool isDragging;
    int dragOffsetX;
    int dragOffsetY;

    string newVarName;
    string newVarValue;
    int selectedValueType;  // 0=number, 1=string, 2=boolean //
    bool createError;
    string errorMessage;

    VariableManager()
            : nextBlockId(0),
              showVariablesPanel(false),
              showBlocksPanel(true),
              showCreateDialog(false),
              showTableView(true),
              panelBGColor({40, 40, 40, 220}),
              titleBarColor({60, 60, 60, 255}),
              textColor({255, 255, 255, 255}),
              valueColor({100, 200, 255, 255}),
              blockColor({80, 120, 200, 255}),
              blockSelectedColor({120, 160, 240, 255}),
              buttonColor({70, 130, 70, 255}),
              buttonHoverColor({90, 170, 90, 255}),
              lineHeight(25),
              font(nullptr),
              titleFont(nullptr),
              isDragging(false),
              dragOffsetX(0),
              dragOffsetY(0),
              selectedValueType(0),
              createError(false),
              errorMessage("")
              {
                panelRect = {500, 10, 280, 400};
                blocksPanelRect = {10, 60, 250, 300};
                tableViewRect = {10, 370, 250, 200};
                createDialogRect = {250, 150, 300, 250};
                newVarName = "";
                newVarValue = "";
              }
};

// Function declarations
void var_init(VariableManager& vm);
bool var_create(VariableManager& vm, const string& name, const Value& initialValue);
bool var_exists(const VariableManager& vm, const string& name);
Value var_get(const VariableManager& vm, const string& name);
bool var_set(VariableManager& vm, const string& name, const Value& newValue);
bool var_delete(VariableManager& vm, const string& name);
vector<string> var_get_all_names(const VariableManager& vm);
void var_clear_all(VariableManager& vm);

int var_create_block(VariableManager& vm, VariableBlockType type, const std::string& varName, const Value& val = Value());
void var_delete_block(VariableManager& vm, int blockId);
void var_update_block_position(VariableManager& vm, int blockId, int x, int y);
VariableBlock* var_get_block_at_position(VariableManager& vm, int x, int y);
void var_execute_block(VariableManager& vm, int blockId);
void var_execute_all_blocks(VariableManager& vm);

//void var_toggle_display(VariableManager& vm);
void var_set_variable_visibility(VariableManager& vm, const string& name, bool visible);
void var_toggle_panel(VariableManager& vm);
void var_toggle_blocks_panel(VariableManager& vm);
void var_toggle_table_view(VariableManager& vm);
void var_show_create_dialog(VariableManager& vm, bool show);

// SDL rendering functions
static void renderText(SDL_Renderer* S_renderer, const string& text,int x, int y, SDL_Color color, TTF_Font* font);
void var_render(SDL_Renderer* S_renderer, VariableManager& vm);
void var_handle_mouse_button(VariableManager& vm, int mouseX, int mouseY, bool buttonDown);
void var_handle_mouse_motion(VariableManager& vm, int mouseX, int mouseY);
void var_handle_mouse_release(VariableManager& vm);
void var_set_fonts(VariableManager& vm, TTF_Font* normal, TTF_Font* title);
void var_handle_keyboard(VariableManager& vm, SDL_Keysym key);
void var_handle_text_input(VariableManager& vm, const char* text);

void var_render_variable(SDL_Renderer* S_renderer, const Variable& var, int x, int y);
void var_handle_click(VariableManager& vm, int mouseX, int mouseY);

// Operators for variables
Value var_add(const Value& a, const Value& b);
Value var_subtract(const Value& a, const Value& b);
Value var_multiply(const Value& a, const Value& b);
Value var_divide(const Value& a, const Value& b);
Value var_equals(const Value& a, const Value& b);
Value var_less_than(const Value& a, const Value& b);
Value var_greater_than(const Value& a, const Value& b);
Value var_not(const Value& a);
Value var_or(const Value& a, const Value& b);
Value var_string_length(const Value& a);
Value var_string_char_at(const Value& str, int index);
Value var_string_concat(const Value& a, const Value& b);

#endif //SP_VARIABLES_VARIABLES_H
