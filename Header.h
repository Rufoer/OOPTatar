#pragma once

// Интерфейс для всех отображаемых объектов
class IDrawable
{
public:
    virtual void Show() = 0;        // Чисто виртуальная функция
    virtual void Hide() = 0;        // Чисто виртуальная функция
};

class Location                  // Класс местонахождения
{
protected:                      // Доступно для потомков
    int x;                      // Координата X
    int y;                      // Координата Y
public:                         // Доступно для всех
    Location(int InitX, int InitY);     // Конструктор
    virtual ~Location();                // Виртуальный деструктор

    int GetX();                         // Получить X
    int GetY();                         // Получить Y
    void SetX(int NewX);                // Установить X
    void SetY(int NewY);                // Установить Y
};

class Point : public Location
{
protected:                      // Доступно для потомков
    bool Visible;               // Видимость точки
public:
    Point(int InitX, int InitY);        // Конструктор
    virtual ~Point();                   // Деструктор

    bool GetVisible();                  // Получить видимость
    void SetVisible(bool NewVisible);   // Установить видимость
    void Show();
    void Hide();
    void MoveTo(int NewX, int NewY);    // Переместить
    void Drag(int xStep);               // Управление точкой
};

/*********************************************************
*                 П Р Е П Я Т С Т В И Е                  *
*********************************************************/
class Obstacle : public Point
{
protected:
    int BRadius;
    int SRadius;
public:
    Obstacle(int InitX, int InitY, int bRadius, int sRadius);
    virtual ~Obstacle();

    virtual void Show() = 0;
    virtual void Hide() = 0;
    int GetBRadius();
    int GetSRadius();
};

/*********************************************************
*             П Р Е П Я Т С Т В И Е  У Р О Н А           *
*********************************************************/
class DamageObstacle : public Obstacle
{
public:
    DamageObstacle(int InitX, int InitY, int bRadius, int sRadius);
    virtual ~DamageObstacle();

    virtual void Show() override;
    virtual void Hide() override;
};

/*********************************************************
*            П Р Е П Я Т С Т В И Е  Л Е Ч Е Н И Я        *
*********************************************************/
class HealingObstacle : public Obstacle
{
public:
    HealingObstacle(int InitX, int InitY, int bRadius, int sRadius);
    virtual ~HealingObstacle();

    virtual void Show() override;
    virtual void Hide() override;
};

/*********************************************************
*           П Р Е П Я Т С Т В И Е  У С К О Р Е Н И Я     *
*********************************************************/
class SpeedBoostObstacle : public Obstacle
{
public:
    SpeedBoostObstacle(int InitX, int InitY, int bRadius, int sRadius);
    virtual ~SpeedBoostObstacle();

    virtual void Show() override;
    virtual void Hide() override;
};

/*********************************************************
*          П Р Е П Я Т С Т В И Е  З А М Е Д Л Е Н И Я    *
*********************************************************/
class SpeedSlowObstacle : public Obstacle
{
public:
    SpeedSlowObstacle(int InitX, int InitY, int bRadius, int sRadius);
    virtual ~SpeedSlowObstacle();

    virtual void Show() override;
    virtual void Hide() override;
};


class Algae : public Obstacle {
protected:
    int width;
    int height;
    bool collected;

    int moveSpeed;      // Скорость движения
    int moveDirection;  // 1 - вниз, -1 - вверх
    int startY;         // Изначальная координата Y (точка отсчета)
    int amplitude;      // Амплитуда колебаний (на сколько пикселей отходит)

public:
    Algae(int InitX, int InitY, int w, int h);
    virtual ~Algae();

    int GetWidth() { return width; }
    int GetHeight() { return height; }
    bool IsCollected() { return collected; }
    void SetCollected(bool c) { collected = c; }

    // Методы для изменения позиции
    void SetPosition(int newX, int newY) {
        Hide();      // Скрываем на старом месте
        x = newX;    // Меняем координаты (x, y наследуются от Point)
        y = newY;
        startY = newY;
        Show();      // Показываем на новом месте
    }

    RECT GetRect() {
        RECT rect;
        rect.left = x - width / 2;
        rect.right = x + width / 2;
        rect.top = y - height;      // Верх - выше центра (растет вверх)
        rect.bottom = y;            // Низ - в центре (точка крепления)
        return rect;
    }

    void UpdatePosition() {
        if (collected) return; // Если собрана, не двигаем

        // Двигаем по Y
        y += moveSpeed * moveDirection;

        // Если вышли за пределы амплитуды, меняем направление
        if (y > startY + amplitude) {
            moveDirection = -1; // Двигаться вверх
        }
        else if (y < startY - amplitude) {
            moveDirection = 1;  // Двигаться вниз
        }
    }

    virtual void Show() = 0;
    virtual void Hide() = 0;
};

// Хорошие водоросли (дают очки)
class GoodAlgae : public Algae {
public:
    GoodAlgae(int InitX, int InitY, int w = 20, int h = 80);
    virtual ~GoodAlgae();

    virtual void Show() override;
    virtual void Hide() override;
};

// Плохие водоросли (отнимают очки)
class BadAlgae : public Algae {
public:
    BadAlgae(int InitX, int InitY, int w = 20, int h = 80);
    virtual ~BadAlgae();

    virtual void Show() override;
    virtual void Hide() override;
};


/********************************************************
*                    Р Ы Б К А                          *
********************************************************/
class Fish : public Point, public IDrawable  // Абстрактный базовый класс для рыб
{
protected:
    int BRadius;                // Больший радиус рыбы
    int SRadius;                // Меньший радиус рыбы
    bool isDead;                // Флаг смерти рыбы
    bool isSmall;               // Флаг уменьшенной рыбы
    int isBoosted;              // Флаг ускорена ли рыбка
    int Direction;              // Текущее направление рыбки

public:
    Fish(int InitX, int InitY, int bRad, int sRad); // Конструктор
    virtual ~Fish();                                // Деструктор

    virtual void Show() override;                   // Показать рыбу
    virtual void Hide() override;                   // Скрыть рыбу

    virtual void DrawBody() = 0;        // Показать тело рыбы
    virtual void DrawTail() = 0;        // Показать хвост рыбы
    virtual void DrawEye() = 0;         // Показать глазик рыбы
    virtual void DrawFin() = 0;         // Показать плавники рыбы

    virtual void HideBody() = 0;        // Скрыть тело рыбы
    virtual void HideTail() = 0;        // Скрыть хвост рыбы
    virtual void HideEye() = 0;         // Скрыть глазик рыбы
    virtual void HideFin() = 0;         // Скрыть плавники рыбы

    void Drag(int xStep);           // Управление рыбой (W, A, S, D)

    int GetBRadius();               // Получить больший радиус
    int GetSRadius();               // Получить меньший радиус
    bool GetIsDead();               // Получить жива рыба или нет
    bool GetIsSmall();              // Получить размер рыбы
    int GetIsBoosted();             // Ускорена ли рыбка?
    int GetDirection();             // Получить текущее направление рыбы
    void SetDead(bool dead);        // Установить жива рыба или нет
    void SetSmall(bool smal);       // Установить размер рыбы
    void SetBoost(bool boost);      // Установить скорость рыбы
};


/*************************************************
*           З О Л О Т А Я Р Ы Б К А              *
*************************************************/
class GoldFish : public Fish
{
public:
    GoldFish(int InitX, int InitY, int bRad, int sRad); // Конструктор
    virtual ~GoldFish();                                // Деструктор

    virtual void DrawBody() override;
    virtual void DrawTail() override;
    virtual void DrawEye() override;
    virtual void DrawFin() override;

    virtual void HideBody() override;
    virtual void HideTail() override;
    virtual void HideEye() override;
    virtual void HideFin() override;
};


/***************************************************
*    М А Л Е Н Ь К А Я З О Л О Т А Я Р Ы Б К А     *
***************************************************/
class SmallGoldFish : public GoldFish
{
public:
    SmallGoldFish(int InitX, int InitY, int bRad, int sRad); // Конструктор
    virtual ~SmallGoldFish();                                // Деструктор
};


/************************************************************
*    М А Л Е Н Ь К А Я ПОГИБШАЯ З О Л О Т А Я Р Ы Б К А     *
************************************************************/
class DeadSmallGoldFish : public SmallGoldFish
{
public:
    DeadSmallGoldFish(int InitX, int InitY, int bRad, int sRad); // Конструктор
    virtual ~DeadSmallGoldFish();                                // Деструктор

    virtual void DrawEye() override;

    virtual void HideEye() override;
};


/***************************************************
*      Р А Н Е Н Н А Я З О Л О Т А Я Р Ы Б К А     *
***************************************************/
class InjuredGoldFish : public GoldFish
{
public:
    InjuredGoldFish(int InitX, int InitY, int bRad, int sRad);
    virtual ~InjuredGoldFish();

    virtual void DrawBody() override;
    virtual void DrawTail() override;

    virtual void HideTail() override;
};


/*************************************************************
*      Р А Н Е Н Н А Я МАЛЕНЬКАЯ З О Л О Т А Я Р Ы Б К А     *
*************************************************************/
class SmallInjuredGoldFish : public InjuredGoldFish
{
public:
    SmallInjuredGoldFish(int InitX, int InitY, int bRad, int sRad);
    virtual ~SmallInjuredGoldFish();
};


/*************************************************************
*      Р А Н Е Н Н А Я МАЛЕНЬКАЯ З О Л О Т А Я Р Ы Б К А     *
*************************************************************/
class DeadSmallInjuredGoldFish : public SmallInjuredGoldFish
{
public:
    DeadSmallInjuredGoldFish(int InitX, int InitY, int bRad, int sRad);
    virtual ~DeadSmallInjuredGoldFish();

    virtual void DrawEye() override;

    virtual void HideEye() override;
};


/****************************************************
*      Р А Н Е Н Н А Я З О Л О Т А Я Р Ы Б К А 1    *
****************************************************/
class InjuredGoldFish1 : public InjuredGoldFish
{
public:
    InjuredGoldFish1(int InitX, int InitY, int bRad, int sRad);
    virtual ~InjuredGoldFish1();

    virtual void DrawBody() override;
    virtual void DrawTail() override;
    virtual void DrawFin() override;

    virtual void HideTail() override;
    virtual void HideFin() override;
};


/************************************************************
*     ПОГИБШАЯ Р А Н Е Н Н А Я З О Л О Т А Я Р Ы Б К А 1    *
************************************************************/
class DeadInjuredGoldFish1 : public InjuredGoldFish1
{
public:
    DeadInjuredGoldFish1(int InitX, int InitY, int bRad, int sRad);
    virtual ~DeadInjuredGoldFish1();

    virtual void DrawEye() override;

    virtual void HideEye() override;
};


/**************************************************************
*      Р А Н Е Н Н А Я МАЛЕНЬКАЯ З О Л О Т А Я Р Ы Б К А 1    *
**************************************************************/
class SmallInjuredGoldFish1 : public InjuredGoldFish1
{
public:
    SmallInjuredGoldFish1(int InitX, int InitY, int bRad, int sRad);
    virtual ~SmallInjuredGoldFish1();
};


/***********************************************************************
*      ПОГИБШАЯ Р А Н Е Н Н А Я МАЛЕНЬКАЯ З О Л О Т А Я Р Ы Б К А 1    *
***********************************************************************/
class DeadSmallInjuredGoldFish1 : public SmallInjuredGoldFish1
{
public:
    DeadSmallInjuredGoldFish1(int InitX, int InitY, int bRad, int sRad);
    virtual ~DeadSmallInjuredGoldFish1();

    virtual void DrawEye() override;

    virtual void HideEye() override;
};


/***********************************************
*            Х И Щ Н А Я  Р Ы Б А              *
***********************************************/
class PredatorFish : public Fish
{
public:
    PredatorFish(int InitX, int InitY, int bRad, int sRad);
    virtual ~PredatorFish();

    virtual void DrawBody() override;
    virtual void DrawTail() override;
    virtual void DrawEye() override;
    virtual void DrawFin() override;

    virtual void HideBody() override;
    virtual void HideTail() override;
    virtual void HideEye() override;
    virtual void HideFin() override;

    void Chase(Fish* target, int speed = 3) {
        if (isDead || target == nullptr || target->GetIsDead()) return;

        int myX = GetX();
        int myY = GetY();
        int targetX = target->GetX();
        int targetY = target->GetY();

        // Вычисляем разницу
        int dx = targetX - myX;
        int dy = targetY - myY;

        // Нормализуем вектор, чтобы двигаться с постоянной скоростью
        int stepX = 0, stepY = 0;

        if (dx > speed) stepX = speed;
        else if (dx < -speed) stepX = -speed;
        else stepX = dx;

        if (dy > speed) stepY = speed;
        else if (dy < -speed) stepY = -speed;
        else stepY = dy;

        // Двигаем хищника (без Hide/Show, так как у нас двойная буферизация)
        SetX(myX + stepX);
        SetY(myY + stepY);
    }
};
