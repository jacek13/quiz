#include<QNetworkReply>

#include "connectionthread.h"

void ConnectionThread::onResultFromPage(QNetworkReply *_reply)
{
    dataLoaded = true;
    emit resultReady(_reply);
    qDebug() << "onresultFromPage: " << _reply->errorString();
}

void ConnectionThread::doWhenRunning()
{
    dataLoaded = false;
    //QObject::connect(networkManager, &QNetworkAccessManager::finished, this, &ConnectionThread::resultReady);
    QObject::connect(networkManager, &QNetworkAccessManager::finished, this, &ConnectionThread::onResultFromPage);
    networkManager->get(QNetworkRequest(QUrl(url)));
    qDebug() << "Connection Thread is running!";
}

void ConnectionThread::run()
{
    QObject::connect(this, &ConnectionThread::threadStarted, this, &ConnectionThread::doWhenRunning);
    emit ConnectionThread::threadStarted();
}

ConnectionThread::ConnectionThread(QObject * _parent, const QUrl & _url) :
    QThread(_parent),
    url(_url)
{
    networkManager = new QNetworkAccessManager(_parent);
}

void ConnectionThread::setUrl(const QString &_s)
{
    url = QUrl(_s);
}

