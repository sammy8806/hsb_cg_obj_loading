#include "triangle.h"

Triangle::Triangle()
{

}

Triangle::Triangle(float x, float y, float z)
{
    this->vIndex[0] = --x;
    this->vIndex[1] = --y;
    this->vIndex[2] = --z;
}
