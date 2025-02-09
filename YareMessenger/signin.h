#ifndef SIGNIN_H
#define SIGNIN_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

#include <QSoundEffect>

#include "messengerwin.h"

namespace Ui {
class SignIn;
}

class SignIn : public QWidget
{
    Q_OBJECT

public:
    explicit SignIn(QWidget *parent = nullptr);

    QLabel* signIn;
    QLineEdit* enter_signIn;
    QLabel* password;
    QLineEdit* enter_pass;
    QLineEdit* confirm_pass;
    QPushButton* closeWindow;
    QPushButton* sendData;
    QPushButton* changeField;
    QVBoxLayout *Vlayout;

    QTcpSocket *socket;
    QJsonObject json;

    QLabel* info;

    QSoundEffect effect;

    void playSystemSound(const QString& soundName);

    ~SignIn();
private slots:
     void onServerResponse();


private:
    Ui::SignIn *ui;
};

#endif // SIGNIN_H
