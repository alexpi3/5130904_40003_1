#ifndef POINT_H
#define POINT_H
/*
Основные структуры и шаблоны для парсинга
*/
#include <vector>
#include <algorithm>
#include <regex>

struct Point {
    int x, y;

    // для команды ECHO (дублирует всякое вхождение указанной в параметре фигуры)
    bool operator!=(const Point& other) const {
        return (this->x != other.x || this->y != other.y);
    }
};

struct Polygon {
    std::vector<Point> points_;

    /* При сравнении многоугольников считаю, что порядок вершин важен, т.е.
    "3 (0;0) (0;1) (1;1)" и "3 (1;1) (0;0) (0;1)" - разные фигуры */
    bool operator==(const Polygon& other) const {
        if (this->points_.size() != other.points_.size()) return false;

        for (size_t i = 0; i < this->points_.size(); i++) {
            if (this->points_[i] != other.points_[i]) return false;
        }
        return true;
    }
};


enum class CommandType {
    AREA, MAX, MIN, COUNT, ECHO, RIGHTSHAPES,
    SAME, RECTS
};

struct Command {
    CommandType type;

    std::string subtype;
    bool has_subtype = false;

    Polygon poly;
    bool has_polygon = false;

    int numOfVertexes = 0;
    bool has_nov = false;
};


namespace patterns {
    const std::string pointFormat = "\\((-?\\d+);(-?\\d+)\\)";
    const std::string polygonFormat = "\\d+(\\s" + pointFormat + ")+";

    std::regex pointPattern(pointFormat);
    std::regex polygonLinePattern(polygonFormat);


/*
Мне достался 4 варинат, там 4 типа команд по формату вызова:
1) <type> (RIGHTSHAPES)
2) <type> <subtype> (AREA EVEN, COUNT ODD и др)
3) <type> <num-of-vertexes> (AREA *число*, COUNT *число*)
4) <type> <polygon> (ECHO *polygonFormat*)
*/
    const std::string cmdFormat_1 = "(\\w+)";
    const std::string cmdFormat_2 = cmdFormat_1 + "\\s(\\d+)";
    const std::string cmdFormat_3 = cmdFormat_1 + "\\s" + cmdFormat_1;
    const std::string cmdFormat_4 = cmdFormat_1 + "\\s(" + polygonFormat + ")";

    std::regex commandLinePattern_1(cmdFormat_1);
    std::regex commandLinePattern_2(cmdFormat_2);
    std::regex commandLinePattern_3(cmdFormat_3);
    std::regex commandLinePattern_4(cmdFormat_4);
}
#endif // POINT_H
