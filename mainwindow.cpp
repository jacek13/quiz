#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QGuiApplication>
#include <QScreen>
#include <QPushButton>
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QThread>
#include <exception>

#include "emoji.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    AppElapsedTime(0),
    currentQuestionIndex(0),
    quizIsActive(false)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->pageMain);
    ui->comboBoxCategorySelection->setVisible(false);
    // move(QGuiApplication::screens().at(0)->geometry().center() - frameGeometry().center()); // TODO to dziala ale powinno być wywaołane w innym miejscach

    allocateAttributes();

    QObject::connect(timer, &QTimer::timeout, this, &MainWindow::handleTimer);
    QObject::connect(jsonHandler, &JsonHandler::dataFromWebIsReady, this, &MainWindow::onResultFromWebDialog);

#ifdef __wasm__
    jsonHandler->loadJsonFromFile("resources/questions.json");
    storage = jsonHandler->getParsedQuizData();
    updateMainComboBox();
    ui->comboBoxCategorySelection->setVisible(true);
#else
    QObject::connect(dialog, &QFileDialog::urlSelected, this, &MainWindow::onResultFromFileDialog);
#endif

    timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::allocateAttributes()
{
    jsonHandler = new JsonHandler(this);
    timer = new QTimer(this);
    verticalLayout = new QVBoxLayout(ui->pageQuestionMultipleFromN);
    verticalLayoutPrim = new QVBoxLayout(ui->pageQuestionOneFromN);

#ifndef __wasm__
    dialog = new QFileDialog(this);
#endif
}

void MainWindow::updateMainComboBox()
{
    // Update combo box
    ui->comboBoxCategorySelection->clear();
    for(auto & category : storage.getCategries())
    {
        ui->comboBoxCategorySelection->addItem(category);
    }
}

template <typename T>
void MainWindow::free(QVector<T> &_widgets)
{
    QWidget garbageCollector;
    for(auto & item : _widgets)
        item->setParent(&garbageCollector);
}


void MainWindow::on_pushButtonStartQuiz_clicked()
{
    try
    {
        if(storage.getStorageSize() < 1)
        {
            #ifndef __wasm__
                throw std::invalid_argument("Storage is empty!");
            #else
                handleError("Storage is empty!");
            #endif
        }
        else
        {
            quizIsActive = true;
            storageCurrentCategory = storage.getQuestionsFromCategory(ui->comboBoxCategorySelection->currentText());
            auto localStorage = storageCurrentCategory.getStorage();
            handleNextQuestion(localStorage[0].getQuestionType());
        }
    }
    catch(const std::invalid_argument & _ia)
    {
        handleError(_ia.what());
        qDebug() << _ia.what();
    }
    catch(const std::out_of_range & _oor)
    {
        handleError(_oor.what());
        qDebug() << _oor.what();
    }
    catch(const std::logic_error & _le)
    {
        handleError(_le.what());
        qDebug() << _le.what();
    }
    catch(...)
    {
        handleError("Unknown Error!");
        qDebug() << "Unhandled ERROR!";
    }
}

#ifndef __wasm__
void MainWindow::onResultFromFileDialog(const QUrl &_url)
{
    filePath = _url.toString();
    filePath = filePath.remove("file:///");
    jsonHandler->loadJsonFromFile(filePath);
    storage = jsonHandler->getParsedQuizData();

    updateMainComboBox();
    ui->comboBoxCategorySelection->setVisible(true);
    qDebug() << filePath;
}
#endif

void MainWindow::onResultFromWebDialog()
{
    storage = jsonHandler->getParsedQuizData();
    updateMainComboBox();
    ui->comboBoxCategorySelection->setVisible(true);
}

void MainWindow::handleTimer()
{
    if(quizIsActive)
    {
        QString timeLabelUpdate = QString::number(++AppElapsedTime) + " [s]";
        ui->TimerLabel->setText(timeLabelUpdate);
        ui->TimerLabelPrim->setText(timeLabelUpdate);
    }
}

void MainWindow::displayQuestionWidgetOneFromMany()
{
    ui->stackedWidget->setCurrentWidget(ui->pageQuestionOneFromN);
    Question currentQuestion = storageCurrentCategory.at(currentQuestionIndex);
    ui->questionOneFromN->setText(currentQuestion.getQuestionContent());

    for(auto &item : currentQuestion.getAnswers())
    {
        answersRadioButtons.push_back(new QRadioButton(item.first, ui->pageQuestionOneFromN));
        verticalLayoutPrim->addWidget(answersRadioButtons.back());
        //ui->layoutPageOneFromN->addWidget(answersRadioButtons.back());
        qDebug() << answersRadioButtons.back()->text();
    }
    ui->answersContainerOneFromN->setLayout(verticalLayoutPrim);
    resize(minimumSizeHint());
    // move(QGuiApplication::screens().at(0)->geometry().center() - frameGeometry().center()); // TODO to dziala ale powinno być wywaołane w innym miejscach

// ui->layoutPageOneFromN->addLayout(verticalLayoutPrim, answersRadioButtons.size(), 1);

}

void MainWindow::displayQuestionWidgetManyFromMany()
{
    ui->stackedWidget->setCurrentWidget(ui->pageQuestionMultipleFromN);
    Question currentQuestion = storageCurrentCategory.at(currentQuestionIndex);
    ui->questionNFromM->setText(currentQuestion.getQuestionContent());

    for(auto &item : currentQuestion.getAnswers())
    {
        answersCheckBoxes.push_back(new QCheckBox(item.first, ui->pageQuestionMultipleFromN));
        verticalLayout->addWidget(answersCheckBoxes.back());
        //ui->layoutPageMFromN->addWidget(answersCheckBoxes.back());
        qDebug() << answersCheckBoxes.back()->text();
    }
    //ui->pageQuestionMultipleFromN->setLayout(verticalLayout);
    ui->answersConatainerMFromN->setLayout(verticalLayout);
    resize(minimumSizeHint());
    // move(QGuiApplication::screens().at(0)->geometry().center() - frameGeometry().center()); // TODO to dziala ale powinno być wywaołane w innym miejscach
    //TODO Przyciski Next, check
    //TODO walidacja
}

void MainWindow::displaySummary()
{
    quizIsActive = false;
    QString singleQuestionData = "";
    float score = 0.0f;
    for(auto &item : finalAnswers)
    {
        score += item.second;
        singleQuestionData = item.first + "\t" + QString::number(item.second) + "\n";
        ui->summaryAnswerList->addItem(singleQuestionData);
    }
    score /= (float)finalAnswers.size();

    QString finalScore = "Final Score: " + QString::number(score);

    ui->stackedWidget->setCurrentWidget(ui->pageSummary);
    ui->summary->setText(finalScore);
    ui->progressBar->setValue(score * 100.0f);
    resize(minimumSizeHint());
}

void MainWindow::handleFileInput()
{
#ifdef __wasm__
    auto fileContentReady = [&](const QString &filename, const QByteArray &fileContent)
    {
        if (filename.isEmpty())
        {
            qDebug() << "No file was selected.";
        }
        else
        {
            // Use fileName and fileContent
            fileName = filename;
            dataFromFile = fileContent;
            jsonHandler->loadJsonFromFile(fileName, dataFromFile);

            storage = jsonHandler->getParsedQuizData();
            updateMainComboBox();
            ui->comboBoxCategorySelection->setVisible(true);
        }
    };
    QFileDialog::getOpenFileContent("*.json",  fileContentReady);
#else
    dialog->setNameFilter("*.json");
    dialog->exec();
#endif
}

void MainWindow::handleFileInputWeb()
{
    DialogLoading * loadingDialog = new DialogLoading(jsonHandler, this);
    loadingDialog->setModal(true);
    loadingDialog->show();
}

void MainWindow::handleNextQuestion(const QuestionType & _type)
{
    switch (_type)
    {
    case QUESTION_MANY_FROM_MANY:
        displayQuestionWidgetManyFromMany();
        break;
    case QUESTION_ONE_FROM_MANY:
        displayQuestionWidgetOneFromMany();
        break;
    case QUESTION_TYPE_TEXT:
        throw std::logic_error("Not implemented!");
        break;
    default:
        break;
    }
}


void MainWindow::on_loadFile_clicked()
{
    handleFileInput();
}


void MainWindow::on_ButtonNextQuestion_clicked()
{
    size_t numberOfCorrectAnswers = 0;
    Question currentQuestion = storageCurrentCategory.at(currentQuestionIndex);
    auto currentCorrectAnswers = currentQuestion.getAnswers();
    for(int i = 0; i < currentCorrectAnswers.size(); i++ )
    {
        if(answersCheckBoxes[i]->isChecked() == currentCorrectAnswers[i].second)
        {
            ++numberOfCorrectAnswers;
        }
    }
    free<QCheckBox*>(answersCheckBoxes);
    finalAnswers.push_back(QPair<QString, float>(currentQuestion.getQuestionContent(), (float)numberOfCorrectAnswers / (float)currentCorrectAnswers.size()));
    ++currentQuestionIndex;
    answersCheckBoxes.clear();

    if((int)currentQuestionIndex < storageCurrentCategory.getStorageSize())
    {
        Question nextQuestion = storageCurrentCategory.at(currentQuestionIndex);
        handleNextQuestion(nextQuestion.getQuestionType());
    }
    else
        displaySummary();
}


void MainWindow::on_ButtonNextQuestionPrim_clicked()
{
    size_t numberOfCorrectAnswers = 0;
    Question currentQuestion = storageCurrentCategory.at(currentQuestionIndex);
    auto currentCorrectAnswers = currentQuestion.getAnswers();
    for(int i = 0; i < currentCorrectAnswers.size(); i++ )
    {
        if(currentCorrectAnswers[i].second && currentCorrectAnswers[i].second == answersRadioButtons[i]->isChecked())
        {
            ++numberOfCorrectAnswers;
        }
    }
    free<QRadioButton*>(answersRadioButtons);
    finalAnswers.push_back(QPair<QString, float>(currentQuestion.getQuestionContent(), (float)numberOfCorrectAnswers));
    ++currentQuestionIndex;
    answersRadioButtons.clear();

    if((int)currentQuestionIndex < storageCurrentCategory.getStorageSize())
    {
        Question nextQuestion = storageCurrentCategory.at(currentQuestionIndex);
        handleNextQuestion(nextQuestion.getQuestionType());
    }
    else
        displaySummary();
}


void MainWindow::on_ButtonReturn_clicked()
{
    AppElapsedTime = currentQuestionIndex = 0;
    quizIsActive = false;
    ui->stackedWidget->setCurrentWidget(ui->pageMain);
    ui->summaryAnswerList->clear();
    resize(minimumSizeHint());
    // move(QGuiApplication::screens().at(0)->geometry().center() - frameGeometry().center()); // TODO to dziala ale powinno być wywaołane w innym miejscach
    finalAnswers.clear();
    //free<QLabel*>(finalAnswersLabels);
}

void MainWindow::handleError(const QString &_message)
{
#ifndef __wasm__
    QMessageBox * messageBox = new QMessageBox(this);
    messageBox->setText(_message);
    messageBox->exec();
#else
    //QWidget gc;
    QMessageBox * messageBox = new QMessageBox(this);
    messageBox->setText(_message);
    messageBox->setModal(true);
    messageBox->show();
    qDebug() << _message;
#endif
}

void MainWindow::on_LoadFileWeb_clicked()
{
#ifdef __wasm__
    handleError("This option is not available in WebAssembly.");
#else
    handleFileInputWeb();
#endif
}

