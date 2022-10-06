#include "pch.h"
#include <SDL_image/SDL_image.h>

using namespace ark;

graphics::theme::style window_style;

void
graphics::init()
{
	window_style = theme::style::dark;
	theme::change();
	
	ui::init();
	camera::init();
}

void
graphics::destroy()
{
	ui::destroy();
}

void
graphics::draw_convex_poly_filled(
    ImDrawList* draw_list,
    const ark_float_vec2* points,
    const int points_count,
    ImU32 col
)
{
    if (points_count < 3) return;

    const ark_float_vec2 uv = draw_list->_Data->TexUvWhitePixel;

    if (draw_list->Flags & ImDrawListFlags_AntiAliasedFill) {
        // Anti-aliased Fill
        const ImU32 col_trans = col & ~IM_COL32_A_MASK;
        const int idx_count = (points_count - 2) * 3 + points_count * 6;
        const int vtx_count = (points_count * 2);
        draw_list->PrimReserve(idx_count, vtx_count);

        // Add indexes for fill
        const unsigned int vtx_inner_idx = draw_list->_VtxCurrentIdx;
        const unsigned int vtx_outer_idx = draw_list->_VtxCurrentIdx + 1;
        for (int i = 2; i < points_count; i++) {
            draw_list->_IdxWritePtr[0] = static_cast<ImDrawIdx>(vtx_inner_idx);
            draw_list->_IdxWritePtr[1] = static_cast<ImDrawIdx>(vtx_inner_idx + ((i - 1) << 1));
            draw_list->_IdxWritePtr[2] = static_cast<ImDrawIdx>(vtx_inner_idx + (i << 1));
            draw_list->_IdxWritePtr += 3;
        }

        // Compute normals
        auto* temp_normals = static_cast<ark_float_vec2*>(alloca(points_count * sizeof(ark_float_vec2))); //-V630
        for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++) {
            const ark_float_vec2& p0 = points[i0];
            const ark_float_vec2& p1 = points[i1];
            float dx = p1.x - p0.x;
            float dy = p1.y - p0.y;
            {
                float d2 = dx * dx + dy * dy;
                if (d2 > 0.0f) {
                    const float inv_len = 1.0f / ImSqrt(d2);
                    dx *= inv_len;
                    dy *= inv_len;
                }
            }

            temp_normals[i0].x = dy;
            temp_normals[i0].y = -dx;
        }

        for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++) {
            constexpr float AA_SIZE = 1.0f;
            
            // Average normals
            const ark_float_vec2& n0 = temp_normals[i0];
            const ark_float_vec2& n1 = temp_normals[i1];
            float dm_x = (n0.x + n1.x) * 0.5f;
            float dm_y = (n0.y + n1.y) * 0.5f;
            {
                float d2 = dm_x * dm_x + dm_y * dm_y;
                if (d2 < 0.5f) d2 = 0.5f;
                float inv_lensq = 1.0f / d2;
                dm_x *= inv_lensq;
                dm_y *= inv_lensq;
            }
            dm_x *= AA_SIZE * 0.5f;
            dm_y *= AA_SIZE * 0.5f;

            // Add vertices
            draw_list->_VtxWritePtr[0].pos.x = (points[i1].x - dm_x);
            draw_list->_VtxWritePtr[0].pos.y = (points[i1].y - dm_y);
            draw_list->_VtxWritePtr[0].uv = uv;
            draw_list->_VtxWritePtr[0].col = col;        // Inner
            draw_list->_VtxWritePtr[1].pos.x = (points[i1].x + dm_x);
            draw_list->_VtxWritePtr[1].pos.y = (points[i1].y + dm_y);
            draw_list->_VtxWritePtr[1].uv = uv;
            draw_list->_VtxWritePtr[1].col = col_trans;  // Outer
            draw_list->_VtxWritePtr += 2;

            // Add indexes for fringes
            draw_list->_IdxWritePtr[0] = static_cast<ImDrawIdx>(vtx_inner_idx + (i1 << 1));
            draw_list->_IdxWritePtr[1] = static_cast<ImDrawIdx>(vtx_inner_idx + (i0 << 1));
            draw_list->_IdxWritePtr[2] = static_cast<ImDrawIdx>(vtx_outer_idx + (i0 << 1));
            draw_list->_IdxWritePtr[3] = static_cast<ImDrawIdx>(vtx_outer_idx + (i0 << 1));
            draw_list->_IdxWritePtr[4] = static_cast<ImDrawIdx>(vtx_outer_idx + (i1 << 1));
            draw_list->_IdxWritePtr[5] = static_cast<ImDrawIdx>(vtx_inner_idx + (i1 << 1));
            draw_list->_IdxWritePtr += 6;
        }
        
        draw_list->_VtxCurrentIdx += static_cast<ImDrawIdx>(vtx_count);
    } else {
        // Non Anti-aliased Fill
        const int idx_count = (points_count - 2) * 3;
        const int vtx_count = points_count;
        draw_list->PrimReserve(idx_count, vtx_count);
        
        for (int i = 0; i < vtx_count; i++) {
            draw_list->_VtxWritePtr[0].pos = points[i];
            draw_list->_VtxWritePtr[0].uv = uv;
            draw_list->_VtxWritePtr[0].col = col;
            draw_list->_VtxWritePtr++;
        }
        
        for (int i = 2; i < points_count; i++) {
            draw_list->_IdxWritePtr[0] = static_cast<ImDrawIdx>(draw_list->_VtxCurrentIdx);
            draw_list->_IdxWritePtr[1] = static_cast<ImDrawIdx>(draw_list->_VtxCurrentIdx + i - 1);
            draw_list->_IdxWritePtr[2] = static_cast<ImDrawIdx>(draw_list->_VtxCurrentIdx + i);
            draw_list->_IdxWritePtr += 3;
        }
        
        draw_list->_VtxCurrentIdx += static_cast<ImDrawIdx>(vtx_count);
    }
}

void
graphics::draw_rect(
	ImColor color,
	ark_float_vec2 p_min,
	ark_float_vec2 p_max,
	bool filled
)
{
	OPTICK_EVENT("graphics draw rect");
	if (filled) {
		ImGui::GetBackgroundDrawList()->AddRectFilled(p_min, p_max, color);
	} else {
		ImGui::GetBackgroundDrawList()->AddRect(p_min, p_max, color);
	}
}

void
graphics::draw_background(ImTextureID texture_id)
{
	OPTICK_EVENT("graphics draw background");
	const int64_t width = ui::get_cmd_int("window_width");
	const int64_t height = ui::get_cmd_int("window_height");
	ImGui::GetBackgroundDrawList()->AddImage(texture_id, {0,0}, {static_cast<float>(width), static_cast<float>(height)});
}

void
graphics::draw_physical_object(b2Body* object, const ImColor& clr)
{
	OPTICK_EVENT("graphics draw phys object");
	const auto poly = dynamic_cast<b2PolygonShape*>(object->GetFixtureList()->GetShape());
	ark_assert(poly != nullptr, "Can't cast shape to polygon shape", return);
    
	const int32 vertexCount = poly->m_count;
	ark_assert(vertexCount <= b2_maxPolygonVertices, "Vertices count overflow", return);
	ark_float_vec2 vertices[b2_maxPolygonVertices];

	for (int32 i = 0; i < vertexCount; ++i) {
		vertices[i] = camera::world_to_screen(b2Mul(object->GetTransform(), poly->m_vertices[i]));
	}

	draw_convex_poly_filled(ImGui::GetBackgroundDrawList(), vertices, vertexCount, clr);
}

void
graphics::draw_physical_circle_object(b2Body* object, const ImColor& clr)
{
	OPTICK_EVENT("graphics draw phys circle");
	b2CircleShape* circle = (b2CircleShape*)object->GetFixtureList()->GetShape();
	b2Transform xf = object->GetTransform();

	ark_float_vec2 center = camera::world_to_screen(b2Mul(xf, circle->m_p));

	float radius = camera::scale_factor(circle->m_radius);

	ImGui::GetBackgroundDrawList()->AddCircleFilled(center, radius , clr);
}

void
graphics::tick(float dt)
{
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ static_cast<float>(ui::get_cmd_int("window_width")), static_cast<float>(ui::get_cmd_int("window_height")) });
	
	draw(dt);

	//if (ImGui::Begin(" ", 0, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration)) {
	//	ImGui::End();
	//}
	
	ui::tick(dt);
	camera::tick(dt);
}

void
graphics::draw(float dt)
{
	OPTICK_EVENT("scene draw")
	systems::draw_tick(dt);
}

void graphics::theme::change()
{
	switch (window_style) {
	case style::dark:
		dark();
		break;
	case style::red:
		red();
		break;
	case style::invalid:
	default:
		break;
	}
}

void graphics::theme::dark()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
	colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
	colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowPadding = ark_float_vec2(8.00f, 8.00f);
	style.FramePadding = ark_float_vec2(5.00f, 2.00f);
	style.CellPadding = ark_float_vec2(6.00f, 6.00f);
	style.ItemSpacing = ark_float_vec2(6.00f, 6.00f);
	style.ItemInnerSpacing = ark_float_vec2(6.00f, 6.00f);
	style.TouchExtraPadding = ark_float_vec2(0.00f, 0.00f);
	style.IndentSpacing = 25;
	style.ScrollbarSize = 15;
	style.GrabMinSize = 10;
	style.WindowBorderSize = 1;
	style.ChildBorderSize = 1;
	style.PopupBorderSize = 1;
	style.FrameBorderSize = 1;
	style.TabBorderSize = 1;
	style.WindowRounding = 0;
	style.ChildRounding = 0;
	style.FrameRounding = 0;
	style.PopupRounding = 0;
	style.ScrollbarRounding = 9;
	style.GrabRounding = 3;
	style.LogSliderDeadzone = 4;
	style.TabRounding = 4;
}

void graphics::theme::red()
{
	auto& style = ImGui::GetStyle();
	style.FrameRounding = 4.0f;
	style.WindowBorderSize = 0.0f;
	style.PopupBorderSize = 0.0f;
	style.GrabRounding = 4.0f;

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.73f, 0.75f, 0.74f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.09f, 0.94f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.84f, 0.66f, 0.66f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.84f, 0.66f, 0.66f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.47f, 0.22f, 0.22f, 0.67f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.47f, 0.22f, 0.22f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.47f, 0.22f, 0.22f, 0.67f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.34f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.71f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.84f, 0.66f, 0.66f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.47f, 0.22f, 0.22f, 0.65f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.71f, 0.39f, 0.39f, 0.65f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);
	colors[ImGuiCol_Header] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.84f, 0.66f, 0.66f, 0.65f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.84f, 0.66f, 0.66f, 0.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.84f, 0.66f, 0.66f, 0.66f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.84f, 0.66f, 0.66f, 0.66f);
	colors[ImGuiCol_Tab] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.84f, 0.66f, 0.66f, 0.66f);
	colors[ImGuiCol_TabActive] = ImVec4(0.84f, 0.66f, 0.66f, 0.66f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}
