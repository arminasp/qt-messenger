// path to common includes
// #include "/Messenger/common_includes/std.h"

#include <QtWidgets/QApplication>

#include "window.h"
#include "user.h"
#include "connection.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Window w;
    w.show();

    return a.exec();
}
