#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtCore/QProcess>
#include <QtCore/QTemporaryFile>
#include <QtCore/QDir>
#include <QtWidgets/QMessageBox>
#include <QtCore/QTimer>
#include <QtWidgets/QFileDialog>
#include <QtGui/QClipboard>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

bool takeScreenshot(const QString& outputPath) {
    QProcess process;
    process.start("spectacle", QStringList() << "-b" << "-r" << "-n" << "-o" << outputPath);
    process.waitForFinished();
    return process.exitCode() == 0;
}

struct OcrResult {
    QString text;
    bool success;
    QString errorMessage;
};

OcrResult extractText(const QString& imagePath) {
    OcrResult result;
    result.success = true;

    tesseract::TessBaseAPI* ocr = new tesseract::TessBaseAPI();

    // Initialize tesseract with English language
    if (ocr->Init(nullptr, "eng")) {
        delete ocr;
        result.success = false;
        result.errorMessage = "Error initializing Tesseract OCR";
        return result;
    }

    // Open image with Leptonica
    Pix* image = pixRead(imagePath.toUtf8().constData());
    if (!image) {
        ocr->End();
        delete ocr;
        result.success = false;
        result.errorMessage = "Failed to load image";
        return result;
    }

    ocr->SetImage(image);

    char* outText = ocr->GetUTF8Text();
    result.text = QString::fromUtf8(outText);

    delete[] outText;
    pixDestroy(&image);
    ocr->End();
    delete ocr;

    return result;
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Screenshot OCR Tool");
    window.resize(500, 400);

    QVBoxLayout* layout = new QVBoxLayout();

    QLabel* label = new QLabel("Processing screenshot...");
    layout->addWidget(label);

    QTextEdit* textEdit = new QTextEdit();
    textEdit->setMinimumHeight(200);
    layout->addWidget(textEdit);

    QWidget* buttonContainer = new QWidget();
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonContainer);

    QPushButton* copyButton = new QPushButton("Copy to Clipboard");
    QPushButton* saveButton = new QPushButton("Save to File");

    buttonLayout->addWidget(copyButton);
    buttonLayout->addWidget(saveButton);
    layout->addWidget(buttonContainer);

    window.setLayout(layout);

    QString tempPath = QDir::tempPath() + "/screenshot.png";

    auto processScreenshot = [&]() {

        label->setText("Taking screenshot...");
        QApplication::processEvents();

        if (takeScreenshot(tempPath))
        {
            label->setText("Extracting text...");
            QApplication::processEvents();

            OcrResult result = extractText(tempPath);

            if (!result.success) {
                textEdit->setText("");
                label->setText(result.errorMessage);
            }
            else {
                textEdit->setText(result.text);
                label->setText("Text extracted successfully");
            }
        }
        else
        {
            label->setText("Failed to take screenshot");
            QMessageBox::critical(&window, "Error", "Failed to launch Spectacle or take screenshot");
        }
        };

    // Connect copy button to clipboard functionality
    QObject::connect(copyButton, &QPushButton::clicked, [&]() {
        if (!textEdit->toPlainText().isEmpty()) {
            QApplication::clipboard()->setText(textEdit->toPlainText());
            label->setText("Text copied to clipboard");
        }
        else {
            label->setText("No text to copy");
        } });

        // Connect save button to file save functionality
        QObject::connect(saveButton, &QPushButton::clicked, [&]() {
            if (!textEdit->toPlainText().isEmpty()) {
                QString fileName = QFileDialog::getSaveFileName(&window,
                    "Save OCR Text", QDir::homePath(), "Text Files (*.txt);;All Files (*)");

                if (!fileName.isEmpty()) {
                    QFile file(fileName);
                    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                        QTextStream out(&file);
                        out << textEdit->toPlainText();
                        file.close();
                        label->setText("Text saved to file");
                    }
                    else {
                        label->setText("Failed to save file");
                        QMessageBox::critical(&window, "Error", "Failed to save the file");
                    }
                }
            }
            else {
                label->setText("No text to save");
            } });

            // Take screenshot first before showing window
            if (takeScreenshot(tempPath)) {
                // Process the screenshot
                OcrResult result = extractText(tempPath);

                if (!result.success) {
                    textEdit->setText("");
                    label->setText(result.errorMessage);
                }
                else {
                    // Update UI with results
                    textEdit->setText(result.text);
                    label->setText("Text extracted successfully. You can edit the text before copying or saving.");
                }

                // Show window after screenshot is processed
                window.show();
            }
            else {
                // Show window with error message if screenshot fails
                textEdit->setText("");
                label->setText("Error occurred while taking screenshot");
                window.show();
                QMessageBox::critical(&window, "Error", "Failed to launch Spectacle or take screenshot");
            }

            return app.exec();
}