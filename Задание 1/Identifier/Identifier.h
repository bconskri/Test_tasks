#ifndef IDENTIFIER_IDENTIFIER_H
#define IDENTIFIER_IDENTIFIER_H

#include <string>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <regex>
#include <mutex>

class IdentifireInvalid : public std::invalid_argument {
public:
    using std::invalid_argument::invalid_argument;
};

class IdentifireOverFlow : public std::overflow_error {
public:
    using std::overflow_error::overflow_error;
};

class Identifier {
public:
    Identifier();
    Identifier(const std::string&);
    ~Identifier() = default;

    std::string SetCurrentID(const std::string&);
    std::string GetCurrentID() const;
    std::string IncreaseID();

    //переопределим операторы для класса, чтобы упростить синтаксис при работе с идентификаторами
    Identifier &operator=(const std::string&& rhs);
    Identifier &operator++();
    Identifier &operator++(int);

 private:
    // максимальное количество групп в идентификаторе
    static const size_t MAX_GROUP_COUNT_ = 10;

    // разделитель разрядов
    static const char GROUP_SEPARATOR_ = '-';

    bool CheckId(const std::string&) const;
    bool IsAlphaValid(const char) const;
    std::string GetFormatedID() const;

    std::string id_;
    mutable std::mutex m_;
 };

std::ostream &operator<<(std::ostream &, const Identifier &);
#endif //IDENTIFIER_IDENTIFIER_H