#include "snake.h"
#include <QPainter>
#include <QTime>
#include <QRandomGenerator>
#include <QDebug>
#include <cmath>

namespace SnakeNamespace
{ // Начало пространства имен

    SnakeGame::SnakeGame(QWidget *parent)
        : QWidget(parent), angle(0), paused(false)
    {
        setStyleSheet("background-color:black;");
        setFixedSize(B_WIDTH, B_HEIGHT);
        loadImages();
        initGame();
    }

    void SnakeGame::loadImages()
    {
        dot.load(":/images/dot.png");
        headSprite.load(":/images/head.png");
        tailSprite.load(":/images/tail.png");
        apple.load(":/images/apple.png");
    }

    void SnakeGame::initGame()
    {
        dots = 3;
        appleCount = 0;
        angle = 0;
        paused = false;

        for (int i = 0; i < dots; ++i)
        {
            x[i] = 50 - i * DOT_SIZE;
            y[i] = 50;
        }

        locateApple();
        timerId = startTimer(DELAY);
    }

    void SnakeGame::locateApple()
    {
        int safemargin = DOT_SIZE * 2; // Безопасное расстояние яблока от краёв

        bool valid = false;
        while (!valid)
        {
            apple_x = safemargin + (QRandomGenerator::global()->bounded((B_WIDTH - 2 * safemargin) / DOT_SIZE)) * DOT_SIZE;
            apple_y = safemargin + (QRandomGenerator::global()->bounded((B_HEIGHT - 2 * safemargin) / DOT_SIZE)) * DOT_SIZE;

            valid = true;

            for (int i = 0; i < dots; ++i)
            {
                if (x[i] == apple_x && y[i] == apple_y)
                {
                    valid = false;
                    break;
                }
            }
        }
    }

    void SnakeGame::move()
    {
        for (int i = dots; i > 0; --i)
        {
            x[i] = x[i - 1];
            y[i] = y[i - 1];
            angles[i] = angles[i - 1]; // Сдвиг углов в массиве для сохранения истории углов
        }

        // Применяем матрицу трансформации к голове
        transformMatrix.reset();
        transformMatrix.translate(x[0], y[0]);
        transformMatrix.rotate(angle);
        QPointF newHeadPos = transformMatrix.map(QPointF(DOT_SIZE, 0));

        x[0] = std::round(newHeadPos.x());
        y[0] = std::round(newHeadPos.y());
        angles[0] = angle; // Сохраняем текущий угол головы
    }

    void SnakeGame::checkCollision()
    {
        // Проверка столкновения головы с телом змейки, 
        // начиная с 3-го сегмента, чтобы не было ложных срабатываний
        for (int i = 2; i < dots; ++i)
        {
            int headLeft = x[0];
            int headRight = x[0] + HEAD_SIZE;
            int headTop = y[0];
            int headBottom = y[0] + HEAD_SIZE;

            int bodyLeft = x[i];
            int bodyRight = x[i] + DOT_SIZE;
            int bodyTop = y[i];
            int bodyBottom = y[i] + DOT_SIZE;

            bool overlap = (headRight > bodyLeft && headLeft < bodyRight &&
                            headBottom > bodyTop && headTop < bodyBottom);

            if (overlap)
            {
                killTimer(timerId);
                timerId = 0;
                return;
            }
        }

        // Проверка столкновения головы со стенами
        int headLeft = x[0];
        int headRight = x[0] + HEAD_SIZE;
        int headTop = y[0];
        int headBottom = y[0] + HEAD_SIZE;

        if (headRight >= B_WIDTH || headLeft < 0 || headBottom >= B_HEIGHT || headTop < 0)
        {
            killTimer(timerId);
            timerId = 0;
        }
    }

    int SnakeGame::getTailAngle()
    {
        if (dots > 1)
        {
            return angles[dots - 1]; // Угол хвоста
        }
        return 0;
    }

    void SnakeGame::togglePause()
    {
        if (paused)
        {
            timerId = startTimer(DELAY); // Перезапуск таймера при снятии паузы
        }
        else
        {
            killTimer(timerId); // Остановка таймера при нажатии паузы
        }
        paused = !paused; // Переключение состояния паузы
        repaint();        // Перерисовка для отображения статуса
    }

    void SnakeGame::timerEvent(QTimerEvent *)
    {
        if (timerId != 0 && !paused)
        { // Обновление если игра не на паузе
            int headLeft = x[0];
            int headRight = x[0] + HEAD_SIZE;
            int headTop = y[0];
            int headBottom = y[0] + HEAD_SIZE;

            int appleLeft = apple_x;
            int appleRight = apple_x + DOT_SIZE;
            int appleTop = apple_y;
            int appleBottom = apple_y + DOT_SIZE;

            bool appleEaten = (headRight > appleLeft && headLeft < appleRight &&
                               headBottom > appleTop && headTop < appleBottom);

            if (appleEaten)
            {
                if (dots < ALL_DOTS)
                {
                    dots++;
                }
                appleCount++;
                locateApple();
            }

            move();
            checkCollision();
        }
        repaint();
    }

    void SnakeGame::paintEvent(QPaintEvent *)
    {
        QPainter qp(this);

        if (paused)
        { // Постановка на паузу
            QString message = "Paused";
            QFont font("Arial", 24, QFont::Bold);
            QFontMetrics fm(font);
            int textWidth = fm.horizontalAdvance(message);

            qp.setPen(Qt::white);
            qp.setFont(font);
            qp.drawText((width() - textWidth) / 2, height() / 2, message);
        }
        else if (timerId != 0)
        { // Отрисовка яблока
            qp.drawImage(apple_x, apple_y, apple);

            for (int i = 0; i < dots; ++i)
            {
                if (i == 0)
                { // Отрисовка головы
                    QTransform transform;
                    transform.translate(x[i] + HEAD_SIZE / 2, y[i] + HEAD_SIZE / 2);
                    transform.rotate(angle);
                    transform.translate(-HEAD_SIZE / 2, -HEAD_SIZE / 2);

                    QImage rotatedHead = headSprite.transformed(transform, Qt::SmoothTransformation);
                    int drawX = x[i] - (rotatedHead.width() - DOT_SIZE) / 2;
                    int drawY = y[i] - (rotatedHead.height() - DOT_SIZE) / 2;
                    qp.drawImage(drawX, drawY, rotatedHead);
                }
                else if (i == dots - 1)
                { // Отрисовка хвоста
                    int tailAngle = getTailAngle();
                    QTransform transform;
                    transform.translate(x[i] + DOT_SIZE / 2, y[i] + DOT_SIZE / 2);
                    transform.rotate(tailAngle);
                    transform.translate(-DOT_SIZE / 2, -DOT_SIZE / 2);

                    QImage rotatedTail = tailSprite.transformed(transform, Qt::SmoothTransformation);
                    int drawX = x[i] - (rotatedTail.width() - DOT_SIZE) / 2;
                    int drawY = y[i] - (rotatedTail.height() - DOT_SIZE) / 2;
                    qp.drawImage(drawX, drawY, rotatedTail);
                }
                else
                { // Отрисовка тела
                    qp.drawImage(x[i], y[i], dot);
                }
            }
            // Счётчик съеденных яблок
            QString score = QString("Score: %1").arg(appleCount);
            qp.setPen(Qt::white);
            qp.setFont(QFont("Arial", 10, QFont::Bold));
            qp.drawText(10, 20, score);
        }
        else
        { // Завершение игры
            QString message1 = "Game Over";
            QString message2 = "Press «ENTER» to restart.";

            QFont font1("Arial", 24, QFont::Bold);
            QFontMetrics fm1(font1);
            int textWidth1 = fm1.horizontalAdvance(message1);
            qp.setPen(Qt::white);
            qp.setFont(font1);
            qp.drawText((width() - textWidth1) / 2, height() / 2 - 10, message1);

            QFont font2("Arial", 8);
            QFontMetrics fm2(font2);
            int textWidth2 = fm2.horizontalAdvance(message2);
            qp.setFont(font2);
            qp.drawText((width() - textWidth2) / 2, height() / 2 + 20, message2);
        }
    }

    void SnakeGame::keyPressEvent(QKeyEvent *e)
    {
        int key = e->key();

        // Проверяем, является ли нажатие реальным, а не повторным удержанием клавиши
        if (!paused && !e->isAutoRepeat())
        {
            if (key == Qt::Key_Left)
            {
                angle -= 15;
            }
            else if (key == Qt::Key_Right)
            {
                angle += 15;
            }
        }

        if (key == Qt::Key_Return)
        {
            if (timerId == 0)
            {
                initGame();
            }
        }
        else if (key == Qt::Key_P)
        {
            togglePause();
        }

        QWidget::keyPressEvent(e);
    }

} // Конец SnakeNamespace