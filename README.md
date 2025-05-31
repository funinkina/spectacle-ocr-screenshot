# Spectacle OCR Screenshot

A simple Qt application that integrates KDE Spectacle screenshot tool with Tesseract OCR to extract text from screenshots as well QR codes.

![Screenshot](screenshot.png)

## Features

- Capture screenshots using KDE's Spectacle tool
- Extract text from screenshots using Tesseract OCR
- Decode QR codes from screenshots
- Display extracted text in a user-friendly interface
- Support for multiple languages
- Edit extracted text before saving
- Copy text to clipboard
- Save text to file
- Save the screenshot as .png

## Requirements

- Qt 6.x
- Tesseract OCR
- Leptonica
- KDE Spectacle
- Zxing (for QR code decoding)

## Installation

### Get the binaries from: [Releases Page](https://github.com/funinkina/spectacle-ocr-screenshot/releases/).

## Usage

Run the application:

```bash
./spectacle-ocr-screenshot
```
> [!TIP]
> ## Recommended Usage
> Create a symlink to the executable in your local `PATH` for easy access:
>
> ```bash
> sudo ln -s spectacle-ocr-screenshot /usr/local/bin/
> ```
>
> Then you can run the application from anywhere using or by assigning a keyboard shortcut to `spectacle-ocr-screenshot`

## The application will:
1. Launch Spectacle in region selection mode
2. After capturing, click on save, this will save to `/tmp`
3. The extracted text will be displayed in the application window
4. You can edit the text, copy it to clipboard or save it to a file

### Command Line Options

- `--lang <language>`: Specify the language(s) for OCR (default: eng)
  - Use ISO 639-3 language codes
  - For multiple languages, join them with '+' (e.g., `--lang eng+hin` for English and Hindi)

- `--disable-qr`: Disable QR code detection

#### Examples:
```bash
# Use English OCR (default)
./spectacle-ocr-screenshot

# Use German OCR
./spectacle-ocr-screenshot --lang deu

# Use multiple languages (English and Spanish)
./spectacle-ocr-screenshot --lang eng+spa
```

## Available Languages

Tesseract OCR supports many languages. Some common language codes:

- `eng` - English
- `deu` - German
- `hin` - Hindi

## Manual Installation
### Building from Source

#### 1. Clone the repository:

```bash
git clone https://github.com/yourusername/spectacle-ocr-screenshot.git
cd spectacle-ocr-screenshot
```

#### 2. Install build dependencies:

For Arch Linux:
```bash
sudo pacman -S qt6-base tesseract leptonica spectacle
```

For Ubuntu/Debian:
```bash
sudo apt install qt6-base-dev tesseract-ocr libleptonica-dev kde-spectacle
```

For Fedora:
```bash
sudo dnf install qt6-qtbase tesseract leptonica spectacle
```

#### 3. Build the project:

```bash
git clone https://github.com/funinkina/spectacle-ocr-screenshot
qmake6 simple.pro
make
```

### You can also build using `cmake`:
Make sure you have cmake installed!

```bash
mkdir build && cd build
cmake ..
make
```

> [!NOTE] 
>You may need to install language packs for Tesseract OCR separately.

## License

[MIT](LICENSE)

## Star History

[![Star History Chart](https://api.star-history.com/svg?repos=funinkina/spectacle-ocr-screenshot&type=Date)](https://www.star-history.com/#funinkina/spectacle-ocr-screenshot&Date)
