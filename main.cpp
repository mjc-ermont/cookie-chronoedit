#include <QtGui>
#if QT_VERSION >= 0x050000
    #include <QtWidgets>
#endif

#include "chronoedit.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ChronoEdit w;
    w.show();
    
    return a.exec();
}
