#include "rubikgl.h"
#include "ui_rubik.h"
#include <QThread>
#include <QMessageBox>
#include <QTimer>
#include <QElapsedTimer>
#include <iostream>
#include <cmath>
#include <algorithm>

RubikGLWidget::RubikGLWidget(State *_state)
{  
    state = _state;
    renderMode = mode_x;
    rotateDelta = 0;   
    rotateViewX = 28; 
    rotateViewY = -45; 
    rotateViewZ = 0;
    lastPos = QPoint(-1, -1);
}  
  
RubikGLWidget::~RubikGLWidget()  
{  
    close();  
}  

void RubikGLWidget::initializeGL()     
{  
    glClearColor(255, 255, 255, 0.0); 
    glShadeModel(GL_SMOOTH);                            
    glClearDepth(1.0);               
    glEnable(GL_DEPTH_TEST);        
    glEnable(GL_POINT_SMOOTH);  
    glEnable(GL_LINE_SMOOTH); 
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);     
    glEnable(GL_MULTISAMPLE_ARB); 
    glDepthFunc(GL_LEQUAL);  
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); 
    glHint(GL_POLYGON_SMOOTH, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);     
}  

void GLUPerspective(GLfloat fovY, GLfloat aspect, GLfloat zNear, GLfloat zFar )
{
    GLfloat fH = tan(fovY / 360 * M_PI) * zNear;
    GLfloat fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}

void RubikGLWidget::resizeGL(int w, int h) 
{  
    glViewport(0, 0, (GLint)w, (GLint)h);       
    glMatrixMode(GL_PROJECTION);                
    glLoadIdentity();                        
    GLUPerspective(35.0, (GLfloat)w/(GLfloat)h, 0.1, 100.0);  
    glMatrixMode(GL_MODELVIEW);                     
    glLoadIdentity();    
}  

void RubikGLWidget::paintGL()  
{  
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
    
    GLfloat strike = 0.05;
    GLfloat w = (2 - strike * 4) / 3;
    
    vector<Cell> cells[3];
    
    GLfloat rx = 0, ry = 0, rz = 0;

    if (renderMode == mode_x)
    {
        rx = -1;
        for (int k = 0; k < 3; ++k)
        {
            if (k == 0)
            {
                for (int i = 0; i < 3; ++i) 
                    for (int j = 0; j < 3; ++j)
                        cells[k].push_back(Cell(faceR, i, j));
            }
            if (k == 2)
            {
                for (int i = 0; i < 3; ++i)
                    for (int j = 0; j < 3; ++j)
                        cells[k].push_back(Cell(faceL, i, j));
            }
            for (int i = 0; i < 3; ++i) 
                cells[k].push_back(Cell(faceF, i, 2 - k));  
            for (int i = 0; i < 3; ++i) 
                cells[k].push_back(Cell(faceB, i, k));   
            for (int i = 0; i < 3; ++i)
                cells[k].push_back(Cell(faceU, i, 2 - k));
            for (int i = 0; i < 3; ++i)
                cells[k].push_back(Cell(faceD, i, 2 - k));
        }
    }    
    else if (renderMode == mode_y)
    {
        ry = -1;
        for (int k = 0; k < 3; ++k)
        {
            if (k == 0)
            {
                for (int i = 0; i < 3; ++i) 
                    for (int j = 0; j < 3; ++j)
                        cells[k].push_back(Cell(faceU, i, j));
            }
            if (k == 2)
            {
                for (int i = 0; i < 3; ++i)
                    for (int j = 0; j < 3; ++j)
                        cells[k].push_back(Cell(faceD, i, j));
            }
            for (int i = 0; i < 3; ++i) 
                cells[k].push_back(Cell(faceF, k, i));    
            for (int j = 0; j < 3; ++j)
                cells[k].push_back(Cell(faceR, k, 2 - j));
            for (int j = 0; j < 3; ++j)
                cells[k].push_back(Cell(faceL, k, j));
            for (int i = 0; i < 3; ++i) 
                cells[k].push_back(Cell(faceB, k, 2 - i));        
        }
    }
    else if (renderMode == mode_z)
    {
        rz = -1;
        for (int k = 0; k < 3; ++k)
        {
            if (k == 0)
            {
                for (int i = 0; i < 3; ++i) 
                    for (int j = 0; j < 3; ++j)
                        cells[k].push_back(Cell(faceF, i, j));
            }
            if (k == 2)
            {
                for (int i = 0; i < 3; ++i)
                    for (int j = 0; j < 3; ++j)
                        cells[k].push_back(Cell(faceB, i, j));
            }
            for (int j = 0; j < 3; ++j)
                cells[k].push_back(Cell(faceU, 2 - k, j)); 
            for (int i = 0; i < 3; ++i)
                cells[k].push_back(Cell(faceR, i, k));
            for (int i = 0; i < 3; ++i)
                cells[k].push_back(Cell(faceL, i, 2 - k));
            for (int j = 0; j < 3; ++j)
                cells[k].push_back(Cell(faceD, k, j));
        }        
    }
   
    
    for (int k = 0; k < 3; ++k)
    {
        glLoadIdentity();
        glTranslatef(0, 0.2, -6); 
        glRotatef(rotateViewX, 1, 0, 0);
        glRotatef(rotateViewY, 0, 1, 0);
        glRotatef(rotateViewZ, 0, 0, 1);
        if (k == rotateLayer)
            glRotatef(rotateDelta, rx, ry, rz);    
        glBegin(GL_QUADS);
            if (k > 0)
            {
                GLfloat h = 1 - (strike + w) * k;
                drawRect(-rx * h, -ry * h, -rz * h, -1, -1, 2, 2, black);
            }
            if (k < 2)
            {
                GLfloat h = 1 - (strike + w) * (k + 1);
                drawRect(-rx * h, -ry * h, -rz * h, -1, -1, 2, 2, black);                
            }
            for (auto c:cells[k])
                drawCell(c.face, c.x, c.y);
        glEnd();  
    }
}  

void RubikGLWidget::drawRect(double sx, double sy, double sz, GLfloat x, GLfloat y, GLfloat w, GLfloat h, int color)
{
    setColor(color);
    if (fabs(sx) > 0.1)
    {
        glVertex3f(sx, x, y);
        glVertex3f(sx, x, y + h);
        glVertex3f(sx, x + w, y + h);
        glVertex3f(sx, x + w, y);
    }    
    if (fabs(sy) > 0.1)
    {
        glVertex3f(x, sy, y);
        glVertex3f(x, sy, y + h);
        glVertex3f(x + w, sy, y + h);
        glVertex3f(x + w, sy, y);
    }
    if (fabs(sz) > 0.1)
    {
        glVertex3f(x, y, sz);
        glVertex3f(x, y + h, sz);
        glVertex3f(x + w, y + h, sz);
        glVertex3f(x + w, y, sz);
    }    
}

void RubikGLWidget::drawCell(int face, int i, int j)
{
    GLfloat strike = 0.05;
    GLfloat w = (2 - strike * 4) / 3;  
    GLfloat x = 0, y = 0;
    double sx = 0, sy = 0, sz = 0;
    
    if (face == faceU)
        x = -1 + j * (strike + w), y = -1 + i * (strike + w), sy = 1;
    else if (face == faceF)
        x = -1 + j * (strike + w), y = -1 + (2 - i) * (strike + w), sz = 1;  
    else if (face == faceR)
        x = -1 + (2 - i) * (strike + w), y = -1 + (2 - j) * (strike + w), sx = 1;  
    else if (face == faceL)
        x = -1 + (2 - i) * (strike + w), y = -1 + j * (strike + w), sx = -1;  
    else if (face == faceB)
        x = -1 + (2 - j) * (strike + w), y = -1 + (2 - i) * (strike + w), sz = -1;  
    else if (face == faceD)
        x = -1 + j * (strike + w), y = -1 + (2 - i) * (strike + w), sy = -1;
    
    drawRect(sx, sy, sz, x + strike, y + strike, w, w, state->color[face][i][j]);
    drawRect(sx, sy, sz, x, y, strike, w + 2 * strike, black);
    drawRect(sx, sy, sz, x + strike + w, y, strike, w + 2 * strike, black);
    drawRect(sx, sy, sz, x, y, w + 2 * strike, strike, black);
    drawRect(sx, sy, sz, x, y + strike + w, w + 2 * strike, strike, black);
}

void RubikGLWidget::keyPressEvent(QKeyEvent *event)  
{  
    int delay = 200;
    switch (event->key())   
    {   
        case Qt::Key_F1:  
            fullscreen = !fullscreen;  
            if (fullscreen)  
            {  
                showFullScreen();  
            }  
            else  
            {  
                showNormal();  
            }  
            updateGL();
            break; 
        case Qt::Key_Left:
            performRotateView(0, 15, 0, delay);
            break;
        case Qt::Key_Right:
            performRotateView(0, -15, 0, delay);
            break;
        case Qt::Key_Up:
            performRotateView(15, 0, 0, delay);        
            break;
        case Qt::Key_Down:
            performRotateView(-15, 0, 0, delay);          
            break;
    }   
}  

void RubikGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void RubikGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() && Qt::LeftButton)
    {
        int dx = event->x() - lastPos.x();
        int dy = event->y() - lastPos.y();
        lastPos = event->pos();
        performRotateView(dy, dx, 0, 0);
    }
}

void RubikGLWidget::setColor(int color)
{
    if (color == red) glColor3f(183 / 255., 18 / 255., 52 / 255.); 
    else if (color == orange) glColor3f(255 / 255., 88 / 255., 0);
    else if (color == yellow) glColor3f(255 / 255., 213 / 255., 0);
    else if (color == blue) glColor3f(0, 70 / 255., 173 / 255.);
    else if (color == green) glColor3f(0, 155 / 255., 72 / 255.);
    else if (color == white) glColor3f(255 / 255., 255 / 255., 255 / 255.);
    else if (color == black) glColor3f(0, 0, 0);
}

void RubikGLWidget::performRotate(string r, int delay)
{
    int D = 0;
    rotateLayer = -1;

    if (r[0] == 'L' || r[0] == 'R') renderMode = mode_x;    
    else if (r[0] == 'U' || r[0] == 'D') renderMode = mode_y;
    else if (r[0] == 'F' || r[0] == 'B') renderMode = mode_z;
    
    if (r == "U" || r == "F" || r == "R") D = 1, rotateLayer = 0;
    else if (r == "Ui" || r == "Fi" || r == "Ri") D = -1, rotateLayer = 0;
    
    else if (r == "D" || r == "B" || r == "L") D = -1, rotateLayer = 2;
    else if (r == "Di" || r == "Bi" || r == "Li") D = 1, rotateLayer = 2;
    
    QElapsedTimer *timer = new QElapsedTimer();
    timer->start();
    GLfloat d = rotateDelta;
    while (timer->elapsed() < delay)
    {
        double percentage = timer->elapsed() * 1. / delay;
        rotateDelta = d + D * 90 * percentage;
        updateGL();
        qApp->processEvents(); 
    }
    rotateDelta = 0;
    delete timer;
}

void RubikGLWidget::performRotateView(GLfloat x, GLfloat y, GLfloat z, int delay)
{
    QElapsedTimer *timer = new QElapsedTimer();
    timer->start();
    GLfloat cx = rotateViewX;
    GLfloat cy = rotateViewY;
    GLfloat cz = rotateViewZ;
    while (timer->elapsed() < delay)
    {
        double percentage = timer->elapsed() * 1. / delay;
        rotateViewX = cx + percentage * x;
        rotateViewY = cy + percentage * y;
        rotateViewZ = cz + percentage * z;
        updateGL();
        qApp->processEvents(); 
    }
    rotateViewX = cx + x;
    rotateViewY = cy + y;
    rotateViewZ = cz + z;
    updateGL();
    delete timer;
}
