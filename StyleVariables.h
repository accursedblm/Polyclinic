#ifndef STYLEVARIABLES_H
#define STYLEVARIABLES_H
#include <QString>
#include <QFile>
#include <QMap>
#include <QRegularExpression>
#include <QtDebug>

class StyleVariables
{
public:
    StyleVariables(QString& path);
    StyleVariables(QString&& path);
    ~StyleVariables() {};
    QMap<QString, QString> variables;
    QString _path;
    int updateVar();
};

#endif // STYLEVARIABLES_H
