#include "rubik.h"
#include "ui_rubik.h"

rubik::rubik(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::rubik)
{
    ui->setupUi(this);
    
    ui->pushButton_Pause->setVisible(false);
    ui->curColor->setVisible(false);
    ui->label_curColor->setVisible(false);
    
    state = new State();
    paint = new Paint();
    env = "production";
    
    QTimer *timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadGL()));  
    timer->start(1);  
}

rubik::~rubik()
{
    delete ui;
}

void rubik::loadGL()
{
    ui->pushButton_Show3D->setVisible(false);
    glW = new RubikGLWidget(state);
    glW->resize(400, 400);  
    glW->move(pos.first + width(), (desktop.second - glW->height()) / 2);
    renderFaces(state);
    glW->show();     
    show();
}

void rubik::renderFace(QLabel *graph, State *state, int fid)
{
    QPixmap tmap = QPixmap(graph->width(), graph->height());
    QPainter painter(&tmap);
    tmap.fill(Qt::black);
    
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
        {
            int cur = state->color[fid][j][i];
            QPen tPen(Qt::black);
            QBrush tBrush(Qt::black);
            painter.setPen(tPen);
        
            if (cur == white)
            {
                tPen.setColor(Qt::white);
                tBrush.setColor(Qt::white);
            }
            else if (cur == green)
            {
                tPen.setColor(QColor(0, 155, 72));
                tBrush.setColor(QColor(0, 155, 72));                
            }
            else if (cur == blue)
            {
                tPen.setColor(QColor(0, 70, 173));
                tBrush.setColor(QColor(0, 70, 173));                
            }
            else if (cur == red)
            {
                tPen.setColor(QColor(183, 18, 52));
                tBrush.setColor(QColor(183, 18, 52));                
            }
            else if (cur == orange)
            {
                tPen.setColor(QColor(255, 88, 0));
                tBrush.setColor(QColor(255, 88, 0));                
            }
            else if (cur == yellow)
            {
                tPen.setColor(QColor(255, 213, 0));
                tBrush.setColor(QColor(255, 213, 0));                
            }
            
            int widthPer = (graph->width() - 4 * sizeStroke) / 3;
            int heightPer = (graph->height() - 4 * sizeStroke) / 3;
            int x = widthPer * i + sizeStroke * (i + 1);
            int y = heightPer * j + sizeStroke * (j + 1);
            QRectF rect(x, y, widthPer, heightPer);
            painter.fillRect(rect, tBrush);
        }
   
    graph->setPixmap(tmap);
}

void rubik::renderFaces(State *state)
{
    renderFace(ui->graphLeft, state, faceL);    
    renderFace(ui->graphFront, state, faceF);
    renderFace(ui->graphRight, state, faceR);
    renderFace(ui->graphBack, state, faceB);
    renderFace(ui->graphTop, state, faceU);
    renderFace(ui->graphBottom, state, faceD);
    glW->state = state;
    glW->updateGL();
}

void rubik::updateResult()
{
    ui->curColor->setVisible(false);    
    ui->pushButton_ViewSolution->setText("View Solution");
    ui->pushButton_ViewSolution->repaint();    
    ui->pushButton_Scramble->setText("Thinking...");
    ui->pushButton_ViewSolution->setEnabled(false);
    ui->pushButton_Scramble->setEnabled(false);
    ui->pushButton_Input->setEnabled(false);
    ui->pushButton_Play->setEnabled(false);
    ui->pushButton_Repaint->setEnabled(false);
    ui->pushButton_Pause->setVisible(false);
    ui->pushButton_Play->setVisible(true);    
    ui->pushButton_Scramble->repaint();
    ui->slider_Progress->setMaximum(0);
    ui->slider_Progress->setValue(0);    
    qApp->processEvents();    
    Solver *solver = new Solver();
    result.clear();
    int finished = true;
    for (int k = 0; k < 6; ++k)
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                if (state->color[k][i][j] != k)
                    finished = false;
    stateSteps.clear();
    steps.clear();    
    
    if (!finished)
    {
        result = solver->solve(*state);     
        // error happened
        if (result.size() == 1)
        {
            ui->slider_Progress->setMaximum(0);
            ui->pushButton_Scramble->setText("Scramble");
            ui->pushButton_Scramble->setEnabled(true);
            ui->pushButton_ViewSolution->setEnabled(true);
            ui->pushButton_Play->setEnabled(true);
            ui->pushButton_Repaint->setEnabled(true);
            ui->pushButton_Input->setEnabled(true);
            ui->pushButton_Scramble->repaint();  
            QMessageBox::warning(this, "Error", "It seems your input is invalid!");
            return;
        }
    }
        
    State *tmp = new State();
    *tmp = *state;
    stateSteps.push_back(*tmp);
    for (auto i:result)
        for (auto j:i)
        {
            tmp->rotate(j);
            stateSteps.push_back(*tmp);
            steps.push_back(j);
        }
    ui->slider_Progress->setMaximum(stateSteps.size() - 1);
    ui->pushButton_Scramble->setText("Scramble");
    ui->pushButton_Scramble->setEnabled(true);
    ui->pushButton_ViewSolution->setEnabled(true);
    ui->pushButton_Play->setEnabled(true);
    ui->pushButton_Repaint->setEnabled(true);
    ui->pushButton_Input->setEnabled(true);
    ui->pushButton_Scramble->repaint();
    qApp->processEvents();
    delete tmp;
    on_pushButton_ViewSolution_clicked();
}

void rubik::on_pushButton_Scramble_clicked()
{
    if (ui->pushButton_Scramble->text() == "Scramble")
    {
        state->random(1000);
    }
    else if (ui->pushButton_Scramble->text() == "Reset")
    {
        state->reset();
        paint->active = false;        
        ui->curColor->setVisible(false);
        ui->label_curColor->setVisible(false);          
    }
    renderFaces(state);          
    updateResult();   
}

void rubik::on_pushButton_Play_clicked()
{
    ui->pushButton_Play->setVisible(false);
    ui->pushButton_Pause->setVisible(true);
    qApp->processEvents();
    if (ui->slider_Progress->value() == ui->slider_Progress->maximum())
        ui->slider_Progress->setValue(0);
    while (ui->pushButton_Pause->isVisible() && ui->slider_Progress->value() < ui->slider_Progress->maximum())
    {
        int delay;
        if (ui->slider_Speed->value() * 3 < ui->slider_Speed->maximum())
            delay = 5 * (ui->slider_Speed->maximum() - ui->slider_Speed->value());
        else if (ui->slider_Speed->value() * 5 / 4 < ui->slider_Speed->maximum())
            delay = ui->slider_Speed->maximum() - ui->slider_Speed->value();
        else 
            delay = (ui->slider_Speed->maximum() - ui->slider_Speed->value()) / 2;
        glW->performRotate(steps[ui->slider_Progress->value()], delay);
        ui->slider_Progress->setValue(ui->slider_Progress->value() + 1);
        renderFaces(&stateSteps[ui->slider_Progress->value()]);
        qApp->processEvents();        
    }
    ui->pushButton_Pause->setVisible(false);
    ui->pushButton_Play->setVisible(true);
}

void rubik::on_pushButton_Pause_clicked()
{
    ui->pushButton_Pause->setVisible(false);
    ui->pushButton_Play->setVisible(true);
    qApp->processEvents();
}

void rubik::on_pushButton_ViewSolution_clicked()
{
    if (ui->pushButton_ViewSolution->text() == "Resume")
    {
        updateResult();
    }
    else
    {   
        if (env == "homework")
        {
            env = "production";
            CustomInputDialog *dialog = new CustomInputDialog();
            string res = "";
            for (int i = 0; i < 3; ++i)
                for (auto j:result[i])
                {
                    string cur = j;
                    if (cur[0] == 'U') cur[0] = originalFace['B'];
                    else if (cur[0] == 'D') cur[0] = originalFace['G'];
                    else if (cur[0] == 'L') cur[0] = originalFace['O'];
                    else if (cur[0] == 'R') cur[0] = originalFace['R'];
                    else if (cur[0] == 'F') cur[0] = originalFace['W'];
                    else if (cur[0] == 'B') cur[0] = originalFace['Y'];
                    res += cur;
                    res += ' ';
                }
            res += "\n";
            dialog->output(res);
        }
        else
        {
            string res = "";
            int cnt = 0;
            if (result.size() < 3)
            {
                QMessageBox::information(this, "View Solution", "Solution is empty!");
                return;
            }
            for (int i = 0; i < 3; ++i)
            {
                cnt += result[i].size();
                res += "<p><b>Steps for layer " + to_string(i + 1) + "</b><br><a style=\"font-weight:normal;\">";
                for (auto j:result[i])
                    res += j, res += ' ';
                res += "</a></p>";
            }
            res += "<p>" + to_string(cnt) + " steps in total</p>";
            QMessageBox::information(this, "View Solution", res.c_str());
        }
    }
}

void rubik::on_slider_Progress_sliderMoved(int position)
{
    renderFaces(&stateSteps[position]); 
}

void rubik::on_pushButton_Repaint_clicked()
{
    paint->active = false;
    ui->pushButton_ViewSolution->setEnabled(false);
    ui->pushButton_Play->setEnabled(false);
    ui->pushButton_Pause->setVisible(false);
    ui->pushButton_Play->setVisible(true);  
    ui->pushButton_Input->setEnabled(false);
    ui->label_curColor->setVisible(true);
    ui->curColor->setVisible(true);
    ui->pushButton_Scramble->setText("Reset");
    ui->slider_Progress->setMaximum(0);
    ui->slider_Progress->setValue(0);
    
    for (int k = 0; k < 6; ++k)
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                if (i != 1 || j != 1)
                    state->color[k][i][j] = white;
    renderFaces(state);
    
    paint->reset();

    ui->curColor->setStyleSheet("font-size: 40px; padding-left: 9px; color: white; border: 5px solid black; background:" + cssColor::red);    
    ui->curColor->setText(QString::number(paint->remain));
}

int rubik::paintFace(QLabel *graph, int fid, int x, int y)
{
    int x1 = graph->pos().x();
    int y1 = graph->pos().y();
    int widthPer = (graph->width() - 4 * sizeStroke) / 3;
    int heightPer = (graph->height() - 4 * sizeStroke) / 3;    
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
        {
            if (i == 1 && j == 1) continue;
            int curx = widthPer * j + sizeStroke * (j + 1);
            int cury = heightPer * i + sizeStroke * (i + 1);  
            if (x >= x1 + curx && y >= y1 + cury && x <= x1 + curx + widthPer 
                    && y <= y1 + cury + heightPer && state->color[fid][i][j] == white)
            {
                state->color[fid][i][j] = paint->color;
                renderFaces(state);
                paint->next();
                return true;
            }
        }
    return false;
}

Paint::Paint()
{
    active = false;
}

void Paint::reset()
{
    active = true;
    color = red;
    remain = 8;
}

int Paint::next()
{
    if (remain > 1) 
    {
        remain--;
        return 0;
    }
    else
    {
        if (color == red) color = yellow;
        else if (color == yellow) color = orange;
        else if (color == orange) color = blue;
        else if (color == blue) color = green;
        else 
        {
            active = false;
            return 1;
        }
        remain = 8;
        return 0;
    }
}

void rubik::mousePressEvent(QMouseEvent *event)
{
    if (paint->active)
    {
        int x = event->x(), y = event->y();
        if (!paintFace(ui->graphFront, faceF, x, y))
        if (!paintFace(ui->graphBack, faceB, x, y))
        if (!paintFace(ui->graphLeft, faceL, x, y))
        if (!paintFace(ui->graphRight, faceR, x, y))
        if (!paintFace(ui->graphTop, faceU, x, y))
        paintFace(ui->graphBottom, faceD, x, y);
        
        int color = paint->color;
        QString css;
        if (color == red) css = cssColor::red;
        else if (color == green) css = cssColor::green;
        else if (color == blue) css = cssColor::blue;
        else if (color == orange) css = cssColor::orange;
        else if (color == yellow) css = cssColor::yellow;
        ui->curColor->setStyleSheet("font-size: 40px; padding-left: 9px; color: white; border: 5px solid black; background:" + css);
        ui->curColor->setText(QString::number(paint->remain));
       
        // finished repainting
        if (!paint->active)
        {
            ui->pushButton_ViewSolution->setEnabled(true);
            ui->pushButton_Scramble->setText("Scramble");
            ui->pushButton_Play->setEnabled(true); 
            ui->curColor->setVisible(false);
            ui->label_curColor->setVisible(false); 
            
            updateResult();
        }
        qApp->processEvents();
    }
}

void rubik::closeEvent (QCloseEvent *event)
{
    Q_UNUSED(event);
    qApp->quit();
}

void rubik::on_pushButton_Input_clicked()
{
    CustomInputDialog *dialog = new CustomInputDialog();
    dialog->exec();
    if (dialog->buffer.toStdString().size() > 500) 
    {
        QMessageBox::warning(this, "Error", "Sorry, your input is too long!");
        return;
    }
    if (dialog->buffer.toStdString().size() == 0) 
        return;
    char buffer[1000];
    sprintf(buffer, "%s", dialog->buffer.toStdString().c_str());
    char f[6][1000];
    sscanf(buffer, "%s%s%s%s%s%s", f[0], f[1], f[2], f[3], f[4], f[5]);
    
    for (int i = 0; i < 6; ++i)
        for (int j = 0; j < 3; ++j)
            for (int k = 0; k < 3; ++k)
                state->color[i][j][k] = black;
    
    setFace(faceF, f[0]);
    setFace(faceB, f[1]);
    setFace(faceR, f[2]);
    setFace(faceL, f[3]);
    setFace(faceU, f[4]);
    setFace(faceD, f[5]);
    
    originalFace[f[0][4]] = 'F';
    originalFace[f[1][4]] = 'B';
    originalFace[f[2][4]] = 'R';
    originalFace[f[3][4]] = 'L';
    originalFace[f[4][4]] = 'U';
    originalFace[f[5][4]] = 'D';
    
    adjustFaces();
    
    env = "homework";
    renderFaces(state);          
    updateResult();      
}

void rubik::adjustFaces()
{
    if (state->color[faceF][1][1] == green)
        state->rotateH();  
    if (state->color[faceL][1][1] == green) 
        state->rotateH();    
    if (state->color[faceB][1][1] == green)
        state->rotateH();
    if (state->color[faceU][1][1] == green)
        state->rotateV();
    if (state->color[faceR][1][1] == green)
        state->rotateV();
    for (int i = 0; i < 4 && state->color[faceF][1][1] != white; ++i)
        state->rotateH();
}

void rubik::setFace(int fid, char *input)
{
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            for (int k = 0; k < 6; ++k)
                if (input[i * 3 + j] == colorString[k][0])
                    state->color[fid][i][j] = k;
}
