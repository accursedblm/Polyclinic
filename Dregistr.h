#ifndef DREGISTR_H
#define DREGISTR_H

#include <QDialog>
#include <QTableWidget>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QString>
#include <QDateTime>
#include <QMovie>
#include "RegisterNewPerson.h"
#include "GetPerson.h"
#include "GetDoctor.h"
#include "StyleVariables.h"
#include "PublicConst.h"
#include "Consultation.h"

namespace Ui {
class DRegistr;
}

class DRegistr : public QDialog
{
    Q_OBJECT

public:
    explicit DRegistr(QWidget *parent = nullptr);
    ~DRegistr();
    void setHiddenTabs(int, bool);

private:
    /**********************Вкладка регистрации записей****************/
    // Переменные для хранения ID выбранных из базы записей
    QString PeopleID;
    QString DoctorID;
    QString SID;
    QString TimeTableID;
    // Таблица графика приемов
    QTableWidget* tw;
    // Заполнить поля клиента
    void fill_fields_People(const QMap<QString,QString>& _Result);
    // Заполнить поля врача
    void fill_fields_Doc(const QMap<QString,QString>& _Result);
    // Настройка и обновление таблицы приемов
    int SetupTable();
    /**********************Вкладка просмотра записей******************/
    // Таблица приемов
    QTableWidget* twp;
    // Gifka
    QMovie *movie;
    // Настрока таблицы приемов
    int SetupTableTwp();
    // Обновление таблицы приемов
    int UpdateTableTwp();
    // Обновление таблицы приемов
    int SetAlignColumn(QTableWidget* tw, int col, Qt::AlignmentFlag flag);
private slots:
    /**********************Вкладка регистрации записей****************/
    // Выбрать существующего клиента
    void on_pButton_getPers_clicked();
    // Добавить нового клиента
    void on_pButton_RegisterNew_clicked();
    // Выбрать врача
    void on_pButton_getDoc_clicked();
    // Обновить таблицу
    void on_pushButton_Update_clicked();
    // Сделать запись в базу
    void on_pushButton_Write_clicked();
    // Выйти из диалога
    void on_pushButton_Exit_clicked();
    // Выбор ячейки в таблице
    void on_tableWidget_itemClicked(QTableWidgetItem *item);
    /**********************Вкладка просмотра записей******************/
    // Кнопка обновления записей
    void on_pushButton_update_clicked(); 

    void on_comboBox_vid_currentIndexChanged(int index);

    void on_pushButton_ShowRecord_clicked();

private:
    Ui::DRegistr *ui;
    // Событие изменения размеров виджета
    virtual void resizeEvent(QResizeEvent * /* event */) override;
    void resizeMovie();
};

#endif // DREGISTR_H
