#-------------------------------------------------
#
# Project created by QtCreator 2012-12-03T21:40:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BezierCurve
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    bezierinterpolator.cpp \
    movingellipseitem.cpp

HEADERS  += mainwindow.h \
    bezierinterpolator.h \
    movingellipseitem.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    android/AndroidManifest.xml \
    android/version.xml \
    android/res/values-es/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/drawable-mdpi/icon.png \
    android/res/values-el/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/drawable/logo.png \
    android/res/drawable/icon.png \
    android/res/layout/splash.xml \
    android/res/values-fa/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/drawable-ldpi/icon.png \
    android/res/values-ro/strings.xml \
    android/res/values-id/strings.xml \
    android/res/drawable-hdpi/icon.png \
    android/res/values-de/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/src/org/kde/necessitas/origo/QtApplication.java
