//
// Created by Taco on 6/17/2024.
//

#ifndef CONSOLE_H
#define CONSOLE_H

#include <string>
#include <vector>

class Console {
public:
    Console();

    ~Console();

    void addEntry(const std::string input);

    const std::vector<std::string> GetConsole();

private:
    std::vector<std::string> m_console;
};


#endif //CONSOLE_H
