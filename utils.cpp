#include "utils.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace utl {
    // Генераторы случайных чисел
    int RandomInt(int min, int max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(min, max);
        return dis(gen);
    }

    double RandomDouble(double min, double max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(min, max);
        return dis(gen);
    }

    double Probability() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        return dis(gen);
    }

    // Строковые функции
    std::string ToLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    std::string ToUpper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return std::toupper(c); });
        return result;
    }

    // Дата и время
    std::string CurrentDate(int& days) {
        // Базовое время: 2026-01-01 00:00:00
        std::tm base = {};
        base.tm_year = 2026 - 1900; // 2026
        base.tm_mon = 0;            // Январь
        base.tm_mday = 1;
        base.tm_hour = 0;
        base.tm_min = 0;
        base.tm_sec = 0;
        
        std::time_t base_time = std::mktime(&base);
        
        // Текущее время
        std::time_t now = std::time(nullptr);
        
        // Разница в секундах
        double diff_seconds = std::difftime(now, base_time);
        
        // Количество дней (с округлением вниз)
        days = static_cast<int>(diff_seconds / (60 * 60 * 24));
        
        // Если дни отрицательные (до 2026-01-01), возвращаем базовую дату
        if (days < 0) {
            days = 0;
            return "2026/01/01";
        }
        
        // Вычисляем текущую дату
        std::time_t current_time = base_time + (days * 24 * 60 * 60);
        std::tm* current_tm = std::localtime(&current_time);
        
        // Форматируем как гг/мм/дд
        std::ostringstream oss;
        oss << std::setfill('0')
            << (current_tm->tm_year + 1900) << "/"
            << std::setw(2) << (current_tm->tm_mon + 1) << "/"
            << std::setw(2) << current_tm->tm_mday;
        
        return oss.str();
    }
}