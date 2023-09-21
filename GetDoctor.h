#ifndef GETDOCTOR_H
#define GETDOCTOR_H

#include <QDialog>
#include <QMap>
#include <QSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QListWidgetItem>
#include <QMessageBox>

namespace Ui {
class GetDoctor;
}

class GetDoctor : public QDialog
{
    Q_OBJECT
private:
    QMap<QString,QString>* Result;
public:
    explicit GetDoctor(QWidget *parent = nullptr, QMap<QString,QString>* _Result = nullptr);
    ~GetDoctor();

private slots:
    void on_pushButton_Search_clicked();
    void on_pushButton_Select_clicked();
    void on_listWidget_itemClicked(QListWidgetItem *item);
    void on_pushButton_Exit_clicked();

    void on_comboBox_Dep_currentIndexChanged(int index);

private:
    Ui::GetDoctor *ui;
private:
    void FillTW(QSqlQuery _query);
    QSqlQuery SearchQuery();
    int UpdateComboDep(int DocID);
    int UpdateSID();
};

#endif // GETDOCTOR_H
