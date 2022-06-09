#include "dialogloading.h"
#include "ui_dialogloading.h"

DialogLoading::DialogLoading(JsonHandler *& _handler, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogLoading),
    jsonHandler(_handler),
    elapsedIntervals(0),
    elapsedIntervalsSafety(0)
{
    ui->setupUi(this);
    animation = "Loading... " + getEmojiFrameLoading(0);
    ui->LoadingLabel->setText(animation);
    ui->LoadingLabel->setVisible(false);
    ui->ButtonClose->setText("Connect");

    QObject::connect(jsonHandler, &JsonHandler::dataFromWebIsReady, this, &DialogLoading::onDataReady);

    // Setting timer
    timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, &DialogLoading::handleTimer);
    timer->start(100);
    startAnimation = closeState = false;
}

DialogLoading::~DialogLoading()
{
    delete ui;
}

void DialogLoading::on_ButtonClose_clicked()
{
    if(!closeState)
        startAnimation = true;

    if(startAnimation)
    {
        //QObject::connect(jsonHandler, &JsonHandler::dataFromWebIsReady, this, &DialogLoading::onDataReady);
        ui->LoadingLabel->setVisible(true);
        jsonHandler->setConnection(ui->lineEdit->text());
    }

    if(closeState)
        accept();
}

void DialogLoading::handleTimer()
{
    if(startAnimation)
    {
        elapsedIntervals = elapsedIntervals > 23 ? 0 : ++elapsedIntervals;
        animation = "Loading... " + getEmojiFrameLoading(elapsedIntervals);
        ui->LoadingLabel->setText(animation);
    }

    if(++elapsedIntervalsSafety > 50)
    {
        closeState = true;
        startAnimation = false;
        ui->LoadingLabel->setText("Unable to connect");
        ui->LoadingLabel->setVisible(true);
        ui->ButtonClose->setText("Close");
    }
}

void DialogLoading::onDataReady()
{
    closeState = true;
    startAnimation = false;
    ui->LoadingLabel->setText("Data loaded");
    ui->ButtonClose->setText("Close");
    timer->stop();
}

