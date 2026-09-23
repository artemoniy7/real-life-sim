#pragma once
#include <string>
#include <map>
#include <vector>

// Размеры груди
enum class BreastSize {
    flat,
    small,
    medium,
    large,
    huge
};

// Менструальный цикл
enum class CyclePhase {
    Menstruation,
    Follicular,
    Ovulation,
    Luteal
};

// Эрогенные зоны
enum class ErogenousZone {
    // Зоны С ГЛУБИНОЙ (для проникновения)
    VaginalEntrance,
    VaginalMiddle,
    VaginalDeep,
    VaginalFornix,
    GSpot,
    Cervix,
    AnalSphincter,
    AnalCanal,
    AnalDeep,
    Urethra,
    Mouth,

    // Зоны БЕЗ ГЛУБИНЫ (поверхностные)
    Clitoris,
    Labia,
    Perineum,
    Nipples,
    Breasts,
    Ears,
    Neck,
    InnerThighs,
    LowerBack,
    Navel
};

// === ОТВЕРСТИЯ (каналы) ===
enum class Hole {
    Vagina,
    Anus,
    Mouth,
    Urethra
};

// Структура чувствительности зоны
struct ZoneSensitivity {
    float base_sensitivity = 50.0f;
    float current_sensitivity = 50.0f;
    float pleasure_multiplier = 1.0f;
    float pain_multiplier = 1.0f;

    // Глубина
    float depth_max = 0.0f;
    float depth_current = 0.0f;

    // Диаметр (для проникающих зон)
    float diameter_min = 1.0f;
    float diameter_max = 4.0f;
    float diameter_current = 0.0f;

    bool is_penetrable = false;
    bool is_active = true;
    std::string description;
};

class WomanIntim {
private:
    // === ОСНОВНЫЕ ПАРАМЕТРЫ ===
    BreastSize breast;
    BreastSize maxbreast;
    bool isvirgin;
    bool isanalvirgin;
    int arousal;

    // === ГЛУБИНЫ ===
    double vaginalDepth;
    double analDepth;
    double mouthDepth;
    double urethraDepth;

    // === СМАЗКА И БОЛЬ ===
    float lubrication;
    float pain_level;
    float lubrication_rate;

    // === ОРГАЗМ ===
    int orgasm_threshold;
    bool in_refractory;
    int refractory_timer;
    int total_orgasms;
    int multi_orgasm_streak;
    bool multi_orgasmic;

    // === МЕНСТРУАЛЬНЫЙ ЦИКЛ ===
    int cycle_day = 1;
    CyclePhase phase = CyclePhase::Follicular;
    int fertility_chance = 5;

    // === ЭРОГЕННЫЕ ЗОНЫ ===
    std::map<ErogenousZone, ZoneSensitivity> zones;

    // === РАЗРАБОТКА ОТВЕРСТИЙ ===
    struct HoleDevelopment {
        int experience = 0;
        float adaptation = 0.0f;
        float max_depth_achieved = 0.0f;
        float max_diameter_achieved = 0.0f;
        bool is_first_time = true;
        float pain_threshold = 0.0f;
        float pleasure_gain = 0.0f;
        int days_since_last_use = 999;
    };

    HoleDevelopment vaginal_dev;
    HoleDevelopment anal_dev;
    HoleDevelopment oral_dev;
    HoleDevelopment urethral_dev;

    // === ПРИВАТНЫЕ МЕТОДЫ ===
    void InitializeZones();
    void UpdateSensitivityFromArousal();
    void UpdateHoleDevelopment(ErogenousZone zone, float depth, float diameter, float pleasure, float pain);
    HoleDevelopment* GetHoleDev(ErogenousZone zone);
    const HoleDevelopment* GetHoleDev(ErogenousZone zone) const;

public:
    WomanIntim();

    // === ГРУДЬ ===
    void SetMaxBreast(BreastSize size);
    void SetCurrentBreastSize(BreastSize size);
    BreastSize GetBreast() const;
    std::string GetBreastString() const;

    // === ДЕВСТВЕННОСТЬ ===
    bool IsVirgin() const;
    bool IsAnalVirgin() const;
    void SetVirgin(bool virgin);
    void SetAnalVirgin(bool virgin);

    // === ВОЗБУЖДЕНИЕ ===
    int GetArousal() const;
    void ChangeArousal(int delta);
    float GetArousalModifier() const;

    // === СМАЗКА ===
    float GetLubrication() const;
    void UpdateLubrication();
    void SetLubricationRate(float rate);

    // === БОЛЬ ===
    float GetPainLevel() const;
    void ChangePain(float delta);
    void ResetPain();

    // === ОРГАЗМ ===
    bool CheckOrgasm();
    bool IsInRefractory() const;
    int GetTotalOrgasms() const;
    int GetMultiOrgasmStreak() const;
    void AdvanceRefractory(int seconds);
    void SetMultiOrgasmic(bool value);
    void SetOrgasmThreshold(int threshold);

    // === ГЛУБИНЫ ===
    void SetVaginalDepth(double depth);
    void SetAnalDepth(double depth);
    void SetMouthDepth(double depth);
    void SetUrethraDepth(double depth);
    double GetVaginalDepth() const;
    double GetAnalDepth() const;
    double GetMouthDepth() const;
    double GetUrethraDepth() const;

    // === МЕНСТРУАЛЬНЫЙ ЦИКЛ ===
    void AdvanceCycle(int days = 1);
    CyclePhase GetCyclePhase() const;
    int GetCycleDay() const { return cycle_day; }
    int GetFertilityChance() const;
    float GetLibidoModifier() const;
    std::string GetCyclePhaseString() const;

    // === ВОЗРАСТНЫЕ ИЗМЕНЕНИЯ ===
    void UpdateDepthsByAge(int age);

    // === ЭРОГЕННЫЕ ЗОНЫ ===
    const ZoneSensitivity& GetZone(ErogenousZone zone) const;
    float GetZoneSensitivity(ErogenousZone zone) const;
    float GetZoneMaxDepth(ErogenousZone zone) const;
    float GetZoneMaxDiameter(ErogenousZone zone) const;
    bool HasDepth(ErogenousZone zone) const;

    float StimulateZone(ErogenousZone zone, float intensity, float depth = 0.0f);
    float StimulateZone(ErogenousZone zone, float intensity, float depth, float diameter);

    std::string GetZoneName(ErogenousZone zone) const;
    std::string GetZoneDescription(ErogenousZone zone) const;
    std::vector<ErogenousZone> GetAllZones() const;
    void RestoreSensitivity(float amount = 2.0f);

    // === ОТВЕРСТИЯ (Holes) ===
    static Hole GetHoleByZone(ErogenousZone zone);
    ErogenousZone GetZoneByDepth(Hole hole, float depth) const;
    float GetHoleMaxDepth(Hole hole) const;
    std::string GetHoleName(Hole hole) const;

    // === РАЗРАБОТКА ОТВЕРСТИЙ (PUBLIC) ===
    float GetHoleAdaptation(ErogenousZone zone) const;
    int GetHoleExperience(ErogenousZone zone) const;
    float GetHoleMaxDepthZone(ErogenousZone zone) const;
    float GetHoleMaxDiameter(ErogenousZone zone) const;
    bool IsHoleFirstTime(ErogenousZone zone) const;
    std::string GetHoleDevelopmentStatus(ErogenousZone zone) const;
    float GetHolePainModifier(ErogenousZone zone) const;
    float GetHolePleasureModifier(ErogenousZone zone) const;
    std::vector<ErogenousZone> GetPenetrableZones() const;

    // Деадаптация
    void DecayAdaptation(float factor);
    void AdvanceHoleRest(int days);
};