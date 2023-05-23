#pragma once

#include "Component.h"

class Renderer : public Component
{
public:
	Renderer() {};
	virtual ~Renderer() {};

	virtual void Render() abstract;
};

