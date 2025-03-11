QT += core widgets gui

CONFIG += c++17

TARGET = spectacle-ocr-screenshot
TEMPLATE = app

SOURCES += main.cpp

# Use pkg-config to find Tesseract and Leptonica
unix:!macx {
    PKGCONFIG += tesseract lept
    LIBS += `pkg-config --libs tesseract lept`
    INCLUDEPATH += `pkg-config --cflags tesseract lept`
}
