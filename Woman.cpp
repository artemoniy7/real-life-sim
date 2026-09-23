#include "Woman.h"
#include <iostream>
#include <iomanip>
#include <cstdio>

// === КОНСТРУКТОРЫ ===
Woman::Woman(const std::string& name, int DaysCount)
    : name(name), DaysCount(DaysCount), BirthDay(0), money(0), alive(true),
      days_since_last_sex(0) {
    intim.UpdateDepthsByAge(GetAge());
}

Woman::Woman(const std::string& name, int DaysCount, int BirthDay)
    : name(name), DaysCount(DaysCount), BirthDay(BirthDay), money(0), alive(true),
      days_since_last_sex(0) {
    intim.UpdateDepthsByAge(GetAge());
}

// === БАЗОВОЕ ===
void Woman::Show() const {
    std::cout << "========================================\n";
    std::cout << "   ЖЕНСКИЙ ПЕРСОНАЖ\n";
    std::cout << "========================================\n";
    std::cout << "Имя: " << name << "\n";
    std::cout << "Дней: " << DaysCount << "\n";
    std::cout << "Возраст: " << GetAge() << " лет\n";
    std::cout << "Грудь: " << intim.GetBreastString() << "\n";
    std::cout << "Возбуждение: " << intim.GetArousal() << "%\n";
    std::cout << "Смазка: " << std::fixed << std::setprecision(1)
              << intim.GetLubrication() << "%\n";
    std::cout << "Боль: " << intim.GetPainLevel() << "%\n";
    std::cout << "Цикл: " << intim.GetCyclePhaseString() << "\n";
    std::cout << "Фертильность: " << intim.GetFertilityChance() << "%\n";
    std::cout << "Глубина влагалища: " << intim.GetVaginalDepth() << " см\n";
    std::cout << "Глубина ануса: " << intim.GetAnalDepth() << " см\n";
    std::cout << "Девственница: " << (intim.IsVirgin() ? "Да" : "Нет") << "\n";
    std::cout << "Анальная девственница: " << (intim.IsAnalVirgin() ? "Да" : "Нет") << "\n";
    std::cout << "Оргазмы: " << total_orgasms
              << " (в рефрактерном: " << (intim.IsInRefractory() ? "Да" : "Нет") << ")\n";
    std::cout << "Среднее удовольствие: " << std::setprecision(1)
              << average_pleasure << "%\n";
    std::cout << "Средняя боль: " << average_pain << "%\n";
    std::cout << "========================================\n";
}

void Woman::ShowStats() const {
    std::cout << "========================================\n";
    std::cout << "   СТАТИСТИКА: " << name << "\n";
    std::cout << "========================================\n";
    std::cout << "Всего оргазмов: " << total_orgasms << "\n";
    std::cout << "Всего партнёров: " << total_partners << "\n";
    std::cout << "Дней без секса: " << days_since_last_sex << "\n";
    std::cout << "Среднее удовольствие: " << std::fixed << std::setprecision(1)
              << average_pleasure << "%\n";
    std::cout << "Средняя боль: " << average_pain << "%\n";
    std::cout << "Макс. удовольствие: " << max_pleasure_ever << "%\n";
    std::cout << "Макс. боль: " << max_pain_ever << "%\n";
    std::cout << "Событий в истории: " << history.size() << "\n";
    std::cout << "========================================\n";
}

void Woman::ShowHistory() const {
    std::cout << "========================================\n";
    std::cout << "   ИСТОРИЯ: " << name << "\n";
    std::cout << "========================================\n";
    if (history.empty()) {
        std::cout << "  (история пуста)\n";
    } else {
        for (const auto& ev : history) {
            std::cout << "День " << ev.day
                      << " | Партнёр: " << (ev.partner_name.empty() ? "-" : ev.partner_name)
                      << " | Удовольствие: " << std::fixed << std::setprecision(1)
                      << ev.max_pleasure << "%"
                      << " | Боль: " << ev.max_pain << "%"
                      << " | Оргазм: " << (ev.orgasm ? "Да" : "Нет")
                      << (ev.first_time_zone ? " | ПЕРВЫЙ РАЗ" : "")
                      << "\n";
        }
    }
    std::cout << "========================================\n";
}

int Woman::GetAge() const {
    int age_days = DaysCount - BirthDay;
    if (age_days < 0) return 0;
    return age_days / 365;
}

// === ВРЕМЯ ===
void Woman::AdvanceTime(int days) { AdvanceTime(days, true); }

void Woman::AdvanceTime(int days, bool include_rest) {
    DaysCount += days;
    intim.AdvanceCycle(days);
    UpdateDays(days);
    intim.UpdateDepthsByAge(GetAge());

    if (include_rest) {
        intim.AdvanceHoleRest(days);
        if (days_since_last_sex > 30) {
            float factor = 1.0f - std::min(0.01f, (days_since_last_sex - 30) * 0.001f);
            intim.DecayAdaptation(factor);
        }
    }
    intim.RestoreSensitivity(2.0f * days);
}

// === СТАТИСТИКА ===
void Woman::UpdatePleasure(float pleasure) {
    // ИСПРАВЛЕНО: больше не трогает total_orgasms
    pleasure_events++;
    float total = average_pleasure * (pleasure_events - 1) + pleasure;
    average_pleasure = total / pleasure_events;
    if (pleasure > max_pleasure_ever) max_pleasure_ever = pleasure;
}

void Woman::UpdatePain(float pain) {
    int events = static_cast<int>(history.size()) + 1;
    float total = average_pain * (events - 1) + pain;
    average_pain = total / events;
    if (pain > max_pain_ever) max_pain_ever = pain;
}

void Woman::UpdateDays(int days_passed) {
    days_since_last_sex += days_passed;
}

// === ВЗАИМОДЕЙСТВИЕ С ЗОНАМИ ===
float Woman::StimulateZone(ErogenousZone zone, float intensity, float depth) {
    return StimulateZone(zone, intensity, depth, 0.0f);
}

float Woman::StimulateZone(ErogenousZone zone, float intensity, float depth, float diameter) {
    // ИСПРАВЛЕНО: убрана автоматическая проверка оргазма и LogEvent.
    // Управление оргазмом теперь в main.cpp (DoStroke).
    float pleasure = intim.StimulateZone(zone, intensity, depth, diameter);
    if (pleasure > 0.0f) {
        UpdatePleasure(pleasure);
        days_since_last_sex = 0;
    }
    return pleasure;
}

void Woman::ShowZonesInfo() const {
    std::cout << "========================================\n";
    std::cout << "   ЭРОГЕННЫЕ ЗОНЫ\n";
    std::cout << "========================================\n\n";

    auto zones = intim.GetAllZones();

    std::cout << std::left
              << std::setw(22) << "Зона"
              << std::setw(8) << "Чувств."
              << std::setw(10) << "Макс.глуб."
              << std::setw(10) << "Макс.диам."
              << std::setw(8) << "Проник."
              << std::setw(12) << "Адаптация"
              << std::setw(10) << "Опыт"
              << "\n";
    std::cout << std::string(90, '-') << "\n";

    for (auto zone : zones) {
        auto& params = intim.GetZone(zone);
        std::string depth_str = "-";
        if (params.depth_max > 0) {
            char buffer[16];
            snprintf(buffer, sizeof(buffer), "%.1f см", params.depth_max);
            depth_str = buffer;
        }
        std::string diam_str = "-";
        if (params.is_penetrable && params.diameter_max > 0) {
            char buffer[16];
            snprintf(buffer, sizeof(buffer), "%.1f см", params.diameter_max);
            diam_str = buffer;
        }
        float adapt = intim.GetHoleAdaptation(zone);
        int exp = intim.GetHoleExperience(zone);
        bool first = intim.IsHoleFirstTime(zone);

        std::string adapt_str = "-";
        if (params.is_penetrable) {
            if (first) adapt_str = "ПЕРВЫЙ РАЗ";
            else {
                char buffer[16];
                snprintf(buffer, sizeof(buffer), "%.0f%%", adapt);
                adapt_str = buffer;
            }
        }
        std::string exp_str = params.is_penetrable ? std::to_string(exp) : "-";

        std::cout << std::left
                  << std::setw(22) << intim.GetZoneName(zone)
                  << std::setw(8) << (int)params.current_sensitivity
                  << std::setw(10) << depth_str
                  << std::setw(10) << diam_str
                  << std::setw(8) << (params.is_penetrable ? "Да" : "Нет")
                  << std::setw(12) << adapt_str
                  << std::setw(10) << exp_str
                  << "\n";
    }
    std::cout << "\n=== СТАТУС РАЗРАБОТКИ ОТВЕРСТИЙ ===\n";
    auto penetrable = intim.GetPenetrableZones();
    for (auto zone : penetrable) {
        std::cout << "  " << intim.GetZoneName(zone) << ": "
                  << intim.GetHoleDevelopmentStatus(zone) << "\n";
    }
    std::cout << "\n";
}

// === СОБЫТИЯ ===
void Woman::LogEvent(const SexEvent& event) {
    history.push_back(event);
    if (event.max_pain > 0) UpdatePain(event.max_pain);
    if (event.orgasm) AddOrgasm();
}

// === СЕССИЯ ===
void Woman::StartSession() {
    intim.ResetPain();
}
void Woman::EndSession(bool orgasm) {
    if (orgasm) AddOrgasm();
    intim.ResetPain();
}