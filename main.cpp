#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Create the main window
    QWidget window;
    window.setWindowTitle("Simple KDE GUI with Qt6");
    window.resize(300, 200);

    // Create a layout
    QVBoxLayout *layout = new QVBoxLayout();

    // Add a text label
    QLabel *label = new QLabel("Hello, KDE Plasma 6!");
    layout->addWidget(label);

    // Add a button
    QPushButton *button = new QPushButton("Click Me");
    QObject::connect(button, &QPushButton::clicked, [&]() {
        label->setText("Button clicked!");
    });
    layout->addWidget(button);

    // Set the layout to the window
    window.setLayout(layout);

    // Show the window
    window.show();

    return app.exec();
}