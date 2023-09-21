#ifndef TIMEEDITOR_H
#define TIMEEDITOR_H

#include <QWidget>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QString>
#include "StyleVariables.h"
#include "PublicConst.h"

namespace Ui {
class TimeEditor;
};

class TimeEditor : public QDialog
{
    Q_OBJECT

public:
    explicit TimeEditor(QWidget *parent = nullptr);
    ~TimeEditor();

private slots:
    void on_comboBox_Doc_currentIndexChanged(int index);
    void on_comboBox_Dep_currentIndexChanged(int index);
    void on_pushButton_Update_clicked();
    void on_pushButton_Exit_clicked();
    void on_pushButton_insGrath_clicked();
    void on_pushButton_Delete_clicked();

private:
    Ui::TimeEditor *ui;
    QString SID;
    QMap<int,bool> job;
    int UpdateTable();
    int UpdateComboDoc();
    int UpdateComboDep(int DocID);
    int UpdateSID();
    int SetupTable();
    void UpdateCb();
};

#endif // TIMEEDITOR_H
