QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
# disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Consultation.cpp \
    DepartmentEditor.cpp \
    RegisterNewDoctor.cpp \
    RegisterNewPerson.cpp \
    DataBase.cpp \
    Dregistr.cpp \
    GetDoctor.cpp \
    GetPerson.cpp \
    StaffEditor.cpp \
    StyleVariables.cpp \
    TimeEditor.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Consultation.h \
    DepartmentEditor.h \
    PublicConst.h \
    RegisterNewDoctor.h \
    RegisterNewPerson.h \
    DataBase.h \
    Dregistr.h \
    GetDoctor.h \
    GetPerson.h \
    StaffEditor.h \
    StyleVariables.h \
    TimeEditor.h \
    mainwindow.h

FORMS += \
    Consultation.ui \
    DepartmentEditor.ui \
    RegisterNewDoctor.ui \
    RegisterNewPerson.ui \
    Dregistr.ui \
    GetDoctor.ui \
    GetPerson.ui \
    StaffEditor.ui \
    TimeEditor.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

TRANSLATIONS += QtLanguage_ru.ts

RESOURCES += \
    Res.qrc

win32:RC_ICONS = $$PWD/res/icon.ico
