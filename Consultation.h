#ifndef CONSULTATION_H
#define CONSULTATION_H

#include <QDialog>
#include <QAbstractButton>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDate>
#include <QInputDialog>
#include <QCompleter>
#include <QPainter>
#include <QLabel>

namespace Ui {
class Consultation;
}

class Consultation : public QDialog
{
    Q_OBJECT

public:
    explicit Consultation(QString& DocName, QString& PeopName,
                          QString& Text, QString CID, QString PID,
                          QString SID,  QString DID, QWidget *parent = nullptr);
    ~Consultation();
protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_checkBox_closeList_stateChanged(int arg1);

private:
    Ui::Consultation *ui;
    QString _CID;
    QString _PID;
    QString _SID;
    QString _DID;
    QMap<QString,QString> Diag;
};

#endif // CONSULTATION_H
