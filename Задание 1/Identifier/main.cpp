#include "Identifier.h"

#include <iostream>
#include <string>

using namespace std::literals;

void TestIdentifierClass() {
    //test default constructor
    Identifier identifier1;
    assert(identifier1.GetCurrentID() == "A1"s);

    //test constructor with parameter
    Identifier identifier2("Z9");
    assert(identifier2.GetCurrentID() == "Z9"s);

    //test creation from rvalue
    Identifier identifier3 = "B2"s;
    assert(identifier3.GetCurrentID() == "B2"s);

    //test SetCurrentID
    Identifier identifier4;
    assert(identifier1.GetCurrentID() == "A1"s);
    identifier4.SetCurrentID("A1-A1"s);
    assert(identifier4.GetCurrentID() == "A1-A1"s);
    identifier4 = "B2"s;
    assert(identifier4.GetCurrentID() == "B2"s);

    //test wrong format id set/create
    Identifier identifier5;
    try {
        identifier5 = "D1"s;
        assert(false);
    }
    catch (const IdentifireInvalid& e) {
        std::cout << "Test 1 wrong format id set throw: "s << e.what() << std::endl;
        assert(true);
    }
    try {
        identifier5 = "D"s;
        assert(false);
    }
    catch (const IdentifireInvalid& e) {
        std::cout << "Test 2 wrong format id set throw: "s << e.what() << std::endl;
        assert(true);
    }
    try {
        identifier5 = "A"s;
        assert(false);
    }
    catch (const IdentifireInvalid& e) {
        std::cout << "Test 3 wrong format id set throw: "s << e.what() << std::endl;
        assert(true);
    }
    try {
        identifier5 = "A1-B"s;
        assert(false);
    }
    catch (const IdentifireInvalid& e) {
        std::cout << "Test 4 wrong format id set throw: "s << e.what() << std::endl;
        assert(true);
    }
    try {
        identifier5 = "A1-"s;
        assert(false);
    }
    catch (const IdentifireInvalid& e) {
        std::cout << "Test 5 wrong format id set throw: "s << e.what() << std::endl;
        assert(true);
    }
    try {
        identifier5 = "A1-A1-A1-A1-A1-A1-A1-A1-A1-A1-A1"s;
        assert(false);
    }
    catch (const IdentifireInvalid& e) {
        std::cout << "Test 6 wrong id length set throw: "s << e.what() << std::endl;
        assert(true);
    }
    try {
        identifier5 = "A0"s;
        assert(false);
    }
    catch (const IdentifireInvalid& e) {
        std::cout << "Test 7 wrong format id set throw: "s << e.what() << std::endl;
        assert(true);
    }

    //test simple increase ++ and IncreaseID()
    Identifier identifier6;
    ++identifier6;
    identifier6.IncreaseID();
    assert(identifier6.GetCurrentID() == "A3"s);

    //test increase carry from digit to alpha
    Identifier identifier7 = "A9"s;
    ++identifier7;
    assert(identifier7.GetCurrentID() == "B1"s);

    //test increase carry from group digit & alpha to another group
    Identifier identifier8 = "Z9"s;
    ++identifier8;
    assert(identifier8.GetCurrentID() == "A1-A1"s);
    identifier8 = "Z9-Z9"s;
    ++identifier8;
    assert(identifier8.GetCurrentID() == "A1-A1-A1"s);

    //test overflow increase& 10 groups already
    Identifier identifier9 = "Z9-Z9-Z9-Z9-Z9-Z9-Z9-Z9-Z9-Z9"s;
    try {
        ++identifier9;
        assert(false);
    }
    catch (const IdentifireOverFlow& e) {
        std::cout << "Test increase id overflow: "s << e.what() << std::endl;
        assert(true);
    }

    //test increase in middle group
    Identifier identifier10 = "A9-Z9-Z9"s;
    ++identifier10;
    assert(identifier10.GetCurrentID() == "B1-A1-A1"s);
}

int main() {
    TestIdentifierClass();
    std::cout << "Tests passed"s << std::endl;
    return 0;
}