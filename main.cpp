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

// Tesseract OCR library
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

// Function to take screenshot with Spectacle
bool takeScreenshot(const QString &outputPath)
{
    QProcess process;
    process.start("spectacle", QStringList() << "-b" << "-n" << "-o" << outputPath);
    process.waitForFinished();
    return process.exitCode() == 0;
}

// Function to perform OCR on an image
QString performOCR(const QString &imagePath)
{
    tesseract::TessBaseAPI *ocr = new tesseract::TessBaseAPI();

    // Initialize tesseract with English language
    if (ocr->Init(nullptr, "eng"))
    {
        delete ocr;
        return "Error initializing Tesseract OCR";
    }

    // Open image with Leptonica
    Pix *image = pixRead(imagePath.toUtf8().constData());
    if (!image)
    {
        ocr->End();
        delete ocr;
        return "Failed to load image";
    }

    // Set image data
    ocr->SetImage(image);

    // Get OCR result
    char *outText = ocr->GetUTF8Text();
    QString result = QString::fromUtf8(outText);

    // Clean up
    delete[] outText;
    pixDestroy(&image);
    ocr->End();
    delete ocr;

    return result;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Create the main window
    QWidget window;
    window.setWindowTitle("Screenshot OCR Tool");
    window.resize(500, 400);

    // Create a layout
    QVBoxLayout *layout = new QVBoxLayout();

    // Add a text label
    QLabel *label = new QLabel("Processing screenshot...");
    layout->addWidget(label);

    // Add a text edit area to display OCR results
    QTextEdit *textEdit = new QTextEdit();
    textEdit->setReadOnly(true);
    textEdit->setMinimumHeight(200);
    layout->addWidget(textEdit);

    // Add a button to capture another screenshot
    QPushButton *button = new QPushButton("Take New Screenshot");
    layout->addWidget(button);

    // Set the layout to the window
    window.setLayout(layout);

    // Create temporary file for screenshot
    QString tempPath = QDir::tempPath() + "/screenshot.png";

    // Function to handle screenshot and OCR
    auto processScreenshot = [&]()
    {
        // Take screenshot
        label->setText("Taking screenshot...");
        QApplication::processEvents();

        if (takeScreenshot(tempPath))
        {
            label->setText("Extracting text...");
            QApplication::processEvents();

            // Perform OCR on the screenshot
            QString extractedText = performOCR(tempPath);

            // Display the extracted text
            textEdit->setText(extractedText);
            label->setText("Text extracted successfully");
        }
        else
        {
            label->setText("Failed to take screenshot");
            QMessageBox::critical(&window, "Error", "Failed to launch Spectacle or take screenshot");
        }
    };

    // Connect button to the screenshot process
    QObject::connect(button, &QPushButton::clicked, processScreenshot);

    // Take screenshot first before showing window
    if (takeScreenshot(tempPath))
    {
        // Process the screenshot
        QString extractedText = performOCR(tempPath);

        // Update UI with results
        textEdit->setText(extractedText);
        label->setText("Text extracted successfully");

        // Show window after screenshot is processed
        window.show();
    }
    else
    {
        // Show window with error message if screenshot fails
        textEdit->setText("Failed to take screenshot");
        label->setText("Error occurred");
        window.show();
        QMessageBox::critical(&window, "Error", "Failed to launch Spectacle or take screenshot");
    }

    return app.exec();
}