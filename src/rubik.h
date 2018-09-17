#ifndef RUBIK_H
#define RUBIK_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QInputDialog>
#include <QPainter>
#include <QMouseEvent>
#include <QThread>
#include <QPixmap>
#include <QTimer>
#include <QRect>
#include <QRectF>
#include <string>
#include <vector>
#include <ctime>
#include <iostream>

#include "state.h"
#include "solver.h"
#include "rubikgl.h"
#include "custominputdialog.h"

using namespace std;

const int sizeStroke = 5;

namespace cssColor
{
    const QString red = "#b71234";
    const QString green = "#009b48";
    const QString blue = "#0046ad";
    const QString orange = "#ff5800";
    const QString yellow = "#ffd500";
    const QString white = "#000000";
}

namespace Ui {
class rubik;
}

class Paint
{
public:
    
    int active, color, remain;
    
    Paint();
    
    void reset();
    int next();
};

class rubik : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit rubik(QWidget *parent = 0);
    ~rubik();
    
    pair<int, int> pos, desktop;    
    
    void renderFace(QLabel *graph, State *state, int fid);
    
    void renderFaces(State *state);
    
    void updateResult();
    
    int paintFace(QLabel *graph, int fid, int x, int y);
    
    void setFace(int fid, char *input);
    
private slots:
    
    void on_pushButton_Scramble_clicked();
    
    void on_pushButton_Play_clicked();
    
    void on_pushButton_ViewSolution_clicked();
    
    void on_pushButton_Pause_clicked();
    
    void on_slider_Progress_sliderMoved(int position);
    
    void on_pushButton_Repaint_clicked();
    
    void loadGL();
    
    void adjustFaces();
    
    void on_pushButton_Input_clicked();
    
protected:
    
    void mousePressEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);
    
private:
    Ui::rubik *ui;
    Paint *paint;
    State *state;
    RubikGLWidget *glW;
    string env; // environment
    char originalFace[1000];
    
    vector<vector<string> > result;
    vector<State> stateSteps;
    vector<string> steps;

};

#endif // RUBIK_H
