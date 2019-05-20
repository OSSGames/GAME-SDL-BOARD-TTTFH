# -------------------------------------------------
# Project created by QtCreator 2010-04-18T12:19:53
# -------------------------------------------------
TARGET = tttfh
TEMPLATE = app
SOURCES += main.cpp \
    tttfhgui.cpp \
    sessiondialog.cpp \
    Tavola.cpp \
    TrisMachine.cpp \
    aboutdialog.cpp \
    softgamearea.cpp \
    qtcolorpicker.cpp \
    trisnet.cpp \
    qled.cpp \
    netdock.cpp \
    chatdock.cpp \
    jstranslator.cpp
HEADERS += tttfhgui.h \
    sessiondialog.h \
    Tavola.h \
    TrisMachine.h \
    aboutdialog.h \
    softgamearea.h \
    qtcolorpicker.h \
    trisnet.h \
    qled.h \
    netdock.h \
    chatdock.h \
    jstranslator.h
FORMS += tttfhgui.ui \
    sessiondialog.ui \
    aboutdialog.ui \
    netdock.ui \
    chatdock.ui
RESOURCES += Icons.qrc \
    Langs.qrc \
    qled.qrc
target.path = /usr/bin/
images.path = /usr/share/icons/tttfh
images.files += tttfh.png
desktop.path = /usr/share/applications
desktop.files += tttfh.desktop
INSTALLS += target \
    images \
    desktop
OTHER_FILES += CHANGELOG.txt \
    TODO.txt
QT += webkit \
    network \
    svg \
    script
TRANSLATIONS = tttfh_it.ts \
    tttfh_cs.ts
