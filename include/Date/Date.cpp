#include<string>
#include"Date.h"

std::string Date::toString() const {
    return std::to_string(year) + '/' +
           std::to_string(month) + '/' +
           std::to_string(day);
}

bool Date::isLeapYear() const {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int Date::daysInMonth() const {
    switch(month) {
        case 2: return isLeapYear() ? 29 : 28;
        case 4: case 6: case 9: case 11: return 30;
        default: return 31;
    }
}

void Date::nextDay() {
    int maxDay = daysInMonth();
    if (day < maxDay) {
        day++;
    } else {
        day = 1;
        if (month < 12) {
            month++;
        } else {
            month = 1;
            year++;
        }
    }
}