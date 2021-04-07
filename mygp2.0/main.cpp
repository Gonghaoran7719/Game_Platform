#include "mygp.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mygp w;

   w.hide();
   // w.show();
    return a.exec();
}
