#include "vertex.h"

Vertex::Vertex()
{

}

Vertex::Vertex(float x, float y, float z)
{
    this->coordinates[0] = x;
    this->coordinates[1] = y;
    this->coordinates[2] = z;
}
