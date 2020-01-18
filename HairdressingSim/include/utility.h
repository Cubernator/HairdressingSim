#pragma once

#include "glm.h"

struct Viewport
{
	int x, y, width, height;

	void apply() const;
	float getAspectRatio() const;
};

void printError();

glm::vec3 getColorFromInt(unsigned int c);