QT += core widgets gui

CONFIG += c++17

TARGET = Screenshot_OCR
TEMPLATE = app

SOURCES += main.cpp

unix:!macx {
    PKGCONFIG += tesseract lept
}
