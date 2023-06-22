#include "pch.h"

using namespace Asura;
using namespace Asura::UI;

static size_t CurrentID = -1;
static stl::vector<Render::texture_id> Data;

size_t CursorManager::Register(FileSystem::Path File)
{
	FileSystem::Platform::NormalizePath(File);

	auto ResData = ResourcesManager::Load(File);
	auto TextureID = Render::LoadTexture(ResData.Get());
	Data.push_back(TextureID);

	return Data.size() - 1;
}

void CursorManager::SetActive(size_t ID)
{
	CurrentID = ID;
	ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
}

void CursorManager::Tick()
{
	if (CurrentID != size_t(-1))
	{
		Math::FVec2 Pos = ImGui::GetMousePos();
		Math::FVec2 PosEnd = Pos + Math::FVec2{25.f, 25.f};

		ImGui::SetMouseCursor(ImGuiMouseCursor_None);
		ImGui::GetForegroundDrawList()->AddImage(Data[CurrentID], Pos, PosEnd);
	}
}