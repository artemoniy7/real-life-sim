#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace {

enum class Gender { Woman, Man, NonBinary };

std::string GenderText(Gender gender) {
    switch (gender) {
        case Gender::Woman: return "женщина";
        case Gender::Man: return "мужчина";
        case Gender::NonBinary: return "небинарный человек";
    }
    return "человек";
}

struct Character {
    std::string name;
    std::string profession;
    std::string trait = "любознательный";
    Gender gender = Gender::NonBinary;
    int age = 24;
    int energy = 75;
    int hunger = 25;
    int mood = 65;
    int health = 80;
    int money = 120;
    int skills = 1;
    int confidence = 50;
    int protection = 2;
};

struct Home {
    std::string district = "Тихий квартал";
    int comfort = 55;
    int food = 4;
    int rent = 35;
};

struct Person {
    std::string name;
    std::string profession;
    std::string location;
    int age = 25;
    int friendship = 0;
    int romance = 0;
    bool acquainted = false;
    bool partner = false;
};

struct City {
    std::string name = "Люмен";
    int population = 12400;
    int day = 1;
    int minutes = 8 * 60;
    std::vector<std::string> places = {
        "Дом", "Кафе «Уголок»", "Городской парк", "Библиотека", "Рынок", "Офис"
    };
    std::vector<Person> people = {
        {"Мира", "фотограф", "Городской парк", 27},
        {"Денис", "бариста", "Кафе «Уголок»", 26},
        {"София", "библиотекарь", "Библиотека", 31},
        {"Илья", "продавец", "Рынок", 29},
        {"Лев", "разработчик", "Офис", 28}
    };
};

int Clamp(int value) { return std::clamp(value, 0, 100); }

void ClearScreen() { std::cout << "\033[2J\033[H"; }

int AskChoice(const std::string& prompt, int min, int max) {
    int value = 0;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= min && value <= max) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        if (std::cin.eof()) return min;
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

std::string TimeText(const City& city) {
    std::ostringstream result;
    result << std::setfill('0') << std::setw(2) << city.minutes / 60 << ':'
           << std::setw(2) << city.minutes % 60;
    return result.str();
}

std::string DateText(const City& city) {
    int year = 2026;
    int dayOfYear = city.day;
    const int monthLengths[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int month = 0;
    while (dayOfYear > monthLengths[month]) {
        dayOfYear -= monthLengths[month];
        ++month;
        if (month == 12) { month = 0; ++year; }
    }
    std::ostringstream result;
    result << year << '-' << std::setfill('0') << std::setw(2) << month + 1 << '-'
           << std::setw(2) << dayOfYear;
    return result.str();
}

std::string WeatherText(const City& city) {
    const std::string weather[] = {"солнечно", "облачно", "лёгкий дождь", "ясно", "ветрено"};
    return weather[city.day % 5];
}

void Apply(Character& hero, int energy, int hunger, int mood, int health, int money) {
    hero.energy = Clamp(hero.energy + energy);
    hero.hunger = Clamp(hero.hunger + hunger);
    hero.mood = Clamp(hero.mood + mood);
    hero.health = Clamp(hero.health + health);
    hero.money += money;
}

void RefreshPeopleLocations(City& city) {
    const int hour = city.minutes / 60;
    const std::vector<std::string> morning = {"Городской парк", "Кафе «Уголок»", "Библиотека", "Рынок", "Офис"};
    const std::vector<std::string> evening = {"Городской парк", "Кафе «Уголок»", "Кафе «Уголок»", "Рынок", "Городской парк"};
    const auto& schedule = (hour >= 18 || hour < 8) ? evening : morning;
    for (std::size_t i = 0; i < city.people.size(); ++i) city.people[i].location = schedule[i];
}

void AdvanceTime(Character& hero, Home& home, City& city, int minutes) {
    city.minutes += minutes;
    while (city.minutes >= 24 * 60) {
        city.minutes -= 24 * 60;
        ++city.day;
        Apply(hero, 12 + home.comfort / 12, 12, 2, hero.hunger > 75 ? -7 : 1, 0);
        if (city.day % 7 == 1) {
            if (hero.money >= home.rent) {
                hero.money -= home.rent;
                std::cout << "\nОплачена недельная аренда: " << home.rent << " мон.\n";
            } else {
                home.comfort = std::max(0, home.comfort - 8);
                std::cout << "\nНе удалось оплатить аренду. Уют дома снизился.\n";
            }
        }
    }
    RefreshPeopleLocations(city);
}

void ShowStatus(const Character& hero, const Home& home, const City& city) {
    std::cout << "====================================================\n";
    std::cout << city.name << " | " << DateText(city) << " (день " << city.day << ") | " << TimeText(city)
              << " | " << WeatherText(city) << "\n";
    std::cout << "====================================================\n";
    std::cout << hero.name << ", " << hero.age << " лет, " << GenderText(hero.gender)
              << " — " << hero.profession << ", " << hero.trait << "\n";
    std::cout << "Энергия: " << std::setw(3) << hero.energy
              << "  Сытость: " << std::setw(3) << 100 - hero.hunger
              << "  Настроение: " << std::setw(3) << hero.mood
              << "  Здоровье: " << std::setw(3) << hero.health << "\n";
    std::cout << "Деньги: " << hero.money << " мон. | Навык профессии: " << hero.skills
              << " | Уверенность: " << hero.confidence << "\n";
    std::cout << "Дом: " << home.district << " (уют: " << home.comfort
              << ", запас еды: " << home.food << ")\n";
    std::cout << "Личная жизнь: уважение границ включено | защита: " << hero.protection << "\n";
}

void ShowMap(const City& city) {
    std::cout << "\n--- КАРТА ГОРОДА " << city.name << " ---\n";
    for (std::size_t i = 0; i < city.places.size(); ++i) std::cout << "  " << i + 1 << ". " << city.places[i] << "\n";
}

void ShowPeopleAt(const City& city, const std::string& location) {
    bool found = false;
    for (const Person& person : city.people) {
        if (person.location == location) {
            std::cout << "  - " << person.name << ", " << person.age << " лет, " << person.profession;
            if (person.acquainted) std::cout << " (дружба: " << person.friendship << ", симпатия: " << person.romance << ')';
            std::cout << "\n";
            found = true;
        }
    }
    if (!found) std::cout << "  Сейчас здесь никого из знакомых нет.\n";
}

void Interact(Character& hero, Home& home, City& city, const std::string& location) {
    std::vector<std::size_t> present;
    for (std::size_t i = 0; i < city.people.size(); ++i)
        if (city.people[i].location == location) present.push_back(i);
    if (present.empty()) {
        std::cout << "Сейчас не с кем поговорить.\n";
        return;
    }
    std::cout << "\n--- ЛЮДИ РЯДОМ ---\n";
    for (std::size_t i = 0; i < present.size(); ++i)
        std::cout << "  " << i + 1 << ". " << city.people[present[i]].name << " — " << city.people[present[i]].age
                  << " лет, " << city.people[present[i]].profession << "\n";
    const int selected = AskChoice("С кем поговорить (0 — назад): ", 0, static_cast<int>(present.size()));
    if (selected == 0) return;
    Person& person = city.people[present[selected - 1]];

    std::cout << "\n1. Познакомиться / поболтать\n2. Угостить кофе (12 мон.)\n3. Предложить помощь\n4. Пригласить на прогулку\n5. Обсудить личную близость\n0. Назад\n";
    switch (AskChoice("Действие: ", 0, 5)) {
        case 1:
            person.acquainted = true;
            person.friendship = Clamp(person.friendship + 8);
            Apply(hero, -4, 3, 7, 0, 0);
            AdvanceTime(hero, home, city, 30);
            std::cout << "Вы поговорили с " << person.name << ". Дружба выросла.\n";
            break;
        case 2:
            if (hero.money < 12) { std::cout << "Не хватает денег на кофе.\n"; break; }
            person.acquainted = true;
            person.friendship = Clamp(person.friendship + 16);
            Apply(hero, -3, -12, 13, 0, -12);
            AdvanceTime(hero, home, city, 45);
            std::cout << person.name << " с радостью принял(а) приглашение.\n";
            break;
        case 3:
            if (hero.energy < 12) { std::cout << "Сначала стоит восстановить силы.\n"; break; }
            person.acquainted = true;
            person.friendship = Clamp(person.friendship + 12);
            Apply(hero, -12, 8, 6, 0, 0);
            AdvanceTime(hero, home, city, 60);
            std::cout << "Вы помогли " << person.name << ". Это не осталось незамеченным.\n";
            break;
        case 4:
            if (!person.acquainted || person.friendship < 15) {
                std::cout << "Сначала лучше узнать друг друга.\n";
                break;
            }
            person.romance = Clamp(person.romance + 10);
            person.friendship = Clamp(person.friendship + 5);
            Apply(hero, -8, 8, 12, 0, 0);
            AdvanceTime(hero, home, city, 75);
            std::cout << "Прогулка прошла тепло. Симпатия выросла.\n";
            break;
        case 5:
            if (!person.acquainted || person.friendship < 30 || person.romance < 20) {
                std::cout << "Для такого разговора нужны доверие и взаимная симпатия.\n";
                break;
            }
            std::cout << "Обсудите границы, взаимное желание и безопасность. Продолжить?\n"
                      << "1. Да, только при ясном согласии\n0. Отмена\n";
            if (AskChoice("Выбор: ", 0, 1) == 0) {
                std::cout << "Вы уважительно отложили разговор.\n";
                break;
            }
            if (hero.protection <= 0) {
                std::cout << "Для безопасной близости нужна защита. Её можно купить на рынке.\n";
                break;
            }
            --hero.protection;
            person.romance = Clamp(person.romance + 16);
            person.friendship = Clamp(person.friendship + 6);
            Apply(hero, -10, 5, 15, 0, 0);
            AdvanceTime(hero, home, city, 90);
            if (person.romance >= 50) person.partner = true;
            std::cout << "Вы провели личное время бережно и по взаимному согласию.\n";
            if (person.partner) std::cout << person.name << " теперь считает вас партнёром.\n";
            break;
    }
}

void GoHome(Character& hero, Home& home, City& city) {
    std::cout << "\nВы дома.\n1. Отдохнуть (2 ч.)\n2. Приготовить еду (30 мин.)\n3. Обустроить дом (20 мон.)\n4. Поспать до утра\n0. Назад\n";
    switch (AskChoice("Выбор: ", 0, 4)) {
        case 1: Apply(hero, 25 + home.comfort / 10, 6, 6, 2, 0); AdvanceTime(hero, home, city, 120); std::cout << "Отдых восстановил силы.\n"; break;
        case 2:
            if (home.food > 0) { --home.food; Apply(hero, 5, -35, 10, 2, 0); AdvanceTime(hero, home, city, 30); std::cout << "Домашняя еда вернула настроение и сытость.\n"; }
            else std::cout << "Запас еды закончился — загляните на рынок.\n";
            break;
        case 3:
            if (hero.money >= 20) { hero.money -= 20; home.comfort = Clamp(home.comfort + 10); AdvanceTime(hero, home, city, 60); std::cout << "В доме стало уютнее.\n"; }
            else std::cout << "Не хватает денег.\n";
            break;
        case 4: {
            int untilMorning = (24 * 60 - city.minutes) + 8 * 60;
            Apply(hero, 45 + home.comfort / 8, 10, 10, 6, 0);
            AdvanceTime(hero, home, city, untilMorning);
            std::cout << "Вы хорошо выспались. Наступило утро дня " << city.day << ".\n";
            break;
        }
    }
}

void VisitCity(Character& hero, Home& home, City& city) {
    ShowMap(city);
    const int place = AskChoice("Куда пойти (0 — назад): ", 0, static_cast<int>(city.places.size()));
    if (place == 0) return;
    const std::string& location = city.places[place - 1];
    if (place == 1) { GoHome(hero, home, city); return; }
    AdvanceTime(hero, home, city, 20);
    std::cout << "\nВы пришли: " << location << ". Время: " << TimeText(city) << ".\n";
    ShowPeopleAt(city, location);
    std::cout << "\n1. Осмотреться / взаимодействовать с местом\n2. Поговорить с людьми\n0. Назад\n";
    const int action = AskChoice("Действие: ", 0, 2);
    if (action == 2) { Interact(hero, home, city, location); return; }
    if (action != 1) return;
    switch (place) {
        case 2:
            if (hero.money >= 12) { Apply(hero, 5, -25, 14, 0, -12); AdvanceTime(hero, home, city, 35); std::cout << "Кофе и перекус подняли настроение.\n"; }
            else std::cout << "В кошельке недостаточно денег для кафе.\n";
            break;
        case 3: Apply(hero, -12, 10, 18, 3, 0); AdvanceTime(hero, home, city, 50); std::cout << "Прогулка в парке освежила мысли.\n"; break;
        case 4: Apply(hero, -10, 6, 8, 0, 0); ++hero.skills; AdvanceTime(hero, home, city, 90); std::cout << "Несколько часов за книгами повысили навык.\n"; break;
        case 5:
            std::cout << "1. Продукты (15 мон.)\n2. Защита (10 мон.)\n0. Назад\n";
            {
                const int purchase = AskChoice("Покупка: ", 0, 2);
                if (purchase == 1 && hero.money >= 15) { hero.money -= 15; home.food += 3; AdvanceTime(hero, home, city, 30); std::cout << "Вы купили продукты на три приёма пищи.\n"; }
                else if (purchase == 2 && hero.money >= 10) { hero.money -= 10; ++hero.protection; AdvanceTime(hero, home, city, 15); std::cout << "Вы купили защиту.\n"; }
                else if (purchase != 0) std::cout << "Недостаточно денег.\n";
            }
            break;
        case 6:
            if (hero.energy < 15) std::cout << "Слишком мало сил для работы.\n";
            else { const int income = 18 + hero.skills * 4; Apply(hero, -25, 18, -5, 0, income); AdvanceTime(hero, home, city, 180); std::cout << "Рабочий блок завершён. Заработано: " << income << " мон.\n"; }
            break;
    }
}

void ShowContacts(const City& city) {
    std::cout << "\n--- ЗНАКОМЫЕ ---\n";
    bool found = false;
    for (const Person& person : city.people) {
        if (person.acquainted) {
            std::cout << "  " << person.name << " — " << person.profession << ", дружба: " << person.friendship
                      << ", симпатия: " << person.romance << (person.partner ? " | партнёр" : "") << "\n";
            found = true;
        }
    }
    if (!found) std::cout << "  Пока никого. Познакомьтесь с людьми в городе.\n";
}

}  // namespace

int main() {
    Character hero;
    Home home;
    City city;
    RefreshPeopleLocations(city);
    std::cout << "========================================\n        REAL LIFE SIM: НАЧАЛО\n========================================\n";
    std::cout << "Имя главного героя [Алекс]: "; std::getline(std::cin, hero.name);
    if (hero.name.empty()) hero.name = "Алекс";
    std::cout << "Пол / гендер\n1. Женщина\n2. Мужчина\n3. Небинарный человек\n";
    switch (AskChoice("Выбор: ", 1, 3)) {
        case 1: hero.gender = Gender::Woman; break;
        case 2: hero.gender = Gender::Man; break;
        case 3: hero.gender = Gender::NonBinary; break;
    }
    hero.age = AskChoice("Возраст (18-80): ", 18, 80);
    std::cout << "Профессия [стажёр]: "; std::getline(std::cin, hero.profession);
    if (hero.profession.empty()) hero.profession = "стажёр";
    std::cout << "Черта характера\n1. Общительный (+10 уверенности)\n2. Бережливый (+30 монет)\n3. Спортивный (+10 здоровья)\n4. Любознательный (+1 навык)\n";
    switch (AskChoice("Выбор: ", 1, 4)) {
        case 1: hero.trait = "общительный"; hero.confidence += 10; break;
        case 2: hero.trait = "бережливый"; hero.money += 30; break;
        case 3: hero.trait = "спортивный"; hero.health += 10; break;
        case 4: hero.trait = "любознательный"; ++hero.skills; break;
    }

    while (true) {
        ClearScreen();
        ShowStatus(hero, home, city);
        std::cout << "\n--- ДЕЙСТВИЯ ---\n  1. Отправиться в город\n  2. Зайти домой\n  3. Осмотреть карту\n  4. Посмотреть знакомых\n  5. Подождать час\n  0. Выйти из симуляции\n";
        const int choice = AskChoice("Выбор: ", 0, 5);
        if (choice == 0) break;
        ClearScreen();
        if (choice == 1) VisitCity(hero, home, city);
        if (choice == 2) GoHome(hero, home, city);
        if (choice == 3) ShowMap(city);
        if (choice == 4) ShowContacts(city);
        if (choice == 5) { AdvanceTime(hero, home, city, 60); std::cout << "Прошёл один час.\n"; }
        Pause();
    }
    ClearScreen();
    std::cout << "Спасибо за игру, " << hero.name << ".\n";
    ShowStatus(hero, home, city);
    return 0;
}
