#include "pch.h"

using namespace Asura::GamePlay;
using Asura::Physics::DebugDraw;

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	const size_t NewCount = vertexCount + 1;

	Math::FVec2* draw_vec = new Math::FVec2[NewCount];

	for (int32 iter = 0; iter < vertexCount; iter++)
	{
		draw_vec[iter] = Camera::World2Screen(vertices[iter]);
	}

	// FX: Swap for last line
	draw_vec[vertexCount] = draw_vec[0];

	ImGui::GetForegroundDrawList()->AddPolyline((ImVec2*)draw_vec, NewCount, ImColor(color.r, color.g, color.b, color.a), ImDrawFlags_None, 1);

	delete[] draw_vec;
}

//
void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	Math::FVec2* draw_vec = new Math::FVec2[vertexCount];

	for (int32 iter = 0; iter < vertexCount; iter++) {
		draw_vec[iter] = Camera::World2Screen(vertices[iter]);
	}

	Graphics::DrawConvexFilled(ImGui::GetForegroundDrawList(), draw_vec, vertexCount, ImColor(color.r, color.g, color.b, color.a));
	delete[] draw_vec;
}
//
void DebugDraw::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
{
	Math::FVec2 screen_center = Camera::World2Screen(center);
	ImGui::GetForegroundDrawList()->AddCircle(screen_center, radius, ImColor(color.r, color.g, color.b, color.a));
}

//
void DebugDraw::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
{
	Math::FVec2 screen_center = Camera::World2Screen(center);
	ImGui::GetForegroundDrawList()->AddCircleFilled(screen_center, radius, ImColor(color.r, color.g, color.b, color.a));
}

//
void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	Math::FVec2 screen_line = Camera::World2Screen(p1);
	Math::FVec2 screen_line2 = Camera::World2Screen(p2);
	ImGui::GetForegroundDrawList()->AddLine(screen_line, screen_line2, ImColor(color.r, color.g, color.b, color.a));
}

//
void DebugDraw::DrawTransform(const b2Transform& xf)
{
	const float k_axisScale = 0.4f;
	ImColor red(1.0f, 0.0f, 0.0f);
	ImColor green(0.0f, 1.0f, 0.0f);
	Math::FVec2 p1 = Camera::World2Screen(xf.p), p2;

	p2 = Camera::World2Screen(p1 + k_axisScale * xf.q.GetXAxis());
	ImGui::GetForegroundDrawList()->AddLine(p1, p2, red);

	p2 = Camera::World2Screen(p1 + k_axisScale * xf.q.GetYAxis());
	ImGui::GetForegroundDrawList()->AddLine(p1, p2, green);
}

//
void DebugDraw::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
{
	Math::FVec2 screen_line = Camera::World2Screen(p);
	ImGui::GetForegroundDrawList()->AddLine(screen_line, screen_line, ImColor(color.r, color.g, color.b, color.a));
}

//
void DebugDraw::DrawAABB(b2AABB* aabb, const b2Color& color)
{
	Math::FVec2 p1 = Camera::World2Screen(aabb->lowerBound);
	Math::FVec2 p2 = Camera::World2Screen({ aabb->upperBound.x, aabb->lowerBound.y });
	Math::FVec2 p3 = Camera::World2Screen(aabb->upperBound);
	Math::FVec2 p4 = Camera::World2Screen({ aabb->lowerBound.x, aabb->upperBound.y });

	ImGui::GetForegroundDrawList()->AddLine(p1, p2, ImColor(color.r, color.g, color.b, color.a));
	ImGui::GetForegroundDrawList()->AddLine(p2, p3, ImColor(color.r, color.g, color.b, color.a));
	ImGui::GetForegroundDrawList()->AddLine(p3, p4, ImColor(color.r, color.g, color.b, color.a));
	ImGui::GetForegroundDrawList()->AddLine(p4, p1, ImColor(color.r, color.g, color.b, color.a));
}