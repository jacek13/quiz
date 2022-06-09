#include "mainwindow.h"
#include "ui_mainwindow.h"

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

    allocateAttributes();

    //QObject::connect(ui->pushButtonStartQuiz, &QPushButton::pressed, this, &MainWindow::);
    QObject::connect(timer, &QTimer::timeout, this, &MainWindow::handleTimer);
    QObject::connect(dialog, &QFileDialog::urlSelected, this, &MainWindow::onResultFromFileDialog);
    QObject::connect(jsonHandler, &JsonHandler::dataFromWebIsReady, this, &MainWindow::onResultFromWebDialog);

    timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::allocateAttributes()
{
    jsonHandler = new JsonHandler(this);
    dialog = new QFileDialog(this);
    timer = new QTimer(this);
    verticalLayout = new QVBoxLayout(ui->pageQuestionMultipleFromN);
    verticalLayoutPrim = new QVBoxLayout(ui->pageQuestionOneFromN);
    verticalLayoutSummary = new QVBoxLayout(ui->pageSummary);
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
            throw std::invalid_argument("Storage is empty!");
        else
        {
            quizIsActive = true;
            storageCurrentCategory = storage.getQuestionsFromCategory(ui->comboBoxCategorySelection->currentText());
            auto localStorage = storageCurrentCategory.getStorage();
            switch (localStorage[0].getQuestionType())
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

    // Add button and label from ui
    verticalLayoutPrim->addWidget(ui->QuestionAndTimerContainerPrim);
    verticalLayoutPrim->addWidget(ui->ButtonNextQuestionPrim);

    for(auto &item : currentQuestion.getAnswers())
    {
        answersRadioButtons.push_back(new QRadioButton(item.first, ui->pageQuestionOneFromN));
        verticalLayoutPrim->addWidget(answersRadioButtons.back());
        qDebug() << answersRadioButtons.back()->text();
    }
    ui->pageQuestionOneFromN->setLayout(verticalLayoutPrim);

}

void MainWindow::displayQuestionWidgetManyFromMany()
{
    ui->stackedWidget->setCurrentWidget(ui->pageQuestionMultipleFromN);
    Question currentQuestion = storageCurrentCategory.at(currentQuestionIndex);
    ui->questionNFromM->setText(currentQuestion.getQuestionContent());

    verticalLayout->addWidget(ui->QuestionAndTimerContainet);
    verticalLayout->addWidget(ui->ButtonNextQuestion);

    for(auto &item : currentQuestion.getAnswers())
    {
        answersCheckBoxes.push_back(new QCheckBox(item.first, ui->pageQuestionMultipleFromN));
        verticalLayout->addWidget(answersCheckBoxes.back());
        qDebug() << answersCheckBoxes.back()->text();
    }
    ui->pageQuestionMultipleFromN->setLayout(verticalLayout);
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
        singleQuestionData = item.first + "\t" + QString::number(item.second);
        finalAnswersLabels.push_back(new QLabel(singleQuestionData, ui->pageSummary));
        ui->verticalLayoutSummary->addWidget(finalAnswersLabels.back());
    }
    score /= (float)finalAnswers.size();

    QString finalScore = "Final Score: " + QString::number(score);

    verticalLayoutSummary->addWidget(ui->summary);
    verticalLayoutSummary->addWidget(ui->progressBar);
    verticalLayoutSummary->addWidget(ui->scrollArea);

    ui->stackedWidget->setCurrentWidget(ui->pageSummary);
    ui->summary->setText(finalScore);
    ui->progressBar->setValue(score * 100.0f);
}

void MainWindow::handleFileInput()
{
    dialog->setNameFilter("*.json");
    dialog->exec();
}

void MainWindow::handleFileInputWeb()
{
    QString animation = "Loading data... 📥";
    jsonHandler->setConnection("https://jsonkeeper.com/b/ARNB");
    QMessageBox * messageBox = new QMessageBox(this);
    messageBox->setText(animation);
    messageBox->exec();
    // Dialog animation
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
    finalAnswers.clear();
    free<QLabel*>(finalAnswersLabels);
}

void MainWindow::handleError(const QString &_message)
{
    QMessageBox * messageBox = new QMessageBox(this);
    messageBox->setText(_message);
    messageBox->exec();
}

void MainWindow::on_LoadFileWeb_clicked()
{
    handleFileInputWeb();
}

