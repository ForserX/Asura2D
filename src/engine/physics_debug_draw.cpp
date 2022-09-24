#include "pch.h"

using ark::physics::DebugDraw;

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	ark_float_vec2* draw_vec = new ark_float_vec2[vertexCount];

	for (int32 iter = 0; iter < vertexCount; iter++) {
		draw_vec[iter] = camera::world_to_screen(vertices[iter]);
	}

	ImGui::GetForegroundDrawList()->AddPolyline((ImVec2*)draw_vec, vertexCount, ImColor(color.r, color.g, color.b, color.a), ImDrawFlags_None, 1);

	delete[] draw_vec;
}

//
void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	ark_float_vec2* draw_vec = new ark_float_vec2[vertexCount];

	for (int32 iter = 0; iter < vertexCount; iter++) {
		draw_vec[iter] = camera::world_to_screen(vertices[iter]);
	}

	graphics::draw_convex_poly_filled(ImGui::GetForegroundDrawList(), draw_vec, vertexCount, ImColor(color.r, color.g, color.b, color.a));
	delete[] draw_vec;
}
//
void DebugDraw::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
{
	ark_float_vec2 screen_center = camera::world_to_screen(center);
	ImGui::GetForegroundDrawList()->AddCircle(screen_center, radius, ImColor(color.r, color.g, color.b, color.a));
}

//
void DebugDraw::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
{
	ark_float_vec2 screen_center = camera::world_to_screen(center);
	ImGui::GetForegroundDrawList()->AddCircleFilled(screen_center, radius, ImColor(color.r, color.g, color.b, color.a));
}

//
void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	ark_float_vec2 screen_line = camera::world_to_screen(p1);
	ark_float_vec2 screen_line2 = camera::world_to_screen(p2);
	ImGui::GetForegroundDrawList()->AddLine(screen_line, screen_line2, ImColor(color.r, color.g, color.b, color.a));
}

//
void DebugDraw::DrawTransform(const b2Transform& xf)
{
	const float k_axisScale = 0.4f;
	ImColor red(1.0f, 0.0f, 0.0f);
	ImColor green(0.0f, 1.0f, 0.0f);
	ark_float_vec2 p1 = camera::world_to_screen(xf.p), p2;

	p2 = camera::world_to_screen(p1 + k_axisScale * xf.q.GetXAxis());
	ImGui::GetForegroundDrawList()->AddLine(p1, p2, red);

	p2 = camera::world_to_screen(p1 + k_axisScale * xf.q.GetYAxis());
	ImGui::GetForegroundDrawList()->AddLine(p1, p2, green);
}

//
void DebugDraw::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
{
	ark_float_vec2 screen_line = camera::world_to_screen(p);
	ImGui::GetForegroundDrawList()->AddLine(screen_line, screen_line, ImColor(color.r, color.g, color.b, color.a));
}

//
void DebugDraw::DrawAABB(b2AABB* aabb, const b2Color& color)
{
	ark_float_vec2 p1 = camera::world_to_screen(aabb->lowerBound);
	ark_float_vec2 p2 = camera::world_to_screen({ aabb->upperBound.x, aabb->lowerBound.y });
	ark_float_vec2 p3 = camera::world_to_screen(aabb->upperBound);
	ark_float_vec2 p4 = camera::world_to_screen({ aabb->lowerBound.x, aabb->upperBound.y });

	ImGui::GetForegroundDrawList()->AddLine(p1, p2, ImColor(color.r, color.g, color.b, color.a));
	ImGui::GetForegroundDrawList()->AddLine(p2, p3, ImColor(color.r, color.g, color.b, color.a));
	ImGui::GetForegroundDrawList()->AddLine(p3, p4, ImColor(color.r, color.g, color.b, color.a));
	ImGui::GetForegroundDrawList()->AddLine(p4, p1, ImColor(color.r, color.g, color.b, color.a));
}

#ifdef ARKANE_BOX2D_OPTIMIZED
void
DebugDraw::DrawParticles(
	const b2Vec2* centers,
	float32 radius,
	const b2ParticleColor* colors,
	int32 count
)
{
}
#endif