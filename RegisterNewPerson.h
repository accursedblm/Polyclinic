#ifndef REGISTERNEWPERSON_H
#define REGISTERNEWPERSON_H

#include <QDialog>
#include <QMap>
#include <QSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QListWidgetItem>
#include "QMessageBox"

namespace Ui {
class RegisterNewPerson;
}

class RegisterNewPerson : public QDialog
{
    Q_OBJECT
private:
    QMap<QString,QString>* Result;
public:
    explicit RegisterNewPerson(QWidget *parent = nullptr, QMap<QString,QString>* _Result = nullptr);
    ~RegisterNewPerson();

private slots:
    void on_pushButton_Select_clicked();
    void on_pushButton_Exit_clicked();

private:
    Ui::RegisterNewPerson *ui;
private:
    void WriteInBase();
};

#endif // REGISTERNEWPERSON_H
