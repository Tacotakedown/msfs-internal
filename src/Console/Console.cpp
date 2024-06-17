//
// Created by Taco on 6/17/2024.
//

#include "Console.h"


Console::Console() {
    std::string starter = "Welcome To Thuggy, The shitty MSFS Devs are not safe from this";
    m_console.push_back(starter);
}

Console::~Console() {
}


void Console::addEntry(const std::string input) {
    this->m_console.push_back(input);
}

const std::vector<std::string> Console::GetConsole() {
    return this->m_console;
}


