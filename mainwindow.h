#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>                //Для работы с файлами
#include <QDebug>               //Для работы с дебагом
#include <QtSql/QSqlTableModel> //Для работы с SQL
#include "DataBase.h"           //Для работы с базой

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_Registr_triggered();
    void on_action_AddPerson_triggered();
    void on_action_AddDoctor_triggered();
    void on_action_Departments_triggered();
    void on_action_StaffEditor_triggered();
    void on_action_TimeEditor_triggered();
    void on_action_Cons_triggered();

protected:
    void resizeEvent(QResizeEvent *evt);

private:
    Ui::MainWindow *ui;
    void ResizeBackground();

public:
    DataBase        *db;
};

#endif // MAINWINDOW_H
