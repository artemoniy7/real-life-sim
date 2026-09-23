// ManIntim.cpp
#include "ManIntim.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

ManIntim::ManIntim()
    : length(14.0f),
      girth(11.0f),
      size_category(PenisSize::average),
      girth_category(PenisGirth::average),
      curve(PenisCurve::straight),
      stamina(70.0f),
      current_stamina(70.0f),
      refractory_seconds(0),
      in_refractory(false),
      skill(0.3f),
      rhythm_control(0.3f),
      depth_control(0.3f),
      total_partners(0),
      total_orgasms(0),
      arousal(0),
      erect(false),
      erection_quality(0.0f),
      circumcised(false),
      sensitivity(0.7f),
      refractory_duration(300) {   // 5 минут по умолчанию
}

// === РАЗМЕРЫ ===
void ManIntim::SetLength(float cm) {
    length = std::clamp(cm, 5.0f, 25.0f);

    if (length < 8.0f)       size_category = PenisSize::micro;
    else if (length < 12.0f) size_category = PenisSize::small;
    else if (length < 16.0f) size_category = PenisSize::average;
    else if (length < 20.0f) size_category = PenisSize::large;
    else                     size_category = PenisSize::huge;
}

void ManIntim::SetGirth(float cm) {
    girth = std::clamp(cm, 6.0f, 18.0f);

    if (girth < 9.0f)        girth_category = PenisGirth::thin;
    else if (girth < 12.0f)  girth_category = PenisGirth::average;
    else if (girth < 14.0f)  girth_category = PenisGirth::thick;
    else                     girth_category = PenisGirth::very_thick;
}

void ManIntim::SetCurve(PenisCurve c) {
    curve = c;
}

std::string ManIntim::GetSizeString() const {
    switch(size_category) {
        case PenisSize::micro:   return "микро";
        case PenisSize::small:   return "маленький";
        case PenisSize::average: return "средний";
        case PenisSize::large:   return "большой";
        case PenisSize::huge:    return "огромный";
        default: return "неизвестно";
    }
}

std::string ManIntim::GetGirthString() const {
    switch(girth_category) {
        case PenisGirth::thin:       return "тонкий";
        case PenisGirth::average:    return "средний";
        case PenisGirth::thick:      return "толстый";
        case PenisGirth::very_thick: return "очень толстый";
        default: return "неизвестно";
    }
}

std::string ManIntim::GetCurveString() const {
    switch(curve) {
        case PenisCurve::straight: return "прямой";
        case PenisCurve::up:       return "изогнут вверх";
        case PenisCurve::down:     return "изогнут вниз";
        case PenisCurve::left:     return "изогнут влево";
        case PenisCurve::right:    return "изогнут вправо";
        default: return "неизвестно";
    }
}

// === ЭРЕКЦИЯ ===
void ManIntim::SetErect(bool value) {
    erect = value;
    if (!erect) erection_quality = 0.0f;
}

void ManIntim::UpdateErection() {
    // В рефрактерном периоде эрекции нет
    if (in_refractory) {
        erect = false;
        erection_quality = 0.0f;
        return;
    }

    if (arousal < 20) {
        erect = false;
        erection_quality = 0.0f;
        return;
    }

    erect = true;

    // Качество зависит от возбуждения и усталости
    float base = (arousal - 20) / 80.0f;            // 0-1
    float stamina_mod = (stamina > 0.0f) ? (current_stamina / stamina) : 0.0f;
    erection_quality = std::clamp(base * (0.5f + stamina_mod * 0.5f), 0.0f, 1.0f);
}

// === ВОЗБУЖДЕНИЕ ===
void ManIntim::ChangeArousal(int delta) {
    arousal += delta;
    arousal = std::clamp(arousal, 0, 100);

    // === АВТО-ЭЯКУЛЯЦИЯ ===
    if (arousal >= 95 && !in_refractory) {
        total_orgasms++;

        // Откат возбуждения
        arousal = 10;

        // Рефрактерный период
        in_refractory = true;
        refractory_seconds = refractory_duration;

        // Потеря эрекции и выносливости
        SetErect(false);
        DrainStamina(20.0f);
        return;
    }

    UpdateErection();
}

// === ВЫНОСЛИВОСТЬ ===
void ManIntim::SetStamina(float value) {
    stamina = std::clamp(value, 10.0f, 100.0f);
    current_stamina = std::min(current_stamina, stamina);
}

void ManIntim::DrainStamina(float amount) {
    current_stamina -= amount;
    current_stamina = std::max(0.0f, current_stamina);
    UpdateErection();
}

void ManIntim::RestoreStamina(float amount) {
    current_stamina = std::min(stamina, current_stamina + amount);
    UpdateErection();
}

void ManIntim::AdvanceRefractory(int seconds) {
    if (!in_refractory) return;

    refractory_seconds -= seconds;
    if (refractory_seconds <= 0) {
        in_refractory = false;
        refractory_seconds = 0;
        UpdateErection();
    }
}

void ManIntim::SetRefractoryDuration(int seconds) {
    refractory_duration = std::clamp(seconds, 30, 3600);
}

// === НАВЫКИ ===
void ManIntim::SetSkill(float value) {
    skill = std::clamp(value, 0.0f, 1.0f);
}

void ManIntim::SetRhythmControl(float value) {
    rhythm_control = std::clamp(value, 0.0f, 1.0f);
}

void ManIntim::SetDepthControl(float value) {
    depth_control = std::clamp(value, 0.0f, 1.0f);
}

// === ОСОБЕННОСТИ ===
void ManIntim::SetCircumcised(bool value) {
    circumcised = value;
}

void ManIntim::SetSensitivity(float value) {
    sensitivity = std::clamp(value, 0.0f, 1.0f);
}

// === ВЫВОД ===
void ManIntim::Show() const {
    std::cout << "========================================\n";
    std::cout << "   МУЖСКИЕ ИНТИМ-ХАРАКТЕРИСТИКИ\n";
    std::cout << "========================================\n";
    std::cout << "Длина: " << std::fixed << std::setprecision(1) << length
              << " см (" << GetSizeString() << ")\n";
    std::cout << "Обхват: " << girth << " см (" << GetGirthString() << ")\n";
    std::cout << "Изгиб: " << GetCurveString() << "\n";
    std::cout << "Обрезан: " << (circumcised ? "Да" : "Нет") << "\n";
    std::cout << "Чувствительность: " << (int)(sensitivity * 100) << "%\n";
    std::cout << "----------------------------------------\n";
    std::cout << "Эрекция: " << (erect ? "Да" : "Нет")
              << " (качество: " << (int)(erection_quality * 100) << "%)\n";
    std::cout << "Возбуждение: " << arousal << "%\n";
    std::cout << "Выносливость: " << (int)current_stamina
              << "/" << (int)stamina << "\n";

    if (in_refractory) {
        std::cout << "Рефрактерный: Да (" << refractory_seconds << " сек осталось)\n";
    } else {
        std::cout << "Рефрактерный: Нет\n";
    }

    std::cout << "----------------------------------------\n";
    std::cout << "Навык: " << (int)(skill * 100) << "%\n";
    std::cout << "Контроль ритма: " << (int)(rhythm_control * 100) << "%\n";
    std::cout << "Контроль глубины: " << (int)(depth_control * 100) << "%\n";
    std::cout << "Партнёров: " << total_partners << "\n";
    std::cout << "Оргазмов: " << total_orgasms << "\n";
    std::cout << "========================================\n";
}