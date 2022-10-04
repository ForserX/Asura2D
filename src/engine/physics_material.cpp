#include "pch.h"

using namespace ark;

static material::data material_data[static_cast<uint8_t>(material::type::out_of)] = {};

void
material::init()
{
	material_data[static_cast<uint8_t>(type::rubber)].friction = 0.1f;
	material_data[static_cast<uint8_t>(type::rubber)].density = 1.f;
	material_data[static_cast<uint8_t>(type::rubber)].restitution = 0.9f;
}							  

void
material::destroy()
{
}

const material::data& 
material::get(type type)
{
	return material_data[static_cast<uint8_t>(type)];
}
