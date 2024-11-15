#ifndef SNAKEGAME_H
#define SNAKEGAME_H

#include <QWidget>
#include <QImage>
#include <QKeyEvent>

namespace SnakeNamespace
{ // Начало пространства имен

    class SnakeGame : public QWidget
    {
        Q_OBJECT

    public:
        explicit SnakeGame(QWidget *parent = nullptr);

    protected:
        void paintEvent(QPaintEvent *) override;
        void timerEvent(QTimerEvent *) override;
        void keyPressEvent(QKeyEvent *) override;

    private:
        void loadImages();     // Загрузка изображений
        void initGame();       // Инициализация игры
        void locateApple();    // Размещение яблока на поле
        void move();           // Движение змеи
        void checkCollision(); // Проверка столкновений
        int getTailAngle();    // Угол хвоста
        void togglePause();    // Пауза

        static const int B_WIDTH = 400;                       // Ширина поля
        static const int B_HEIGHT = 400;                      // Высота поля
        static const int DOT_SIZE = 10;                       // Размер сегмента
        static const int ALL_DOTS = 1600;                     // Максимум сегментов змеи
        static const int RAND_POS = (B_WIDTH / DOT_SIZE) - 1; // Координаты для яблока
        static const int DELAY = 300;                         // Задержка таймера
        static const int HEAD_SIZE = 12;                      // Размер головы

        int timerId;    // ID таймера для игры
        int dots;       // Текущая длина змеи
        int apple_x;    // Координата X яблока
        int apple_y;    // Координата Y яблока
        int appleCount; // Количество съеденных яблок

        int x[ALL_DOTS];      // Массив для хранения X координат сегментов змеи
        int y[ALL_DOTS];      // Массив для хранения Y координат сегментов змеи
        int angles[ALL_DOTS]; // Массив для хранения углов каждого сегмента

        bool paused;                // Флаг для состояния паузы
        double angle;               // Текущий угол головы
        QTransform transformMatrix; // Матрица для преобразования

        QImage dot;        // Изображение сегмента тела
        QImage headSprite; // Изображение головы
        QImage tailSprite; // Изображение хвоста
        QImage apple;      // Изображение яблока
    };

}
#endif