#ifndef GETPERSON_H
#define GETPERSON_H

#include <QDialog>
#include <QMap>
#include <QSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QListWidgetItem>

namespace Ui {
class GetPerson;
}

class GetPerson : public QDialog
{
    Q_OBJECT
private:
    QMap<QString,QString>* Result;
public:
    explicit GetPerson(QWidget *parent = nullptr, QMap<QString,QString>* _Result = nullptr);
    ~GetPerson();

private slots:
    void on_pushButton_Search_clicked();
    void on_pushButton_Select_clicked();
    void on_listWidget_itemClicked(QListWidgetItem *item);
    void on_pushButton_Exit_clicked();

private:
    Ui::GetPerson *ui;
private:
    void FillTW(QSqlQuery _query);
    QSqlQuery SearchQuery();
};

#endif // GETPERSON_H
