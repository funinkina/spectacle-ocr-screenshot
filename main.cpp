#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>

#include <QtCore/QCommandLineParser>
#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QTemporaryFile>
#include <QtCore/QTimer>
#include <QtGui/QClipboard>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

bool takeScreenshot(const QString& outputPath) {
    QProcess process;
    process.start("spectacle", QStringList()
        << "-b" << "-r" << "-n" << "-o" << outputPath);
    process.waitForFinished();
    return process.exitCode() == 0;
}

struct OcrResult {
    QString text;
    bool success;
    QString errorMessage;
};

OcrResult extractText(const QString& imagePath, const QString& language) {
    OcrResult result;
    result.success = true;

    tesseract::TessBaseAPI* ocr = new tesseract::TessBaseAPI();

    if (ocr->Init(nullptr, language.toUtf8().constData())) {
        delete ocr;
        result.success = false;
        result.errorMessage =
            "Error initializing Tesseract OCR for language: " + language;
        return result;
    }

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

    QCommandLineParser parser;
    parser.setApplicationDescription("Extract text from spectacle screenshots using OCR");
    parser.addHelpOption();

    QCommandLineOption langOption(
        QStringList() << "lang",
        "Language(s) for OCR (e.g., eng, hin, or eng+hin for multiple languages)",
        "language", "eng");
    parser.addOption(langOption);
    parser.process(app);

    QString language = parser.value(langOption);

    QWidget window;
    window.setWindowTitle("Spectacle Screenshot OCR - Language: " + language);
    window.resize(500, 400);

    QVBoxLayout* layout = new QVBoxLayout();

    QLabel* label = new QLabel();
    layout->addWidget(label);

    QTextEdit* textEdit = new QTextEdit();
    textEdit->setMinimumHeight(100);
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

    QObject::connect(copyButton, &QPushButton::clicked, [&]() {
        if (!textEdit->toPlainText().isEmpty()) {
            QApplication::clipboard()->setText(textEdit->toPlainText());
            label->setText("Text copied to clipboard");
        }
        else {
            label->setText("No text to copy");
        }
        });

    QObject::connect(saveButton, &QPushButton::clicked, [&]() {
        if (!textEdit->toPlainText().isEmpty()) {
            QString fileName = QFileDialog::getSaveFileName(
                &window, "Save OCR Text", QDir::homePath(),
                "Text Files (*.txt);;All Files (*)");

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
        }
        });

    if (takeScreenshot(tempPath)) {
        OcrResult result = extractText(tempPath, language);

        if (!result.success) {
            textEdit->setText("");
            label->setText(result.errorMessage);
        }
        else {
            textEdit->setText(result.text);
            label->setText(
                "Text extracted successfully. You can edit the text before copying "
                "or saving.");
        }
        window.show();
    }
    else {
        textEdit->setText("");
        label->setText("Error occurred while taking screenshot");
        window.show();
        QMessageBox::critical(&window, "Error",
            "Failed to launch Spectacle or take screenshot");
    }

    return app.exec();
}