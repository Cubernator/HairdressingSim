#include "HairdressingSim.h"

#pragma comment(lib, "glew32")
#pragma comment(lib, "glfw3")
#pragma comment(lib, "opengl32")

int main()
{
	HairdressingSim app;

	app.run();

	return app.getError();
}