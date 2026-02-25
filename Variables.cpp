//
// Created by IRAN Poshtiban(NX1) on 2/19/2026.
//

#include "variables.h"
#include <iostream>
#include <algorithm>

using namespace std;

void var_init(VariableManager& vm)
{
    vm.variables.clear();
    vm.variableBlocks.clear();
    vm.nextBlockId = 0;
    vm.showVariablesPanel = false;
    vm.showBlocksPanel = true;
    vm.showCreateDialog = false;
    vm.showTableView = true;
    vm.isDragging = false;
    vm.createError = false;
    vm.newVarName = "";
    vm.newVarValue = "";
    vm.selectedValueType = 0;
}

bool var_create(VariableManager& vm, const string& name, const Value& initialValue)
{
    if (name.empty())
        return false;

    if (vm.variables.find(name) != vm.variables.end())
        return false;

    vm.variables.emplace(name,Variable(name, initialValue));
    return true;
}

bool var_exists(const VariableManager& vm, const string& name)
{
    return vm.variables.find(name) != vm.variables.end();
}

Value var_get(const VariableManager& vm, const string& name)
{
    auto it = vm.variables.find(name);
    if (it != vm.variables.end())
    {return it->second.value;}
    return {};
}

bool var_set(VariableManager& vm, const string& name, const Value& newValue)
{
    auto it = vm.variables.find(name);
    if (it != vm.variables.end())
    {
        it->second.value = newValue;
        return true;
    }
    return false;
}

bool var_delete(VariableManager& vm, const string& name)
{
    return vm.variables.erase(name) > 0;
}

vector<string> var_get_all_names(const VariableManager& vm)
{
    vector<string> names;
    for (const auto& pair : vm.variables)
    {names.push_back(pair.first);}
    return names;
}

void var_clear_all(VariableManager& vm)
{ vm.variables.clear(); }

int var_create_block(VariableManager& vm, VariableBlockType type, const string& varName, const Value& val)
{
    if (!var_exists(vm, varName))
        return -1;

    int blockId = vm.nextBlockId++;
    vm.variableBlocks.push_back(VariableBlock(type, varName, val, blockId));

    int index = vm.variableBlocks.size() - 1;
    vm.variableBlocks[index].rect = {50 + (index % 2) * 120, 100 + (index / 2) * 40, 110, 30};

    return blockId;
}

void var_delete_block(VariableManager& vm, int blockId)
{
    auto it = find_if(vm.variableBlocks.begin(), vm.variableBlocks.end(),
                           [blockId](const VariableBlock& block) { return block.blockId == blockId; });

    if (it != vm.variableBlocks.end())
    {
        vm.variableBlocks.erase(it);
    }
}

void var_update_block_position(VariableManager& vm, int blockId, int x, int y)
{
    auto it = find_if(vm.variableBlocks.begin(), vm.variableBlocks.end(),
                           [blockId](const VariableBlock& block) { return block.blockId == blockId; });
    if (it != vm.variableBlocks.end())
    {
        it->rect.x = x;
        it->rect.y = y;
    }
}

VariableBlock* var_get_block_at_position(VariableManager& vm, int x, int y)
{
    for (auto& block : vm.variableBlocks)
    {
        if (x >= block.rect.x && x <= block.rect.x + block.rect.w &&
            y >= block.rect.y && y <= block.rect.y + block.rect.h)
        { return &block; }
    }
    return nullptr;
}

void var_execute_block(VariableManager& vm, int blockId)
{
    auto it = find_if(vm.variableBlocks.begin(), vm.variableBlocks.end(),
                           [blockId](const VariableBlock& block) { return block.blockId == blockId; });

    if (it == vm.variableBlocks.end() || !var_exists(vm, it->varName))
        return;

    switch (it->type)
    {
        case VariableBlockType::SET:
            var_set(vm, it->varName, it->value);
            break;
        case VariableBlockType::CHANGE:
            var_set(vm, it->varName, var_add(var_get(vm, it->varName), it->value));
            break;
        case VariableBlockType::SHOW:
            var_set_variable_visibility(vm, it->varName, true);
            break;
        case VariableBlockType::HIDE:
            var_set_variable_visibility(vm, it->varName, false);
            break;
        case VariableBlockType::GET:
            break;
    }
}

void var_execute_all_blocks(VariableManager& vm)
{
    for (const auto& block : vm.variableBlocks)
    {
        var_execute_block(vm, block.blockId);
    }
}

void var_toggle_panel(VariableManager& vm)
{
    vm.showVariablesPanel = !vm.showVariablesPanel;
    vm.isDragging = false;
}

void var_toggle_blocks_panel(VariableManager& vm)
{
    vm.showBlocksPanel = !vm.showBlocksPanel;
}

void var_toggle_table_view(VariableManager& vm)
{
    vm.showTableView = !vm.showTableView;
}

void var_show_create_dialog(VariableManager& vm, bool show)
{
    vm.showCreateDialog = show;
    if (show)
    {
        vm.newVarName = "";
        vm.newVarValue = "";
        vm.selectedValueType = 0;
        vm.createError = false;
    }
}

void var_set_variable_visibility(VariableManager& vm, const string& name, bool visible)
{
    auto it = vm.variables.find(name);
    if (it != vm.variables.end())
    { it->second.isVisible = visible; }
}

void var_set_fonts(VariableManager& vm, TTF_Font* normal, TTF_Font* title)
{
    vm.font = normal;
    vm.titleFont = title;
}

static void renderText(SDL_Renderer* S_renderer, const string& text,int x, int y, SDL_Color color, TTF_Font* font)
{
    if (!font || text.empty()) return;

    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(S_renderer, surface);
    SDL_Rect dest = {x, y, surface->w, surface->h};
    SDL_RenderCopy(S_renderer, texture, NULL, &dest);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void var_render_variable(SDL_Renderer* S_renderer, const Variable& var, int x, int y)
{
    if (!var.isVisible) return;
}

static void renderButton(SDL_Renderer* S_renderer, SDL_Rect rect, const string& text, SDL_Color bgColor,
                         SDL_Color textColor, TTF_Font* font, bool hover = false)
                         {
    SDL_Color color = hover ? SDL_Color{static_cast<Uint8>(bgColor.r + 20), static_cast<Uint8>(bgColor.g + 20),
                                        static_cast<Uint8>(bgColor.b + 20), bgColor.a} : bgColor;
    SDL_SetRenderDrawColor(S_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(S_renderer, &rect);
    SDL_SetRenderDrawColor(S_renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(S_renderer, &rect);

    int textWidth = text.length() * 10;
    renderText(S_renderer, text, rect.x + (rect.w - textWidth)/2, rect.y + 5, textColor, font);
}

void var_render(SDL_Renderer* S_renderer, VariableManager& vm)
{
    if (!vm.font) return;

    // Render variable blocks
    if (vm.showBlocksPanel)
    {
        SDL_Rect panelBg = {vm.blocksPanelRect.x, vm.blocksPanelRect.y, vm.blocksPanelRect.w, vm.blocksPanelRect.h};
        SDL_SetRenderDrawBlendMode(S_renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(S_renderer, vm.panelBGColor.r, vm.panelBGColor.g, vm.panelBGColor.b, 200);
        SDL_RenderFillRect(S_renderer, &panelBg);

        renderText(S_renderer, "Variable Blocks", vm.blocksPanelRect.x + 10, vm.blocksPanelRect.y + 5,
                   vm.textColor, vm.titleFont ? vm.titleFont : vm.font);

        int yOffset = vm.blocksPanelRect.y + 30;
        renderButton(S_renderer, {vm.blocksPanelRect.x + 10, yOffset, 110, 25}, "SET Block",
                     vm.buttonColor, vm.textColor, vm.font);
        renderButton(S_renderer, {vm.blocksPanelRect.x + 130, yOffset, 110, 25}, "CHANGE Block",
                     vm.buttonColor, vm.textColor, vm.font);

        yOffset += 30;
        renderButton(S_renderer, {vm.blocksPanelRect.x + 10, yOffset, 110, 25}, "SHOW Block",
                     vm.buttonColor, vm.textColor, vm.font);
        renderButton(S_renderer, {vm.blocksPanelRect.x + 130, yOffset, 110, 25}, "HIDE Block",
                     vm.buttonColor, vm.textColor, vm.font);

        yOffset += 30;
        renderButton(S_renderer, {vm.blocksPanelRect.x + 10, yOffset, 110, 25}, "GET Block",
                     vm.buttonColor, vm.textColor, vm.font);
        renderButton(S_renderer, {vm.blocksPanelRect.x + 130, yOffset, 110, 25}, "New Variable",
                     SDL_Color{100, 100, 200, 255}, vm.textColor, vm.font);
    }

    // Render variable table
    if (vm.showTableView && !vm.variables.empty())
    {
        SDL_Rect tableBg = {vm.tableViewRect.x, vm.tableViewRect.y, vm.tableViewRect.w, vm.tableViewRect.h};
        SDL_SetRenderDrawBlendMode(S_renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(S_renderer, vm.panelBGColor.r, vm.panelBGColor.g, vm.panelBGColor.b, 200);
        SDL_RenderFillRect(S_renderer, &tableBg);

        renderText(S_renderer, "Variables Table", vm.tableViewRect.x + 10, vm.tableViewRect.y + 5,
                   vm.textColor, vm.titleFont ? vm.titleFont : vm.font);

        int yOffset = vm.tableViewRect.y + 30;
        vector<pair<string, Variable>> sorted(vm.variables.begin(), vm.variables.end());
        sort(sorted.begin(), sorted.end(),
                  [](const auto& a, const auto& b) { return a.first < b.first; });

        for (const auto& pair : sorted)
        {
            string display = pair.first + ": " + pair.second.value.toString();
            renderText(S_renderer, display, vm.tableViewRect.x + 10, yOffset,
                       pair.second.isVisible ? vm.valueColor : SDL_Color{150, 150, 150, 255}, vm.font);
            yOffset += 20;
            if (yOffset > vm.tableViewRect.y + vm.tableViewRect.h - 20)
                break;
        }
    }

    // Render placed blocks
    for (auto& block : vm.variableBlocks)
    {
        SDL_Color blockColor = block.isSelected ? vm.blockSelectedColor : vm.blockColor;
        SDL_SetRenderDrawColor(S_renderer, blockColor.r, blockColor.g, blockColor.b, 255);
        SDL_RenderFillRect(S_renderer, &block.rect);
        SDL_SetRenderDrawColor(S_renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(S_renderer, &block.rect);

        string blockText;
        switch (block.type)
        {
            case VariableBlockType::SET:
                blockText = "SET " + block.varName + " = " + block.value.toString();
                break;
            case VariableBlockType::CHANGE:
                blockText = "CHANGE " + block.varName + " by " + block.value.toString();
                break;
            case VariableBlockType::SHOW:
                blockText = "SHOW " + block.varName;
                break;
            case VariableBlockType::HIDE:
                blockText = "HIDE " + block.varName;
                break;
            case VariableBlockType::GET:
                blockText = "GET " + block.varName;
                break;
        }
        renderText(S_renderer, blockText, block.rect.x + 5, block.rect.y + 5, vm.textColor, vm.font);
    }

    // Render variables panel
    if (vm.showVariablesPanel && !vm.variables.empty())
    {
        int visibleCount = 0;
        for (const auto& pair : vm.variables)
        {
            if (pair.second.isVisible)
            {visibleCount++;}
        }

        int panelHeight = 40 + visibleCount * vm.lineHeight;
        vm.panelRect.h = panelHeight;

        SDL_SetRenderDrawBlendMode(S_renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(S_renderer, vm.panelBGColor.r, vm.panelBGColor.g,vm.panelBGColor.b, vm.panelBGColor.a);
        SDL_RenderFillRect(S_renderer, &vm.panelRect);

        SDL_Rect titleBarRect = {vm.panelRect.x, vm.panelRect.y, vm.panelRect.w, 30};
        SDL_SetRenderDrawBlendMode(S_renderer, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(S_renderer, vm.titleBarColor.r, vm.titleBarColor.g,vm.titleBarColor.b, vm.titleBarColor.a);
        SDL_RenderFillRect(S_renderer, &titleBarRect);

        SDL_SetRenderDrawColor(S_renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(S_renderer, &vm.panelRect);

        renderText(S_renderer, "Variables (drag here)", vm.panelRect.x + 10, vm.panelRect.y + 5,vm.textColor, vm.font);

        SDL_Rect closeRect = {vm.panelRect.x + vm.panelRect.w - 25, vm.panelRect.y + 5, 20, 20};
        SDL_SetRenderDrawColor(S_renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(S_renderer, &closeRect);
        renderText(S_renderer, "X", closeRect.x + 5, closeRect.y + 2, vm.textColor, vm.font);

        int yOffset = vm.panelRect.y + 35;

        vector<pair<string, Variable>> sorted(vm.variables.begin(), vm.variables.end());
        sort(sorted.begin(), sorted.end(),
                  [](const auto& a, const auto& b) { return a.first < b.first; });

        for (auto& pair : sorted) 
        {
            if (!pair.second.isVisible) continue;

            pair.second.displayRect = {vm.panelRect.x + 5, yOffset - 2,vm.panelRect.w - 10, vm.lineHeight};

            renderText(S_renderer, pair.first + " =", vm.panelRect.x + 10, yOffset,vm.textColor, vm.font);

            int nameWidth = pair.first.length() * 10;
            renderText(S_renderer, pair.second.value.toString(),vm.panelRect.x + 20 + nameWidth, yOffset,
                       vm.valueColor, vm.font);

            yOffset += vm.lineHeight;
        }
    }

    // Render create variable dialog
    if (vm.showCreateDialog) 
    {
        SDL_SetRenderDrawBlendMode(S_renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(S_renderer, 50, 50, 50, 240);
        SDL_RenderFillRect(S_renderer, &vm.createDialogRect);
        SDL_SetRenderDrawColor(S_renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(S_renderer, &vm.createDialogRect);

        renderText(S_renderer, "Create New Variable", vm.createDialogRect.x + 50, vm.createDialogRect.y + 10,
                   vm.textColor, vm.titleFont ? vm.titleFont : vm.font);

        renderText(S_renderer, "Name:", vm.createDialogRect.x + 20, vm.createDialogRect.y + 50, vm.textColor, vm.font);
        SDL_Rect nameRect = {vm.createDialogRect.x + 80, vm.createDialogRect.y + 45, 150, 25};
        SDL_SetRenderDrawColor(S_renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(S_renderer, &nameRect);
        SDL_SetRenderDrawColor(S_renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(S_renderer, &nameRect);
        renderText(S_renderer, vm.newVarName, nameRect.x + 5, nameRect.y + 5, vm.textColor, vm.font);

        renderText(S_renderer, "Value:", vm.createDialogRect.x + 20, vm.createDialogRect.y + 90, vm.textColor, vm.font);
        SDL_Rect valueRect = {vm.createDialogRect.x + 80, vm.createDialogRect.y + 85, 150, 25};
        SDL_SetRenderDrawColor(S_renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(S_renderer, &valueRect);
        SDL_SetRenderDrawColor(S_renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(S_renderer, &valueRect);
        renderText(S_renderer, vm.newVarValue, valueRect.x + 5, valueRect.y + 5, vm.textColor, vm.font);

        renderText(S_renderer, "Type:", vm.createDialogRect.x + 20, vm.createDialogRect.y + 130, vm.textColor, vm.font);
        renderButton(S_renderer, {vm.createDialogRect.x + 80, vm.createDialogRect.y + 125, 60, 25}, "Number",
                     vm.selectedValueType == 0 ? vm.buttonHoverColor : vm.buttonColor, vm.textColor, vm.font);
        renderButton(S_renderer, {vm.createDialogRect.x + 150, vm.createDialogRect.y + 125, 60, 25}, "String",
                     vm.selectedValueType == 1 ? vm.buttonHoverColor : vm.buttonColor, vm.textColor, vm.font);
        renderButton(S_renderer, {vm.createDialogRect.x + 220, vm.createDialogRect.y + 125, 40, 25}, "Bool",
                     vm.selectedValueType == 2 ? vm.buttonHoverColor : vm.buttonColor, vm.textColor, vm.font);

        renderButton(S_renderer, {vm.createDialogRect.x + 50, vm.createDialogRect.y + 180, 80, 30}, "Create",
                     vm.buttonColor, vm.textColor, vm.font);
        renderButton(S_renderer, {vm.createDialogRect.x + 160, vm.createDialogRect.y + 180, 80, 30}, "Cancel",
                     SDL_Color{150, 70, 70, 255}, vm.textColor, vm.font);

        if (vm.createError) 
        {
            renderText(S_renderer, vm.errorMessage, vm.createDialogRect.x + 20, vm.createDialogRect.y + 220,
                       SDL_Color{255, 100, 100, 255}, vm.font);
        }
    }
}

void var_handle_mouse_button(VariableManager& vm, int mouseX, int mouseY, bool buttonDown)
{
    if (buttonDown)
    {
        // Check blocks panel buttons
        if (vm.showBlocksPanel)
        {
            SDL_Rect setBtn = {vm.blocksPanelRect.x + 10, vm.blocksPanelRect.y + 30, 110, 25};
            SDL_Rect changeBtn = {vm.blocksPanelRect.x + 130, vm.blocksPanelRect.y + 30, 110, 25};
            SDL_Rect showBtn = {vm.blocksPanelRect.x + 10, vm.blocksPanelRect.y + 60, 110, 25};
            SDL_Rect hideBtn = {vm.blocksPanelRect.x + 130, vm.blocksPanelRect.y + 60, 110, 25};
            SDL_Rect getBtn = {vm.blocksPanelRect.x + 10, vm.blocksPanelRect.y + 90, 110, 25};
            SDL_Rect newVarBtn = {vm.blocksPanelRect.x + 130, vm.blocksPanelRect.y + 90, 110, 25};

            if (!vm.variables.empty())
            {
                string firstVar = vm.variables.begin()->first;

                if (mouseX >= setBtn.x && mouseX <= setBtn.x + setBtn.w && mouseY >= setBtn.y && mouseY <= setBtn.y + setBtn.h)
                {
                    var_create_block(vm, VariableBlockType::SET, firstVar, Value(0.000));
                }
                else if (mouseX >= changeBtn.x && mouseX <= changeBtn.x + changeBtn.w && mouseY >= changeBtn.y && mouseY <= changeBtn.y + changeBtn.h)
                {
                    var_create_block(vm, VariableBlockType::CHANGE, firstVar, Value(1.000));
                }
                else if (mouseX >= showBtn.x && mouseX <= showBtn.x + showBtn.w && mouseY >= showBtn.y && mouseY <= showBtn.y + showBtn.h)
                {
                    var_create_block(vm, VariableBlockType::SHOW, firstVar);
                }
                else if (mouseX >= hideBtn.x && mouseX <= hideBtn.x + hideBtn.w && mouseY >= hideBtn.y && mouseY <= hideBtn.y + hideBtn.h)
                {
                    var_create_block(vm, VariableBlockType::HIDE, firstVar);
                }
                else if (mouseX >= getBtn.x && mouseX <= getBtn.x + getBtn.w && mouseY >= getBtn.y && mouseY <= getBtn.y + getBtn.h)
                {
                    var_create_block(vm, VariableBlockType::GET, firstVar);
                }
                else if (mouseX >= newVarBtn.x && mouseX <= newVarBtn.x + newVarBtn.w && mouseY >= newVarBtn.y && mouseY <= newVarBtn.y + newVarBtn.h)
                {
                    var_show_create_dialog(vm, true);
                }
            }
        }

        // Check create dialog buttons
        if (vm.showCreateDialog)
        {
            SDL_Rect numBtn = {vm.createDialogRect.x + 80, vm.createDialogRect.y + 125, 60, 25};
            SDL_Rect strBtn = {vm.createDialogRect.x + 150, vm.createDialogRect.y + 125, 60, 25};
            SDL_Rect boolBtn = {vm.createDialogRect.x + 220, vm.createDialogRect.y + 125, 40, 25};
            SDL_Rect createBtn = {vm.createDialogRect.x + 50, vm.createDialogRect.y + 180, 80, 30};
            SDL_Rect cancelBtn = {vm.createDialogRect.x + 160, vm.createDialogRect.y + 180, 80, 30};

            if (mouseX >= numBtn.x && mouseX <= numBtn.x + numBtn.w && mouseY >= numBtn.y && mouseY <= numBtn.y + numBtn.h)
            {
                vm.selectedValueType = 0;
                vm.newVarValue = "";
            }
            else if (mouseX >= strBtn.x && mouseX <= strBtn.x + strBtn.w && mouseY >= strBtn.y && mouseY <= strBtn.y + strBtn.h)
            {
                vm.selectedValueType = 1;
                vm.newVarValue = "";
            }
            else if (mouseX >= boolBtn.x && mouseX <= boolBtn.x + boolBtn.w && mouseY >= boolBtn.y && mouseY <= boolBtn.y + boolBtn.h)
            {
                vm.selectedValueType = 2;
                vm.newVarValue = "true";
            }
            else if (mouseX >= createBtn.x && mouseX <= createBtn.x + createBtn.w && mouseY >= createBtn.y && mouseY <= createBtn.y + createBtn.h)
            {
                vm.createError = false;

                if (vm.newVarName.empty())
                {
                    vm.createError = true;
                    vm.errorMessage = "Name cannot be empty";
                }
                else
                {
                    Value newVal;
                    if (vm.selectedValueType == 0)
                    {
                        try { newVal = Value(stod(vm.newVarValue)); }
                        catch (...) { newVal = Value(0.0); }
                    }
                    else if (vm.selectedValueType == 1)
                    {
                        newVal = Value(vm.newVarValue);
                    }
                    else
                    {
                        newVal = Value(vm.newVarValue == "true");
                    }

                    if (var_create(vm, vm.newVarName, newVal))
                    {
                        vm.showCreateDialog = false;
                    }
                    else
                    {
                        vm.createError = true;
                        vm.errorMessage = "Variable already exists";
                    }
                }
            }
            else if (mouseX >= cancelBtn.x && mouseX <= cancelBtn.x + cancelBtn.w && mouseY >= cancelBtn.y && mouseY <= cancelBtn.y + cancelBtn.h)
            { vm.showCreateDialog = false; }

            SDL_Rect nameRect = {vm.createDialogRect.x + 80, vm.createDialogRect.y + 45, 150, 25};
            if (mouseX >= nameRect.x && mouseX <= nameRect.x + nameRect.w && mouseY >= nameRect.y && mouseY <= nameRect.y + nameRect.h)
            { SDL_StartTextInput(); }

            SDL_Rect valueRect = {vm.createDialogRect.x + 80, vm.createDialogRect.y + 85, 150, 25};
            if (mouseX >= valueRect.x && mouseX <= valueRect.x + valueRect.w && mouseY >= valueRect.y && mouseY <= valueRect.y + valueRect.h)
            { SDL_StartTextInput(); }
        }

        // Check panel title bar for dragging
        if (vm.showVariablesPanel)
        {
            SDL_Rect titleBarRect = {vm.panelRect.x, vm.panelRect.y, vm.panelRect.w, 30};
            if (mouseX >= titleBarRect.x && mouseX <= titleBarRect.x + titleBarRect.w &&
                mouseY >= titleBarRect.y && mouseY <= titleBarRect.y + titleBarRect.h)
            {
                SDL_Rect closeRect = {vm.panelRect.x + vm.panelRect.w - 25, vm.panelRect.y + 5, 20, 20};
                if (mouseX >= closeRect.x && mouseX <= closeRect.x + closeRect.w &&
                    mouseY >= closeRect.y && mouseY <= closeRect.y + closeRect.h)
                {
                    vm.showVariablesPanel = false;
                    return;
                }
                vm.isDragging = true;
                vm.dragOffsetX = mouseX - vm.panelRect.x;
                vm.dragOffsetY = mouseY - vm.panelRect.y;
            }
        }
    }
}

void var_handle_mouse_motion(VariableManager& vm, int mouseX, int mouseY)
{
    if (vm.isDragging)
    {
        vm.panelRect.x = mouseX - vm.dragOffsetX;
        vm.panelRect.y = mouseY - vm.dragOffsetY;
        if (vm.panelRect.x < 0)
        {vm.panelRect.x = 0;}
        if (vm.panelRect.y < 0)
        {vm.panelRect.y = 0;}
    }
}

void var_handle_mouse_release(VariableManager& vm)
{
    vm.isDragging = false;
}

void var_handle_keyboard(VariableManager& vm, SDL_Keysym key) {
    if (vm.showCreateDialog) {
        if (key.sym == SDLK_RETURN)
        {
            // Simulate create button click
            vm.createError = false;

            if (vm.newVarName.empty())
            {
                vm.createError = true;
                vm.errorMessage = "Name cannot be empty";
            }
            else
            {
                Value newVal;
                if (vm.selectedValueType == 0)
                {
                    try { newVal = Value(stod(vm.newVarValue)); }
                    catch (...) { newVal = Value(0.000); }
                }
                else if (vm.selectedValueType == 1)
                {
                    newVal = Value(vm.newVarValue);
                }
                else
                {
                    newVal = Value(vm.newVarValue == "true");
                }

                if (var_create(vm, vm.newVarName, newVal))
                {
                    vm.showCreateDialog = false;
                }
                else
                {
                    vm.createError = true;
                    vm.errorMessage = "Variable already exists";
                }
            }
        }
        else if (key.sym == SDLK_ESCAPE)
        {
            vm.showCreateDialog = false;
        }
    }
}

void var_handle_text_input(VariableManager& vm, const char* text)
{
    if (vm.showCreateDialog)
    {
        // Simple text input handling - you'd need to track which field is active
        // This is a simplified version
        vm.newVarName += text;
    }
}
//|:-)//
void var_handle_click(VariableManager& vm, int mouseX, int mouseY)
{
    if (vm.showVariablesPanel)
    {
        if (mouseX >= vm.panelRect.x && mouseX <= vm.panelRect.x + vm.panelRect.w &&
            mouseY >= vm.panelRect.y && mouseY <= vm.panelRect.y + 30)
        {}
    }
}

// Operator implementations
Value var_add(const Value& a, const Value& b)
{
    if (a.type == ValueType::NUMBER && b.type == ValueType::NUMBER)
    {return {a.num + b.num};}
//    concatenate
    return {a.toString() + b.toString()};
}

Value var_subtract(const Value& a, const Value& b)
{
    return {a.toNumber() - b.toNumber()};
}

Value var_multiply(const Value& a, const Value& b)
{
    return {a.toNumber() * b.toNumber()};
}

Value var_divide(const Value& a, const Value& b)
{
    double denominator = b.toNumber();
    if (denominator == 0)
    {
        cout << "Error: Division by zero" << endl;
        return {0.000};
    }
    return {a.toNumber() / denominator};
}

Value var_equals(const Value& a, const Value& b)
{
    if (a.type == b.type)
    {
        switch (a.type)
        {
            case ValueType::NUMBER:
                return {a.num == b.num};
            case ValueType::STRING:
                return {a.str == b.str};
            case ValueType::BOOLEAN2:
                return {a.boolean == b.boolean};
            default:
                return {false};
        }
    }

    return {a.toString() == b.toString()};
}

Value var_less_than(const Value& a, const Value& b)
{
    if (a.type == ValueType::NUMBER && b.type == ValueType::NUMBER)
    {return {a.num < b.num};}

    return {a.toString() < b.toString()};
}

Value var_greater_than(const Value& a, const Value& b)
{
    if (a.type == ValueType::NUMBER && b.type == ValueType::NUMBER)
    {return {a.num > b.num};}

    return {a.toString() > b.toString()};
}

Value var_not(const Value& a)
{return {!a.toBoolean()};}

Value var_or(const Value& a, const Value& b)
{
    return {a.toBoolean() || b.toBoolean()};
}

Value var_string_length(const Value& a)
{
    return {static_cast<double>(a.toString().length())};
}

Value var_string_char_at(const Value& str, int index)
{
    string s = str.toString();
    if (index >= 1 && index <= static_cast<int>(s.length()))
    {
        return Value(string(1, s[index - 1]));
    }
    return {""};
}

Value var_string_concat(const Value& a, const Value& b)
{
    return Value(a.toString() + b.toString());
}

