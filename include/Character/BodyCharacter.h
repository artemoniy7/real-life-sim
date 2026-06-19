#ifndef BODY_CHARACTER_H
#define BODY_CHARACTER_H
#include<string>
#include<vector>
#include<map>
#include<functional>
#include"../Date/Date.h"
enum class Gender{
    Male,
    Female
};
enum class BodyPart{
    Head,
    Torso,
    LeftArm,
    RightArm,
    LeftHand,
    RightHand,
    LeftLeg,
    RightLeg,
    LeftFeet,
    RightFeet,
    Genitals,
    Chest,
    Buttocks
};
enum class InjuryType{
    None,
    Bruise,
    Cut,
    Laceration,
    Fracture,
    Burn,
    Bite
};
struct Injury{
    InjuryType type;
    float severity; //0.0-1.0
    float bleedingRate; //мл в минуту
    bool isBandaged;
    float healingProgress;
    int timeSinceInjury; //минуты
    //конструктор (раны пока нет)
    Injury(): type(InjuryType::None),severity(0.0f), bleedingRate(0.0f),
    isBandaged(false), healingProgress(0.0f),timeSinceInjury(0){}
};
struct BodyPartState{
    BodyPart part;
    float health; //0.0-1.0 (Мертва - Превосходна)
    float maxHealth;
    float painLevel; //0.0-1.0
    float staminaCost; //множитель для траты стамины, используя эту часть
    bool isDisabled;
    Injury currentInjury;
    float temperature; //температура локально
    //конструтор идеальной конечности
    BodyPartState(): health(1.0f), maxHealth(1.0f), painLevel(0.0f),
    staminaCost(1.0f), isDisabled(false), temperature(36.6){}
};
struct PhysiologicalStats{
    float hydration; //0.0-1.0
    float nutrition; //0.0-1.0
    float bladder; //0.0-1.0
    float bowel; //0.0-1.0
    float fatigue; //0.0-1.0
    float stress;//0.0-1.0
    float boredom;
    float pain; 
    float bloodVolume; //норма ～5000мл
    float bodyTemperature; //36.6 норма
    float hygiene;
    float arousal;
    //Конструтор
    PhysiologicalStats(): hydration(0.8f), nutrition(0.7f), bladder(0.2f),
    bowel(0.2f), fatigue(0.3f), stress(0.0f), boredom(0.0f), pain(0.0f), bloodVolume(5000.0f),
    bodyTemperature(36.6f), hygiene(0.7f), arousal(0.0f){}
};
struct MovenmentData{
    float walkSpeed;
    float runSpeed;
    float crouchSpeed;
    float staminaDrainMultiplier;
    float painPenalty;
    bool isLimping;
    bool canSprint;
    MovenmentData(): walkSpeed(5.0f), runSpeed(10.0f), crouchSpeed(2.5f),
    staminaDrainMultiplier(1.0f), painPenalty(1.0f), isLimping(false),
    canSprint(true){}
};
class BodyCharacter{
    //Базовая Информация
    std::string name;
    std::string surname;
    int age;
    Date date;
    Gender gender;
    float height;
    float weight;
    float fitnessLevel;
    float strengthLevel;
    std::map<BodyPart, BodyPartState> bodyParts;
    PhysiologicalStats physioStats;
    MovenmentData movement;
    //Женские штучки
    bool isMenstruating;
    int menstrualCycleDay;
    float pregnencyProbability; //0.0-1.0
    //----------------
    
};  

#endif 