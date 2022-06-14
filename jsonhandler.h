#ifndef JSONHANDLER_H
#define JSONHANDLER_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QUrl>
#include <QNetworkReply>

#include "quizstorage.h"
#include "connectionthread.h"

class JsonHandler : public QObject
{
    Q_OBJECT
public:
    //explicit JsonHandler() = default;
    //~JsonHandler() = default;
    JsonHandler(QObject * parent = nullptr);

    QuizStorage getParsedQuizData();
    ConnectionThread * connectionThread;
#ifdef __wasm__
    void loadJsonFromFile(const QString _name, QByteArray & _dataFromFile);
    void loadJsonFromFile(const QString _path);
#else
    void loadJsonFromFile(const QString _path);
#endif

private:
    QNetworkAccessManager * networkManager;
    QuizStorage quiz;
public slots:
    void setConnection(QString _url);
    void onResultFromPage(QNetworkReply * _reply);
signals:
    void dataFromWebIsReady();
};

#endif // JSONHANDLER_H
