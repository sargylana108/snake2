#include <QApplication>
#include "snake.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    SnakeNamespace::SnakeGame window;
    window.setWindowTitle("Snake 2");
    window.show();

    return app.exec();
}