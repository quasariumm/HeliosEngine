#ifndef COMMON_GLSL
#define COMMON_GLSL

/*
	Stores some basic constants
*/

const float PI = 3.14159265357;
const float TWO_PI = 2.0 * PI;
const float INVPI = 1.0 / 3.14159265357;
const float INV2PI = 0.5 * INVPI;

float atan2(float y, float x)
{
	return mod(atan(y,x) + PI, TWO_PI);
}

#endif // COMMON_GLSL