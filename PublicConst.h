#ifndef PUBLICCONST_H
#define PUBLICCONST_H
#include <QString>
#include <QObject>
#include <QMap>
const QMap<int,QString> dayName {{1,QObject::tr("ПН")},
                                 {2,QObject::tr("ВТ")},
                                 {3,QObject::tr("СР")},
                                 {4,QObject::tr("ЧТ")},
                                 {5,QObject::tr("ПТ")},
                                 {6,QObject::tr("СБ")},
                                 {7,QObject::tr("ВС")}};

#endif // PUBLICCONST_H
