#ifndef CONNECTIONTHREAD_H
#define CONNECTIONTHREAD_H

#include <QThread>
#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>

class ConnectionThread : public QThread
{
    Q_OBJECT
    void run() override;
private:
    QNetworkAccessManager * networkManager;
    QUrl url;
public slots:
    void onResultFromPage(QNetworkReply * _reply);

private slots:
    void doWhenRunning();

public:
    ConnectionThread(QObject * _parent, const QUrl & _url = QUrl("https://jsonkeeper.com/b/ARNB"));
    void setUrl(const QString & _s);
    volatile bool dataLoaded;
signals:
    void resultReady(QNetworkReply * _reply);
    void threadStarted();
};

#endif // CONNECTIONTHREAD_H
