/*
 * File: Tester.cpp
 * Project: Assignment02
 * File Created: Tuesday, 13th October 2020 8:46:54 pm
 * Author: Shane Arcaro (sma237@njit.edu)
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include "Analyzer.cpp"

std::vector<std::string> ReadCommandLine();

int main(int argc, char** argv) {
    LexItem token;
    std::ifstream file;
    file.open(argv[1]);
    int lineNumber = 1;

    if (file.is_open()) {
        while (true) {
            token = getNextToken(file, lineNumber);
            std::cout << token << std::endl;

            if (token == DONE || token == ERR)
                break;
        }
    }
}

