/******************************************************************
* О Б Ъ Е К Т Н О - О Р И Е Н Т И Р О В А Н Н О Е ПРОГРАММИРОВАНИЕ*
*-----------------------------------------------------------------*
* Project Type  : QT Console Application                          *
* Project Name  : PRINCIPES_ООП\                                  *
* File Name     : OOP_Kurs.CPP                                    *
* Language      :                                                 *
* Programmer(s) : ПЕТРОВСКИЙ Д.Е.                                 *
* Modified By   :                                                 *
* Created       : 27/09/25                                        *
* Last Revision : 01/12/25                                        *
* Comment(s)    : ИЕРАРХИЯ ОБЪЕКТОВ И ВИРТ.ФУНКЦИИ                *
*                                                                 *
******************************************************************/

#include <windows.h>
#include <iostream>
using namespace std;

#include "Header.h"

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

// =========================================================
// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ
// =========================================================
HDC hdc;
HDC memDC;
HBITMAP hBitmap;
int screenW = 0, screenH = 0;
HDC consoleDC;

int globalScore = 0;
int scoreDisplayX = 0;
int scoreDisplayY = 0;

char statusMessage[200] = "";
int messageTimer = 0;

// =========================================================
// ФУНКЦИИ
// =========================================================
void DrawTextOnScreen(int x, int y, const char* text, COLORREF color = RGB(255, 255, 255)) {
    SetTextColor(hdc, color);
    SetBkColor(hdc, RGB(0, 0, 0));
    TextOutA(hdc, x, y, text, strlen(text));
}

void ShowMessage(const char* text, int frames = 60) {
    strcpy_s(statusMessage, text);
    messageTimer = frames;
}

void InitializeScoreDisplay(int x, int y) {
    scoreDisplayX = x;
    scoreDisplayY = y;
    globalScore = 0;
}

void AddToScore(int points) {
    globalScore += points;
    if (globalScore < 0) globalScore = 0;
}

void UpdateScoreDisplay() {
    char scoreText[50];
    sprintf_s(scoreText, "SCORE: %d", globalScore);
    DrawTextOnScreen(scoreDisplayX, scoreDisplayY, scoreText, RGB(255, 255, 0));
}

// Проверка столкновения с обычным препятствием
bool Check_Obst_Collision(Obstacle& obstacle, Fish& fish)
{
    int obstX = obstacle.GetX();
    int obstY = obstacle.GetY();
    int obstBRadius = obstacle.GetBRadius();
    int obstSRadius = obstacle.GetSRadius();

    int fishX = fish.GetX();
    int fishY = fish.GetY();
    int fishBRadius = fish.GetBRadius();
    int fishSRadius = fish.GetSRadius();

    if (fishX + fishBRadius < obstX - obstBRadius) return false;
    if (fishX > obstX + obstBRadius) return false;
    if (fishY + fishSRadius < obstY - obstSRadius) return false;
    if (fishY > obstY + obstSRadius) return false;

    return true;
}

// Проверка столкновения с хищной рыбкой
bool Check_Fish_Collision(Fish& fish, PredatorFish prFish)
{
    int prFishX = prFish.GetX();
    int prFishY = prFish.GetY();
    int prFishBRadius = prFish.GetBRadius();
    int prFishSRadius = prFish.GetSRadius();

    int fishX = fish.GetX();
    int fishY = fish.GetY();
    int fishBRadius = fish.GetBRadius();
    int fishSRadius = fish.GetSRadius();

    if (fishX + fishBRadius < prFishX) return false;
    if (fishX > prFishX + prFishBRadius) return false;
    if (fishY + fishSRadius < prFishY) return false;
    if (fishY > prFishY + prFishSRadius) return false;

    return true;
}

// Проверка столкновения с водорослями
bool CheckAlgaeCollision(Fish& fish, Algae& algae) {
    RECT algaeRect = algae.GetRect();

    int fishX = fish.GetX();
    int fishY = fish.GetY();
    int fishBRadius = fish.GetBRadius();
    int fishSRadius = fish.GetSRadius();

    RECT fishRect;
    fishRect.left = fishX;
    fishRect.right = fishX + fishBRadius;
    fishRect.top = fishY;
    fishRect.bottom = fishY + fishSRadius;

    if (fishRect.right < algaeRect.left) return false;
    if (fishRect.left > algaeRect.right) return false;
    if (fishRect.bottom < algaeRect.top) return false;
    if (fishRect.top > algaeRect.bottom) return false;

    return true;
}

// Отталкивание объекта от конфликтного
void backObject(int& xPx, int& xPy, int xDirection, int xStep)
{
    if (xDirection == 1) xPy = xPy + xStep;
    else if (xDirection == 2) xPx = xPx + xStep;
    else if (xDirection == 3) xPy = xPy - xStep;
    else if (xDirection == 4) xPx = xPx - xStep;
}

void DrawAllObjects(IDrawable** objects, int count)
{
    for (int i = 0; i < count; i++) objects[i]->Show();
}

void HideAllObjects(IDrawable** objects, int count)
{
    for (int i = 0; i < count; i++) objects[i]->Hide();
}

// =========================================================
// ГЛАВНАЯ ПРОГРАММА
// =========================================================
int main()
{
    int Step = 10;
    int x0 = 400;
    int y0 = 400;
    const int xSRadius = 50;
    const int xBRadius = 100;
    const int obstacleRadius = 30;

    HWND hwnd = GetConsoleWindow();
    if (hwnd == NULL) { cout << "Console Window Descriptor NOT FOUND !\n"; return 1; }

    hdc = GetWindowDC(hwnd);
    consoleDC = hdc;
    if (hdc == 0) { cout << "Handle Device Context NOT FOUND !\n"; return 2; }

    screenW = GetSystemMetrics(SM_CXSCREEN);
    screenH = GetSystemMetrics(SM_CYSCREEN);

    memDC = CreateCompatibleDC(hdc);
    hBitmap = CreateCompatibleBitmap(hdc, screenW, screenH);
    SelectObject(memDC, hBitmap);

    SendMessage(GetConsoleWindow(), WM_SYSCOMMAND, SC_MAXIMIZE, 0);
    Sleep(100);
    setlocale(LC_ALL, "Ru");

    InitializeScoreDisplay(screenW - 200, 50);

    // =========== СОЗДАЕМ ВОДОРОСЛИ ===========
    const int goodAlgaeCount = 3;
    const int badAlgaeCount = 2;

    GoodAlgae* goodAlgaeArray[goodAlgaeCount];
    BadAlgae* badAlgaeArray[badAlgaeCount];

    srand(GetTickCount());

    for (int i = 0; i < goodAlgaeCount; i++) {
        int x = 300 + (i * 600);
        int y = 820 + (i * 30);
        goodAlgaeArray[i] = new GoodAlgae(x, y, 25, 80);
        goodAlgaeArray[i]->Show();
    }

    for (int i = 0; i < badAlgaeCount; i++) {
        int x = 600 + (i * 600);
        int y = 840 + (i * 20);
        badAlgaeArray[i] = new BadAlgae(x, y, 25, 80);
        badAlgaeArray[i]->Show();
    }

    // =========== СОЗДАЕМ ПРЕПЯТСТВИЯ ===========
    const int damageCount = 3;
    DamageObstacle* damageObstacles[damageCount] = {
        new DamageObstacle(400, 250, obstacleRadius, obstacleRadius),
        new DamageObstacle(1000, 350, obstacleRadius, obstacleRadius),
        new DamageObstacle(1600, 200, obstacleRadius, obstacleRadius)
    };

    const int healingCount = 2;
    HealingObstacle* healingObstacles[healingCount] = {
        new HealingObstacle(700, 600, 30, 30),
        new HealingObstacle(1300, 650, 30, 30)
    };

    const int speedBoostCount = 2;
    SpeedBoostObstacle* speedObstacles[speedBoostCount] = {
        new SpeedBoostObstacle(800, 150, 25, 25),
        new SpeedBoostObstacle(1400, 100, 25, 25)
    };

    const int speedSlowCount = 2;
    SpeedSlowObstacle* slowObstacles[speedSlowCount] = {
        new SpeedSlowObstacle(200, 550, 25, 25),
        new SpeedSlowObstacle(1100, 500, 25, 25)
    };

    // =========== СОЗДАЕМ РЫБОК ===========
    GoldFish goldFish(x0, y0, xBRadius, xSRadius);
    SmallGoldFish smallGoldFish(0, 0, xBRadius, xSRadius);
    DeadSmallGoldFish deadSmallGoldFish(0, 0, xBRadius, xSRadius);
    InjuredGoldFish injGoldFish(0, 0, xBRadius, xSRadius);
    SmallInjuredGoldFish smallInjGoldFish(0, 0, xBRadius, xSRadius);
    DeadSmallInjuredGoldFish deadSmallInjGoldFish(0, 0, xBRadius, xSRadius);
    InjuredGoldFish1 injGoldFish1(0, 0, xBRadius, xSRadius);
    DeadInjuredGoldFish1 deadInjGoldFish1(0, 0, xBRadius, xSRadius);
    SmallInjuredGoldFish1 smallInjGoldFish1(0, 0, xBRadius, xSRadius);
    DeadSmallInjuredGoldFish1 deadSmallInjGoldFish1(0, 0, xBRadius, xSRadius);

    PredatorFish predatorFish(700, 200, xBRadius, xSRadius);
    PredatorFish predatorFish1(-200, -200, xBRadius, xSRadius);

    // =========== МАССИВ ДЛЯ ИНТЕРФЕЙСА ===========
    const int totalObjectsCount = 10;
    IDrawable* allObjects[totalObjectsCount];
    int objectIndex = 0;

    for (int i = 0; i < damageCount; i++) damageObstacles[i]->Show();
    for (int i = 0; i < healingCount; i++) healingObstacles[i]->Show();
    for (int i = 0; i < speedBoostCount; i++) speedObstacles[i]->Show();
    for (int i = 0; i < speedSlowCount; i++) slowObstacles[i]->Show();

    allObjects[objectIndex++] = &goldFish;
    allObjects[objectIndex++] = &smallGoldFish;
    allObjects[objectIndex++] = &deadSmallGoldFish;
    allObjects[objectIndex++] = &injGoldFish;
    allObjects[objectIndex++] = &smallInjGoldFish;
    allObjects[objectIndex++] = &deadSmallInjGoldFish;
    allObjects[objectIndex++] = &injGoldFish1;
    allObjects[objectIndex++] = &deadInjGoldFish1;
    allObjects[objectIndex++] = &smallInjGoldFish1;
    allObjects[objectIndex++] = &deadSmallInjGoldFish1;

    Fish* currentObject = &goldFish;

    int Px, Py;
    bool Boost = 0;

    // =========================================================
    // ГЛАВНЫЙ ЦИКЛ
    // =========================================================
    while (true)
    {
        if (KEY_DOWN(VK_ESCAPE)) break;

        // 1. ОЧИСТКА БУФЕРА
        HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
        RECT fullRect = { 0, 0, screenW, screenH };
        FillRect(memDC, &fullRect, blackBrush);
        DeleteObject(blackBrush);

        // 2. ПОДМЕНА КОНТЕКСТА
        hdc = memDC;

        // 3. ОБНОВЛЕНИЕ ЛОГИКИ
        currentObject->Drag(Step);

        for (int i = 0; i < goodAlgaeCount; i++) {
            if (!goodAlgaeArray[i]->IsCollected()) goodAlgaeArray[i]->UpdatePosition();
        }
        for (int i = 0; i < badAlgaeCount; i++) {
            if (!badAlgaeArray[i]->IsCollected()) badAlgaeArray[i]->UpdatePosition();
        }

        // Хищник преследует (только если он не съеден)
        if (predatorFish.GetX() > -500) {
            predatorFish.Chase(currentObject, 2);
        }

        // 4. ПОЛНАЯ ОТРИСОВКА
        currentObject->Show();

        for (int i = 0; i < goodAlgaeCount; i++) {
            if (!goodAlgaeArray[i]->IsCollected()) goodAlgaeArray[i]->Show();
        }
        for (int i = 0; i < badAlgaeCount; i++) {
            if (!badAlgaeArray[i]->IsCollected()) badAlgaeArray[i]->Show();
        }

        for (int i = 0; i < damageCount; i++) damageObstacles[i]->Show();
        for (int i = 0; i < healingCount; i++) healingObstacles[i]->Show();
        for (int i = 0; i < speedBoostCount; i++) speedObstacles[i]->Show();
        for (int i = 0; i < speedSlowCount; i++) slowObstacles[i]->Show();

        if (predatorFish.GetX() > -500) predatorFish.Show();

        UpdateScoreDisplay();

        // Инструкции внизу экрана
        DrawTextOnScreen(20, screenH - 120, "WASD - движение, ESC - выход", RGB(200, 200, 200));
        DrawTextOnScreen(20, screenH - 90, "Красные камни - урон, Зелёные - лечение, Синие стрелки - ускорение, Красные стрелки - замедление", RGB(150, 150, 150));

        // Сообщения (если есть)
        if (messageTimer > 0) {
            DrawTextOnScreen(50, 50, statusMessage, RGB(0, 255, 0));
            messageTimer--;
        }

        // 5. КОПИРОВАНИЕ НА ЭКРАН
        hdc = consoleDC;
        BitBlt(consoleDC, 0, 0, screenW, screenH, memDC, 0, 0, SRCCOPY);

        // 6. ПРОВЕРКИ СТОЛКНОВЕНИЙ

        // Хорошие водоросли
        for (int i = 0; i < goodAlgaeCount; i++) {
            if (goodAlgaeArray[i]->GetVisible() &&
                !goodAlgaeArray[i]->IsCollected() &&
                CheckAlgaeCollision(*currentObject, *goodAlgaeArray[i])) {

                AddToScore(10);
                char msg[100];
                sprintf_s(msg, "+10 очков! Всего: %d", globalScore);
                ShowMessage(msg);

                goodAlgaeArray[i]->SetCollected(true);
                Sleep(200);

                int newX = 300 + rand() % 1200;
                int newY = 820 + rand() % 60;
                goodAlgaeArray[i]->SetPosition(newX, newY);
                goodAlgaeArray[i]->SetCollected(false);
            }
        }

        // Плохие водоросли
        for (int i = 0; i < badAlgaeCount; i++) {
            if (badAlgaeArray[i]->GetVisible() &&
                !badAlgaeArray[i]->IsCollected() &&
                CheckAlgaeCollision(*currentObject, *badAlgaeArray[i])) {

                AddToScore(-5);
                ShowMessage("-5 очков!");

                badAlgaeArray[i]->SetCollected(true);
                Sleep(200);

                int newX = 600 + rand() % 600;
                int newY = 840 + rand() % 40;
                badAlgaeArray[i]->SetPosition(newX, newY);
                badAlgaeArray[i]->SetCollected(false);
            }
        }

        // Эволюция
        if (globalScore < 50 && currentObject == &predatorFish1) {
            ShowMessage("Эволюция в обычную рыбу!");
            int x = currentObject->GetX();
            int y = currentObject->GetY();
            currentObject->Hide();
            currentObject = &goldFish;
            currentObject->SetX(x);
            currentObject->SetY(y);
            currentObject->Show();
            Step = 15;
        }
        if (globalScore >= 50 && currentObject != &predatorFish1) {
            ShowMessage("Эволюция в хищника!");
            int x = currentObject->GetX();
            int y = currentObject->GetY();
            currentObject->Hide();
            currentObject = &predatorFish1;
            currentObject->SetX(x);
            currentObject->SetY(y);
            currentObject->Show();
            Step = 15;
        }

        if (globalScore >= 100) {
            ShowMessage("Ты победил!");
            break;
        }

        // Столкновения с уроном
        for (int i = 0; i < damageCount; i++) {
            if (Check_Obst_Collision(*damageObstacles[i], *currentObject)) {
                if (currentObject->GetIsSmall() == false) {
                    currentObject->Hide();
                    Sleep(500);
                    Px = currentObject->GetX();
                    Py = currentObject->GetY();
                    Boost = currentObject->GetIsBoosted();
                    backObject(Px, Py, currentObject->GetDirection(), Step);

                    if (currentObject == &goldFish) currentObject = &smallGoldFish;
                    else if (currentObject == &injGoldFish) currentObject = &smallInjGoldFish;
                    else if (currentObject == &injGoldFish1) currentObject = &smallInjGoldFish1;

                    currentObject->SetX(Px);
                    currentObject->SetY(Py);
                    currentObject->SetBoost(Boost);
                    currentObject->Show();
                    break;
                }
                else {
                    currentObject->Hide();
                    Px = currentObject->GetX();
                    Py = currentObject->GetY();

                    if (currentObject == &smallGoldFish) currentObject = &deadSmallGoldFish;
                    else if (currentObject == &smallInjGoldFish) currentObject = &deadSmallInjGoldFish;
                    else if (currentObject == &smallInjGoldFish1) currentObject = &deadSmallInjGoldFish1;

                    currentObject->SetX(Px);
                    currentObject->SetY(Py);
                    currentObject->Show();
                    break;
                }
            }
        }

        // Столкновения с лечением
        for (int i = 0; i < healingCount; i++)
        {
            if (Check_Obst_Collision(*healingObstacles[i], *currentObject))
            {
                // Проверяем, нуждается ли рыбка в лечении
                bool needsHealing = false;
                Fish* healedFish = nullptr;

                // 1. Если рыбка ранена (но не мертва) — лечим раны
                if (currentObject == &injGoldFish) {
                    healedFish = &goldFish;
                    needsHealing = true;
                }
                else if (currentObject == &injGoldFish1) {
                    healedFish = &goldFish; // Из сильно раненой сразу в здоровую
                    needsHealing = true;
                }
                // 2. Если рыбка маленькая раненая — лечим и раны, и размер
                else if (currentObject == &smallInjGoldFish) {
                    healedFish = &goldFish;
                    needsHealing = true;
                }
                else if (currentObject == &smallInjGoldFish1) {
                    healedFish = &goldFish;
                    needsHealing = true;
                }
                // 3. Если рыбка просто маленькая (не раненая) — возвращаем размер
                else if (currentObject == &smallGoldFish) {
                    healedFish = &goldFish;
                    needsHealing = true;
                }

                // Если лечение нужно — применяем
                if (needsHealing && healedFish != nullptr)
                {
                    currentObject->Hide();
                    Px = currentObject->GetX();
                    Py = currentObject->GetY();
                    Boost = currentObject->GetIsBoosted();

                    currentObject = healedFish;

                    currentObject->SetX(Px);
                    currentObject->SetY(Py);
                    currentObject->SetBoost(Boost);
                    currentObject->Show();

                    ShowMessage("Исцеление!");
                    break;
                }
            }
        }

        // Ускорение
        for (int i = 0; i < speedBoostCount; i++) {
            if (Check_Obst_Collision(*speedObstacles[i], *currentObject) && currentObject->GetIsBoosted() == false) {
                currentObject->SetBoost(true);
                Step = Step * 2;
                ShowMessage("Ускорение! Скорость увеличена.");
            }
        }

        // Замедление
        for (int i = 0; i < speedSlowCount; i++) {
            if (Check_Obst_Collision(*slowObstacles[i], *currentObject) && currentObject->GetIsBoosted() == true) {
                currentObject->SetBoost(false);
                Step = Step / 2;
                ShowMessage("Замедление! Скорость уменьшена.");
            }
        }

        // Столкновение с хищником
        if (Check_Fish_Collision(*currentObject, predatorFish) && predatorFish.GetX() > -500) {
            if (currentObject == &predatorFish1) {
                predatorFish.Hide();
                AddToScore(50);
                char msg[100];
                sprintf_s(msg, "Вы съели хищника! +50 очков. Всего: %d", globalScore);
                ShowMessage(msg);
                predatorFish.SetX(-1000);
                predatorFish.SetY(-1000);
                Sleep(300);
            }
            else {
                currentObject->Hide();
                Sleep(500);
                Px = currentObject->GetX();
                Py = currentObject->GetY();
                Boost = currentObject->GetIsBoosted();
                backObject(Px, Py, currentObject->GetDirection(), Step);

                if (currentObject == &goldFish) currentObject = &injGoldFish;
                else if (currentObject == &injGoldFish) currentObject = &injGoldFish1;
                else if (currentObject == &injGoldFish1) currentObject = &deadInjGoldFish1;

                currentObject->SetX(Px);
                currentObject->SetY(Py);
                currentObject->SetBoost(Boost);
                currentObject->Show();
            }
        }

        // Проверка на смерть
        if (currentObject == &deadSmallGoldFish || currentObject == &deadInjGoldFish1
            || currentObject == &deadSmallInjGoldFish || currentObject == &deadSmallInjGoldFish1) {
            ShowMessage("Золотая рыбка погибла!");
            Sleep(2000);
            break;
        }

        Sleep(30); // Стабильность анимации
    }

    // ДЕМОНСТРАЦИЯ ИНТЕРФЕЙСА
    HideAllObjects(allObjects, totalObjectsCount);
    Sleep(1000);
    DrawAllObjects(allObjects, totalObjectsCount);
    Sleep(1000);

    // ОЧИСТКА ПАМЯТИ
    for (int i = 0; i < goodAlgaeCount; i++) delete goodAlgaeArray[i];
    for (int i = 0; i < badAlgaeCount; i++) delete badAlgaeArray[i];
    for (int i = 0; i < damageCount; i++) delete damageObstacles[i];
    for (int i = 0; i < healingCount; i++) delete healingObstacles[i];
    for (int i = 0; i < speedBoostCount; i++) delete speedObstacles[i];
    for (int i = 0; i < speedSlowCount; i++) delete slowObstacles[i];

    DeleteObject(hBitmap);
    DeleteDC(memDC);

    cout << "\nПрограмма завершена успешно!\n";
    return 0;
}