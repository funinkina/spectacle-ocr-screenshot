#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#include <ZXing/ReadBarcode.h>
#include <ZXing/Barcode.h>
// qt imports
#include <QCommandLineParser>
#include <QDir>
#include <QProcess>
#include <QTemporaryFile>
#include <QTimer>
#include <QClipboard>
#include <QApplication>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QHBoxLayout>
#include <QDateTime>
#include <QImage>
#include <memory>

bool takeScreenshot(const QString& outputPath) {
	int exitCode = QProcess::execute("spectacle", QStringList()
		<< "-b" << "-r" << "-n" << "-o" << outputPath);
	return exitCode == 0;
}

struct OcrResult {
	QString text;
	bool success;
	QString errorMessage;
	bool isQrCode = false;
};

OcrResult detectQrCode(const QString& imagePath) {
	OcrResult result;
	result.success = false;

	QImage image(imagePath);
	if (image.isNull()) {
		result.errorMessage = "Failed to load image for QR detection";
		return result;
	}

	ZXing::ReaderOptions options;
	options.setFormats(ZXing::BarcodeFormat::QRCode);
	options.setTryHarder(true);

	const uchar* data = image.constBits();
	int width = image.width();
	int height = image.height();
	int bytesPerLine = image.bytesPerLine();

	ZXing::ImageFormat format = image.format() == QImage::Format_Grayscale8 ?
		ZXing::ImageFormat::Lum :
		ZXing::ImageFormat::RGBA;

	ZXing::ImageView imageView(data, width, height, format, bytesPerLine);
	auto zxingResult = ZXing::ReadBarcode(imageView, options);

	if (zxingResult.isValid()) {
		result.text = QString::fromStdString(zxingResult.text());
		result.success = true;
		result.isQrCode = true;
	}
	else {
		result.errorMessage = "Failed to detect valid QR code";
	}

	return result;
}

OcrResult extractText(const QString& imagePath, const QString& language) {
	OcrResult result;
	result.success = true;

	auto ocr = std::make_unique<tesseract::TessBaseAPI>();

	if (ocr->Init(nullptr, language.toUtf8().constData())) {
		result.success = false;
		result.errorMessage =
			"Error initializing Tesseract OCR for language: " + language;
		return result;
	}

	Pix* image = pixRead(imagePath.toUtf8().constData());
	if (!image) {
		ocr->End();
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

	QCommandLineOption disable_qr(
		QStringList() << "disable-qr",
		"Disable QR code detection and extraction.");

	parser.addOption(langOption);
	parser.addOption(disable_qr);
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

	QPushButton* copyButton = new QPushButton("Copy Text");
	QPushButton* saveButton = new QPushButton("Save Text");
	QPushButton* saveImageButton = new QPushButton("Save Image");

	buttonLayout->addWidget(copyButton);
	buttonLayout->addWidget(saveButton);
	buttonLayout->addWidget(saveImageButton);
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

	QObject::connect(saveImageButton, &QPushButton::clicked, [&]() {
		QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
		QString defaultImageName = QDir::homePath() + "/Screenshot_" + timestamp;
		QString imageFileName = QFileDialog::getSaveFileName(
			&window, "Save Screenshot", defaultImageName,
			"Image Files (*.png);;All Files (*)");
		if (!imageFileName.isEmpty()) {
			if (QFile::copy(tempPath, imageFileName))
				label->setText("Screenshot saved successfully");
			else {
				label->setText("Failed to save screenshot");
				QMessageBox::critical(&window, "Error", "Failed to save the screenshot file");
			}
		}
		});

	if (takeScreenshot(tempPath)) {
		OcrResult result;
		if (!parser.isSet(disable_qr)) {
			result = detectQrCode(tempPath);
			if (result.success) {
				textEdit->setText(result.text);
				label->setText("QR code detected and decoded successfully");
				window.show();
				return app.exec();
			}
		}

		result = extractText(tempPath, language);
		if (!result.success) {
			textEdit->setText("");
			label->setText(result.errorMessage);
		}
		else {
			textEdit->setText(result.text);
			label->setText("Text extracted successfully.");
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
