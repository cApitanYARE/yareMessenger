#ifndef SIGNIN_H
#define SIGNIN_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

#include <QSoundEffect>

#include "messengerwin.h"

#include <boost/asio.hpp>


namespace Ui {
class SignIn;
}

class SignIn : public QWidget
{
    Q_OBJECT

public:
    explicit SignIn(QWidget *parent = nullptr,boost::asio::io_context& context = *(new boost::asio::io_context()));

    QLabel* signIn;
    QLineEdit* enter_signIn;

    QLabel* password;
    QLineEdit* enter_pass;

    QLineEdit* confirm_pass;

    QPushButton* closeWindow;
    QPushButton* sendData;
    QPushButton* changeField;

    QVBoxLayout *Vlayout;

    QJsonObject json;

    QLabel* info;

    std::string jsonData;

    QSoundEffect effect;
    void playSystemSound(const QString& soundName);

    ~SignIn();
private slots:
     void ServerResponse();
private:
    //asio
    boost::asio::io_context& io;
    std::shared_ptr<boost::asio::ip::tcp::socket> socket;
    boost::asio::streambuf bufferFromServer;
    boost::asio::steady_timer timer;

    Ui::SignIn *ui;
};

#endif // SIGNIN_H
