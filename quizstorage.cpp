#include "quizstorage.h"

#include <QDebug>

QuizStorage::QuizStorage()
{

}

QuizStorage::QuizStorage(const QuizStorage &_quiz)
{
    if(_quiz.storage.size() > 0)
        for(auto & item : _quiz.getStorage())
        {
            storage.push_back(item);
        }
    //mersenTwister = _quiz.mersenTwister;
}

QuizStorage::QuizStorage(QVector<Question> _storage)
{
    storage = _storage;
}

void QuizStorage::shuffle()
{
    std::random_device randomDevice;
    std::mt19937 mersenTwister(randomDevice());

    std::shuffle(storage.begin(), storage.end(), mersenTwister);

    for(auto & question : storage)
    {
        question.shuffleAnswers();
    }
}

bool QuizStorage::addQuestionToCategory(Question _question)
{
    storage.push_back(_question); // TODO validation
    return true;
}

int QuizStorage::getStorageSize()
{
    return storage.size();
}

const QVector<Question> QuizStorage::getStorage() const
{
    return storage;
}

QVector<QString> QuizStorage::getCategries()
{
    QVector<QString> categories;
    for(auto & item : storage)
    {
        if(!categories.contains(item.getCategory()))
        {
            categories.push_back(item.getCategory());
        }
    }
    return categories;
}

QuizStorage QuizStorage::getQuestionsFromCategory(const QString &_category)
{
    QVector<Question> newStorage;
    for(auto item : storage)
    {
        if(item.getCategory() == _category)
        {
            newStorage.push_back(item);
        }
    }
    return newStorage;
}

const Question QuizStorage::at(int _index) const
{
    return _index < storage.count() && _index >= 0 ? storage[_index] : throw new std::out_of_range("Index out of range!");
}

Question::Question()
{
    addQuestionContent("Empty");
    setAnswerType();
}

Question::Question(QString _question) : content(_question)
{
    setAnswerType();
}

Question::Question(QString _question, QString _category, QVector<QPair<QString, bool>> _answers) :
    content(_question), category(_category), answers(_answers)
{
    setAnswerType();
}

void Question::addQuestionContent(QString _question)
{
    content = _question; // TODO validation!
}

void Question::addAnswer(QString _answer, bool _value)
{
    answers.push_back(QPair<QString, bool>(_answer, _value)); // TODO validation!
}

void Question::shuffleAnswers()
{
    std::random_device randomDevice;
    std::mt19937 mersenTwister(randomDevice());

    std::shuffle(answers.begin(), answers.end(), mersenTwister);
}

bool Question::setAnswerType(QuestionType _type)
{
    switch(_type)
    {
        case QUESTION_ONE_FROM_MANY:
            type = QUESTION_ONE_FROM_MANY;
            return true;
        break;
        case QUESTION_MANY_FROM_MANY:
            type = QUESTION_MANY_FROM_MANY;
            return true;
        break;
        case QUESTION_TYPE_TEXT:
            type = QUESTION_TYPE_TEXT;
            return true;
        break;
        default:
            type = QUESTION_ONE_FROM_MANY;
            qWarning() << "Wrong question type!";
            return false;
        break;
    }
}

bool Question::setCategory(QString _category)
{
    category = _category; // TODO validation!
    return true;
}

QString Question::getCategory()
{
    return category;
}

QString Question::getQuestionContent()
{
    return content;
}

QVector<QPair<QString, bool> > Question::getAnswers()
{
    return answers;
}

QuestionType Question::getQuestionType()
{
    return type;
}
