#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QProgressBar>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QFileDialog>
#include <QCheckBox>
#include <QRadioButton>
#include <QVBoxLayout>

#include "jsonhandler.h"
#include "quizstorage.h"
#include "dialogloading.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_pushButtonStartQuiz_clicked();

    void onResultFromFileDialog(const QUrl& _url);
    void onResultFromWebDialog();
    void handleTimer();
    void displayQuestionWidgetOneFromMany();
    void displayQuestionWidgetManyFromMany();
    void displaySummary();
    void handleFileInput();
    void handleFileInputWeb();
    void handleNextQuestion(const QuestionType & _type);

    void on_loadFile_clicked();
    void on_ButtonNextQuestion_clicked();
    void on_ButtonNextQuestionPrim_clicked();
    void on_ButtonReturn_clicked();
    void on_LoadFileWeb_clicked();

private:
    Ui::MainWindow *ui;
    QTimer * timer;
    QFileDialog * dialog;
    QVector<QCheckBox*> answersCheckBoxes;
    QVector<QRadioButton*> answersRadioButtons;
    QVBoxLayout * verticalLayout;
    QVBoxLayout * verticalLayoutPrim;
    QString filePath;
    QVector<QPair<QString, float>> finalAnswers; // TODO zrobic coś bardziej szczegółowego

    QuizStorage storage;
    QuizStorage storageCurrentCategory;
    size_t AppElapsedTime;
    size_t currentQuestionIndex;
    bool quizIsActive;
    JsonHandler * jsonHandler;

    void handleError(const QString & _message);
    void allocateAttributes();
    void updateMainComboBox();

    template<typename T>
    void free(QVector<T> & _vec);
};
#endif // MAINWINDOW_H
