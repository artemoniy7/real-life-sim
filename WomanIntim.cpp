#include "WomanIntim.h"
#include <algorithm>
#include <cmath>
#include "utils.h"

WomanIntim::WomanIntim()
    : breast(BreastSize::flat),
      maxbreast(BreastSize::flat),
      isvirgin(true),
      isanalvirgin(true),
      arousal(0),
      vaginalDepth(8.0),
      analDepth(5.0),
      mouthDepth(4.0),
      urethraDepth(3.0),
      lubrication(0.0f),
      pain_level(0.0f),
      lubrication_rate(1.0f),
      orgasm_threshold(85),
      in_refractory(false),
      refractory_timer(0),
      total_orgasms(0),
      multi_orgasm_streak(0),
      multi_orgasmic(true) {
    InitializeZones();
}

// === ИНИЦИАЛИЗАЦИЯ ЗОН ===
void WomanIntim::InitializeZones() {
    zones[ErogenousZone::VaginalEntrance] = {
        80.0f, 80.0f, 1.2f, 0.8f, 3.0f, 0.0f,
        1.5f, 4.0f, 0.0f,
        true, true,
        "Вход во влагалище (самые чувствительные нервные окончания)"
    };
    zones[ErogenousZone::VaginalMiddle] = {
        60.0f, 60.0f, 1.0f, 0.9f, 7.0f, 0.0f,
        2.0f, 5.0f, 0.0f,
        true, true,
        "Средняя часть влагалища (мышечные кольца)"
    };
    zones[ErogenousZone::VaginalDeep] = {
        50.0f, 50.0f, 0.9f, 1.0f, 10.0f, 0.0f,
        2.5f, 6.0f, 0.0f,
        true, true,
        "Глубокая часть влагалища (чувство наполненности)"
    };
    zones[ErogenousZone::VaginalFornix] = {
        65.0f, 65.0f, 1.3f, 0.7f, 12.0f, 0.0f,
        2.5f, 6.0f, 0.0f,
        true, true,
        "Свод влагалища (A-точка, очень чувствительная)"
    };
    zones[ErogenousZone::GSpot] = {
        90.0f, 90.0f, 1.8f, 0.5f, 6.0f, 0.0f,
        2.0f, 5.0f, 0.0f,
        true, true,
        "G-точка (на передней стенке, 4-6 см от входа)"
    };
    zones[ErogenousZone::Cervix] = {
        30.0f, 30.0f, 0.4f, 1.8f, 11.0f, 0.0f,
        1.0f, 3.0f, 0.0f,
        true, true,
        "Шейка матки (требует осторожности)"
    };
    zones[ErogenousZone::AnalSphincter] = {
        50.0f, 50.0f, 0.6f, 1.5f, 2.0f, 0.0f,
        0.8f, 3.0f, 0.0f,
        true, true,
        "Наружный сфинктер ануса (сильное сжатие)"
    };
    zones[ErogenousZone::AnalCanal] = {
        40.0f, 40.0f, 0.7f, 1.3f, 5.0f, 0.0f,
        1.5f, 4.5f, 0.0f,
        true, true,
        "Анальный канал (много нервных окончаний)"
    };
    zones[ErogenousZone::AnalDeep] = {
        30.0f, 30.0f, 0.8f, 1.0f, 12.0f, 0.0f,
        2.0f, 5.0f, 0.0f,
        true, true,
        "Глубокая часть ануса (ощущение наполненности)"
    };
    zones[ErogenousZone::Urethra] = {
        60.0f, 60.0f, 0.8f, 1.2f, 4.0f, 0.0f,
        0.5f, 1.5f, 0.0f,
        true, true,
        "Уретра (скинова железа, может вызывать сквирт)"
    };
    zones[ErogenousZone::Mouth] = {
        45.0f, 45.0f, 0.7f, 0.5f, 10.0f, 0.0f,
        2.0f, 6.0f, 0.0f,
        true, true,
        "Рот (оральный секс)"
    };

    zones[ErogenousZone::Clitoris] = {
        95.0f, 95.0f, 2.0f, 0.3f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        false, true,
        "Клитор (самая чувствительная зона)"
    };
    zones[ErogenousZone::Labia] = {
        65.0f, 65.0f, 0.9f, 0.6f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        false, true,
        "Половые губы (нежные прикосновения)"
    };
    zones[ErogenousZone::Perineum] = {
        55.0f, 55.0f, 0.7f, 0.4f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        false, true,
        "Промежность (чувствительная зона между анусом и влагалищем)"
    };
    zones[ErogenousZone::Nipples] = {
        75.0f, 75.0f, 1.2f, 0.3f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        false, true,
        "Соски (очень отзывчивые на прикосновения)"
    };
    zones[ErogenousZone::Breasts] = {
        60.0f, 60.0f, 0.8f, 0.2f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        false, true,
        "Грудь (массаж и ласки)"
    };
    zones[ErogenousZone::Ears] = {
        50.0f, 50.0f, 0.6f, 0.1f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        false, true,
        "Мочки ушей (шепот и поцелуи)"
    };
    zones[ErogenousZone::Neck] = {
        55.0f, 55.0f, 0.7f, 0.1f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        false, true,
        "Шея (поцелуи и прикосновения)"
    };
    zones[ErogenousZone::InnerThighs] = {
        45.0f, 45.0f, 0.5f, 0.2f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        false, true,
        "Внутренняя сторона бёдер (очень чувствительная зона)"
    };
    zones[ErogenousZone::LowerBack] = {
        40.0f, 40.0f, 0.4f, 0.1f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        false, true,
        "Поясница (эротичный массаж)"
    };
    zones[ErogenousZone::Navel] = {
        35.0f, 35.0f, 0.3f, 0.1f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        false, true,
        "Пупок (нежные прикосновения)"
    };
}

// === ОБНОВЛЕНИЕ ЧУВСТВИТЕЛЬНОСТИ ===
void WomanIntim::UpdateSensitivityFromArousal() {
    float arousal_factor = 0.6f + (arousal / 100.0f) * 0.8f;
    for (auto& [zone, params] : zones) {
        float target = params.base_sensitivity * arousal_factor;
        params.current_sensitivity += (target - params.current_sensitivity) * 0.1f;
        params.current_sensitivity = std::clamp(params.current_sensitivity, 5.0f, 100.0f);
    }
}

// === ГРУДЬ ===
void WomanIntim::SetMaxBreast(BreastSize size) { maxbreast = size; }
void WomanIntim::SetCurrentBreastSize(BreastSize size) { if (size <= maxbreast) breast = size; }
BreastSize WomanIntim::GetBreast() const { return breast; }
std::string WomanIntim::GetBreastString() const {
    switch(breast) {
        case BreastSize::flat: return "плоская";
        case BreastSize::small: return "маленькая";
        case BreastSize::medium: return "средняя";
        case BreastSize::large: return "большая";
        case BreastSize::huge: return "огромная";
    }
    return "неизвестно";
}

// === ДЕВСТВЕННОСТЬ ===
bool WomanIntim::IsVirgin() const { return isvirgin; }
bool WomanIntim::IsAnalVirgin() const { return isanalvirgin; }
void WomanIntim::SetVirgin(bool v) { isvirgin = v; }
void WomanIntim::SetAnalVirgin(bool v) { isanalvirgin = v; }

// === ВОЗБУЖДЕНИЕ ===
int WomanIntim::GetArousal() const { return arousal; }
void WomanIntim::ChangeArousal(int delta) {
    arousal += delta;
    arousal = std::clamp(arousal, 0, 100);
    UpdateSensitivityFromArousal();
    UpdateLubrication();
}
float WomanIntim::GetArousalModifier() const { return 0.5f + (arousal / 100.0f) * 1.0f; }

// === СМАЗКА ===
float WomanIntim::GetLubrication() const { return lubrication; }
void WomanIntim::SetLubricationRate(float rate) { lubrication_rate = std::clamp(rate, 0.5f, 1.5f); }
void WomanIntim::UpdateLubrication() {
    float target = arousal * 0.9f;
    if (isvirgin) target *= 0.5f;
    if (phase == CyclePhase::Ovulation) target *= 1.3f;
    if (phase == CyclePhase::Menstruation) target *= 0.7f;
    target *= lubrication_rate;
    lubrication += (target - lubrication) * 0.1f;
    lubrication = std::clamp(lubrication, 0.0f, 100.0f);
}

// === БОЛЬ ===
float WomanIntim::GetPainLevel() const { return pain_level; }
void WomanIntim::ChangePain(float delta) {
    pain_level += delta;
    pain_level = std::clamp(pain_level, 0.0f, 100.0f);
    if (pain_level > 30.0f) {
        int penalty = static_cast<int>((pain_level - 30.0f) * 0.3f);
        arousal = std::max(0, arousal - penalty);
    }
}
void WomanIntim::ResetPain() { pain_level = 0.0f; }

// === ОРГАЗМ ===
bool WomanIntim::CheckOrgasm() {
    if (in_refractory) return false;
    if (arousal < orgasm_threshold) return false;

    total_orgasms++;
    multi_orgasm_streak++;
    arousal = 15;

    in_refractory = true;
    if (multi_orgasmic) refractory_timer = utl::RandomInt(20, 90);
    else                refractory_timer = utl::RandomInt(120, 600);

    pain_level = std::max(0.0f, pain_level - 20.0f);
    return true;
}
bool WomanIntim::IsInRefractory() const { return in_refractory; }
int WomanIntim::GetTotalOrgasms() const { return total_orgasms; }
int WomanIntim::GetMultiOrgasmStreak() const { return multi_orgasm_streak; }
void WomanIntim::AdvanceRefractory(int seconds) {
    if (!in_refractory) return;
    refractory_timer -= seconds;
    if (refractory_timer <= 0) { in_refractory = false; refractory_timer = 0; }
}
void WomanIntim::SetMultiOrgasmic(bool v) { multi_orgasmic = v; }
void WomanIntim::SetOrgasmThreshold(int t) { orgasm_threshold = std::clamp(t, 50, 100); }

// === ГЛУБИНЫ ===
void WomanIntim::SetVaginalDepth(double d) { if (d > 0) vaginalDepth = d; }
void WomanIntim::SetAnalDepth(double d) { if (d > 0) analDepth = d; }
void WomanIntim::SetMouthDepth(double d) { if (d > 0) mouthDepth = d; }
void WomanIntim::SetUrethraDepth(double d) { if (d > 0) urethraDepth = d; }
double WomanIntim::GetVaginalDepth() const { return vaginalDepth; }
double WomanIntim::GetAnalDepth() const { return analDepth; }
double WomanIntim::GetMouthDepth() const { return mouthDepth; }
double WomanIntim::GetUrethraDepth() const { return urethraDepth; }

// === МЕНСТРУАЛЬНЫЙ ЦИКЛ ===
void WomanIntim::AdvanceCycle(int days) {
    cycle_day = (cycle_day + days - 1) % 28 + 1;
    if (cycle_day <= 5)       { phase = CyclePhase::Menstruation; fertility_chance = 0; }
    else if (cycle_day <= 13) { phase = CyclePhase::Follicular;  fertility_chance = 10; }
    else if (cycle_day <= 16) { phase = CyclePhase::Ovulation;   fertility_chance = 30; }
    else                      { phase = CyclePhase::Luteal;      fertility_chance = 5; }
}
CyclePhase WomanIntim::GetCyclePhase() const { return phase; }
int WomanIntim::GetFertilityChance() const { return fertility_chance; }
float WomanIntim::GetLibidoModifier() const {
    switch(phase) {
        case CyclePhase::Ovulation: return 1.5f;
        case CyclePhase::Follicular: return 1.2f;
        case CyclePhase::Luteal: return 0.8f;
        case CyclePhase::Menstruation: return 0.5f;
    }
    return 1.0f;
}
std::string WomanIntim::GetCyclePhaseString() const {
    switch(phase) {
        case CyclePhase::Menstruation: return "Менструация";
        case CyclePhase::Follicular: return "Фолликулярная";
        case CyclePhase::Ovulation: return "ОВУЛЯЦИЯ (пик)";
        case CyclePhase::Luteal: return "Лютеиновая";
    }
    return "Неизвестно";
}

// === ВОЗРАСТНЫЕ ИЗМЕНЕНИЯ ===
void WomanIntim::UpdateDepthsByAge(int age) {
    if (age < 12)      vaginalDepth = 4.0 + age * 0.3;
    else if (age < 18) vaginalDepth = 6.0 + (age - 12) * 0.4;
    else if (age < 30) vaginalDepth = 9.0 + (age - 18) * 0.1;
    else if (age < 45) vaginalDepth = 10.0 - (age - 30) * 0.05;
    else               vaginalDepth = 9.0 - (age - 45) * 0.08;
    vaginalDepth = std::max(6.0, std::min(12.0, vaginalDepth));

    if (age < 16)      analDepth = 3.0 + age * 0.15;
    else if (age < 25) analDepth = 5.0 + (age - 16) * 0.1;
    else if (age < 40) analDepth = 6.0 + (age - 25) * 0.08;
    else               analDepth = 7.0 + (age - 40) * 0.05;
    analDepth = std::max(3.0, std::min(8.0, analDepth));

    if (age < 18) mouthDepth = 3.0 + age * 0.1;
    else          mouthDepth = 5.0 + std::min(age - 18, 20) * 0.05;
    mouthDepth = std::max(3.0, std::min(7.0, mouthDepth));

    if (age < 18) urethraDepth = 1.5 + age * 0.08;
    else          urethraDepth = 3.0 + std::min(age - 18, 20) * 0.03;
    urethraDepth = std::max(1.5, std::min(4.0, urethraDepth));

    zones[ErogenousZone::VaginalEntrance].depth_max = vaginalDepth * 0.25f;
    zones[ErogenousZone::VaginalMiddle].depth_max   = vaginalDepth * 0.7f;
    zones[ErogenousZone::VaginalDeep].depth_max     = vaginalDepth * 0.9f;
    zones[ErogenousZone::VaginalFornix].depth_max   = vaginalDepth * 1.1f;
    zones[ErogenousZone::GSpot].depth_max           = vaginalDepth * 0.5f;
    zones[ErogenousZone::Cervix].depth_max          = vaginalDepth * 1.0f;

    zones[ErogenousZone::AnalSphincter].depth_max = analDepth * 0.2f;
    zones[ErogenousZone::AnalCanal].depth_max     = analDepth * 0.5f;
    zones[ErogenousZone::AnalDeep].depth_max      = analDepth * 1.0f;

    zones[ErogenousZone::Mouth].depth_max   = mouthDepth * 0.8f;
    zones[ErogenousZone::Urethra].depth_max = urethraDepth * 0.9f;

    for (auto& [zone, params] : zones) {
        if (params.depth_current > params.depth_max) params.depth_current = params.depth_max;
    }
}

// === ЭРОГЕННЫЕ ЗОНЫ ===
const ZoneSensitivity& WomanIntim::GetZone(ErogenousZone zone) const {
    static ZoneSensitivity empty;
    auto it = zones.find(zone);
    if (it != zones.end()) return it->second;
    return empty;
}
float WomanIntim::GetZoneSensitivity(ErogenousZone zone) const {
    auto it = zones.find(zone);
    return (it != zones.end()) ? it->second.current_sensitivity : 0.0f;
}
float WomanIntim::GetZoneMaxDepth(ErogenousZone zone) const {
    auto it = zones.find(zone);
    return (it != zones.end()) ? it->second.depth_max : 0.0f;
}
float WomanIntim::GetZoneMaxDiameter(ErogenousZone zone) const {
    auto it = zones.find(zone);
    return (it != zones.end()) ? it->second.diameter_max : 0.0f;
}
bool WomanIntim::HasDepth(ErogenousZone zone) const {
    auto it = zones.find(zone);
    return (it != zones.end()) ? (it->second.depth_max > 0.0f) : false;
}

float WomanIntim::StimulateZone(ErogenousZone zone, float intensity, float depth) {
    return StimulateZone(zone, intensity, depth, 0.0f);
}

float WomanIntim::StimulateZone(ErogenousZone zone, float intensity, float depth, float diameter) {
    auto it = zones.find(zone);
    if (it == zones.end()) return 0.0f;

    auto& z = it->second;
    if (!z.is_active) return 0.0f;

    if (z.is_penetrable) {
        if (depth > 0.0f) {
            depth = std::min(depth, z.depth_max);
            z.depth_current = depth;
        }
        if (diameter > 0.0f) {
            z.diameter_current = std::min(diameter, z.diameter_max);
        }
    }

    float arousal_mod = GetArousalModifier();
    float cycle_mod = GetLibidoModifier();
    float pain_mod = GetHolePainModifier(zone);
    float pleasure_mod = GetHolePleasureModifier(zone);

    float pleasure = (z.current_sensitivity / 100.0f) * (intensity / 100.0f) * z.pleasure_multiplier;
    pleasure *= arousal_mod * cycle_mod * pleasure_mod;
    pleasure = std::clamp(pleasure * 100.0f, 0.0f, 100.0f);

    float pain = 0.0f;
    if (z.is_penetrable && depth > 0.0f && z.depth_max > 0.0f) {
        float depth_ratio = depth / z.depth_max;
        if (depth_ratio > 0.6f) pain += (depth_ratio - 0.6f) * 60.0f;
    }
    if (z.is_penetrable && diameter > 0.0f && z.diameter_max > 0.0f) {
        float stretch_ratio = diameter / z.diameter_max;
        if (stretch_ratio > 0.7f) pain += (stretch_ratio - 0.7f) * 80.0f;
        if (diameter > z.diameter_max) pain += (diameter - z.diameter_max) * 40.0f;
    }
    pain *= z.pain_multiplier * pain_mod;
    pain *= (1.0f - lubrication / 150.0f);
    pain = std::clamp(pain, 0.0f, 100.0f);

    if (z.is_penetrable && depth > 0.0f) {
        UpdateHoleDevelopment(zone, depth, diameter, pleasure, pain);
    }

    int arousal_delta = static_cast<int>(pleasure * 0.5f) - static_cast<int>(pain * 0.3f);
    ChangeArousal(arousal_delta);
    ChangePain(pain * 0.1f);

    z.current_sensitivity -= intensity * 0.03f;
    z.current_sensitivity = std::max(5.0f, z.current_sensitivity);

    return pleasure;
}

std::string WomanIntim::GetZoneName(ErogenousZone zone) const {
    switch(zone) {
        case ErogenousZone::VaginalEntrance: return "Вход во влагалище";
        case ErogenousZone::VaginalMiddle: return "Средняя часть влагалища";
        case ErogenousZone::VaginalDeep: return "Глубокая часть влагалища";
        case ErogenousZone::VaginalFornix: return "Свод влагалища";
        case ErogenousZone::GSpot: return "G-точка";
        case ErogenousZone::Cervix: return "Шейка матки";
        case ErogenousZone::AnalSphincter: return "Наружный сфинктер ануса";
        case ErogenousZone::AnalCanal: return "Анальный канал";
        case ErogenousZone::AnalDeep: return "Глубокая часть ануса";
        case ErogenousZone::Urethra: return "Уретра";
        case ErogenousZone::Mouth: return "Рот";
        case ErogenousZone::Clitoris: return "Клитор";
        case ErogenousZone::Labia: return "Половые губы";
        case ErogenousZone::Perineum: return "Промежность";
        case ErogenousZone::Nipples: return "Соски";
        case ErogenousZone::Breasts: return "Грудь";
        case ErogenousZone::Ears: return "Мочки ушей";
        case ErogenousZone::Neck: return "Шея";
        case ErogenousZone::InnerThighs: return "Внутренняя сторона бёдер";
        case ErogenousZone::LowerBack: return "Поясница";
        case ErogenousZone::Navel: return "Пупок";
    }
    return "Неизвестно";
}

std::string WomanIntim::GetZoneDescription(ErogenousZone zone) const {
    auto it = zones.find(zone);
    return (it != zones.end()) ? it->second.description : "";
}
std::vector<ErogenousZone> WomanIntim::GetAllZones() const {
    std::vector<ErogenousZone> result;
    for (const auto& [zone, _] : zones) result.push_back(zone);
    return result;
}
void WomanIntim::RestoreSensitivity(float amount) {
    for (auto& [zone, params] : zones) {
        params.current_sensitivity = std::min(params.base_sensitivity,
                                              params.current_sensitivity + amount);
    }
}

// === ОТВЕРСТИЯ (Holes) ===
Hole WomanIntim::GetHoleByZone(ErogenousZone zone) {
    switch (zone) {
        case ErogenousZone::VaginalEntrance:
        case ErogenousZone::VaginalMiddle:
        case ErogenousZone::VaginalDeep:
        case ErogenousZone::VaginalFornix:
        case ErogenousZone::GSpot:
        case ErogenousZone::Cervix:
            return Hole::Vagina;
        case ErogenousZone::AnalSphincter:
        case ErogenousZone::AnalCanal:
        case ErogenousZone::AnalDeep:
            return Hole::Anus;
        case ErogenousZone::Mouth:
            return Hole::Mouth;
        case ErogenousZone::Urethra:
            return Hole::Urethra;
        default:
            return Hole::Vagina;
    }
}

ErogenousZone WomanIntim::GetZoneByDepth(Hole hole, float depth) const {
    if (hole == Hole::Vagina) {
        if (depth <= vaginalDepth * 0.25f) return ErogenousZone::VaginalEntrance;
        if (depth <= vaginalDepth * 0.65f) return ErogenousZone::VaginalMiddle;
        if (depth <= vaginalDepth * 0.9f)  return ErogenousZone::VaginalDeep;
        return ErogenousZone::VaginalFornix;
    }
    if (hole == Hole::Anus) {
        if (depth <= analDepth * 0.2f) return ErogenousZone::AnalSphincter;
        if (depth <= analDepth * 0.6f) return ErogenousZone::AnalCanal;
        return ErogenousZone::AnalDeep;
    }
    if (hole == Hole::Mouth)   return ErogenousZone::Mouth;
    if (hole == Hole::Urethra) return ErogenousZone::Urethra;
    return ErogenousZone::VaginalEntrance;
}

float WomanIntim::GetHoleMaxDepth(Hole hole) const {
    switch (hole) {
        case Hole::Vagina:  return vaginalDepth;
        case Hole::Anus:    return analDepth;
        case Hole::Mouth:   return mouthDepth;
        case Hole::Urethra: return urethraDepth;
    }
    return 0.0f;
}

std::string WomanIntim::GetHoleName(Hole hole) const {
    switch (hole) {
        case Hole::Vagina:  return "Влагалище";
        case Hole::Anus:    return "Анус";
        case Hole::Mouth:   return "Рот";
        case Hole::Urethra: return "Уретра";
    }
    return "?";
}

// === РАЗРАБОТКА ОТВЕРСТИЙ (приватные хелперы) ===
WomanIntim::HoleDevelopment* WomanIntim::GetHoleDev(ErogenousZone zone) {
    switch(zone) {
        case ErogenousZone::VaginalEntrance:
        case ErogenousZone::VaginalMiddle:
        case ErogenousZone::VaginalDeep:
        case ErogenousZone::VaginalFornix:
        case ErogenousZone::GSpot:
        case ErogenousZone::Cervix:
            return &vaginal_dev;
        case ErogenousZone::AnalSphincter:
        case ErogenousZone::AnalCanal:
        case ErogenousZone::AnalDeep:
            return &anal_dev;
        case ErogenousZone::Mouth:
            return &oral_dev;
        case ErogenousZone::Urethra:
            return &urethral_dev;
        default:
            return nullptr;
    }
}
const WomanIntim::HoleDevelopment* WomanIntim::GetHoleDev(ErogenousZone zone) const {
    return const_cast<WomanIntim*>(this)->GetHoleDev(zone);
}

void WomanIntim::UpdateHoleDevelopment(
    ErogenousZone zone, float depth, float diameter, float pleasure, float pain) {
    HoleDevelopment* dev = GetHoleDev(zone);
    if (!dev) return;

    dev->experience++;
    dev->days_since_last_use = 0;

    if (dev->is_first_time) {
        dev->is_first_time = false;
        dev->adaptation += 5.0f;
    }
    if (depth > dev->max_depth_achieved)    dev->max_depth_achieved = depth;
    if (diameter > dev->max_diameter_achieved) dev->max_diameter_achieved = diameter;

    float gain = 0.0f;
    int exp = dev->experience;
    if (exp <= 3)       gain = 12.0f;
    else if (exp <= 8)  gain = 6.0f;
    else if (exp <= 15) gain = 3.0f;
    else                gain = 1.0f;

    if (pleasure > 50.0f) gain *= 1.4f;
    if (pleasure > 80.0f) gain *= 1.3f;
    if (pain > 40.0f) gain *= 0.7f;
    if (pain > 70.0f) {
        gain *= 0.5f;
        dev->adaptation = std::max(0.0f, dev->adaptation - 1.0f);
    }

    dev->adaptation = std::min(100.0f, dev->adaptation + gain);
    dev->pain_threshold = 20.0f + dev->adaptation * 0.6f;
    dev->pleasure_gain = dev->adaptation * 0.008f;
}

// === РАЗРАБОТКА ОТВЕРСТИЙ (PUBLIC) ===
float WomanIntim::GetHoleAdaptation(ErogenousZone zone) const {
    auto* dev = GetHoleDev(zone);
    return dev ? dev->adaptation : 0.0f;
}
int WomanIntim::GetHoleExperience(ErogenousZone zone) const {
    auto* dev = GetHoleDev(zone);
    return dev ? dev->experience : 0;
}
float WomanIntim::GetHoleMaxDepthZone(ErogenousZone zone) const {
    auto* dev = GetHoleDev(zone);
    return dev ? dev->max_depth_achieved : 0.0f;
}
float WomanIntim::GetHoleMaxDiameter(ErogenousZone zone) const {
    auto* dev = GetHoleDev(zone);
    return dev ? dev->max_diameter_achieved : 0.0f;
}
bool WomanIntim::IsHoleFirstTime(ErogenousZone zone) const {
    auto* dev = GetHoleDev(zone);
    return dev ? dev->is_first_time : true;
}
std::string WomanIntim::GetHoleDevelopmentStatus(ErogenousZone zone) const {
    float adapt = GetHoleAdaptation(zone);
    bool first = IsHoleFirstTime(zone);
    if (first) return "[!!!] ПЕРВЫЙ РАЗ! (будет больно)";
    if (adapt < 20.0f) return "[MIN] Начальный уровень (ещё больно)";
    if (adapt < 40.0f) return "[LOW] Новичок (уже терпимо)";
    if (adapt < 60.0f) return "[MID] Средний уровень (приятно)";
    if (adapt < 80.0f) return "[HI]  Опытная (очень приятно)";
    return "[MAX] Мастер (полное наслаждение)";
}
float WomanIntim::GetHolePainModifier(ErogenousZone zone) const {
    return std::max(0.2f, 1.0f - GetHoleAdaptation(zone) * 0.008f);
}
float WomanIntim::GetHolePleasureModifier(ErogenousZone zone) const {
    return 0.5f + GetHoleAdaptation(zone) * 0.015f;
}
std::vector<ErogenousZone> WomanIntim::GetPenetrableZones() const {
    std::vector<ErogenousZone> result;
    for (const auto& [zone, params] : zones) {
        if (params.is_penetrable) result.push_back(zone);
    }
    return result;
}

// === ДЕАДАПТАЦИЯ ===
void WomanIntim::DecayAdaptation(float factor) {
    factor = std::clamp(factor, 0.9f, 1.0f);
    vaginal_dev.adaptation *= factor;
    anal_dev.adaptation *= factor;
    oral_dev.adaptation *= factor;
    urethral_dev.adaptation *= factor;
}
void WomanIntim::AdvanceHoleRest(int days) {
    vaginal_dev.days_since_last_use += days;
    anal_dev.days_since_last_use += days;
    oral_dev.days_since_last_use += days;
    urethral_dev.days_since_last_use += days;
}