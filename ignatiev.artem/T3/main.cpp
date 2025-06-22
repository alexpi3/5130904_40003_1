#include <iostream>
#include <fstream>
#include <iomanip>
#include "geometry.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Ошибка: не указано имя файла\n";
        return 1;
    }

    std::ifstream input(argv[1]);
    if (!input.is_open())
    {
        std::cerr << "Ошибка: не удалось открыть файл " << argv[1] << "\n";
        return 1;
    }

    std::vector<Polygon> polygons;
    std::string line;
    while (std::getline(input, line))
    {
        std::istringstream iss(line);
        Polygon polygon;
        if (parsePolygon(iss, polygon))
        {
            polygons.push_back(std::move(polygon));
        }
    }

    std::unordered_map<std::string, std::function<void(const std::vector<Polygon>&, std::istringstream&)>> handlers =
    {
        {"AREA", handleArea},
        {"COUNT", handleCount},
        {"PERMS", handlePerms},
        {"MAXSEQ", handleMaxSeq}
    };

    std::unordered_map<std::string, std::function<void(const std::vector<Polygon>&, const std::string&, std::istringstream&)>> maxMinHandlers =
    {
        {"MAX", handleMaxMin},
        {"MIN", handleMaxMin}
    };

    std::cout << std::fixed << std::setprecision(1);

    processFileInput(polygons, handlers, maxMinHandlers, line);

    return 0;
}
