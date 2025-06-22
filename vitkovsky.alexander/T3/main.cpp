#include <iostream>
#include <fstream>
#include <string>

#include "BaseStructs.h"
#include "Utils.h"
#include "Commands.h"


int main(int argc, char* argv[]) {
    if (argc != 2) {
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        return 2;
    }

    std::vector<Polygon> polygons;
    std::string fileLine;
    while (std::getline(inputFile, fileLine)) {
        try {
            Polygon poly = parsePolygonLine(fileLine);
            polygons.push_back(poly);
        } catch (const std::invalid_argument& ex) {
            continue;
        }
    }
    inputFile.close();


    std::string line;
    bool wasInput = false;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        wasInput = true;
        try {
            Command cmd = parseCommandLine(line);
            commandHandler(polygons, cmd);
        } catch (const std::invalid_argument& ex) {
            std::cout << ex.what() << std::endl;
        }
    }
    if (!wasInput) {
        std::cout << "Atleast 2 optional supported commands" << std::endl;
    }

    return 0;
}
