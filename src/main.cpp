#include "rubik.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    rubik w;
    w.pos.first = (a.desktop()->width() - w.width()) / 2 - 200;
    w.pos.second = (a.desktop()->height() - w.height()) / 2;
    w.desktop = make_pair(a.desktop()->width(), a.desktop()->height());
    w.move(w.pos.first, w.pos.second);    
    //w.show(); 
    
    return a.exec();
}
