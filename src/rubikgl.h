#ifndef RUBIKGL
#define RUBIKGL

#include <QWidget>  
#include <QGLWidget>  
#include <QKeyEvent>
#include "state.h"

// GL libraries
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#endif
#ifdef _WIN32
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#endif
#ifdef __linux__
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

class Cell
{
public:
    int face, x, y;
    Cell(){}
    Cell(int _face, int _x, int _y): face(_face), x(_x), y(_y){}
};

enum mode
{
    mode_x, mode_y, mode_z
};
  
class RubikGLWidget : public QGLWidget  
{  
    Q_OBJECT 
    
public:  
    explicit RubikGLWidget(State *_state);  
    ~RubikGLWidget();  
    
    void performRotate(string r, int delay);
    void performRotateView(GLfloat x, GLfloat y, GLfloat z, int delay);
    
    State *state;
    
protected:  
    void initializeGL();  
    void resizeGL(int w, int h);  
    void paintGL();  
      
    void keyPressEvent(QKeyEvent *event); 
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
      
private:  
    bool fullscreen;
    mode renderMode;
    GLfloat rotateDelta, rotateViewX, rotateViewY, rotateViewZ;
    int rotateLayer;
    QPoint lastPos;
    
    void setColor(int color);
    void drawRect(double sx, double sy, double sz, GLfloat x, GLfloat y, GLfloat w, GLfloat h, int color);
    void drawCell(int face, int x, int y);
};  

#endif // RUBIKGL

