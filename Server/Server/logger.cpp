#include "logger.h"

#include <QTextCharFormat>
#include <QDebug>


Logger::Logger(QObject *parent) :
    QObject{parent}
{
}

void Logger::log(const QString &msg, LogLevel level, const QString &category)
{
    QString formatted = fmtLog(msg, level, category);
    emit logPosted(formatted, level);
}

QString Logger::fmtLog(const QString &msg, LogLevel level, const QString &category)
{
        QString timestamp = QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss]");

        QString levelStr;
        switch (level) {
        case Debug:   levelStr = "DEBUG";   break;
        case Info:    levelStr = "INFO";    break;
        case Warning: levelStr = "WARNING"; break;
        case Error:   levelStr = "ERROR";   break;
        }

        return QString("%1 [%2] <%3> %4").arg(timestamp, levelStr, category, msg);
}
