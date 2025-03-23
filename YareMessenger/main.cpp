#include "messengerwin.h"
#include "login.h"
#include "signin.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    boost::asio::io_context context;
    SignIn startedWin(nullptr,context);

    std::thread ioThread([&context]() {
        context.run();
    });


    startedWin.setWindowFlag(Qt::FramelessWindowHint);
    startedWin.show();

    int result = a.exec();
    context.stop();
    if (ioThread.joinable()) ioThread.join();

    return a.exec();
}
