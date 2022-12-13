#include "pch.h"

using namespace Asura;

static Physics::Material::data material_data[static_cast<uint8_t>(Physics::Material::type::out_of)] = {};

void Physics::Material::Init()
{
	material_data[static_cast<uint8_t>(type::rubber)].friction = 0.1f;
	material_data[static_cast<uint8_t>(type::rubber)].density = 1.f;
	material_data[static_cast<uint8_t>(type::rubber)].restitution = 0.9f;
}							  

void Physics::Material::Destroy()
{
}

const Physics::Material::data& Physics::Material::get(type type)
{
	return material_data[static_cast<uint8_t>(type)];
}
