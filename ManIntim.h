// ManIntim.h
#pragma once
#include <string>

// Размер пениса (по длине в см)
enum class PenisSize {
    micro,      // < 8 см
    small,      // 8-11 см
    average,    // 12-15 см
    large,      // 16-19 см
    huge        // 20+ см
};

// Форма изгиба
enum class PenisCurve {
    straight,   // прямой
    up,         // вверх
    down,       // вниз
    left,       // влево
    right       // вправо
};

// Толщина (обхват в см)
enum class PenisGirth {
    thin,       // < 9 см
    average,    // 9-12 см
    thick,      // 12-14 см
    very_thick  // 14+ см
};

class ManIntim {
private:
    // === РАЗМЕРЫ ===
    float length;               // длина, см (эрегированная)
    float girth;                // обхват, см
    PenisSize size_category;
    PenisGirth girth_category;
    PenisCurve curve;

    // === ВЫНОСЛИВОСТЬ ===
    float stamina;              // 0-100 (максимум)
    float current_stamina;      // текущая
    int refractory_seconds;     // секунды до восстановления
    bool in_refractory;

    // === НАВЫКИ ===
    float skill;                // 0-1
    float rhythm_control;       // 0-1
    float depth_control;        // 0-1
    int total_partners;
    int total_orgasms;

    // === СОСТОЯНИЕ ===
    int arousal;                // 0-100
    bool erect;                 // эрекция
    float erection_quality;     // 0-1

    // === ОСОБЕННОСТИ ===
    bool circumcised;
    float sensitivity;          // 0-1

    // === ВНУТРЕННЕЕ ===
    int refractory_duration;    // базовая длительность рефрактерного периода (сек)

public:
    ManIntim();

    // === РАЗМЕРЫ ===
    void SetLength(float cm);
    void SetGirth(float cm);
    void SetCurve(PenisCurve c);
    float GetLength() const { return length; }
    float GetGirth() const { return girth; }
    PenisSize GetSizeCategory() const { return size_category; }
    PenisGirth GetGirthCategory() const { return girth_category; }
    PenisCurve GetCurve() const { return curve; }
    std::string GetSizeString() const;
    std::string GetGirthString() const;
    std::string GetCurveString() const;

    // === ЭРЕКЦИЯ ===
    bool IsErect() const { return erect; }
    void SetErect(bool value);
    float GetErectionQuality() const { return erection_quality; }
    void UpdateErection();

    // === ВОЗБУЖДЕНИЕ ===
    int GetArousal() const { return arousal; }
    void ChangeArousal(int delta);
    void ResetArousal() { arousal = 0; UpdateErection(); }

    // === ВЫНОСЛИВОСТЬ ===
    float GetStamina() const { return stamina; }
    float GetCurrentStamina() const { return current_stamina; }
    void SetStamina(float value);
    void DrainStamina(float amount);
    void RestoreStamina(float amount);
    bool IsInRefractory() const { return in_refractory; }
    int GetRefractorySeconds() const { return refractory_seconds; }
    void AdvanceRefractory(int seconds);
    void SetRefractoryDuration(int seconds);
    int GetRefractoryDuration() const { return refractory_duration; }

    // === НАВЫКИ ===
    float GetSkill() const { return skill; }
    float GetRhythmControl() const { return rhythm_control; }
    float GetDepthControl() const { return depth_control; }
    void SetSkill(float value);
    void SetRhythmControl(float value);
    void SetDepthControl(float value);
    int GetTotalPartners() const { return total_partners; }
    int GetTotalOrgasms() const { return total_orgasms; }
    void AddPartner() { total_partners++; }
    void AddOrgasm() { total_orgasms++; }

    // === ОСОБЕННОСТИ ===
    bool IsCircumcised() const { return circumcised; }
    void SetCircumcised(bool value);
    float GetSensitivity() const { return sensitivity; }
    void SetSensitivity(float value);

    // === СБРОС ===
    void Reset() {
        arousal = 0;
        erect = false;
        erection_quality = 0.0f;
        in_refractory = false;
        refractory_seconds = 0;
    }

    // === ВЫВОД ===
    void Show() const;
};