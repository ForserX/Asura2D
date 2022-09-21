#include "pch.h"
#include <cstdlib>
#include <string.h>
#include "console.h"
#include <imgui_internal.h>

using namespace ark;
extern bool fullscreen_mode;
extern int window_width;
extern int window_height;

extern ui::UIConsole console;
bool show_console = false;

// Test 
b2Body* TestGround;
b2Body* TestObject;
b2Body* TestObject2;

void
ui::init()
{
    TestGround = physical.create_ground({ 10, 0 }, { 1000, 10 });
    TestObject = physical.create_body({ 10, 50 }, { 20, 10 });
    TestObject2 = physical.create_body({ 10, 100 }, { 140, 20 });
}

void AddConvexPolyFilled(ImDrawList* DrawList, const ImVec2* points, const int points_count, ImU32 col) {
    if (points_count < 3) return;

    const ImVec2 uv = DrawList->_Data->TexUvWhitePixel;

    if (DrawList->Flags & ImDrawListFlags_AntiAliasedFill) {
        // Anti-aliased Fill
        const float AA_SIZE = 1.0f;
        const ImU32 col_trans = col & ~IM_COL32_A_MASK;
        const int idx_count = (points_count - 2) * 3 + points_count * 6;
        const int vtx_count = (points_count * 2);
        DrawList->PrimReserve(idx_count, vtx_count);

        // Add indexes for fill
        unsigned int vtx_inner_idx = DrawList->_VtxCurrentIdx;
        unsigned int vtx_outer_idx = DrawList->_VtxCurrentIdx + 1;
        for (int i = 2; i < points_count; i++) {
            DrawList->_IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx);
            DrawList->_IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + ((i - 1) << 1));
            DrawList->_IdxWritePtr[2] = (ImDrawIdx)(vtx_inner_idx + (i << 1));
            DrawList->_IdxWritePtr += 3;
        }

        // Compute normals
        ImVec2* temp_normals = (ImVec2*)alloca(points_count * sizeof(ImVec2)); //-V630
        for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++) {
            const ImVec2& p0 = points[i0];
            const ImVec2& p1 = points[i1];
            float dx = p1.x - p0.x;
            float dy = p1.y - p0.y;
            {
                float d2 = dx * dx + dy * dy;
                if (d2 > 0.0f) {
                    float inv_len = 1.0f / ImSqrt(d2);
                    dx *= inv_len;
                    dy *= inv_len;
                }
            }

            temp_normals[i0].x = dy;
            temp_normals[i0].y = -dx;
        }

        for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++) {
            // Average normals
            const ImVec2& n0 = temp_normals[i0];
            const ImVec2& n1 = temp_normals[i1];
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
            DrawList->_VtxWritePtr[0].pos.x = (points[i1].x - dm_x);
            DrawList->_VtxWritePtr[0].pos.y = (points[i1].y - dm_y);
            DrawList->_VtxWritePtr[0].uv = uv;
            DrawList->_VtxWritePtr[0].col = col;        // Inner
            DrawList->_VtxWritePtr[1].pos.x = (points[i1].x + dm_x);
            DrawList->_VtxWritePtr[1].pos.y = (points[i1].y + dm_y);
            DrawList->_VtxWritePtr[1].uv = uv;
            DrawList->_VtxWritePtr[1].col = col_trans;  // Outer
            DrawList->_VtxWritePtr += 2;

            // Add indexes for fringes
            DrawList->_IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx + (i1 << 1));
            DrawList->_IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + (i0 << 1));
            DrawList->_IdxWritePtr[2] = (ImDrawIdx)(vtx_outer_idx + (i0 << 1));
            DrawList->_IdxWritePtr[3] = (ImDrawIdx)(vtx_outer_idx + (i0 << 1));
            DrawList->_IdxWritePtr[4] = (ImDrawIdx)(vtx_outer_idx + (i1 << 1));
            DrawList->_IdxWritePtr[5] = (ImDrawIdx)(vtx_inner_idx + (i1 << 1));
            DrawList->_IdxWritePtr += 6;
        }
        DrawList->_VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
    else {
        // Non Anti-aliased Fill
        const int idx_count = (points_count - 2) * 3;
        const int vtx_count = points_count;
        DrawList->PrimReserve(idx_count, vtx_count);
        for (int i = 0; i < vtx_count; i++) {
            DrawList->_VtxWritePtr[0].pos = points[i];
            DrawList->_VtxWritePtr[0].uv = uv;
            DrawList->_VtxWritePtr[0].col = col;
            DrawList->_VtxWritePtr++;
        }
        for (int i = 2; i < points_count; i++) {
            DrawList->_IdxWritePtr[0] = (ImDrawIdx)(DrawList->_VtxCurrentIdx);
            DrawList->_IdxWritePtr[1] = (ImDrawIdx)(DrawList->_VtxCurrentIdx + i - 1);
            DrawList->_IdxWritePtr[2] = (ImDrawIdx)(DrawList->_VtxCurrentIdx + i);
            DrawList->_IdxWritePtr += 3;
        }
        DrawList->_VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
}

void
ui::draw_physical_object(b2Body* object, const ImColor& clr)
{
    b2PolygonShape* poly = (b2PolygonShape*)object->GetFixtureList()->GetShape();
    int32 vertexCount = poly->m_count;
    b2Assert(vertexCount <= b2_maxPolygonVertices);
    b2Vec2 vertices[b2_maxPolygonVertices];

    for (int32 i = 0; i < vertexCount; ++i)
    {
        vertices[i] = b2Mul(object->GetTransform(), poly->m_vertices[i]);
        vertices[i].y = ui::get_cmd_int("window_height") - vertices[i].y;
    }

    AddConvexPolyFilled(ImGui::GetWindowDrawList(), (ImVec2*)vertices, vertexCount, clr);
}

void
ui::tick(float dt)
{
    if (show_console) {
        console.draw("Arkane console", &show_console);
    }

    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::SetNextWindowSize({ (float)ui::get_cmd_int("window_width"), (float)ui::get_cmd_int("window_height") });

    if (ImGui::Begin(" ", 0, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration))
    {
        draw_physical_object(TestGround, ImColor(0, 235, 123));
        draw_physical_object(TestObject, ImColor(13, 54, 123));
        draw_physical_object(TestObject2, ImColor(13, 54, 123));
        ImGui::End();
    }
}

int64_t 
ui::get_cmd_int(std::string_view str)
{
    if (str == "window_fullscreen") {
        return fullscreen_mode;
    }
    else if (str == "window_width") {
        return window_width;
    }
    else if (str == "window_height") {
        return window_height;
    }
}

void 
ui::push_console_string(std::string_view str)
{
    console.push_log_item(str);
}

void
ui::destroy()
{
   console.clear_log();
}
