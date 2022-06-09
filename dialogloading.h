#ifndef DIALOGLOADING_H
#define DIALOGLOADING_H

#include <QDialog>
#include <QTimer>
#include <QString>

#include "jsonhandler.h"
#include "emoji.h"

namespace Ui {
class DialogLoading;
}

class DialogLoading : public QDialog
{
    Q_OBJECT

public:
    explicit DialogLoading(JsonHandler *& _handler, QWidget *parent = nullptr);
    ~DialogLoading();

private slots:
    void on_ButtonClose_clicked();
    void handleTimer();
    void onDataReady();

private:
    Ui::DialogLoading *ui;
    JsonHandler *& jsonHandler;
    QTimer * timer;
    QString animation;
    size_t elapsedIntervals;
    size_t elapsedIntervalsSafety;
    bool closeState;
    bool startAnimation;
};

#endif // DIALOGLOADING_H
