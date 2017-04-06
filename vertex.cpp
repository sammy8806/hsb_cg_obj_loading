#include "vertex.h"

Vertex::Vertex()
{

}

Vertex::Vertex(float x, float y, float z){
    coordinates[0]=x;
    coordinates[1]=y;
    coordinates[2]=z;
}

void Vertex::operator *= (float a){
    coordinates[0] *=a;
    coordinates[1] *=a;
    coordinates[2] *=a;
}

Vertex operator+(Vertex a, Vertex b){
    Vertex c;
    c.coordinates[0] = a.coordinates[0] + b.coordinates[0];
    c.coordinates[1] = a.coordinates[1] + b.coordinates[1];
    c.coordinates[2] = a.coordinates[2] + b.coordinates[2];
    return c;
}

Vertex operator-(Vertex a, Vertex b){
    Vertex c;
    c.coordinates[0] = a.coordinates[0] - b.coordinates[0];
    c.coordinates[1] = a.coordinates[1] - b.coordinates[1];
    c.coordinates[2] = a.coordinates[2] - b.coordinates[2];
    return c;
}
void Vertex:: operator+=(Vertex b){
    coordinates[0] += b.coordinates[0];
    coordinates[1] += b.coordinates[1];
    coordinates[2] += b.coordinates[2];
}

Vertex operator*(float a, Vertex b){
    Vertex c;
    c.coordinates[0] = b.coordinates[0] * a;
    c.coordinates[1] = b.coordinates[1] * a;
    c.coordinates[2] = b.coordinates[2] * a;
    return c;
}

Vertex operator/(Vertex b, float a){
    Vertex c;
    c.coordinates[0] = b.coordinates[0] / a;
    c.coordinates[1] = b.coordinates[1] / a;
    c.coordinates[2] = b.coordinates[2] / a;
    return c;
}

Vertex operator%(Vertex a, Vertex b){//cross product
    Vertex c;
    c.coordinates[0] = a.coordinates[1]*b.coordinates[2]-a.coordinates[2]*b.coordinates[1];
    c.coordinates[1] = a.coordinates[2]*b.coordinates[0]-a.coordinates[0]*b.coordinates[2];
    c.coordinates[2] = a.coordinates[0]*b.coordinates[1]-a.coordinates[1]*b.coordinates[0];
    return c;
}
