#include <QVector>
#include <QPair>
#include <QString>
#include <QFile>

#include "jsonhandler.h"

JsonHandler::JsonHandler(QObject * parent) : QObject(parent)
{
    quiz = QuizStorage();
    connectionThread = nullptr;
}

QuizStorage JsonHandler::getParsedQuizData()
{
    return quiz;
}

void JsonHandler::loadJsonFromFile(const QString _path)
{
    QFile file(_path);
    QJsonDocument document;
    QJsonObject root;
    QVector<Question> storage;
    int numberOfTrueAnswers = 0;
    storage.clear();

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        document = QJsonDocument::fromJson(file.readAll());
        file.close();
        root = document.object();

        QJsonValue jv = root.value("categories");
        if(jv.isArray())
        {
            QJsonArray ja = jv.toArray();
            for(auto item : ja)
            {
                QJsonObject subtree = item.toObject();
                QString currentCategory = subtree.value("name").toString();
                QuestionType currentQuestionType = QUESTION_ONE_FROM_MANY;
                qDebug() << subtree.keys().at(0) << " : " << subtree.value("name").toString();
                qDebug() << subtree.keys().at(1) << " : " << subtree.value("questions").toString();
                QJsonValue subJv = subtree.value("questions");

                if(subJv.isArray())
                {
                    QJsonArray questionArray = subJv.toArray();
                    for(auto question : questionArray)
                    {
                        QJsonObject questionObject = question.toObject();
                        qDebug() << questionObject.keys().at(0) << " : " << questionObject.value("question").toString();
                        QString currentQuestion = questionObject.value("question").toString();
                        QVector<QPair<QString, bool>> answersWithValues;
                        qDebug() << questionObject.keys().at(1) << " : " << questionObject.value("answers").toString();
                        qDebug() << questionObject.keys().at(2) << " : " << questionObject.value("values").toString();

                        QJsonValue answers = questionObject.value("answers");
                        if(answers.isArray())
                        {
                            QJsonArray answersArray = answers.toArray();
                            for(auto answer : answersArray)
                            {
                                //QJsonObject answerObject = answer.toObject();
                                qDebug() << answer;
                                answersWithValues.push_back(QPair<QString, bool>(answer.toString(), false));
                            }
                        }

                        QJsonValue values = questionObject.value("values");
                        if(values.isArray())
                        {
                            numberOfTrueAnswers = 0;
                            QJsonArray valuesArray = values.toArray();
                            for(int i = 0; i < valuesArray.size() && i < answersWithValues.size(); i++)
                            {
                                answersWithValues[i].second = valuesArray.at(i).toBool();
                                //qDebug() << value;
                                if(answersWithValues.at(i).second)
                                    ++numberOfTrueAnswers;
                            }
                        }
                        storage.push_back(Question(currentQuestion, currentCategory, answersWithValues));
                        storage.back().setAnswerType((numberOfTrueAnswers > 1 ? QUESTION_MANY_FROM_MANY : QUESTION_ONE_FROM_MANY));
                    }
                }
            }
        }

        for(auto& item : root.keys())
        {
            qDebug() << item;
        }
        for(int i = 0; i < root.keys().count(); i++)
        {
            qDebug() << root.keys().at(i);
        }

        for(Question question : storage)
        {
            qDebug() << question.getQuestionContent() << " : " << question.getCategory();
        }

        quiz = QuizStorage(storage);
        qDebug() << "Storage size: " << quiz.getStorageSize();

    }
    else
        qDebug() << "Cannot open file from path: " << _path;
}

void JsonHandler::setConnection(QString _url)
{
    if(connectionThread != nullptr)
    {
        qDebug() << connectionThread->dataLoaded;
        QObject gc;
        connectionThread->setParent(&gc);
    }
    connectionThread = new ConnectionThread(this, QUrl(_url));
    QObject::connect(connectionThread, &ConnectionThread::resultReady, this, &JsonHandler::onResultFromPage);
    connectionThread->start();
}

void JsonHandler::onResultFromPage(QNetworkReply * _reply)
{
    if(!_reply->error())
    {
        // Tworzymy dokument do którego odczytujemy dane z odpowiedzi
        QJsonDocument document = QJsonDocument::fromJson(_reply->readAll());
        QJsonObject root = document.object();
        QVector<Question> storage;
        int numberOfTrueAnswers = 0;
        storage.clear();

        QJsonValue jv = root.value("categories");
        if(jv.isArray())
        {
            QJsonArray ja = jv.toArray();
            for(auto item : ja)
            {
                QJsonObject subtree = item.toObject();
                QString currentCategory = subtree.value("name").toString();
                QuestionType currentQuestionType = QUESTION_ONE_FROM_MANY;
                qDebug() << subtree.keys().at(0) << " : " << subtree.value("name").toString();
                qDebug() << subtree.keys().at(1) << " : " << subtree.value("questions").toString();
                QJsonValue subJv = subtree.value("questions");

                if(subJv.isArray())
                {
                    QJsonArray questionArray = subJv.toArray();
                    for(auto question : questionArray)
                    {
                        QJsonObject questionObject = question.toObject();
                        qDebug() << questionObject.keys().at(0) << " : " << questionObject.value("question").toString();
                        QString currentQuestion = questionObject.value("question").toString();
                        QVector<QPair<QString, bool>> answersWithValues;
                        qDebug() << questionObject.keys().at(1) << " : " << questionObject.value("answers").toString();
                        qDebug() << questionObject.keys().at(2) << " : " << questionObject.value("values").toString();

                        QJsonValue answers = questionObject.value("answers");
                        if(answers.isArray())
                        {
                            QJsonArray answersArray = answers.toArray();
                            for(auto answer : answersArray)
                            {
                                //QJsonObject answerObject = answer.toObject();
                                qDebug() << answer;
                                answersWithValues.push_back(QPair<QString, bool>(answer.toString(), false));
                            }
                        }

                        QJsonValue values = questionObject.value("values");
                        if(values.isArray())
                        {
                            numberOfTrueAnswers = 0;
                            QJsonArray valuesArray = values.toArray();
                            for(int i = 0; i < valuesArray.size() && i < answersWithValues.size(); i++)
                            {
                                answersWithValues[i].second = valuesArray.at(i).toBool();
                                //qDebug() << value;
                                if(answersWithValues.at(i).second)
                                    ++numberOfTrueAnswers;
                            }
                        }
                        storage.push_back(Question(currentQuestion, currentCategory, answersWithValues));
                        storage.back().setAnswerType((numberOfTrueAnswers > 1 ? QUESTION_MANY_FROM_MANY : QUESTION_ONE_FROM_MANY));
                    }
                }
            }
        }

        for(auto& item : root.keys())
        {
            qDebug() << item;
        }
        for(int i = 0; i < root.keys().count(); i++)
        {
            qDebug() << root.keys().at(i);
        }

        for(Question question : storage)
        {
            qDebug() << question.getQuestionContent() << " : " << question.getCategory();
        }

        quiz = QuizStorage(storage);
        qDebug() << "Storage size: " << quiz.getStorageSize();

        emit dataFromWebIsReady();
    }
}
