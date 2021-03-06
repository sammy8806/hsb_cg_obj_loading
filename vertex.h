#ifndef VERTEX_H
#define VERTEX_H


class Vertex
{
public:
    int valence;
    float coordinates[3];

    Vertex();
    Vertex( float p1[3]);
    Vertex(float x,float y, float z);
    void Print(); // print coordinates on screen
    void operator*=(float a);
    void operator +=(Vertex b);
};

Vertex operator+(Vertex a, Vertex b);
Vertex operator-(Vertex a, Vertex b);
Vertex operator*(float a, Vertex b);
Vertex operator/(Vertex a, float b);
Vertex operator%(Vertex a, Vertex b);//cross product

#endif // VERTEX_H
