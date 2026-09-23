#pragma once
#include <random>
#include <string>

namespace utl {
    //генератор случайных чисел
    int RandomInt(int min, int max);
    double RandomDouble(double min, double max);
    double Probability();
    //строки
    std::string ToLower(const std::string& str);
    std::string ToUpper(const std::string& str);
    //дата и время
    std::string CurrentDate(int& days); //делает строку вида гг/мм/дд с момента начала игры, а именно 2026/01/01
}