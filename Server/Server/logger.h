#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <QObject>
#include <QWidget>
#include <QDateTime>

class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QObject *parent = nullptr);

    enum LogLevel{Debug, Info, Warning, Error};
    Q_ENUM(LogLevel)

    void log(const QString &msg, LogLevel level = Info, const QString &category = "General");
signals:
    void logPosted(const QString &formattedMessage, LogLevel level);

private:
    QString fmtLog(const QString &msg, LogLevel level, const QString &category);
};

#endif // LOGGER_HPP
