#ifndef YARELOGANDREG_H
#define YARELOGANDREG_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace Ui {
class YareLogAndReg;
}

class YareLogAndReg : public QWidget
{
    Q_OBJECT

public:
    explicit YareLogAndReg(QWidget *parent = nullptr);

    QLabel* signIn;
    QLineEdit* enter_signIn;

    QLabel* login;
    QLineEdit* enter_log;

    QLabel* password;
    QLineEdit* enter_pass;
    QLineEdit* confirm_pass;

    QPushButton* closeWindow;
    QPushButton* sendData;
    QPushButton* changeField;

    QVBoxLayout *Vlayout;
    QHBoxLayout *Hlayout;

    ~YareLogAndReg();

private slots:
private:
    Ui::YareLogAndReg *ui;
};

#endif // YARELOGANDREG_H
