#pragma once

#include "components/simple_scene.h"

namespace m1
{
	class Vis2D : public gfxc::SimpleScene
	{
	public:
		struct ViewportSpace
		{
			ViewportSpace() : x(0), y(0), width(1), height(1) {}
			ViewportSpace(int x, int y, int width, int height)
				: x(x), y(y), width(width), height(height)
			{}
			int x;
			int y;
			int width;
			int height;
		};

		struct LogicSpace
		{
			LogicSpace() : x(0), y(0), width(1), height(1) {}
			LogicSpace(float x, float y, float width, float height)
				: x(x), y(y), width(width), height(height)
			{}
			float x;
			float y;
			float width;
			float height;
		};

	public:
		Vis2D();
		~Vis2D();

	};
}