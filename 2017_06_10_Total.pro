#-------------------------------------------------
#
# Project created by QtCreator 2017-06-10T16:18:52
#
#-------------------------------------------------

QT       += core gui svg network
QT       += sql
QT       += xml
QT       += charts
qtHaveModule(printsupport):QT+=printsupport
#QT       + = printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = X_Ship
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    index/index.cpp\
    main/main.cpp \
    main/mainwindow.cpp \
    login/findpassword.cpp \
    login/logininfolabel.cpp \
    login/loginpanel.cpp \
    login/regpanel.cpp \
    internet_assess/internet_access.cpp \
    internet_assess/internetfunction.cpp \
    sys_discript/base_system.cpp \
    sys_discript/system_informa.cpp \
    message_assess/messageassess.cpp \
    message_assess/mywidget.cpp \
    result_display/result_display.cpp \
    wuli_assess/wuli.cpp \
    situation_simulation/situation.cpp \
    workflow/logicalRealtionship.cpp \
    workflow/wflow.cpp \
    workflow/workflow.cpp \
    workflow/workflow_assess.cpp \
    graphviz/grahpviz.cpp \
    graphviz/internet_info_graphviz.cpp \
    graphviz/workflow_graphviz.cpp \
    newproject/newproject.cpp \
    newproject/project_guide.cpp \
    main/admin_config.cpp \
    index/admin_index_conf.cpp \
    workflow/cconfassess.cpp \
    questionnaire_analysis/questionnaire_analysis.cpp \
    svg_show/svgwidget.cpp \
    svg_show/svgwindow.cpp \
    newproject/project_info.cpp \
    licence/Licence.cpp \
    graphviz/chineseletterhelper.cpp \
    newproject/history.cpp

HEADERS  += \
    index/index.h\
    main/mainwindow.h \
    login/findpassword.h \
    login/logininfolabel.h \
    login/loginpanel.h \
    login/regpanel.h \
    internet_assess/internet_access.h \
    internet_assess/internetfunction.h \
    sys_discript/base_informa.h \
    sys_discript/system_informa.h \
    message_assess/messageassess.h \
    message_assess/mywidget.h \
    result_display/result_display.h \
    wuli_assess/wuli.h \
    situation_simulation/situation.h \
    workflow/logicalRealtionship.h \
    workflow/wflow.h \
    workflow/workflow.h \
    workflow/workflow_assess.h \
    graphviz/graphviz.h \
    graphviz/internet_info_graphviz.h \
    graphviz/workflow_graphviz.h \
    newproject/newproject.h \
    newproject/project_guide.h \
    main/admin_config.h \
    index/admin_index_conf.h \
    workflow/cconfassess.h \
    questionnaire_analysis/questionnaire_analysis.h \
    svg_show/svgwidget.h \
    svg_show/svgwindow.h \
    newproject/project_info.h \
    licence/Licence.h \
    graphviz/chineseletterhelper.h \
    newproject/history.h


FORMS    += \
    main/mainwindow.ui \
    message_assess/messageassess.ui \
    wuli_assess/wuli.ui \
    situation_simulation/situation.ui \
    workflow/wflow.ui \
    workflow/workflow_assess.ui \
    newproject/newproject.ui \
    newproject/project_guide.ui \
    main/admin_config.ui \
    index/admin_index_conf.ui \
    workflow/cconfassess.ui \
    newproject/project_info.ui \
    internet_assess/internet_access.ui \
    newproject/history.ui

DISTFILES += \
    qss/chatClient.qss \
    rc/radar.png \
    qss/MainWindow.qss

RESOURCES += \
    resource.qrc \
    file.qrc
