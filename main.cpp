#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace {

struct Character {
    std::string name;
    std::string profession;
    int age = 24;
    int energy = 75;
    int hunger = 25;
    int mood = 65;
    int health = 80;
    int money = 120;
    int skills = 1;
};

struct Home {
    std::string district = "Тихий квартал";
    int comfort = 55;
    int food = 4;
    int rent = 35;
};

struct City {
    std::string name = "Люмен";
    int population = 12400;
    int day = 1;
    std::vector<std::string> places = {
        "Дом", "Кафе «Уголок»", "Городской парк", "Библиотека", "Рынок", "Офис"
    };
};

int Clamp(int value) {
    return std::clamp(value, 0, 100);
}

void ClearScreen() {
    // ANSI works in contemporary terminals and does not require platform headers.
    std::cout << "\033[2J\033[H";
}

int AskChoice(const std::string& prompt, int min, int max) {
    int value = 0;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= min && value <= max) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        if (std::cin.eof()) {
            return min;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Введите число от " << min << " до " << max << ".\n";
    }
}

void Pause() {
    std::cout << "\n[Enter] чтобы продолжить...";
    std::string line;
    std::getline(std::cin, line);
}

void Apply(Character& hero, int energy, int hunger, int mood, int health, int money) {
    hero.energy = Clamp(hero.energy + energy);
    hero.hunger = Clamp(hero.hunger + hunger);
    hero.mood = Clamp(hero.mood + mood);
    hero.health = Clamp(hero.health + health);
    hero.money += money;
}

void ShowStatus(const Character& hero, const Home& home, const City& city) {
    std::cout << "====================================================\n";
    std::cout << city.name << " | день " << city.day << " | население: " << city.population << "\n";
    std::cout << "====================================================\n";
    std::cout << hero.name << ", " << hero.age << " лет — " << hero.profession << "\n";
    std::cout << "Энергия: " << std::setw(3) << hero.energy
              << "  Сытость: " << std::setw(3) << 100 - hero.hunger
              << "  Настроение: " << std::setw(3) << hero.mood
              << "  Здоровье: " << std::setw(3) << hero.health << "\n";
    std::cout << "Деньги: " << hero.money << " мон. | Навык профессии: " << hero.skills << "\n";
    std::cout << "Дом: " << home.district << " (уют: " << home.comfort
              << ", запас еды: " << home.food << ")\n";
}

void ShowMap(const City& city) {
    std::cout << "\n--- КАРТА ГОРОДА " << city.name << " ---\n";
    for (std::size_t i = 0; i < city.places.size(); ++i) {
        std::cout << "  " << i + 1 << ". " << city.places[i] << "\n";
    }
}

void GoHome(Character& hero, Home& home) {
    std::cout << "\nВы дома.\n1. Отдохнуть\n2. Приготовить еду\n3. Обустроить дом (20 мон.)\n0. Назад\n";
    switch (AskChoice("Выбор: ", 0, 3)) {
        case 1:
            Apply(hero, 35 + home.comfort / 10, 8, 8, 3, 0);
            std::cout << "Отдых восстановил силы.\n";
            break;
        case 2:
            if (home.food > 0) {
                --home.food;
                Apply(hero, 5, -35, 10, 2, 0);
                std::cout << "Домашняя еда вернула настроение и сытость.\n";
            } else {
                std::cout << "Запас еды закончился — загляните на рынок.\n";
            }
            break;
        case 3:
            if (hero.money >= 20) {
                hero.money -= 20;
                home.comfort = Clamp(home.comfort + 10);
                std::cout << "В доме стало уютнее.\n";
            } else {
                std::cout << "Не хватает денег.\n";
            }
            break;
    }
}

void VisitCity(Character& hero, Home& home, const City& city) {
    ShowMap(city);
    const int place = AskChoice("Куда пойти (0 — назад): ", 0, static_cast<int>(city.places.size()));
    switch (place) {
        case 1: GoHome(hero, home); break;
        case 2:
            if (hero.money >= 12) {
                Apply(hero, 5, -25, 14, 0, -12);
                std::cout << "За чашкой кофе вы встретили знакомых.\n";
            } else std::cout << "В кошельке недостаточно денег для кафе.\n";
            break;
        case 3: Apply(hero, -12, 10, 18, 3, 0); std::cout << "Прогулка в парке освежила мысли.\n"; break;
        case 4: Apply(hero, -10, 6, 8, 0, 0); ++hero.skills; std::cout << "Несколько часов за книгами повысили навык.\n"; break;
        case 5:
            if (hero.money >= 15) {
                hero.money -= 15;
                home.food += 3;
                std::cout << "Вы купили продукты на три приёма пищи.\n";
            } else std::cout << "На продукты нужно 15 монет.\n";
            break;
        case 6:
            if (hero.energy < 15) {
                std::cout << "Слишком мало сил для работы.\n";
            } else {
                const int income = 18 + hero.skills * 4;
                Apply(hero, -25, 18, -5, 0, income);
                std::cout << "Рабочий день завершён. Заработано: " << income << " мон.\n";
            }
            break;
    }
}

void AdvanceDay(Character& hero, Home& home, City& city) {
    ++city.day;
    Apply(hero, 12 + home.comfort / 12, 12, 2, hero.hunger > 75 ? -7 : 1, 0);
    if (city.day % 7 == 1) {
        if (hero.money >= home.rent) {
            hero.money -= home.rent;
            std::cout << "Оплачена недельная аренда: " << home.rent << " мон.\n";
        } else {
            home.comfort = std::max(0, home.comfort - 8);
            std::cout << "Не удалось оплатить аренду. Уют дома снизился.\n";
        }
    }
    std::cout << "Наступил день " << city.day << ".\n";
}

}  // namespace

int main() {
    Character hero;
    Home home;
    City city;

    std::cout << "========================================\n";
    std::cout << "        REAL LIFE SIM: НАЧАЛО\n";
    std::cout << "========================================\n";
    std::cout << "Имя главного героя [Алекс]: ";
    std::getline(std::cin, hero.name);
    if (hero.name.empty()) hero.name = "Алекс";
    std::cout << "Профессия [стажёр]: ";
    std::getline(std::cin, hero.profession);
    if (hero.profession.empty()) hero.profession = "стажёр";

    while (true) {
        ClearScreen();
        ShowStatus(hero, home, city);
        std::cout << "\n--- ДЕЙСТВИЯ ---\n"
                  << "  1. Отправиться в город\n"
                  << "  2. Зайти домой\n"
                  << "  3. Осмотреть карту\n"
                  << "  4. Завершить день\n"
                  << "  0. Выйти из симуляции\n";
        const int choice = AskChoice("Выбор: ", 0, 4);
        if (choice == 0) break;

        ClearScreen();
        if (choice == 1) VisitCity(hero, home, city);
        if (choice == 2) GoHome(hero, home);
        if (choice == 3) ShowMap(city);
        if (choice == 4) AdvanceDay(hero, home, city);
        Pause();
    }

    ClearScreen();
    std::cout << "Спасибо за игру, " << hero.name << ".\n";
    ShowStatus(hero, home, city);
    return 0;
}
