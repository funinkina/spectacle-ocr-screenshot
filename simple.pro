QT += core widgets gui

CONFIG += c++17

TARGET = spectacle-ocr-screenshot
TEMPLATE = app

SOURCES += main.cpp

# Use pkg-config to find Tesseract and Leptonica
unix:!macx {
    CONFIG += link_pkgconfig
    PKGCONFIG += tesseract lept
}

# ZXing dependency - adjust paths if needed
unix:!macx {
    # If ZXing is installed via package manager or system-wide
    LIBS += -lZXing
    
    # If custom installation, you might need to specify include and lib paths
    # INCLUDEPATH += /usr/local/include
    # LIBS += -L/usr/local/lib -lZXing
}

# Default application description
QMAKE_TARGET_DESCRIPTION = "Extract text from spectacle screenshots using OCR"
