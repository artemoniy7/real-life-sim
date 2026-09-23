#pragma once
#include "WomanIntim.h"
#include <string>
#include <vector>

struct SexEvent {
    int day;
    std::string partner_name;
    std::vector<ErogenousZone> zones_used;
    float max_pleasure;
    float max_pain;
    bool orgasm;
    bool first_time_zone;
};

class Woman {
private:
    int DaysCount;
    int BirthDay;
    std::string name;
    int money;
    bool alive;

    WomanIntim intim;

    int total_orgasms = 0;
    int total_partners = 0;
    int days_since_last_sex = 0;
    int pleasure_events = 0;              // НОВОЕ: для среднего удовольствия
    float average_pleasure = 0.0f;
    float average_pain = 0.0f;
    float max_pleasure_ever = 0.0f;
    float max_pain_ever = 0.0f;

    std::vector<SexEvent> history;

public:
    Woman(const std::string& name, int DaysCount);
    Woman(const std::string& name, int DaysCount, int BirthDay);

    // === БАЗОВОЕ ===
    void Show() const;
    void ShowStats() const;
    void ShowHistory() const;
    int GetAge() const;
    int GetDaysCount() const { return DaysCount; }
    int GetBirthDay() const { return BirthDay; }
    std::string GetName() const { return name; }
    bool IsAlive() const { return alive; }

    // === ВРЕМЯ ===
    void AdvanceTime(int days);
    void AdvanceTime(int days, bool include_rest);

    // === ИНТИМ ===
    void AddOrgasm() { total_orgasms++; }
    void AddPartner() { total_partners++; }
    void UpdatePleasure(float pleasure);
    void UpdatePain(float pain);
    void UpdateDays(int days_passed);

    WomanIntim& GetIntim() { return intim; }
    const WomanIntim& GetIntim() const { return intim; }

    // === СТАТИСТИКА ===
    int GetTotalOrgasms() const { return total_orgasms; }
    int GetTotalPartners() const { return total_partners; }
    float GetAveragePleasure() const { return average_pleasure; }
    float GetAveragePain() const { return average_pain; }
    float GetMaxPleasureEver() const { return max_pleasure_ever; }
    float GetMaxPainEver() const { return max_pain_ever; }
    int GetDaysSinceLastSex() const { return days_since_last_sex; }

    // === ВЗАИМОДЕЙСТВИЕ ===
    float StimulateZone(ErogenousZone zone, float intensity, float depth = 0.0f);
    float StimulateZone(ErogenousZone zone, float intensity, float depth, float diameter);
    void ShowZonesInfo() const;

    // === СОБЫТИЯ ===
    void LogEvent(const SexEvent& event);
    const std::vector<SexEvent>& GetHistory() const { return history; }
    void ClearHistory() { history.clear(); }

    // === СЕССИЯ ===
    void StartSession();
    void EndSession(bool orgasm);
};