// main.cpp
#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include <vector>
#include <algorithm>
#include <windows.h>
#include <clocale>

#include "Woman.h"
#include "ManIntim.h"
#include "utils.h"

// ============================================================
//   УТИЛИТЫ
// ============================================================

void ClearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int AskInt(const std::string& prompt, int min, int max) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= min && value <= max) return value;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  ! Введите число от " << min << " до " << max << "\n";
    }
}

float AskFloat(const std::string& prompt, float min, float max) {
    float value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= min && value <= max) return value;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  ! Введите число от " << min << " до " << max << "\n";
    }
}

void Pause() {
    std::cout << "\n[Enter] чтобы продолжить...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

// ============================================================
//   СОСТОЯНИЕ
// ============================================================

void ShowStatus(const Woman& w, const ManIntim& m) {
    std::cout << "==================== СОСТОЯНИЕ ====================\n";
    std::cout << "  ЖЕНЩИНА: " << w.GetName()
              << " | Возбужд: " << w.GetIntim().GetArousal() << "%"
              << " | Смазка: " << std::fixed << std::setprecision(1)
              << w.GetIntim().GetLubrication() << "%"
              << " | Боль: " << w.GetIntim().GetPainLevel() << "%"
              << " | Оргазмов: " << w.GetTotalOrgasms() << "\n";
    if (w.GetIntim().IsInRefractory())
        std::cout << "    (в рефрактерном периоде)\n";
    std::cout << "  МУЖЧИНА: Возбужд: " << m.GetArousal() << "%"
              << " | Эрекция: " << (m.IsErect() ? "Да" : "Нет")
              << " (" << (int)(m.GetErectionQuality() * 100) << "%)"
              << " | Выносл: " << (int)m.GetCurrentStamina() << "/" << (int)m.GetStamina()
              << "\n";
    std::cout << "===================================================\n";
}

// ============================================================
//   СОСТОЯНИЕ СЕССИИ
// ============================================================

struct SessionState {
    Hole hole = Hole::Vagina;
    float depth = 0.0f;
    float diameter = 4.0f;
    float intensity = 50.0f;
    bool inside = false;
    bool active = false;
};

// ============================================================
//   ОДИН ТОЛЧОК
// ============================================================

void DoStroke(Woman& woman, ManIntim& man, SessionState& s, float delta) {
    // Мужчина тратит силы
    man.DrainStamina(1.0f);
    man.ChangeArousal(2);

    // Новая глубина
    float max_d = woman.GetIntim().GetHoleMaxDepth(s.hole);
    float new_depth = std::clamp(s.depth + delta, 0.0f, max_d);

    // Зона по глубине
    ErogenousZone zone = woman.GetIntim().GetZoneByDepth(s.hole, new_depth);

    // Стимуляция
    float pleasure = woman.StimulateZone(zone, s.intensity, new_depth, s.diameter);

    s.depth = new_depth;
    s.inside = (new_depth > 0.5f);

    std::cout << "  Толчок " << (delta > 0 ? "ВХОД" : "ВЫХОД")
              << " | Глубина: " << std::fixed << std::setprecision(1) << new_depth << " см"
              << " | Зона: " << woman.GetIntim().GetZoneName(zone)
              << "\n  Удовольствие: " << pleasure
              << " | Возбужд: " << woman.GetIntim().GetArousal() << "%"
              << " | Боль: " << woman.GetIntim().GetPainLevel() << "%\n";

    // Автоматический оргазм женщины
    if (!woman.GetIntim().IsInRefractory() &&
        woman.GetIntim().GetArousal() >= 85) {
        if (woman.GetIntim().CheckOrgasm()) {
            std::cout << "  *** ОРГАЗМ ЖЕНЩИНЫ! ***\n";
            SexEvent ev;
            ev.day = woman.GetDaysCount();
            ev.max_pleasure = pleasure;
            ev.max_pain = woman.GetIntim().GetPainLevel();
            ev.orgasm = true;
            ev.first_time_zone = false;
            woman.LogEvent(ev);
        }
    }
}

// ============================================================
//   МЕНЮ ИНТИМА
// ============================================================

void IntimMenu(Woman& woman, ManIntim& man) {
    SessionState s;

    while (true) {
        ClearScreen();
        ShowStatus(woman, man);

        std::cout << "\n--- СЕССИЯ ---\n";
        if (s.active) {
            std::cout << "  Отверстие: " << woman.GetIntim().GetHoleName(s.hole) << "\n";
            std::cout << "  Глубина: " << std::fixed << std::setprecision(1)
                      << s.depth << " / " << woman.GetIntim().GetHoleMaxDepth(s.hole) << " см\n";
            std::cout << "  Диаметр: " << s.diameter << " см\n";
            std::cout << "  Интенсивность: " << s.intensity << "%\n";
            std::cout << "  Состояние: " << (s.inside ? "ВНУТРИ" : "СНАРУЖИ") << "\n";
        } else {
            std::cout << "  (сессия не начата)\n";
        }

        std::cout << "\n--- ДЕЙСТВИЯ ---\n";
        if (!s.active) {
            std::cout << "  1. Начать сессию\n";
        } else {
            std::cout << "  2. Толчок ВНУТРЬ  (+1 см)\n";
            std::cout << "  3. Толчок НАРУЖУ  (-1 см)\n";
            std::cout << "  4. Глубокий ВНУТРЬ (+3 см)\n";
            std::cout << "  5. Глубокий НАРУЖУ (-3 см)\n";
            std::cout << "  6. Изменить интенсивность\n";
            std::cout << "  7. Изменить диаметр\n";
            std::cout << "  8. Сменить отверстие\n";
        }
        std::cout << "  9. КОНЧИТЬ (мужчине)\n";
        std::cout << " 10. Показать зоны\n";
        std::cout << " 11. Показать статистику\n";
        std::cout << "  0. Завершить сессию\n";

        int choice = AskInt("Выбор: ", 0, 11);

        if (choice == 0) {
            s.active = false;
            s.depth = 0.0f;
            s.inside = false;
            break;
        }

        if (choice == 1 && !s.active) {
            std::cout << "\n--- ВЫБОР ОТВЕРСТИЯ ---\n";
            std::cout << "  1. Влагалище (" << woman.GetIntim().GetVaginalDepth() << " см)\n";
            std::cout << "  2. Анус ("      << woman.GetIntim().GetAnalDepth()    << " см)\n";
            std::cout << "  3. Рот ("       << woman.GetIntim().GetMouthDepth()   << " см)\n";
            std::cout << "  4. Уретра ("    << woman.GetIntim().GetUrethraDepth() << " см)\n";
            std::cout << "  0. Отмена\n";

            int h = AskInt("Отверстие: ", 0, 4);
            if (h == 0) continue;
            switch (h) {
                case 1: s.hole = Hole::Vagina; break;
                case 2: s.hole = Hole::Anus;   break;
                case 3: s.hole = Hole::Mouth;  break;
                case 4: s.hole = Hole::Urethra;break;
            }

            s.diameter = AskFloat("Диаметр (0.5-8): ", 0.5f, 8.0f);
            s.intensity = AskFloat("Интенсивность (10-100): ", 10.0f, 100.0f);

            s.depth = 0.0f;
            s.inside = false;
            s.active = true;

            std::cout << "  Сессия начата: " << woman.GetIntim().GetHoleName(s.hole) << "\n";
            Pause();
        }
        else if (s.active && choice == 2) { ClearScreen(); std::cout << "--- ТОЛЧОК ВНУТРЬ ---\n";  DoStroke(woman, man, s, +1.0f); Pause(); }
        else if (s.active && choice == 3) { ClearScreen(); std::cout << "--- ТОЛЧОК НАРУЖУ ---\n";  DoStroke(woman, man, s, -1.0f); Pause(); }
        else if (s.active && choice == 4) { ClearScreen(); std::cout << "--- ГЛУБОКИЙ ВНУТРЬ ---\n"; DoStroke(woman, man, s, +3.0f); Pause(); }
        else if (s.active && choice == 5) { ClearScreen(); std::cout << "--- ГЛУБОКИЙ НАРУЖУ ---\n"; DoStroke(woman, man, s, -3.0f); Pause(); }
        else if (s.active && choice == 6) { s.intensity = AskFloat("Интенсивность (10-100): ", 10.0f, 100.0f); }
        else if (s.active && choice == 7) { s.diameter  = AskFloat("Диаметр (0.5-8): ", 0.5f, 8.0f); }
        else if (s.active && choice == 8) {
            std::cout << "\n--- СМЕНА ОТВЕРСТИЯ ---\n";
            std::cout << "  1. Влагалище\n  2. Анус\n  3. Рот\n  4. Уретра\n  0. Отмена\n";
            int h = AskInt("Отверстие: ", 0, 4);
            if (h == 0) continue;
            switch (h) {
                case 1: s.hole = Hole::Vagina; break;
                case 2: s.hole = Hole::Anus;   break;
                case 3: s.hole = Hole::Mouth;  break;
                case 4: s.hole = Hole::Urethra;break;
            }
            s.depth = 0.0f;
            s.inside = false;
            std::cout << "  Отверстие сменено: " << woman.GetIntim().GetHoleName(s.hole) << "\n";
            Pause();
        }
        else if (choice == 9) {
            if (!man.IsErect()) {
                std::cout << "  Мужчина не готов.\n";
                Pause();
                continue;
            }
            man.AddOrgasm();
            man.ChangeArousal(-80);
            man.SetErect(false);
            man.DrainStamina(20.0f);
            std::cout << "\n*** ЭЯКУЛЯЦИЯ МУЖЧИНЫ! ***\n";
            std::cout << "Выносливость: " << (int)man.GetCurrentStamina()
                      << "/" << (int)man.GetStamina() << "\n";
            Pause();
        }
        else if (choice == 10) { ClearScreen(); woman.ShowZonesInfo(); Pause(); }
        else if (choice == 11) { ClearScreen(); woman.ShowStats(); woman.ShowHistory(); Pause(); }
    }
}

// ============================================================
//   ГЛАВНОЕ МЕНЮ
// ============================================================

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    system("chcp 65001 > nul");
    setlocale(LC_ALL, "ru_RU.UTF-8");

    ClearScreen();
    std::cout << "========================================\n";
    std::cout << "   ИНТИМ-СИМУЛЯТОР (интерактивный)\n";
    std::cout << "========================================\n\n";

    std::string woman_name;
    std::cout << "Имя женщины [Alice]: ";
    std::getline(std::cin, woman_name);
    if (woman_name.empty()) woman_name = "Alice";

    Woman alice(woman_name, 5000);
    alice.GetIntim().SetMaxBreast(BreastSize::large);
    alice.GetIntim().SetCurrentBreastSize(BreastSize::large);

    ManIntim bob;
    bob.SetLength(16.5f);
    bob.SetGirth(12.5f);
    bob.SetStamina(80.0f);
    bob.SetSkill(0.6f);

    std::cout << "\nПерсонажи созданы. Начинаем.\n";
    Pause();

    while (true) {
        ClearScreen();
        std::cout << "========== ГЛАВНОЕ МЕНЮ ==========\n";
        std::cout << "  1. Интим (стимуляция)\n";
        std::cout << "  2. Показать женщину\n";
        std::cout << "  3. Показать мужчину\n";
        std::cout << "  4. Пропустить 1 день\n";
        std::cout << "  5. Пропустить 10 дней\n";
        std::cout << "  6. Статистика и история\n";
        std::cout << "  0. Выход\n";
        std::cout << "===================================\n";

        int choice = AskInt("Выбор: ", 0, 6);
        if (choice == 0) break;

        switch (choice) {
            case 1:
                if (!bob.IsErect()) {
                    bob.ChangeArousal(50);
                    bob.SetErect(true);
                }
                IntimMenu(alice, bob);
                break;
            case 2: ClearScreen(); alice.Show(); Pause(); break;
            case 3: ClearScreen(); bob.Show();   Pause(); break;
            case 4: alice.AdvanceTime(1);  bob.RestoreStamina(10.0f); std::cout << "  Прошёл 1 день.\n"; Pause(); break;
            case 5: alice.AdvanceTime(10); bob.RestoreStamina(40.0f); std::cout << "  Прошло 10 дней.\n"; Pause(); break;
            case 6: ClearScreen(); alice.ShowStats(); alice.ShowHistory(); Pause(); break;
        }
    }

    ClearScreen();
    std::cout << "========================================\n";
    std::cout << "   ИТОГИ\n";
    std::cout << "========================================\n";
    alice.Show();
    alice.ShowStats();
    bob.Show();

    std::cout << "\nВыход.\n";
    return 0;
}
//g++ -std=c++17 -O2 -Wall -o game main.cpp Woman.cpp WomanIntim.cpp ManIntim.cpp utils.cpp