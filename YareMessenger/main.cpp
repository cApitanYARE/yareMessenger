#include "messengerwin.h"
#include "login.h"
#include "signin.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LogIn startedWin;
    startedWin.setWindowFlag(Qt::FramelessWindowHint);
    startedWin.show();

    return a.exec();
}
