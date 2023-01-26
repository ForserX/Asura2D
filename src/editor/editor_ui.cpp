#include "pch.h"
#include "editor_common.h"

using namespace Asura;

bool editor_is_static = true;
bool editor_is_box = true;

void Editor::UI::Destroy()
{
    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::SetNextWindowSize({ fwindow_width, 200 });

    if (ImGui::Begin("Toolset", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse))
    {

        ImGui::End();
    }

    ImGui::SetNextWindowPos({ fwindow_width - 400.f, 200 });
    ImGui::SetNextWindowSize({ 400, fwindow_height });

    if (ImGui::Begin("Create object tools", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Checkbox("Static object", &editor_is_static);
        ImGui::Checkbox("Box object", &editor_is_box);

        ImGui::End();
    }
}