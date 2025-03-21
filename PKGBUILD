# PKGBUILD for KDE_Screenshot_OCR
pkgname=spectacle-ocr-screenshot
pkgver=0.2.0
pkgrel=1
pkgdesc="Automatically OCR screenshots taken with KDE Spectacle"
arch=('x86_64')
url="https://github.com/funinkina/spectacle-ocr-screenshot"
license=('MIT')
depends=('spectacle', 'tesseract', 'leptonica', 'qt6-base', 'base-devel')
source=("main.cpp", "simple.pro")
sha256sums=('SOME_HASH')
package() {
    cd "${srcdir}/${pkgname}-${pkgver}"
    qmake6
    make
    make install INSTALL_ROOT="${pkgdir}"
}
