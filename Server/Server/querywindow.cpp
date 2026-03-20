#include "querywindow.h"
#include "ui_querywindow.h"

#include <QMessageBox>

QueryWindow::QueryWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QueryWindow)
{
    ui->setupUi(this);

    // 界面初始化
    setWindowTitle("信息查询");
    ui->empRb->setChecked(true);

    m_model = new QSqlTableModel(this);
}

QueryWindow::~QueryWindow()
{
    delete ui;
}

void QueryWindow::on_queryBtn_clicked()
{
    if (!ui->empRb->isChecked() && !ui->attRb->isChecked())
    {
        QMessageBox::warning(this, "查询", "请选择要查询的数据表");
    }

    if (ui->empRb->isChecked())
    {
        m_model->setTable("employee");
    }

    if (ui->attRb->isChecked())
    {
        m_model->setTable("attendance");
    }

    // 设置过滤器
    // m_model->setFilter("name='张三'");
    m_model->select();
    ui->tableView->setModel(m_model); // 将结果显示在 tableView 中

}

