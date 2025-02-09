#include "yarelogandreg.h"
#include "ui_yarelogandreg.h"

YareLogAndReg::YareLogAndReg(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::YareLogAndReg)
{
    ui->setupUi(this);

    this->setStyleSheet("background-color:#dcdcdc;");

    Vlayout = new QVBoxLayout(this);
    Hlayout = new QHBoxLayout(this);

    Vlayout->setContentsMargins(0, 0, 0, 0);

    closeWindow = new QPushButton("CLOSE");
    closeWindow->setObjectName("btnClose");

    closeWindow->setCursor(Qt::PointingHandCursor);

    //styleSheet
    {
    this->setStyleSheet("#Title{"
                        "font-size:25px;"
                        "color:#5d5b5a;"
                        "border-radius:1px;"
                        "}"
    //////////////////////////////////////
                        "#EnterLine{"
                        "background-color:#cacaca;"
                        "min-height:25px;"
                        "max-height:25px;"
                        "font-size:18px;"
                        "color:#5d5b5a;"
                        "border-radius:1px;"
                        "margin:0px 10px;"
                        "}"
   ///////////////////////////////////////
                        "#SendData{"
                        "background-color:#929292;"
                        "color:white;"
                        "min-width:100px;"
                        "padding:5px;"
                        "border: 1px solid #5d5b5a;"
                        "border-radius:3px;"
                        "margin-top:20px;"
                        "}"

                        "#SendData:hover{"
                        "background-color:#5d5b5a;"
                        "}"
    ///////////////////////////////////////
                        "#changeField{"
                        "background-color:#8ab2fc;"
                        "color:white;"
                        "font-weight:bold;"
                        "min-width:30px;"
                        "border: 1px solid #77a7ff;"
                        "border-radius:1px;"
                        "margin:0px 10px;"
                        "}"
    ///////////////////////////////////////
                        "#btnClose{"
                        "background-color:#ff6b6b;"
                        "color:white;"
                        "margin:0;"
                        "padding:0;"
                        "min-width:300px;"
                        "min-height:20px;"
                        "border: none;"
                        "font-weight:bold;"
                        "letter-spacing:5px;"
                        "}"
                        "#btnClose:hover{"
                        "background-color:#ff4747;"
                        "}"
                        );
    }

    if(true){

        login = new QLabel("Log In");
        login->setObjectName("Title");
        login->setAlignment(Qt::AlignHCenter);

        enter_log = new QLineEdit();
        enter_log->setObjectName("EnterLine");
        enter_log->setPlaceholderText("enter this area");

        password = new QLabel("Password");
        password->setObjectName("Title");
        password->setAlignment(Qt::AlignHCenter);

        enter_pass = new QLineEdit();
        enter_pass->setObjectName("EnterLine");
        enter_pass->setPlaceholderText("enter this area");

        sendData = new QPushButton("Confirm");
        sendData->setObjectName("SendData");
        sendData->setCursor(Qt::PointingHandCursor);


        changeField = new QPushButton("Sign Up");
        changeField->setObjectName("changeField");
        changeField->setCursor(Qt::PointingHandCursor);


        Vlayout->addWidget(closeWindow);
        Vlayout->addWidget(login);
        Vlayout->addWidget(enter_log);
        Vlayout->addWidget(password);
        Vlayout->addWidget(enter_pass);
        Vlayout->addWidget(sendData);
        Vlayout->addWidget(changeField);
        Vlayout->setSpacing(20);
        Vlayout->setAlignment(Qt::AlignTop);
        Vlayout->setAlignment(sendData, Qt::AlignHCenter);
        Vlayout->setAlignment(changeField, Qt::AlignBottom | Qt::AlignRight);
        Hlayout->addLayout(Vlayout);

    }else{
        signIn = new QLabel("Sign Up");
        enter_signIn = new QLineEdit("Password");

        password = new QLabel("Password");
        enter_pass = new QLineEdit("Enter");
        confirm_pass = new QLineEdit("confitm password");

        sendData = new QPushButton("Confirm");
    }

    connect(closeWindow, &QPushButton::clicked, this, [this] {
        this->close();
    });
    connect(changeField, &QPushButton::clicked, this, [this] {

    });
}


YareLogAndReg::~YareLogAndReg() {
    delete ui;
}


