#include "pch.h"

using namespace ark::physics;

static material::material_data data[(size_t)material::material_type::out_of];

void
material::init()
{
	data[(size_t)material_type::rubber].restitution = 0.9f;
}

void
material::destroy()
{
}

const material::material_data& 
material::get(material_type type)
{
	return data[(size_t)type];
}