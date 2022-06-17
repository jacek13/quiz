#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QStyle>

int main(int argc, char *argv[])
{
    try
    {
        QApplication a(argc, argv);

        QTranslator translator;
        const QStringList uiLanguages = QLocale::system().uiLanguages();
        for (const QString &locale : uiLanguages) {
            const QString baseName = "quiz_" + QLocale(locale).name();
            if (translator.load(":/i18n/" + baseName)) {
                a.installTranslator(&translator);
                break;
            }
        }
        MainWindow w;
#ifdef __wasm__
        w.showMaximized();
#else
        w.show();
#endif
        return a.exec();
    }
    catch(std::exception & _e)
    {
        qDebug() << _e.what();
    }
}
