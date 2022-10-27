#include "Identifier.h"

using namespace std::literals;

Identifier::Identifier(const std::string &str) {
    SetCurrentID(str);
}

std::string Identifier::SetCurrentID(const std::string &str) {
    std::string tmp;
    if (!CheckId(str)) {
        throw IdentifireInvalid("Invalid identifier. Check format"s);
    }
    //
    std::copy_if(str.crbegin(), str.crend(), back_inserter(tmp), [](const char &c) { return c != '-'; });
    //thread safe access to id_ field
    //minimize critical section
    std::lock_guard<std::mutex> lock(m_);
    //
    std::swap(tmp, id_);

    return GetFormatedID();
}

std::string Identifier::GetCurrentID() const {
    //thread safe access to id_ field
    std::lock_guard<std::mutex> lock(m_);
    //
    return GetFormatedID();
}

bool Identifier::CheckId(const std::string &str) const {
    static const std::regex id_regex(R"/(^[A-CEHIKLNOPR-UW-Z][1-9](-[A-CEHIKLNOPR-UW-Z][1-9]){0,9}$)/");
    std::smatch m;

    if (!std::regex_match(str, m, id_regex)) {
        return false;
    }
    return true;
}

Identifier::Identifier() : id_("1A"s) {}

Identifier &Identifier::operator=(const std::string &&rhs) {
    SetCurrentID(rhs);
    return *this;
}

bool Identifier::IsAlphaValid(const char letter) const {
    return 'A' <= letter &&
           'Z' >= letter &&
           'D' != letter &&
           'F' != letter &&
           'G' != letter &&
           'J' != letter &&
           'M' != letter &&
           'Q' != letter &&
           'V' != letter;
}

std::string Identifier::IncreaseID() {
    //thread safe access to id_ field
    std::lock_guard<std::mutex> lock(m_);
    //
    std::string tmp = id_;
    auto it = tmp.begin();
    int carry = 1;
    while (it != tmp.end() && carry > 0) {
        if (std::isdigit(*it)) {
            if (*it == '9') {
                carry = 1;
                *it = '1';
            } else {
                ++*it;
                carry = 0;
            }
        } else if (std::isalpha(*it)) {
            if (*it == 'Z') {
                carry = 1;
                *it = 'A';
            } else {
                ++*it;
                carry = 0;
                while (!IsAlphaValid(*it)) {
                    ++*it;
                }
            }
        }
        std::advance(it, 1);
    }
    //
    if (carry) {
        if (tmp.size() < MAX_GROUP_COUNT_*2) {
            tmp += "1A"s;
        } else {
            throw IdentifireOverFlow("Increase identifier overflow");
        }
    }
    //
    std::swap(tmp, id_);

    return GetFormatedID();
}

Identifier &Identifier::operator++() {
    IncreaseID();
    return *this;
}

Identifier &Identifier::operator++(int) {
    Identifier &temp = *this;
    ++*this;
    return temp;
}

std::string Identifier::GetFormatedID() const {
    //dont need mutex lock because method call only in already locked section
    //
    std::string res;
    bool first = true;
    for (int i = id_.size() - 1; i > 0; i = i - 2) {
        if (first) {
            res.push_back(id_[i]);
            res.push_back(id_[i - 1]);
            first = false;
            continue;
        }
        res.push_back(GROUP_SEPARATOR_);
        res.push_back(id_[i]);
        res.push_back(id_[i - 1]);
    }
    return res;
}

std::ostream &operator<<(std::ostream &out, const Identifier &value_to_output) {
    out << value_to_output.GetCurrentID();
    return out;
}