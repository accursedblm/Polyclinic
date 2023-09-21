#ifndef REGISTERNEWDOCTOR_H
#define REGISTERNEWDOCTOR_H

#include <QDialog>
#include <QMap>
#include <QSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QListWidgetItem>
#include "QMessageBox"

namespace Ui {
class RegisterNewDoctor;
}

class RegisterNewDoctor : public QDialog
{
    Q_OBJECT
private:
    QMap<QString,QString>* Result;
public:
    explicit RegisterNewDoctor(QWidget *parent = nullptr, QMap<QString,QString>* _Result = nullptr);
    ~RegisterNewDoctor();

private slots:
    void on_pushButton_Select_clicked();
    void on_pushButton_Exit_clicked();

private:
    Ui::RegisterNewDoctor *ui;
private:
    void WriteInBase();
};

#endif // REGISTERNEWDOCTOR_H
