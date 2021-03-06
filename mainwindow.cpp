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
    QString solutionDirectory = QCoreApplication::applicationDirPath() + "/resources/questions.json";
    jsonHandler->loadJsonFromFile(solutionDirectory);
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
            storageCurrentCategory.shuffle();
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
        QString timeLabelUpdate = "Time: " + QString::number(++AppElapsedTime) + " [s]";
        ui->TimerLabel->setText(timeLabelUpdate);
        ui->TimerLabelPrim->setText(timeLabelUpdate);
    }
}

void MainWindow::displayQuestionWidgetOneFromMany()
{
    ui->stackedWidget->setCurrentWidget(ui->pageQuestionOneFromN);
    Question currentQuestion = storageCurrentCategory.at(currentQuestionIndex);
    ui->questionOneFromN->clear();
    ui->questionOneFromN->addItem(currentQuestion.getQuestionContent());
    //ui->questionOneFromN->setWrapping(true);
    ui->questionOneFromN->setWordWrap(true);

    for(auto &item : currentQuestion.getAnswers())
    {
        auto radioButton = new QRadioButton(item.first, ui->pageQuestionOneFromN);
        /* TODO zawijanie tekstu
        auto label = new QLabel(item.first, radioButton);
        label->setWordWrap(true);
        auto buttonWithLabelLayout = new QHBoxLayout(radioButton);
        buttonWithLabelLayout->addWidget(label);
        */
        answersRadioButtons.push_back(radioButton);
        verticalLayoutPrim->addWidget(answersRadioButtons.back());
        qDebug() << answersRadioButtons.back()->text();
    }

    ui->answersContainerOneFromN->setLayout(verticalLayoutPrim);
    // resize(minimumSizeHint());
    // move(QGuiApplication::screens().at(0)->geometry().center() - frameGeometry().center()); // TODO to dziala ale powinno być wywaołane w innym miejscach
}

void MainWindow::displayQuestionWidgetManyFromMany()
{
    ui->stackedWidget->setCurrentWidget(ui->pageQuestionMultipleFromN);
    Question currentQuestion = storageCurrentCategory.at(currentQuestionIndex);
    //ui->questionNFromM->setText(currentQuestion.getQuestionContent());
    ui->questionNFromM->clear();
    ui->questionNFromM->addItem(currentQuestion.getQuestionContent());
    //ui->questionNFromM->setWrapping(true);
    ui->questionNFromM->setWordWrap(true);

    for(auto &item : currentQuestion.getAnswers())
    {
        auto checkBox = new QCheckBox(item.first, ui->pageQuestionMultipleFromN);
        /* TODO zawijanie tekstu
        auto label = new QLabel(item.first, checkBox);
        label->setWordWrap(true);
        auto buttonWithLabelLayout = new QHBoxLayout(checkBox);
        buttonWithLabelLayout->addWidget(label);
        */
        answersCheckBoxes.push_back(checkBox);
        verticalLayout->addWidget(answersCheckBoxes.back());
        //ui->layoutPageMFromN->addWidget(answersCheckBoxes.back());
        qDebug() << answersCheckBoxes.back()->text();
    }
    //ui->pageQuestionMultipleFromN->setLayout(verticalLayout);
    ui->answersConatainerMFromN->setLayout(verticalLayout);
    // resize(minimumSizeHint());
    // move(QGuiApplication::screens().at(0)->geometry().center() - frameGeometry().center()); // TODO to dziala ale powinno być wywaołane w innym miejscach
    //TODO Przyciski Next, check
    //TODO walidacja
}

void MainWindow::displaySummary()
{
    quizIsActive = false;
    QString singleQuestionData = "";
    size_t numberOfCorrectAnswers = 0;
    float singleQuestionScore = 0.0f;
    float score = 0.0f;

    for(auto &item : finalAnswers)
    {
        singleQuestionScore = 0.0f;
        singleQuestionData = item.first.getQuestionContent() + '\n';
        numberOfCorrectAnswers = 0;

        if(item.first.getQuestionType() == QUESTION_MANY_FROM_MANY)
        {
            auto currentCorrectAnswers = item.first.getAnswers();
            for(int i = 0; i < currentCorrectAnswers.size(); i++ )
            {
                singleQuestionData += "\t-" + currentCorrectAnswers[i].first + " (is ";
                singleQuestionData += ( currentCorrectAnswers[i].second ? "true)" : "false)");
                if(item.second[i] == currentCorrectAnswers[i].second)
                {
                    ++numberOfCorrectAnswers;
                    singleQuestionData += "\n\t\t-User answer was: ";
                    singleQuestionData += " +CORRECT\n";
                }
                else
                {
                    singleQuestionData += "\n\t\t-User answer was: ";
                    singleQuestionData += " -WRONG\n";
                }
            }
            singleQuestionScore = (float)numberOfCorrectAnswers / (float)currentCorrectAnswers.size();
        }
        else if( item.first.getQuestionType() == QUESTION_ONE_FROM_MANY)
        {
            auto currentCorrectAnswers = item.first.getAnswers();
            for(int i = 0; i < currentCorrectAnswers.size(); i++ )
            {
                if(currentCorrectAnswers[i].second)
                {
                    singleQuestionData += "\t-Correct answer is: " + currentCorrectAnswers[i].first + '\n';
                    singleQuestionData += "\t-User answer was: ";
                    if(currentCorrectAnswers[i].second == item.second[i])
                    {
                        ++numberOfCorrectAnswers;
                        singleQuestionData += " +CORRECT\n";
                    }
                    else
                        singleQuestionData += " -WRONG\n";
                }
            }
            singleQuestionScore = (numberOfCorrectAnswers == 1 ? 1.0f : 0.0f);
        }
        else
            qDebug() << "Unknown question type!";

        ui->summaryAnswerList->addItem(singleQuestionData);
        score += singleQuestionScore;
    }

    score /= (float)finalAnswers.size();

    QString finalScore = "Final Score: " + QString::number(score * 100.0f) + "%";

    ui->stackedWidget->setCurrentWidget(ui->pageSummary);
    ui->summary->setText(finalScore);
    ui->progressBar->setValue(score * 100.0f);
    // resize(minimumSizeHint());
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
    Question currentQuestion = storageCurrentCategory.at(currentQuestionIndex);
    QVector<bool> currentUserAnswers;

    for(int i = 0; i < currentQuestion.getAnswers().size(); i++ )
        currentUserAnswers.push_back(answersCheckBoxes[i]->isChecked());

    free<QCheckBox*>(answersCheckBoxes);
    finalAnswers.push_back(QPair<Question, QVector<bool>>(currentQuestion, currentUserAnswers));
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
    QVector<bool> currentUserAnswers;
    Question currentQuestion = storageCurrentCategory.at(currentQuestionIndex);

    for(int i = 0; i < currentQuestion.getAnswers().size(); i++ )
        currentUserAnswers.push_back(answersRadioButtons[i]->isChecked());

    free<QRadioButton*>(answersRadioButtons);
    finalAnswers.push_back(QPair<Question, QVector<bool>>(currentQuestion, currentUserAnswers));
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
    // resize(minimumSizeHint());
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

