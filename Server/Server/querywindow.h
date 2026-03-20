#ifndef QUERYWINDOW_H
#define QUERYWINDOW_H

#include <QWidget>
#include <QSqlTableModel>

namespace Ui
{
    class QueryWindow;
}

class QueryWindow : public QWidget
{
    Q_OBJECT

public:
    explicit QueryWindow(QWidget *parent = nullptr);
    ~QueryWindow();

private slots:
    void on_queryBtn_clicked();

private:
    Ui::QueryWindow *ui;
    QSqlTableModel *m_model;
};

#endif // QUERYWINDOW_HPP
