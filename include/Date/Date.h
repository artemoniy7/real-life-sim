#ifndef DATE_H
#define DATE_H
#include<string>

struct Date {
    int year;
    int month;
    int day;
    
    std::string toString() const;
    bool isLeapYear() const;
    int daysInMonth() const;
    void nextDay();
};

#endif