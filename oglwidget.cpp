#include "oglwidget.h"
#include <math.h>
#include <iostream>
#include <algorithm> // for std::find
#include <iterator> // for std::begin, std::end

#define PI 3.14159265358979323846
using namespace std;

static double alpha = 45.0; // rotation angle

// initialize Open GL lighting and projection matrix
void InitLightingAndProjection() // to be executed once before drawing
{
    // light positions and colors
    GLfloat LightPosition1[4] = { 10, 5, 10,  0};
    GLfloat LightPosition2[4] = { -5, 5, -10,  0};
    GLfloat ColorRedish[4] = { 1.0,  .8,  .8,  1}; // white with a little bit of red
    GLfloat ColorBlueish[4] = { .8,  .8,  1.0,  1};// white with a little bit of blue

    glEnable( GL_DEPTH_TEST); // switch on z-buffer
    glDepthFunc( GL_LESS);

    glShadeModel( GL_SMOOTH); // Gouraud shading
    //glShadeModel( GL_FLAT);

    glEnable( GL_LIGHTING); // use lighting
    glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 1); // draw both sides

    // define and switch on light 0
    glLightfv( GL_LIGHT0, GL_POSITION, LightPosition1);
    glLightfv( GL_LIGHT0, GL_DIFFUSE,  ColorRedish);
    glLightfv( GL_LIGHT0, GL_SPECULAR, ColorRedish);
    glEnable( GL_LIGHT0);

    // define and switch on light 1
    glLightfv( GL_LIGHT1, GL_POSITION, LightPosition2);
    glLightfv( GL_LIGHT1, GL_DIFFUSE,  ColorBlueish);
    glLightfv( GL_LIGHT1, GL_SPECULAR, ColorBlueish);
    glEnable( GL_LIGHT1);

    glMatrixMode( GL_PROJECTION); // define camera projection
    glLoadIdentity(); // reset matrix to identity (otherwise existing matrix will be multiplied with)
    glOrtho( -50, 50, -50, 50, -80, 80); // orthogonal projection (xmin xmax ymin ymax zmin zmax)
    //glFrustum( -10, 10, -8, 8, 2, 20); // perspective projektion
}

void DrawCylinder( int reso = 16){ // drawing a cylinder in OpenGL
    // alocate memory for x and y coordinates on a circle
    double *c = new double[ reso+1];
    double *s = new double[ reso+1];

    for( int i=0; i<=reso; i++){ // compute x and y coordinates of citcle
        c[i] = cos( 2.0 * PI * i / reso );
        s[i] = sin( 2.0 * PI * i / reso );
        //cout << i << " " << c[i] << endl;
    }

    glBegin( GL_QUADS); // each 4 points define a polygon
    for( int i=0; i<reso; i++){
        glNormal3f( c[i], s[i], 0.0); // normal vector used for all consecutive points
        glVertex3f( c[i], s[i], 3.0); // 2 points ...
        glVertex3f( c[i], s[i], 0.0);

        glNormal3f( c[i+1], s[i+1], 0.0); // another normal with two more points
        glVertex3f( c[i+1], s[i+1], 0.0);
        glVertex3f( c[i+1], s[i+1], 3.0);
    }
    glEnd(); // concludes GL_QUADS

    delete[] c; // de-allocate space
    delete[] s;
}

// define material color properties for front and back side
void SetMaterialColor( int side, float r, float g, float b){
    float	amb[4], dif[4], spe[4];
    int	i, mat;

    dif[0] = r; // diffuse color as defined by r,g, and b
    dif[1] = g;
    dif[2] = b;
    for( i=0; i<3; i++){
        amb[i] = .1 * dif[i]; // ambient color is 10 percent of diffuse
        spe[i] = .5; // specular color is just white / gray
    }
    amb[3] = dif[3] = spe[3] = 1.0; // alpha component is always 1
    switch( side){
        case 1:	mat = GL_FRONT; break;
        case 2:	mat = GL_BACK; break;
        default: mat = GL_FRONT_AND_BACK; break;
    }
    glMaterialfv( mat, GL_AMBIENT, amb); // define ambient, diffuse and specular components
    glMaterialfv( mat, GL_DIFFUSE, dif);
    glMaterialfv( mat, GL_SPECULAR, spe);
    glMaterialf( mat, GL_SHININESS, 50.0); // Phong constant for the size of highlights
}

OGLWidget::OGLWidget(QWidget *parent) // constructor
    : QOpenGLWidget(parent)/*,
      triangles(new QVector<Triangle>()),
      vertices(new QVector<Vertex>())*/
{
    // Setup the animation timer to fire every x msec
    animtimer = new QTimer(this);
    animtimer->start( 50 );

    // Everytime the timer fires, the animation is going one step forward
    connect(animtimer, SIGNAL(timeout()), this, SLOT(stepAnimation()));

    animstep = 0;

    for(Triangle tri : this->tris) {
        for(int i=0; i < 3; i++)
            tri.midIndex[i] = 0;

        for(int i=0; i < 3; i++)
            tri.neighbours[i] = 0;

        for(int i=0; i < 3; i++)
            tri.vertexIndex[i] = 0.0f;
    }
}

OGLWidget::~OGLWidget() // destructor
{
}

void OGLWidget::stepAnimation()
{
    animstep++;    // Increase animation steps
    update();      // Trigger redraw of scene with paintGL
}

void OGLWidget::lineRead(QString key, float x, float y, float z)
{
    qDebug() << "OGL::lineRead: " << key;
    if(key == "v") {
        this->points.push_back(Vertex(x, y, z));
    } else if(key == "f") {
        this->tris.push_back(Triangle(--x, --y, --z));
    }
}

void OGLWidget::readFinished()
{
    qDebug() << "Reading Finished! .. Doint Subdivision stuff";

    // QVector<Triangle> tris = this->tris;
    // QVector<Vertex> points = this->points;

    if(this->tris.size() > 0) {
        for(int i=0; i<1; i++) {

            connectivity(this->tris, this->points);


            for(Triangle tri : this->tris) {
                qDebug() << "Tri: " << tri.vertexIndex[0] << " " << tri.vertexIndex[1] << " " << tri.vertexIndex[2] << " ";
            }

            for(Vertex vertex : this->points) {
                qDebug() << "Vertex: " << vertex.coordinates[0] << " "<< vertex.coordinates[1] << " "<< vertex.coordinates[2] << " #" << vertex.valence;
            }

            for(int j =0; j <this->tris.size(); j++){
                qDebug() << "tris ind: " << j << "neighbours: "<< tris[j].neighbours[0] << tris[j].neighbours[1] << tris[j].neighbours[2];
            }

            subdivisionEdge(this->tris, this->points);
            subdivisionVertex(this->tris, this->points);
        }
    }
}

void OGLWidget::initializeGL() // initializations to be called once
{
    initializeOpenGLFunctions();
    InitLightingAndProjection(); // define light sources and projection
}

void OGLWidget::paintGL() // draw everything, to be called repeatedly
{
    glEnable(GL_NORMALIZE); // this is necessary when using glScale (keep normals to unit length)

    // set background color
    glClearColor(0.8, 0.8, 1.0, 1.0); // bright blue
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw the scene
    glMatrixMode( GL_MODELVIEW);
    glLoadIdentity();				// Reset The Current Modelview Matrix
    glTranslated( 0 ,0 ,-10.0);     // Move 10 units backwards in z, since camera is at origin
    glScaled( 5.0, 5.0, 5.0);       // scale objects
    glRotated( alpha, 0, 3, 1);     // continuous rotation
    alpha += 5;

    // define color: 1=front, 2=back, 3=both, followed by r, g, and b
    SetMaterialColor( 1, 1.0, .2, .2);  // front color is red
    SetMaterialColor( 2, 0.2, 0.2, 1.0); // back color is blue

    // draw a cylinder with default resolution
    // DrawCylinder();
    drawObject(this->points, this->tris);

    // make it appear (before this, it's hidden in the rear buffer)
    glFlush();
}

void OGLWidget::drawObject(QVector<Vertex> &points, QVector<Triangle> &tris)
{    
    // glBegin(GL_LINE_STRIP);
    glBegin(GL_TRIANGLES);

    glColor3f(1,1,0);
    float normal[3];

    for(int i=0; i<tris.length(); i++) {
        this->cross(
                    normal,
                    points.at(tris[i].vertexIndex[0]).coordinates,
                    points.at(tris[i].vertexIndex[1]).coordinates
        );
        glNormal3fv(normal);

        for(int j=0; j<3; j++) {
            glVertex3fv(points.at(tris[i].vertexIndex[j]).coordinates);
        }
    }

    glEnd();
}

void OGLWidget::cross(float c[3], const float a[], const float b[])
{
    c[0] = a[1]*b[2]-a[2]*b[1];
    c[1] = a[2]*b[0]-a[0]*b[2];
    c[2] = a[0]*b[1]-a[1]*b[0];
}

//perform connectivity algo
void OGLWidget::connectivity(QVector<Triangle> &tris, QVector<Vertex> &points){
    /*for(int j = 0; j<points->size(); j++){//set all valences to 0
        points->at(j).valence = 0;
    }*/

    for(auto point : points) {
        point.valence = 0;
    }

    for(int i = 0; i<tris.size(); i++){
        int a = tris[i].vertexIndex[0];
        int b = tris[i].vertexIndex[1];
        int c = tris[i].vertexIndex[2];

        for(int j = 0; j < tris.size(); j++){

            if(i != j){//not the same triangle
                bool containsa = std::find(begin(tris[j].vertexIndex), end(tris[j].vertexIndex), a) != end(tris[j].vertexIndex);
                bool containsb = std::find(begin(tris[j].vertexIndex), end(tris[j].vertexIndex), b) != end(tris[j].vertexIndex);
                bool containsc = std::find(begin(tris[j].vertexIndex), end(tris[j].vertexIndex), c) != end(tris[j].vertexIndex);

                if(containsb && containsc){ //b and c
                    tris[i].neighbours[0] = j; //j - index of triangle in QVector
                } else if(containsc && containsa){ //a and c
                    tris[i].neighbours[1] = j;
                } else if(containsa && containsb){ //a and b
                    tris[i].neighbours[2] = j;
                }

            } //end ifs

            //valence of points - n of a,b,c - increase here? cause at this points all 3 neighbours are found
            points[a].valence++;
            points[b].valence++;
            points[c].valence++;
       }
   }
    //test connectivity result
     for(int i = 0; i<tris.size(); i++){
         qDebug() << "tris ind: " << i << "neighbours: "<< tris[i].neighbours[0] << tris[i].neighbours[1] << tris[i].neighbours[2];
     }
}

//perform subdivision algo - edge mask
void OGLWidget::subdivisionEdge(QVector<Triangle> &tris, QVector<Vertex> &points){
    //for t0
    for(int i = 0; i < tris.size(); i++){
        // is it really connected ?!
        if(i < tris[i].neighbours[0]){
            Triangle n = tris[tris[i].neighbours[0]];
            int dpos; //position of d in points

            int p1 = tris[i].vertexIndex[1];
            int p2 = tris[i].vertexIndex[2];//1 and 2 - the 2 eq points in both tris

            for(int j = 0; j<3; j++){
                if(n.vertexIndex[j] != p1 && n.vertexIndex[j] != p2){
                    dpos = n.vertexIndex[j];
                }
            }

            Vertex a = points[tris[i].vertexIndex[0]];
            Vertex b = points[tris[i].vertexIndex[1]];
            Vertex c = points[tris[i].vertexIndex[2]];

            Vertex d = points[dpos];

            qDebug() << "D: " << d.coordinates[0] << " " << d.coordinates[1] << " " << d.coordinates[2];

            Vertex e0 = (a + 3*b+3*c+d)/8;

            qDebug() << "e0: " << e0.coordinates[0] << " " << e0.coordinates[1] << " " << e0.coordinates[2];

            points.push_back(e0); //add to points vector

            tris[i].midIndex[0] = points.size() - 1; //e0 was just pushed, so last index will do
            qDebug() << "New Index: " << points.size();
        } else {
            tris[i].midIndex[0] = tris[tris[i].neighbours[0]].midIndex[0];
        }
    }

    //for t1
    for(int i = 0; i<tris.size(); i++){
        if(i<tris[i].neighbours[1]){
            Triangle n = tris[tris[i].neighbours[1]];
            int dpos;
            int p1 = tris[i].vertexIndex[0];
            int p2 = tris[i].vertexIndex[2];
            for(int j = 0; j<3; j++){
                if(n.vertexIndex[j] !=p1 &&n.vertexIndex[j] !=p2){
                    dpos = n.vertexIndex[j];
                }
            }
            Vertex a = points[tris[i].vertexIndex[0]];
            Vertex b = points[tris[i].vertexIndex[1]];
            Vertex c = points[tris[i].vertexIndex[2]];
            Vertex d = points[dpos];

            qDebug() << "D: " << d.coordinates[0] << " " << d.coordinates[1] << " " << d.coordinates[2];

            Vertex e0 = (a + 3*b+3*c+d)/8;

            qDebug() << "e0: " << e0.coordinates[0] << " " << e0.coordinates[1] << " " << e0.coordinates[2];

            points.push_back(e0); //add to points vector

            tris[i].midIndex[1]=points.size() - 1;
            qDebug() << "New Index: " << points.size();
        }else{
            tris[i].midIndex[1]= tris[tris[i].neighbours[1]].midIndex[1];
        }
    }
    //for t2
    for(int i = 0; i<tris.size(); i++){
        if(i<tris[i].neighbours[2]){
            Triangle n = tris[tris[i].neighbours[2]];
            int dpos;
            int p1 = tris[i].vertexIndex[0];
            int p2 = tris[i].vertexIndex[1];
            for(int j = 0; j<3; j++){
                if(n.vertexIndex[j] !=p1 &&n.vertexIndex[j] !=p2){
                    dpos = n.vertexIndex[j];
                }
            }
            Vertex a = points[tris[i].vertexIndex[0]];
            Vertex b = points[tris[i].vertexIndex[1]];
            Vertex c = points[tris[i].vertexIndex[2]];
            Vertex d = points[dpos];

            qDebug() << "D: " << d.coordinates[0] << " " << d.coordinates[1] << " " << d.coordinates[2];

            Vertex e0 = (a + 3*b+3*c+d)/8;

            qDebug() << "e0: " << e0.coordinates[0] << " " << e0.coordinates[1] << " " << e0.coordinates[2];

            points.push_back(e0); //add to points vector

            tris[i].midIndex[2]=points.size() - 1;
            qDebug() << "New Index: " << points.size();
        }else{
            tris[i].midIndex[2]= tris[tris[i].neighbours[2]].midIndex[2];
        }
    }
}

float OGLWidget::calculateBeta(int n){
    float alpha = 3/8 + (3/8 + 1/4 *cos(2* PI /n));
    float beta = 8/5*alpha - 3/5;
    return beta;
}

//perform subdivision algo - vertex mask
void OGLWidget::subdivisionVertex(QVector<Triangle> &tris, QVector<Vertex> &points){
    //Vector<Triangle>trisSubdivided; - in oglwidget.h
    for(int i =0; i <points.size(); i++){
        points[i].valence *= calculateBeta(points[i].valence);
    }

    for(int j =0; j <tris.size(); j++){
        Vertex a = points[tris[j].vertexIndex[0]];
        Vertex b = points[tris[j].vertexIndex[1]];
        Vertex c = points[tris[j].vertexIndex[2]];
        //a
        points[tris[j].vertexIndex[0]] += ( 1.0-calculateBeta(a.valence) / (a.valence) ) * ( points[tris[j].midIndex[1]] + points[tris[j].midIndex[2]] ) /2;
        //b
        points[tris[j].vertexIndex[1]] += ( 1.0-calculateBeta(b.valence) / (b.valence) ) * ( points[tris[j].midIndex[0]] + points[tris[j].midIndex[2]] ) /2;
        //c
        points[tris[j].vertexIndex[2]] += ( 1.0-calculateBeta(c.valence) / (c.valence) ) * ( points[tris[j].midIndex[0]] + points[tris[j].midIndex[1]] ) /2;
    }

    //replace tris
    //since every step adds 3 tris, save the initial size - or it will be endless
    int size = tris.size();
    for(int k =0; k <size; k++){
        //replace old
        Triangle tNew (tris[k].midIndex[1], tris[k].midIndex[0], tris[k].vertexIndex[2]);
        qDebug() << "New Tri: " << k << " " << tNew.vertexIndex[0] << " " << tNew.vertexIndex[1] << " " << tNew.vertexIndex[2];

        //add 3 new
        Triangle t1 (tris[k].midIndex[1], tris[k].midIndex[2], tris[k].midIndex[0]);
        Triangle t2 (tris[k].vertexIndex[0], tris[k].midIndex[2], tris[k].midIndex[1]);
        Triangle t3 (tris[k].midIndex[2], tris[k].vertexIndex[1], tris[k].midIndex[0]);
        tris.push_back(t1);
        tris.push_back(t2);
        tris.push_back(t3);

        tris[k] = tNew;
    }

}

void OGLWidget::resizeGL(int w, int h) // called when window size is changed
{
    // adjust viewport transform
    glViewport(0,0,w,h);
}

