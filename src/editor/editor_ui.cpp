#include "pch.h"
#include "editor_common.h"

using namespace Asura;

extern int window_width;
extern int window_height;

bool editor_is_static = true;
bool editor_is_box = true;

void Editor::UI::Destroy()
{
    ImGui::SetNextWindowPos({ static_cast<float>(window_width - 400), 0 });
    ImGui::SetNextWindowSize({ 400, (float)window_height });

    if (ImGui::Begin("Create object tools", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse)) {
        ImGui::Checkbox("Static object", &editor_is_static);
        ImGui::Checkbox("Box object", &editor_is_box);

        ImGui::End();
    }
}