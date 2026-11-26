#include <cmath>
#include <cstdlib>
#include <iostream>
#include <Windows.h>
#include "Header.h"
using namespace std;

extern HDC hdc;     // Импортируем контекст устройства

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

// Функция для черчения линии
bool Line(HDC hdc, int x1, int y1, int x2, int y2)
{
    MoveToEx(hdc, x1, y1, NULL);        // Перемещаем начальную точку
    return LineTo(hdc, x2, y2);         // Чертим линию
}


/***********************************************************
*                   L O C A T I O N                        *
***********************************************************/

Location::Location(int InitX, int InitY)    // Реализация конструктора
{
    x = InitX;          // Устанавливаем начальные координаты
    y = InitY;
}

Location::~Location() {}                    // Реализация деструктора

int Location::GetX() { return x; }          // Получить координату X

int Location::GetY() { return y; }          // Получить координату Y

void Location::SetX(int NewX) { x = NewX; } // Установить координату X

void Location::SetY(int NewY) { y = NewY; } // Установить координату Y

/***********************************************************
*                       P O I N T                          *
***********************************************************/

Point::Point(int InitX, int InitY) : Location(InitX, InitY) // Конструктор
{
    Visible = false;    // Изначально невидима
}

Point::~Point() {}                          // Деструктор

bool Point::GetVisible()                    // Получить видимость
{
    return Visible;
}

void Point::SetVisible(bool NewVisible)     // Установить видимость
{
    Visible = NewVisible;
}

void Point::Show()                          // РЕАЛИЗАЦИЯ МЕТОДА "ВИДИМАЯ ТОЧКА"
{
    Visible = true;
    SetPixel(hdc, x, y, RGB(255, 0, 0));
    SetPixel(hdc, x + 1, y, RGB(255, 0, 0));
    SetPixel(hdc, x + 2, y, RGB(255, 0, 0));
    SetPixel(hdc, x, y + 1, RGB(255, 0, 0));
    SetPixel(hdc, x, y + 2, RGB(255, 0, 0));
    SetPixel(hdc, x + 1, y + 1, RGB(255, 0, 0));
    SetPixel(hdc, x + 2, y + 1, RGB(255, 0, 0));
    SetPixel(hdc, x + 1, y + 2, RGB(255, 0, 0));
    SetPixel(hdc, x + 2, y + 2, RGB(255, 0, 0));
} //void Show()

void Point::Hide()                          // РЕАЛИЗАЦИЯ МЕТОДА "НЕВИДИМАЯ ТОЧКА"
{
    Visible = false;
    SetPixel(hdc, x, y, RGB(0, 0, 255));
    SetPixel(hdc, x + 1, y, RGB(0, 0, 255));
    SetPixel(hdc, x + 2, y, RGB(0, 0, 255));
    SetPixel(hdc, x, y + 1, RGB(0, 0, 255));
    SetPixel(hdc, x, y + 2, RGB(0, 0, 255));
    SetPixel(hdc, x + 1, y + 1, RGB(0, 0, 255));
    SetPixel(hdc, x + 2, y + 1, RGB(0, 0, 255));
    SetPixel(hdc, x + 1, y + 2, RGB(0, 0, 255));
    SetPixel(hdc, x + 2, y + 2, RGB(0, 0, 255));
} //void Hide()

void Point::MoveTo(int NewX, int NewY)      // Переместить точку
{
    Hide();             // Скрыть объект
    x = NewX;           // Установить новые координаты
    y = NewY;
    Show();             // Показать объект
}

void Point::Drag(int xStep)                 // Управление точкой
{
    int Px, Py;         // Текущие координаты
    Px = GetX();        // Получить стартовые координаты
    Py = GetY();
    Show();             // Показать точку
}

/*********************************************************
*                 Б А З О В О Е  П Р Е П Я Т С Т В И Е   *
*********************************************************/

Obstacle::Obstacle(int InitX, int InitY, int bRadius, int sRadius) : Point(InitX, InitY)
{
    Visible = false;
    BRadius = bRadius;              // Установить больший радиус
    SRadius = sRadius;              // Установить меньший радиус
}

Obstacle::~Obstacle() {}

int Obstacle::GetBRadius() { return BRadius; }
int Obstacle::GetSRadius() { return SRadius; }

/*********************************************************
*             П Р Е П Я Т С Т В И Е  У Р О Н А           *
*********************************************************/

DamageObstacle::DamageObstacle(int InitX, int InitY, int bRadius, int sRadius)
    : Obstacle(InitX, InitY, bRadius, sRadius)
{}

DamageObstacle::~DamageObstacle() {}

void DamageObstacle::Show()        // Показать препятствие урона
{
    Visible = true;
    HPEN Pen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));   // КРАСНЫЙ цвет - опасность!
    HBRUSH Brush = CreateSolidBrush(RGB(200, 0, 0));     // Темно-красная заливка
    SelectObject(hdc, Pen);
    SelectObject(hdc, Brush);
    Ellipse(hdc, x - BRadius, y - SRadius, x + BRadius, y + SRadius);
    DeleteObject(Pen);
    DeleteObject(Brush);
}

void DamageObstacle::Hide()        // Скрыть препятствие урона
{
    Visible = false;
    HPEN Pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HBRUSH Brush = CreateSolidBrush(RGB(0, 0, 0));
    SelectObject(hdc, Pen);
    SelectObject(hdc, Brush);
    Ellipse(hdc, x - BRadius, y - SRadius, x + BRadius, y + SRadius);
    DeleteObject(Pen);
    DeleteObject(Brush);
}

/*********************************************************
*            П Р Е П Я Т С Т В И Е  Л Е Ч Е Н И Я        *
*********************************************************/

HealingObstacle::HealingObstacle(int InitX, int InitY, int bRadius, int sRadius)
    : Obstacle(InitX, InitY, bRadius, sRadius)
{}

HealingObstacle::~HealingObstacle() {}

void HealingObstacle::Show()        // Показать лечащее препятствие (зеленый плюсик)
{
    Visible = true;

    HPEN Pen = CreatePen(PS_SOLID, 4, RGB(0, 255, 0));   // ЗЕЛЕНЫЙ цвет - лечение!
    SelectObject(hdc, Pen);

    // Рисуем плюсик: вертикальная и горизонтальная линии
    int size = BRadius;  // Размер плюсика

    // Вертикальная линия плюсика
    Line(hdc, x, y - size, x, y + size);

    // Горизонтальная линия плюсика  
    Line(hdc, x - size, y, x + size, y);

    DeleteObject(Pen);
}

void HealingObstacle::Hide()        // Скрыть лечащее препятствие (зеленый плюсик)
{
    Visible = false;

    HPEN Pen = CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
    SelectObject(hdc, Pen);

    // Рисуем плюсик: вертикальная и горизонтальная линии
    int size = BRadius;  // Размер плюсика

    // Вертикальная линия плюсика
    Line(hdc, x, y - size, x, y + size);

    // Горизонтальная линия плюсика  
    Line(hdc, x - size, y, x + size, y);

    DeleteObject(Pen);
}

/*********************************************************
*           П Р Е П Я Т С Т В И Е  У С К О Р Е Н И Я     *
*********************************************************/

SpeedBoostObstacle::SpeedBoostObstacle(int InitX, int InitY, int bRadius, int sRadius)
    : Obstacle(InitX, InitY, bRadius, sRadius)
{}

SpeedBoostObstacle::~SpeedBoostObstacle() {}

void SpeedBoostObstacle::Show()        // Показать препятствие ускорения (две стрелки вверх /\)
{
    Visible = true;

    HPEN Pen = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));   // СИНИЙ цвет - ускорение!
    SelectObject(hdc, Pen);

    // Рисуем две стрелки вверх
    int size = BRadius;
    int arrowWidth = size / 2;
    int arrowHeight = size;

    // Левая стрелка вверх /
    MoveToEx(hdc, x - arrowWidth, y + arrowHeight / 2, NULL);  // нижний левый
    LineTo(hdc, x, y - arrowHeight / 2);                       // верхний центр

    // Правая стрелка вверх 
    MoveToEx(hdc, x, y - arrowHeight / 2, NULL);               // верхний центр  
    LineTo(hdc, x + arrowWidth, y + arrowHeight / 2);          // нижний правый

    // Вторая пара стрелок (ниже первой)
    int offset = arrowHeight / 2;

    // Левая стрелка вверх /
    MoveToEx(hdc, x - arrowWidth, y + arrowHeight / 2 + offset, NULL);
    LineTo(hdc, x, y - arrowHeight / 2 + offset);

    // Правая стрелка вверх 
    MoveToEx(hdc, x, y - arrowHeight / 2 + offset, NULL);
    LineTo(hdc, x + arrowWidth, y + arrowHeight / 2 + offset);

    DeleteObject(Pen);
}

void SpeedBoostObstacle::Hide()        // Скрыть препятствие ускорения
{
    Visible = false;

    HPEN PenB = CreatePen(PS_SOLID, 3, RGB(0, 0, 0)); // Черный цвет для стирания
    SelectObject(hdc, PenB);

    // Стираем стрелки черными линиями
    int size = BRadius;
    int arrowWidth = size / 2;
    int arrowHeight = size;

    // Стираем первую пару стрелок
    MoveToEx(hdc, x - arrowWidth, y + arrowHeight / 2, NULL);
    LineTo(hdc, x, y - arrowHeight / 2);
    MoveToEx(hdc, x, y - arrowHeight / 2, NULL);
    LineTo(hdc, x + arrowWidth, y + arrowHeight / 2);

    // Стираем вторую пару стрелок
    int offset = arrowHeight / 2;
    MoveToEx(hdc, x - arrowWidth, y + arrowHeight / 2 + offset, NULL);
    LineTo(hdc, x, y - arrowHeight / 2 + offset);
    MoveToEx(hdc, x, y - arrowHeight / 2 + offset, NULL);
    LineTo(hdc, x + arrowWidth, y + arrowHeight / 2 + offset);

    DeleteObject(PenB);
}

/*********************************************************
*          П Р Е П Я Т С Т В И Е  З А М Е Д Л Е Н И Я    *
*********************************************************/

SpeedSlowObstacle::SpeedSlowObstacle(int InitX, int InitY, int bRadius, int sRadius)
    : Obstacle(InitX, InitY, bRadius, sRadius)
{}

SpeedSlowObstacle::~SpeedSlowObstacle() {}

void SpeedSlowObstacle::Show()        // Показать препятствие замедления (две стрелки вниз \/)
{
    Visible = true;

    HPEN Pen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));   // КРАСНЫЙ цвет - замедление!
    SelectObject(hdc, Pen);

    // Рисуем две стрелки вниз \/
    int size = BRadius;
    int arrowWidth = size / 2;
    int arrowHeight = size;

    // Левая стрелка вниз 
    MoveToEx(hdc, x - arrowWidth, y - arrowHeight / 2, NULL);  // верхний левый
    LineTo(hdc, x, y + arrowHeight / 2);                       // нижний центр

    // Правая стрелка вниз /
    MoveToEx(hdc, x, y + arrowHeight / 2, NULL);               // нижний центр  
    LineTo(hdc, x + arrowWidth, y - arrowHeight / 2);          // верхний правый

    // Вторая пара стрелок (ниже первой)
    int offset = arrowHeight / 2;

    // Левая стрелка вниз 
    MoveToEx(hdc, x - arrowWidth, y - arrowHeight / 2 + offset, NULL);
    LineTo(hdc, x, y + arrowHeight / 2 + offset);

    // Правая стрелка вниз /
    MoveToEx(hdc, x, y + arrowHeight / 2 + offset, NULL);
    LineTo(hdc, x + arrowWidth, y - arrowHeight / 2 + offset);

    DeleteObject(Pen);
}

void SpeedSlowObstacle::Hide()        // Скрыть препятствие замедления
{
    Visible = false;

    HPEN PenB = CreatePen(PS_SOLID, 3, RGB(0, 0, 0)); // Черный цвет для стирания
    SelectObject(hdc, PenB);

    // Стираем стрелки черными линиями
    int size = BRadius;
    int arrowWidth = size / 2;
    int arrowHeight = size;

    // Стираем первую пару стрелок
    MoveToEx(hdc, x - arrowWidth, y - arrowHeight / 2, NULL);
    LineTo(hdc, x, y + arrowHeight / 2);
    MoveToEx(hdc, x, y + arrowHeight / 2, NULL);
    LineTo(hdc, x + arrowWidth, y - arrowHeight / 2);

    // Стираем вторую пару стрелок
    int offset = arrowHeight / 2;
    MoveToEx(hdc, x - arrowWidth, y - arrowHeight / 2 + offset, NULL);
    LineTo(hdc, x, y + arrowHeight / 2 + offset);
    MoveToEx(hdc, x, y + arrowHeight / 2 + offset, NULL);
    LineTo(hdc, x + arrowWidth, y - arrowHeight / 2 + offset);

    DeleteObject(PenB);
}


/*********************************************************
*                   А Л Г А (Водоросли)                 *
*********************************************************/

Algae::Algae(int InitX, int InitY, int w, int h)
    : Obstacle(InitX, InitY, w / 2, h / 2), startX(InitX), startY(InitY)
{
    width = w;
    height = h;
    Visible = false;
    collected = false;  // Изначально не собрана
}

Algae::~Algae() {}

void Algae::SetPosition(int newX, int newY)
{
    Hide();
    x = startX = newX;
    y = startY = newY;
    offsetX = offsetY = 0.0;
    targetX = targetY = 0;
    Show();
}

void Algae::ChooseWanderingTarget()
{
    int nextX;
    int nextY;
    do {
        nextX = std::rand() % (2 * wanderingRadius + 1) - wanderingRadius;
        nextY = std::rand() % (2 * wanderingRadius + 1) - wanderingRadius;
    } while (nextX * nextX + nextY * nextY > wanderingRadius * wanderingRadius ||
        (nextX == targetX && nextY == targetY));

    targetX = nextX;
    targetY = nextY;
}

void Algae::UpdatePosition()
{
    if (collected) return;

    if (offsetX == targetX && offsetY == targetY) {
        ChooseWanderingTarget();
    }

    const double dx = targetX - offsetX;
    const double dy = targetY - offsetY;
    const double distance = std::sqrt(dx * dx + dy * dy);

    if (distance <= wanderingSpeed) {
        offsetX = targetX;
        offsetY = targetY;
    }
    else {
        offsetX += dx * wanderingSpeed / distance;
        offsetY += dy * wanderingSpeed / distance;
    }

    // Truncating each offset toward zero keeps the pixel position inside the circle.
    x = startX + static_cast<int>(offsetX);
    y = startY + static_cast<int>(offsetY);
}


/*********************************************************
*            Х О Р О Ш И Е   В О Д О Р О С Л И           *
*********************************************************/

GoodAlgae::GoodAlgae(int InitX, int InitY, int w, int h) : Algae(InitX, InitY, w, h)
{}

GoodAlgae::~GoodAlgae() {}

void GoodAlgae::Show()
{
    Visible = true;

    // ЗЕЛЕНЫЕ водоросли - хорошие
    HPEN Pen = CreatePen(PS_SOLID, 2, RGB(0, 180, 0));   // Темно-зеленый контур
    HBRUSH Brush = CreateSolidBrush(RGB(50, 200, 50));   // Ярко-зеленая заливка
    SelectObject(hdc, Pen);
    SelectObject(hdc, Brush);

    // Рисуем водоросль как прямоугольник, растущий вверх от точки крепления
    // x, y - точка крепления водоросли ко дну (нижняя середина)
    Rectangle(hdc, x - width / 2, y - height, x + width / 2, y);

    // Добавляем детали - пузырьки кислорода (как хороший эффект)
    HPEN BubblePen = CreatePen(PS_SOLID, 1, RGB(100, 255, 255));  // Голубой
    HBRUSH BubbleBrush = CreateSolidBrush(RGB(200, 255, 255));    // Светло-голубой
    SelectObject(hdc, BubblePen);
    SelectObject(hdc, BubbleBrush);

    // Рисуем несколько пузырьков
    for (int i = 0; i < 3; i++) {
        int bubbleX = x - width / 4 + (i * width / 4);
        int bubbleY = y - height / 2 - i * 10;
        int bubbleSize = 5 + i * 2;
        Ellipse(hdc, bubbleX - bubbleSize, bubbleY - bubbleSize,
            bubbleX + bubbleSize, bubbleY + bubbleSize);
    }

    DeleteObject(BubblePen);
    DeleteObject(BubbleBrush);
    DeleteObject(Pen);
    DeleteObject(Brush);
}

void GoodAlgae::Hide()
{
    Visible = false;

    // Стираем водоросль черным прямоугольником
    HPEN Pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HBRUSH Brush = CreateSolidBrush(RGB(0, 0, 0));
    SelectObject(hdc, Pen);
    SelectObject(hdc, Brush);

    Rectangle(hdc, x - width / 2, y - height, x + width / 2, y);

    // Стираем пузырьки
    for (int i = 0; i < 3; i++) {
        int bubbleX = x - width / 4 + (i * width / 4);
        int bubbleY = y - height / 2 - i * 10;
        int bubbleSize = 5 + i * 2;
        Ellipse(hdc, bubbleX - bubbleSize, bubbleY - bubbleSize,
            bubbleX + bubbleSize, bubbleY + bubbleSize);
    }

    DeleteObject(Pen);
    DeleteObject(Brush);
}

/*********************************************************
*            П Л О Х И Е   В О Д О Р О С Л И             *
*********************************************************/

BadAlgae::BadAlgae(int InitX, int InitY, int w, int h) : Algae(InitX, InitY, w, h)
{}

BadAlgae::~BadAlgae() {}

void BadAlgae::Show()
{
    Visible = true;

    // КОРИЧНЕВЫЕ/ЧЕРНЫЕ водоросли - плохие (ядовитые)
    HPEN Pen = CreatePen(PS_SOLID, 2, RGB(100, 50, 0));   // Темно-коричневый контур
    HBRUSH Brush = CreateSolidBrush(RGB(150, 80, 30));    // Коричневая заливка
    SelectObject(hdc, Pen);
    SelectObject(hdc, Brush);

    // Рисуем водоросль
    Rectangle(hdc, x - width / 2, y - height, x + width / 2, y);

    // Добавляем шипы/колючки (как опасный эффект)
    HPEN ThornPen = CreatePen(PS_SOLID, 2, RGB(255, 50, 50));  // Красный
    SelectObject(hdc, ThornPen);

    // Рисуем шипы по бокам водоросли
    for (int i = 0; i < 5; i++) {
        int spikeY = y - height + i * (height / 4);

        // Левый шип
        Line(hdc, x - width / 2, spikeY, x - width / 2 - 10, spikeY - 5);
        Line(hdc, x - width / 2, spikeY, x - width / 2 - 10, spikeY + 5);

        // Правый шип
        Line(hdc, x + width / 2, spikeY, x + width / 2 + 10, spikeY - 5);
        Line(hdc, x + width / 2, spikeY, x + width / 2 + 10, spikeY + 5);
    }

    // Рисуем череп сверху (символ опасности)
    HPEN SkullPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
    SelectObject(hdc, SkullPen);

    int skullX = x;
    int skullY = y - height - 10;
    int skullSize = 8;

    // Череп (круг)
    Ellipse(hdc, skullX - skullSize, skullY - skullSize,
        skullX + skullSize, skullY + skullSize);

    // Глазницы
    Line(hdc, skullX - 3, skullY - 3, skullX - 6, skullY - 6);
    Line(hdc, skullX + 3, skullY - 3, skullX + 6, skullY - 6);

    // Улыбка (костяная)
    Arc(hdc, skullX - 5, skullY, skullX + 5, skullY + 10,
        skullX - 5, skullY + 5, skullX + 5, skullY + 5);

    DeleteObject(SkullPen);
    DeleteObject(ThornPen);
    DeleteObject(Pen);
    DeleteObject(Brush);
}

void BadAlgae::Hide()
{
    Visible = false;

    // Стираем водоросль
    HPEN Pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HBRUSH Brush = CreateSolidBrush(RGB(0, 0, 0));
    SelectObject(hdc, Pen);
    SelectObject(hdc, Brush);

    Rectangle(hdc, x - width / 2, y - height, x + width / 2, y);

    // Стираем шипы
    HPEN BlackPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    SelectObject(hdc, BlackPen);

    for (int i = 0; i < 5; i++) {
        int spikeY = y - height + i * (height / 4);

        // Левый шип
        Line(hdc, x - width / 2, spikeY, x - width / 2 - 10, spikeY - 5);
        Line(hdc, x - width / 2, spikeY, x - width / 2 - 10, spikeY + 5);

        // Правый шип
        Line(hdc, x + width / 2, spikeY, x + width / 2 + 10, spikeY - 5);
        Line(hdc, x + width / 2, spikeY, x + width / 2 + 10, spikeY + 5);
    }

    // Стираем череп
    HPEN SkullPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    SelectObject(hdc, SkullPen);

    int skullX = x;
    int skullY = y - height - 10;
    int skullSize = 8;

    // Череп (круг)
    Ellipse(hdc, skullX - skullSize, skullY - skullSize,
        skullX + skullSize, skullY + skullSize);

    // Глазницы
    Line(hdc, skullX - 3, skullY - 3, skullX - 6, skullY - 6);
    Line(hdc, skullX + 3, skullY - 3, skullX + 6, skullY - 6);

    // Улыбка (костяная)
    Arc(hdc, skullX - 5, skullY, skullX + 5, skullY + 10,
        skullX - 5, skullY + 5, skullX + 5, skullY + 5);

    DeleteObject(SkullPen);
    DeleteObject(BlackPen);
    DeleteObject(Pen);
    DeleteObject(Brush);
}


/*********************************************************
*                       F I S H                          *
*********************************************************/

Fish::Fish(int InitX, int InitY, int bRad, int sRad) : Point(InitX, InitY)
{
    Visible = false;
    BRadius = bRad;               // Установить больший радиус рыбы
    SRadius = sRad;               // Установить меньший радиус рыбы
    isDead = false;               // Изначально жива
    isSmall = false;              // Изначально нормального размера
    isBoosted = false;            // Изначально нормальная скорость
    Direction = 0;
}

Fish::~Fish() {}                  // Деструктор

void Fish::Show()                 // Показать рыбу
{
    Visible = true;

    DrawBody();
    DrawTail();
    DrawEye();
    DrawFin();
}

void Fish::ShowCamouflaged()
{
    ShowWithBody(RGB(45, 145, 85), false);
}

void Fish::ShowCollisionReaction(COLORREF bodyColor)
{
    ShowWithBody(bodyColor, true);
}

void Fish::ShowWithBody(COLORREF bodyColor, bool angularBody)
{
    Visible = true;

    const COLORREF outlineColor = angularBody ? RGB(255, 255, 255) : RGB(20, 80, 45);
    HPEN outline = CreatePen(PS_SOLID, 2, outlineColor);
    HBRUSH fill = CreateSolidBrush(bodyColor);
    HGDIOBJ previousPen = SelectObject(hdc, outline);
    HGDIOBJ previousBrush = SelectObject(hdc, fill);

    if (angularBody) {
        const POINT body[] = {
            { x, y + SRadius / 2 },
            { x + BRadius / 4, y + SRadius / 6 },
            { x + BRadius * 3 / 4, y + SRadius / 6 },
            { x + BRadius, y + SRadius / 2 },
            { x + BRadius * 3 / 4, y + SRadius * 5 / 6 },
            { x + BRadius / 4, y + SRadius * 5 / 6 }
        };
        Polygon(hdc, body, 6);
    }
    else {
        Ellipse(hdc, x, y, x + BRadius, y + SRadius);
    }

    SelectObject(hdc, previousBrush);
    SelectObject(hdc, previousPen);
    DeleteObject(fill);
    DeleteObject(outline);

    DrawTail();
    DrawEye();
    DrawFin();
}

void Fish::Hide()                 // Скрыть рыбу
{
    Visible = false;

    HideBody();
    HideTail();
    HideEye();
    HideFin();
}

void Fish::Drag(int xStep)        // Переместить рыбу
{
    Hide();

    if (KEY_DOWN(0x57))     // W
    {
        SetY(GetY() - xStep);
        Direction = 1;
    }
    else if (KEY_DOWN(0x41))    // A
    {
        SetX(GetX() - xStep);
        Direction = 2;
    }
    else if (KEY_DOWN(0x53))    // S
    {
        SetY(GetY() + xStep);
        Direction = 3;
    }
    else if (KEY_DOWN(0x44))    // D
    {
        SetX(GetX() + xStep);
        Direction = 4;
    }

    Show();

    Sleep(50);
}

int Fish::GetBRadius() { return BRadius; }           // Получить больший радиус
int Fish::GetSRadius() { return SRadius; }           // Получить меньший радиус
bool Fish::GetIsDead() { return isDead; }            // Получить статус смерти
bool Fish::GetIsSmall() { return isSmall; }          // Получить статус размера
int Fish::GetIsBoosted() { return isBoosted; }
int Fish::GetDirection() { return Direction; }

void Fish::SetDead(bool dead) { isDead = dead; }        // Установить статус смерти
void Fish::SetSmall(bool smal) { isSmall = smal; }      // Установить статус размера
void Fish::SetBoost(bool boost) { isBoosted = boost; }  // Установить скорость рыбки

/*********************************************************
*                   G O L D F I S H                      *
*********************************************************/

GoldFish::GoldFish(int InitX, int InitY, int bRad, int sRad) : Fish(InitX, InitY, bRad, sRad)
{
    Visible = false;
    BRadius = bRad;               // Установить больший радиус рыбы
    SRadius = sRad;               // Установить меньший радиус рыбы
    isDead = false;               // Изначально жива
    isSmall = false;              // Изначально нормального размера
    isBoosted = false;            // Изначально нормальная скорость
}

GoldFish::~GoldFish() {}

void GoldFish::DrawBody()
{
    // Рисуем тело рыбы с градиентом
    HBRUSH brushGold1 = CreateSolidBrush(RGB(255, 215, 0));      // Основной золотой
    HBRUSH brushGold2 = CreateSolidBrush(RGB(255, 230, 120));    // Светлый золотой
    HBRUSH brushGold3 = CreateSolidBrush(RGB(255, 200, 50));     // Темный золотой

    // Основное тело с градиентным эффектом (рисуем несколько эллипсов)
    SelectObject(hdc, CreatePen(PS_SOLID, 1, RGB(255, 225, 100)));
    SelectObject(hdc, brushGold1);
    Ellipse(hdc, x, y, x + BRadius, y + SRadius);

    // Добавляем блик на теле
    SelectObject(hdc, CreatePen(PS_SOLID, 1, RGB(255, 240, 180)));
    SelectObject(hdc, brushGold2);
    Ellipse(hdc, x + BRadius / 4, y + SRadius / 4,
        x + BRadius * 3 / 4, y + SRadius * 2 / 3);

    // Тень в нижней части
    SelectObject(hdc, CreatePen(PS_SOLID, 1, RGB(255, 190, 30)));
    SelectObject(hdc, brushGold3);
    Ellipse(hdc, x + BRadius / 3, y + SRadius * 2 / 3,
        x + BRadius * 2 / 3, y + SRadius * 5 / 6);

    DeleteObject(brushGold1);
    DeleteObject(brushGold2);
    DeleteObject(brushGold3);
}

void GoldFish::DrawTail()
{
    // Хвост СЛЕВА от тела (рыбка смотрит вправо)
    HPEN penTail = CreatePen(PS_SOLID, 2, RGB(255, 220, 100));
    HBRUSH brushTail = CreateSolidBrush(RGB(255, 240, 180));
    SelectObject(hdc, penTail);
    SelectObject(hdc, brushTail);

    // Хвост начинается в ЛЕВОЙ части тела
    int tailBaseX = x;                      // Левая граница тела
    int tailBaseY = y + SRadius / 2;        // Центр по высоте

    // Компактный треугольный хвост (слева)
    POINT tail[4] = {
        { tailBaseX, tailBaseY },                         // Основание (левый край тела)
        { tailBaseX - SRadius / 4, y + SRadius / 4 },     // Верхний кончик (влево от тела)
        { tailBaseX - SRadius / 4, y + SRadius * 3 / 4 }, // Нижний кончик (влево от тела)
        { tailBaseX, tailBaseY }                          // Замыкаем
    };

    Polygon(hdc, tail, 4);

    HPEN penDetail = CreatePen(PS_SOLID, 1, RGB(255, 200, 80));
    SelectObject(hdc, penDetail);

    // Одна линия по центру хвоста
    MoveToEx(hdc, tailBaseX, tailBaseY, NULL);
    LineTo(hdc, tailBaseX - SRadius / 4, tailBaseY);

    DeleteObject(penTail);
    DeleteObject(brushTail);
    DeleteObject(penDetail);
}

void GoldFish::DrawEye()
{
    // Рисуем глаз
    HPEN penEyeOutline = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));      // Черная обводка
    HBRUSH brushEyeWhite = CreateSolidBrush(RGB(255, 255, 255));    // Белок глаза
    HBRUSH brushEyePupil = CreateSolidBrush(RGB(0, 0, 0));          // Зрачок
    HBRUSH brushEyeShine = CreateSolidBrush(RGB(220, 220, 255));    // Блик

    // Белок глаза
    SelectObject(hdc, penEyeOutline);
    SelectObject(hdc, brushEyeWhite);
    Ellipse(hdc, x + BRadius * 2 / 3, y + SRadius / 3,
        x + BRadius * 2 / 3 + SRadius / 4, y + SRadius / 3 + SRadius / 4);

    // Зрачок
    SelectObject(hdc, brushEyePupil);
    Ellipse(hdc, x + BRadius * 2 / 3 + SRadius / 12, y + SRadius / 3 + SRadius / 12,
        x + BRadius * 2 / 3 + SRadius / 6, y + SRadius / 3 + SRadius / 6);

    // Блик в глазу
    SelectObject(hdc, brushEyeShine);
    Ellipse(hdc, x + BRadius * 2 / 3 + SRadius / 10, y + SRadius / 3 + SRadius / 10,
        x + BRadius * 2 / 3 + SRadius / 8, y + SRadius / 3 + SRadius / 8);

    DeleteObject(penEyeOutline);
    DeleteObject(brushEyeWhite);
    DeleteObject(brushEyePupil);
    DeleteObject(brushEyeShine);
}

void GoldFish::DrawFin()
{
    // Рисуем плавник
    HPEN penFin = CreatePen(PS_SOLID, 2, RGB(255, 220, 100));
    HBRUSH brushFin = CreateSolidBrush(RGB(255, 240, 180));
    SelectObject(hdc, penFin);
    SelectObject(hdc, brushFin);

    // Верхний плавник (спинной)
    POINT finPoints[5];
    int finX = x + BRadius / 3;

    finPoints[0] = { finX, y - SRadius / 8 };
    finPoints[1] = { finX - SRadius / 3, y - SRadius / 2 };
    finPoints[2] = { finX, y - SRadius / 3 };
    finPoints[3] = { finX + SRadius / 3, y - SRadius / 2 };
    finPoints[4] = { finX, y - SRadius / 8 };

    Polygon(hdc, finPoints, 5);

    // Нижний плавник (брюшной)
    POINT lowerFin[5];
    lowerFin[0] = { finX, y + SRadius * 7 / 8 };
    lowerFin[1] = { finX - SRadius / 4, y + SRadius + SRadius / 6 };
    lowerFin[2] = { finX, y + SRadius };
    lowerFin[3] = { finX + SRadius / 4, y + SRadius + SRadius / 6 };
    lowerFin[4] = { finX, y + SRadius * 7 / 8 };

    Polygon(hdc, lowerFin, 5);

    // Узоры на плавниках
    HPEN penPattern = CreatePen(PS_SOLID, 1, RGB(255, 200, 80));
    SelectObject(hdc, penPattern);

    // Линии на верхнем плавнике
    for (int i = 0; i < 3; i++) {
        int offset = SRadius * i / 12;
        MoveToEx(hdc, finX, y - SRadius / 8 + offset, NULL);
        LineTo(hdc, finX - SRadius / 4 + offset / 2, y - SRadius / 2 + offset);

        MoveToEx(hdc, finX, y - SRadius / 8 + offset, NULL);
        LineTo(hdc, finX + SRadius / 4 - offset / 2, y - SRadius / 2 + offset);
    }

    DeleteObject(penFin);
    DeleteObject(brushFin);
    DeleteObject(penPattern);
}

void GoldFish::HideBody()
{
    // Стираем тело рыбы
    HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));

    // Основное тело
    SelectObject(hdc, CreatePen(PS_SOLID, 1, RGB(0, 0, 0)));
    SelectObject(hdc, brush);
    Ellipse(hdc, x, y, x + BRadius, y + SRadius);

    DeleteObject(brush);
}

void GoldFish::HideTail()
{
    // Компактный хвост СЛЕВА от тела (рыбка смотрит вправо)
    HPEN penTail = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HBRUSH brushTail = CreateSolidBrush(RGB(0, 0, 0));
    SelectObject(hdc, penTail);
    SelectObject(hdc, brushTail);

    // Хвост начинается в ЛЕВОЙ части тела
    int tailBaseX = x;  // Левая граница тела
    int tailBaseY = y + SRadius / 2;  // Центр по высоте

    // Компактный треугольный хвост (слева)
    POINT tail[4] = {
        { tailBaseX, tailBaseY },                     // Основание (левый край тела)
        { tailBaseX - SRadius / 4, y + SRadius / 4 },     // Верхний кончик (влево от тела)
        { tailBaseX - SRadius / 4, y + SRadius * 3 / 4 },   // Нижний кончик (влево от тела)
        { tailBaseX, tailBaseY }                      // Замыкаем
    };

    Polygon(hdc, tail, 4);

    DeleteObject(penTail);
    DeleteObject(brushTail);
}

void GoldFish::HideEye()
{
    // Рисуем выразительный глаз
    HPEN penEyeOutline = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));      // Черная обводка
    HBRUSH brushEye = CreateSolidBrush(RGB(0, 0, 0));    // Белок глаза

    // Белок глаза
    SelectObject(hdc, penEyeOutline);
    SelectObject(hdc, brushEye);
    Ellipse(hdc, x + BRadius * 2 / 3, y + SRadius / 3,
        x + BRadius * 2 / 3 + SRadius / 4, y + SRadius / 3 + SRadius / 4);

    DeleteObject(penEyeOutline);
    DeleteObject(brushEye);
}

void GoldFish::HideFin()
{
    // Рисуем изящный плавник
    HPEN penFin = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HBRUSH brushFin = CreateSolidBrush(RGB(0, 0, 0));
    SelectObject(hdc, penFin);
    SelectObject(hdc, brushFin);

    // Верхний плавник (спинной)
    POINT finPoints[5];
    int finX = x + BRadius / 3;

    finPoints[0] = { finX, y - SRadius / 8 };
    finPoints[1] = { finX - SRadius / 3, y - SRadius / 2 };
    finPoints[2] = { finX, y - SRadius / 3 };
    finPoints[3] = { finX + SRadius / 3, y - SRadius / 2 };
    finPoints[4] = { finX, y - SRadius / 8 };

    Polygon(hdc, finPoints, 5);

    // Нижний плавник (брюшной)
    POINT lowerFin[5];
    lowerFin[0] = { finX, y + SRadius * 7 / 8 };
    lowerFin[1] = { finX - SRadius / 4, y + SRadius + SRadius / 6 };
    lowerFin[2] = { finX, y + SRadius };
    lowerFin[3] = { finX + SRadius / 4, y + SRadius + SRadius / 6 };
    lowerFin[4] = { finX, y + SRadius * 7 / 8 };

    Polygon(hdc, lowerFin, 5);

    // Узоры на плавниках
    HPEN penPattern = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    SelectObject(hdc, penPattern);

    // Линии на верхнем плавнике
    for (int i = 0; i < 3; i++) {
        int offset = SRadius * i / 12;
        MoveToEx(hdc, finX, y - SRadius / 8 + offset, NULL);
        LineTo(hdc, finX - SRadius / 4 + offset / 2, y - SRadius / 2 + offset);

        MoveToEx(hdc, finX, y - SRadius / 8 + offset, NULL);
        LineTo(hdc, finX + SRadius / 4 - offset / 2, y - SRadius / 2 + offset);
    }

    DeleteObject(penFin);
    DeleteObject(brushFin);
    DeleteObject(penPattern);
}


/*********************************************************
*              S M A L L G O L D F I S H                  *
*********************************************************/

SmallGoldFish::SmallGoldFish(int InitX, int InitY, int bRad, int sRad) : GoldFish(InitX, InitY, bRad, sRad)
{
    Visible = false;
    BRadius = bRad / 2;               // Установить больший радиус рыбы
    SRadius = sRad / 2;               // Установить меньший радиус рыбы
    isDead = false;               // Изначально жива
    isSmall = true;              // Изначально маленького размера
    isBoosted = false;            // Изначально нормальная скорость
}

SmallGoldFish::~SmallGoldFish() {}


/*********************************************************
*          D E A D S M A L L G O L D F I S H             *
*********************************************************/

DeadSmallGoldFish::DeadSmallGoldFish(int InitX, int InitY, int bRad, int sRad) : SmallGoldFish(InitX, InitY, bRad, sRad)
{
    Visible = false;
    BRadius = bRad / 2;               // Установить больший радиус рыбы
    SRadius = sRad / 2;               // Установить меньший радиус рыбы
    isDead = true;               // Изначально погибшая
    isSmall = true;              // Изначально малого размера
    isBoosted = false;            // Изначально нормальная скорость
}

DeadSmallGoldFish::~DeadSmallGoldFish() {}

void DeadSmallGoldFish::DrawEye()
{
    // Рисуем глаз рыбы
    HPEN Pen = CreatePen(PS_SOLID, 4, RGB(0, 0, 0));      // Черный цвет
    SelectObject(hdc, Pen);

    // ГЛАЗ (перекрестие - мертвый глаз)
    Line(hdc, x + BRadius * 2 / 3 - SRadius / 8, y + SRadius / 3 - SRadius / 8,
        x + BRadius * 2 / 3 + SRadius / 8, y + SRadius / 3 + SRadius / 8);
    Line(hdc, x + BRadius * 2 / 3 + SRadius / 8, y + SRadius / 3 - SRadius / 8,
        x + BRadius * 2 / 3 - SRadius / 8, y + SRadius / 3 + SRadius / 8);

    DeleteObject(Pen);
}


void DeadSmallGoldFish::HideEye()
{
    DeadSmallGoldFish::DrawEye();
}

/*********************************************************
*             I N J U R E D G O L D F I S H              *
*********************************************************/

InjuredGoldFish::InjuredGoldFish(int InitX, int InitY, int bRad, int sRad) : GoldFish(InitX, InitY, bRad, sRad)
{
    Visible = false;
    BRadius = bRad;           // Установить больший радиус рыбы
    SRadius = sRad;           // Установить меньший радиус рыбы
    isDead = false;               // Изначально жива
    isSmall = false;               // Нормального размера
    isBoosted = false;            // Изначально нормальная скорость
}

InjuredGoldFish::~InjuredGoldFish() {}

void InjuredGoldFish::DrawBody()
{
    // Тело тоже немного повреждено - царапины
    GoldFish::DrawBody(); // Сначала рисуем обычное тело

    // Добавляем царапины на теле
    HPEN penScratch = CreatePen(PS_SOLID, 1, RGB(200, 150, 100));
    SelectObject(hdc, penScratch);

    // Несколько царапин
    MoveToEx(hdc, x + BRadius / 3, y + SRadius / 4, NULL);
    LineTo(hdc, x + BRadius / 2, y + SRadius / 3);

    MoveToEx(hdc, x + BRadius / 2, y + SRadius * 2 / 3, NULL);
    LineTo(hdc, x + BRadius * 2 / 3, y + SRadius * 3 / 4);

    DeleteObject(penScratch);
}


void InjuredGoldFish::DrawTail()
{
    // Поврежденный хвост (часть откушена)
    HPEN penTail = CreatePen(PS_SOLID, 2, RGB(255, 200, 100));
    HBRUSH brushTail = CreateSolidBrush(RGB(255, 230, 180));
    SelectObject(hdc, penTail);
    SelectObject(hdc, brushTail);

    // Хвост с "надкусом" - неполный треугольник
    POINT tail[4] = {
        { x, y + SRadius / 2 },
        { x - SRadius / 4, y + SRadius / 4 },      // Верхний кончик (укорочен)
        { x - SRadius / 3, y + SRadius / 2 },      // Вместо нижнего кончика - обрубок
        { x, y + SRadius / 2 }
    };

    Polygon(hdc, tail, 4);

    // Рваный край (симуляция укуса)
    HPEN penBite = CreatePen(PS_SOLID, 1, RGB(200, 150, 100));
    SelectObject(hdc, penBite);

    // Несколько линий, показывающих рваный край
    for (int i = 0; i < 3; i++) {
        int biteX = x - SRadius / 3 + i * 2;
        int biteY = y + SRadius / 2 + i * 3;
        MoveToEx(hdc, biteX, biteY, NULL);
        LineTo(hdc, biteX - 3, biteY + 3);
    }

    DeleteObject(penTail);
    DeleteObject(brushTail);
    DeleteObject(penBite);
}

void InjuredGoldFish::HideTail()
{
    // Поврежденный хвост (часть откушена)
    HPEN penTail = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HBRUSH brushTail = CreateSolidBrush(RGB(0, 0, 0));
    SelectObject(hdc, penTail);
    SelectObject(hdc, brushTail);

    // Хвост с "надкусом" - неполный треугольник
    POINT tail[4] = {
        { x, y + SRadius / 2 },
        { x - SRadius / 4, y + SRadius / 4 },      // Верхний кончик (укорочен)
        { x - SRadius / 3, y + SRadius / 2 },      // Вместо нижнего кончика - обрубок
        { x, y + SRadius / 2 }
    };

    Polygon(hdc, tail, 4);

    // Рваный край (симуляция укуса)
    HPEN penBite = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    SelectObject(hdc, penBite);

    // Несколько линий, показывающих рваный край
    for (int i = 0; i < 3; i++) {
        int biteX = x - SRadius / 3 + i * 2;
        int biteY = y + SRadius / 2 + i * 3;
        MoveToEx(hdc, biteX, biteY, NULL);
        LineTo(hdc, biteX - 3, biteY + 3);
    }

    DeleteObject(penTail);
    DeleteObject(brushTail);
    DeleteObject(penBite);
}


/*********************************************************
*         S M A L L I N J U R E D G O L D F I S H        *
*********************************************************/

SmallInjuredGoldFish::SmallInjuredGoldFish(int InitX, int InitY, int bRad, int sRad) : InjuredGoldFish(InitX, InitY, bRad, sRad)
{
    Visible = false;
    BRadius = bRad / 2;           // Установить больший радиус рыбы
    SRadius = sRad / 2;           // Установить меньший радиус рыбы
    isDead = false;               // Изначально жива
    isSmall = true;               // Малого размера
    isBoosted = false;            // Изначально нормальная скорость
}

SmallInjuredGoldFish::~SmallInjuredGoldFish() {}


/*********************************************************
*     D E A D S M A L L I N J U R E D G O L D F I S H    *
*********************************************************/

DeadSmallInjuredGoldFish::DeadSmallInjuredGoldFish(int InitX, int InitY, int bRad, int sRad) : SmallInjuredGoldFish(InitX, InitY, bRad, sRad)
{
    Visible = false;
    BRadius = bRad / 2;           // Установить больший радиус рыбы
    SRadius = sRad / 2;           // Установить меньший радиус рыбы
    isDead = true;                // Изначально погибшая
    isSmall = true;               // Малого размера
    isBoosted = false;            // Изначально нормальная скорость
}

DeadSmallInjuredGoldFish::~DeadSmallInjuredGoldFish() {}

void DeadSmallInjuredGoldFish::DrawEye()
{
    // Рисуем глаз рыбы
    HPEN Pen = CreatePen(PS_SOLID, 4, RGB(0, 0, 0));      // Черный цвет
    SelectObject(hdc, Pen);

    // ГЛАЗ (перекрестие - мертвый глаз)
    Line(hdc, x + BRadius * 2 / 3 - SRadius / 8, y + SRadius / 3 - SRadius / 8,
        x + BRadius * 2 / 3 + SRadius / 8, y + SRadius / 3 + SRadius / 8);
    Line(hdc, x + BRadius * 2 / 3 + SRadius / 8, y + SRadius / 3 - SRadius / 8,
        x + BRadius * 2 / 3 - SRadius / 8, y + SRadius / 3 + SRadius / 8);

    DeleteObject(Pen);
}


void DeadSmallInjuredGoldFish::HideEye()
{
    DeadSmallInjuredGoldFish::DrawEye();
}

/*********************************************************
*            I N J U R E D G O L D F I S H 1             *
*********************************************************/

InjuredGoldFish1::InjuredGoldFish1(int InitX, int InitY, int bRad, int sRad) : InjuredGoldFish(InitX, InitY, bRad, sRad)
{
    Visible = false;
    BRadius = bRad;           // Установить больший радиус рыбы
    SRadius = sRad;           // Установить меньший радиус рыбы
    isDead = false;               // Изначально жива
    isSmall = false;               // Нормального размера
    isBoosted = false;            // Изначально нормальная скорость
}

InjuredGoldFish1::~InjuredGoldFish1() {}

void InjuredGoldFish1::DrawBody()
{
    // Тело с еще большими повреждениями
    InjuredGoldFish::DrawBody(); // Рисуем тело с царапинами

    // Добавляем "синяк" на теле
    HPEN penBruise = CreatePen(PS_SOLID, 1, RGB(180, 130, 80));
    HBRUSH brushBruise = CreateSolidBrush(RGB(220, 180, 140));
    SelectObject(hdc, penBruise);
    SelectObject(hdc, brushBruise);

    // Темное пятно на теле
    Ellipse(hdc, x + BRadius / 2, y + SRadius / 3,
        x + BRadius * 2 / 3, y + SRadius / 2);

    DeleteObject(penBruise);
    DeleteObject(brushBruise);
}

void InjuredGoldFish1::DrawTail()
{
    // Еще более поврежденный хвост
    HPEN penTail = CreatePen(PS_SOLID, 2, RGB(220, 180, 120));
    HBRUSH brushTail = CreateSolidBrush(RGB(240, 210, 160));
    SelectObject(hdc, penTail);
    SelectObject(hdc, brushTail);

    // Очень маленький обрубок хвоста
    POINT tail[4] = {
        { x, y + SRadius / 2 },
        { x - SRadius / 8, y + SRadius / 3 },
        { x - SRadius / 6, y + SRadius * 2 / 3 },
        { x, y + SRadius / 2 }
    };

    Polygon(hdc, tail, 4);

    // Больше следов укусов
    HPEN penBite = CreatePen(PS_SOLID, 1, RGB(180, 130, 80));
    SelectObject(hdc, penBite);

    // Рваные края
    for (int i = 0; i < 5; i++) {
        int biteX = x - SRadius / 6 + i;
        int biteY = y + SRadius / 2 - 3 + i * 2;
        MoveToEx(hdc, biteX, biteY, NULL);
        LineTo(hdc, biteX - 2, biteY + 2);
    }

    DeleteObject(penTail);
    DeleteObject(brushTail);
    DeleteObject(penBite);
}

void InjuredGoldFish1::DrawFin()
{
    // Плавник совсем откушен - только обрубок
    HPEN penStub = CreatePen(PS_SOLID, 2, RGB(220, 180, 120));
    SelectObject(hdc, penStub);

    // Маленький обрубок вместо плавника
    int stubX = x + BRadius / 3;
    MoveToEx(hdc, stubX, y, NULL);
    LineTo(hdc, stubX - 2, y - 5);
    LineTo(hdc, stubX + 2, y - 5);
    LineTo(hdc, stubX, y);

    DeleteObject(penStub);
}

void InjuredGoldFish1::HideTail()
{
    // Еще более поврежденный хвост
    HPEN penTail = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HBRUSH brushTail = CreateSolidBrush(RGB(0, 0, 0));
    SelectObject(hdc, penTail);
    SelectObject(hdc, brushTail);

    // Очень маленький обрубок хвоста
    POINT tail[4] = {
        { x, y + SRadius / 2 },
        { x - SRadius / 8, y + SRadius / 3 },
        { x - SRadius / 6, y + SRadius * 2 / 3 },
        { x, y + SRadius / 2 }
    };

    Polygon(hdc, tail, 4);

    // Больше следов укусов
    HPEN penBite = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    SelectObject(hdc, penBite);

    // Рваные края
    for (int i = 0; i < 5; i++) {
        int biteX = x - SRadius / 6 + i;
        int biteY = y + SRadius / 2 - 3 + i * 2;
        MoveToEx(hdc, biteX, biteY, NULL);
        LineTo(hdc, biteX - 2, biteY + 2);
    }

    DeleteObject(penTail);
    DeleteObject(brushTail);
    DeleteObject(penBite);
}

void InjuredGoldFish1::HideFin()
{
    // Плавник совсем откушен - только обрубок
    HPEN penStub = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    SelectObject(hdc, penStub);

    // Маленький обрубок вместо плавника
    int stubX = x + BRadius / 3;
    MoveToEx(hdc, stubX, y, NULL);
    LineTo(hdc, stubX - 2, y - 5);
    LineTo(hdc, stubX + 2, y - 5);
    LineTo(hdc, stubX, y);

    DeleteObject(penStub);
}


/*********************************************************
*          D E A D I N J U R E D G O L D F I S H 1         *
*********************************************************/

DeadInjuredGoldFish1::DeadInjuredGoldFish1(int InitX, int InitY, int bRad, int sRad) : InjuredGoldFish1(InitX, InitY, bRad, sRad)
{
    Visible = false;
    BRadius = bRad;           // Установить больший радиус рыбы
    SRadius = sRad;           // Установить меньший радиус рыбы
    isDead = true;                  // Изначально погибшая
    isSmall = false;               // Малого размера
    isBoosted = false;            // Изначально нормальная скорость
}

DeadInjuredGoldFish1::~DeadInjuredGoldFish1() {}

void DeadInjuredGoldFish1::DrawEye()
{
    // Рисуем глаз рыбы
    HPEN Pen = CreatePen(PS_SOLID, 4, RGB(0, 0, 0));      // Красный цвет
    SelectObject(hdc, Pen);

    // ГЛАЗ (перекрестие - мертвый глаз)
    Line(hdc, x + BRadius * 2 / 3 - SRadius / 8, y + SRadius / 3 - SRadius / 8,
        x + BRadius * 2 / 3 + SRadius / 8, y + SRadius / 3 + SRadius / 8);
    Line(hdc, x + BRadius * 2 / 3 + SRadius / 8, y + SRadius / 3 - SRadius / 8,
        x + BRadius * 2 / 3 - SRadius / 8, y + SRadius / 3 + SRadius / 8);

    DeleteObject(Pen);
}

void DeadInjuredGoldFish1::HideEye()
{
    DeadInjuredGoldFish1::DrawEye();
}


/*********************************************************
*        S M A L L I N J U R E D G O L D F I S H 1       *
*********************************************************/

SmallInjuredGoldFish1::SmallInjuredGoldFish1(int InitX, int InitY, int bRad, int sRad) : InjuredGoldFish1(InitX, InitY, bRad, sRad)
{
    Visible = false;
    BRadius = bRad / 2;           // Установить больший радиус рыбы
    SRadius = sRad / 2;           // Установить меньший радиус рыбы
    isDead = false;               // Изначально жива
    isSmall = true;               // Малого размера
    isBoosted = false;            // Изначально нормальная скорость
}

SmallInjuredGoldFish1::~SmallInjuredGoldFish1() {}


/*********************************************************
*   D E A D S M A L L I N J U R E D G O L D F I S H 1    *
*********************************************************/

DeadSmallInjuredGoldFish1::DeadSmallInjuredGoldFish1(int InitX, int InitY, int bRad, int sRad) : SmallInjuredGoldFish1(InitX, InitY, bRad, sRad)
{
    Visible = false;
    BRadius = bRad / 2;           // Установить больший радиус рыбы
    SRadius = sRad / 2;           // Установить меньший радиус рыбы
    isDead = true;                // Изначально погибшая
    isSmall = true;               // Малого размера
    isBoosted = false;            // Изначально нормальная скорость
}

DeadSmallInjuredGoldFish1::~DeadSmallInjuredGoldFish1() {}

void DeadSmallInjuredGoldFish1::DrawEye()
{
    // Рисуем глаз рыбы
    HPEN Pen = CreatePen(PS_SOLID, 4, RGB(0, 0, 0));      // Черный цвет
    SelectObject(hdc, Pen);

    // ГЛАЗ (перекрестие - мертвый глаз)
    Line(hdc, x + BRadius * 2 / 3 - SRadius / 8, y + SRadius / 3 - SRadius / 8,
        x + BRadius * 2 / 3 + SRadius / 8, y + SRadius / 3 + SRadius / 8);
    Line(hdc, x + BRadius * 2 / 3 + SRadius / 8, y + SRadius / 3 - SRadius / 8,
        x + BRadius * 2 / 3 - SRadius / 8, y + SRadius / 3 + SRadius / 8);

    DeleteObject(Pen);
}

void DeadSmallInjuredGoldFish1::HideEye()
{
    DeadSmallInjuredGoldFish1::DrawEye();
}


/*********************************************************
*                P R E D A T O R F I S H                 *
*********************************************************/

PredatorFish::PredatorFish(int InitX, int InitY, int bRad, int sRad) : Fish(InitX, InitY, bRad, sRad)
{
    Visible = false;
    BRadius = bRad;           // Установить больший радиус рыбы
    SRadius = sRad;           // Установить меньший радиус рыбы
    isDead = false;               // Изначально жива
    isSmall = true;               // Малого размера
    isBoosted = false;            // Изначально нормальная скорость
}

PredatorFish::~PredatorFish() {}

void PredatorFish::DrawBody()
{
    // Тело - красный эллипс
    HPEN penBody = CreatePen(PS_SOLID, 2, RGB(150, 0, 0));  // Темно-красная обводка
    HBRUSH brushBody = CreateSolidBrush(RGB(220, 50, 50));  // Красное тело
    SelectObject(hdc, penBody);
    SelectObject(hdc, brushBody);

    Ellipse(hdc, x, y, x + BRadius, y + SRadius);  // Основное тело

    // Темная полоса вдоль тела для детализации
    HPEN penStripe = CreatePen(PS_SOLID, 1, RGB(180, 30, 30));
    SelectObject(hdc, penStripe);
    MoveToEx(hdc, x + BRadius / 4, y + SRadius / 3, NULL);
    LineTo(hdc, x + BRadius * 3 / 4, y + SRadius / 3);
    MoveToEx(hdc, x + BRadius / 4, y + SRadius * 2 / 3, NULL);
    LineTo(hdc, x + BRadius * 3 / 4, y + SRadius * 2 / 3);

    // Самые простые зубы - черные черточки
    HPEN penTeeth = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    SelectObject(hdc, penTeeth);

    // 3 черные черточки в передней части
    int teethY = y + SRadius / 2 + SRadius / 6;

    Line(hdc, x + BRadius, teethY, x + BRadius - SRadius / 8, teethY - SRadius / 20);
    Line(hdc, x + BRadius - SRadius / 6, teethY, x + BRadius - SRadius / 4, teethY - SRadius / 20);
    Line(hdc, x + BRadius - SRadius / 3, teethY, x + BRadius - SRadius / 2, teethY - SRadius / 20);

    DeleteObject(penTeeth);
    DeleteObject(penBody);
    DeleteObject(brushBody);
    DeleteObject(penStripe);
}

void PredatorFish::DrawTail()
{
    // Хвост как на фото - треугольный и близко к телу
    HPEN penTail = CreatePen(PS_SOLID, 2, RGB(150, 0, 0));
    HBRUSH brushTail = CreateSolidBrush(RGB(200, 40, 40));
    SelectObject(hdc, penTail);
    SelectObject(hdc, brushTail);

    // Хвост начинается от левой части тела
    int tailX = x;
    int tailY = y + SRadius / 2;

    // Простой треугольный хвост
    POINT tail[3] = {
        { tailX, tailY },                      // Основание у тела
        { tailX - SRadius / 3, y + SRadius / 4 },  // Верхний кончик
        { tailX - SRadius / 3, y + SRadius * 3 / 4 } // Нижний кончик
    };

    Polygon(hdc, tail, 3);

    DeleteObject(penTail);
    DeleteObject(brushTail);
}

void PredatorFish::DrawEye()
{
    // Злой черный глаз
    HPEN penEye = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HBRUSH brushEye = CreateSolidBrush(RGB(0, 0, 0));
    SelectObject(hdc, penEye);
    SelectObject(hdc, brushEye);

    // Глаз ближе к передней части
    Ellipse(hdc, x + BRadius * 3 / 4, y + SRadius / 3,
        x + BRadius * 3 / 4 + SRadius / 5, y + SRadius / 3 + SRadius / 5);

    DeleteObject(penEye);
    DeleteObject(brushEye);
}

void PredatorFish::DrawFin()
{
    // Верхний и нижний плавники близко к телу
    HPEN penFin = CreatePen(PS_SOLID, 1, RGB(150, 0, 0));
    HBRUSH brushFin = CreateSolidBrush(RGB(200, 40, 40));
    SelectObject(hdc, penFin);
    SelectObject(hdc, brushFin);

    // Верхний плавник (ближе к спине)
    POINT topFin[4] = {
        { x + BRadius / 3, y },                      // Начинается от тела
        { x + BRadius / 2, y - SRadius / 4 },          // Немного выступает
        { x + BRadius * 2 / 3, y - SRadius / 6 },        // Плавный изгиб
        { x + BRadius / 2, y }                       // Возвращается к телу
    };
    Polygon(hdc, topFin, 4);

    // Нижний плавник (ближе к брюху)
    POINT bottomFin[4] = {
        { x + BRadius / 3, y + SRadius },            // Начинается от тела
        { x + BRadius / 2, y + SRadius + SRadius / 4 }, // Немного выступает
        { x + BRadius * 2 / 3, y + SRadius + SRadius / 6 }, // Плавный изгиб
        { x + BRadius / 2, y + SRadius }              // Возвращается к телу
    };
    Polygon(hdc, bottomFin, 4);

    DeleteObject(penFin);
    DeleteObject(brushFin);
}

void PredatorFish::HideBody()
{
    // Тело - красный эллипс
    HPEN penBody = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));  // Темно-красная обводка
    HBRUSH brushBody = CreateSolidBrush(RGB(0, 0, 0));  // Красное тело
    SelectObject(hdc, penBody);
    SelectObject(hdc, brushBody);

    Ellipse(hdc, x, y, x + BRadius, y + SRadius);  // Основное тело

    // Темная полоса вдоль тела для детализации
    HPEN penStripe = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    SelectObject(hdc, penStripe);
    MoveToEx(hdc, x + BRadius / 4, y + SRadius / 3, NULL);
    LineTo(hdc, x + BRadius * 3 / 4, y + SRadius / 3);
    MoveToEx(hdc, x + BRadius / 4, y + SRadius * 2 / 3, NULL);
    LineTo(hdc, x + BRadius * 3 / 4, y + SRadius * 2 / 3);

    // Острые черные зубы
    HPEN penTeeth = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    SelectObject(hdc, penTeeth);

    // Несколько треугольных зубов
    int mouthX = x + BRadius * 7 / 8;  // Рот в передней части
    int mouthY = y + SRadius / 2;    // По центру

    // Верхние зубы (3 зуба)
    for (int i = 0; i < 3; i++) {
        int toothY = mouthY - SRadius / 4 + i * SRadius / 12;
        POINT tooth[3] = {
            { mouthX, toothY },
            { mouthX + SRadius / 6, toothY - SRadius / 10 },
            { mouthX + SRadius / 6, toothY + SRadius / 10 }
        };
        Polygon(hdc, tooth, 3);
    }

    // Нижние зубы (3 зуба)
    for (int i = 0; i < 3; i++) {
        int toothY = mouthY + SRadius / 4 - i * SRadius / 12;
        POINT tooth[3] = {
            { mouthX, toothY },
            { mouthX + SRadius / 6, toothY - SRadius / 10 },
            { mouthX + SRadius / 6, toothY + SRadius / 10 }
        };
        Polygon(hdc, tooth, 3);
    }

    DeleteObject(penTeeth);
    DeleteObject(penBody);
    DeleteObject(brushBody);
    DeleteObject(penStripe);
}

void PredatorFish::HideTail()
{
    // Хвост как на фото - треугольный и близко к телу
    HPEN penTail = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HBRUSH brushTail = CreateSolidBrush(RGB(0, 0, 0));
    SelectObject(hdc, penTail);
    SelectObject(hdc, brushTail);

    // Хвост начинается от левой части тела
    int tailX = x;
    int tailY = y + SRadius / 2;

    // Простой треугольный хвост
    POINT tail[3] = {
        { tailX, tailY },                      // Основание у тела
        { tailX - SRadius / 3, y + SRadius / 4 },  // Верхний кончик
        { tailX - SRadius / 3, y + SRadius * 3 / 4 } // Нижний кончик
    };

    Polygon(hdc, tail, 3);

    DeleteObject(penTail);
    DeleteObject(brushTail);
}

void PredatorFish::HideEye()
{
    // Злой черный глаз
    HPEN penEye = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HBRUSH brushEye = CreateSolidBrush(RGB(0, 0, 0));
    SelectObject(hdc, penEye);
    SelectObject(hdc, brushEye);

    // Глаз ближе к передней части
    Ellipse(hdc, x + BRadius * 3 / 4, y + SRadius / 3,
        x + BRadius * 3 / 4 + SRadius / 5, y + SRadius / 3 + SRadius / 5);

    DeleteObject(penEye);
    DeleteObject(brushEye);
}

void PredatorFish::HideFin()
{
    // Верхний и нижний плавники близко к телу
    HPEN penFin = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HBRUSH brushFin = CreateSolidBrush(RGB(0, 0, 0));
    SelectObject(hdc, penFin);
    SelectObject(hdc, brushFin);

    // Верхний плавник (ближе к спине)
    POINT topFin[4] = {
        { x + BRadius / 3, y },                      // Начинается от тела
        { x + BRadius / 2, y - SRadius / 4 },          // Немного выступает
        { x + BRadius * 2 / 3, y - SRadius / 6 },        // Плавный изгиб
        { x + BRadius / 2, y }                       // Возвращается к телу
    };
    Polygon(hdc, topFin, 4);

    // Нижний плавник (ближе к брюху)
    POINT bottomFin[4] = {
        { x + BRadius / 3, y + SRadius },            // Начинается от тела
        { x + BRadius / 2, y + SRadius + SRadius / 4 }, // Немного выступает
        { x + BRadius * 2 / 3, y + SRadius + SRadius / 6 }, // Плавный изгиб
        { x + BRadius / 2, y + SRadius }              // Возвращается к телу
    };
    Polygon(hdc, bottomFin, 4);

    DeleteObject(penFin);
    DeleteObject(brushFin);
}
