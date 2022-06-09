#ifndef QUIZSTORAGE_H
#define QUIZSTORAGE_H

#include <QObject>
#include <QVector>
#include <QPair>
#include <QString>

enum QuestionType
{
    QUESTION_ONE_FROM_MANY = 0,
    QUESTION_MANY_FROM_MANY,
    QUESTION_TYPE_TEXT
};

class Question
{
public:
    Question();
    Question(QString _question);
    Question(QString _question, QString _category, QVector<QPair<QString, bool>> _answers);

    void addQuestionContent(QString _question);
    void addAnswer(QString _answer, bool _value);
    bool setAnswerType(QuestionType _type = QUESTION_ONE_FROM_MANY);
    bool setCategory(QString _category);

    QString getCategory();
    QString getQuestionContent();
    QVector<QPair<QString, bool>> getAnswers();
    QuestionType getQuestionType();

protected:
    QString content;
    QString category;
    QVector<QPair<QString, bool>> answers;
    QuestionType type;
};

class QuizStorage
{
public:
    QuizStorage();
    QuizStorage(const QuizStorage& _quiz);
    QuizStorage(QVector<Question> _storage);

    bool addQuestionToCategory(Question _question);
    int getStorageSize();
    const QVector<Question> getStorage() const;
    QVector<Question> getStorageShuddled();
    QVector<QString> getCategries();
    QuizStorage getQuestionsFromCategory(const QString & _category);
    const Question at(int _index) const;
protected:
    QVector<Question> storage;
};

#endif // QUIZSTORAGE_H
