#ifndef UTILS_H
#define UTILS_H
/*
Вспомогательные функции и парсеры
*/
#include "BaseStructs.h"

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <unordered_map>
#include <cmath>


Polygon parsePolygonLine(const std::string& line) {
    if (line.empty()) throw std::invalid_argument("Error: this line is empty");

    if (!std::regex_match(line, patterns::polygonLinePattern)) {
        throw std::invalid_argument("Error: invalid polygon format in " + line);
    }

    std::stringstream ss(line);
    Polygon poly;
    int pointsNumber;

    ss >> pointsNumber;
    if (pointsNumber < 3) {
        throw std::invalid_argument("Error: insufficient number of points in " + line);
    }

    std::string point_str;
    while (ss >> point_str) {
        std::smatch point_matches;
        if (std::regex_match(point_str, point_matches, patterns::pointPattern)) {
            int x = std::stoi(point_matches[1]);
            int y = std::stoi(point_matches[2]);
            Point point { x, y };
            poly.points_.push_back(point);
        } else {
            throw std::invalid_argument("Error: incorrect dot format in " + point_str);
        }
    }

    if (poly.points_.size() == static_cast<size_t>(pointsNumber)) {
        return poly;
    } else {
        throw std::invalid_argument("Error: wrong number of points in " + line);
    }
}


CommandType stringToCommandType(const std::string& str) {
    static const std::unordered_map<std::string, CommandType> converter = {
        {"AREA", CommandType::AREA},
        {"COUNT", CommandType::COUNT},
        {"MAX", CommandType::MAX},
        {"MIN", CommandType::MIN},
        {"ECHO", CommandType::ECHO},
        {"RIGHTSHAPES", CommandType::RIGHTSHAPES},
        {"SAME", CommandType::SAME},
        {"RECTS", CommandType::RECTS}
    };

    std::string upperStr = str;
    std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);
    auto it = converter.find(upperStr);
    if (it == converter.end()) {
        throw std::invalid_argument("Error: " + str + " is unidentified command type");
    }
    return it->second;
}

Command parseCommandLine (const std::string& line) {
    Command cmd;
    std::smatch matches;

    try {
        if (std::regex_match(line, matches, patterns::commandLinePattern_1)) {

            cmd.type = stringToCommandType(matches[1].str());

        } else if (std::regex_match(line, matches, patterns::commandLinePattern_2)) {

            cmd.type = stringToCommandType(matches[1].str());
            cmd.numOfVertexes = std::stoi(matches[2]);
            cmd.has_nov = true;

        } else if (std::regex_match(line, matches, patterns::commandLinePattern_3)) {

            cmd.type = stringToCommandType(matches[1].str());
            cmd.subtype = matches[2].str();
            cmd.has_subtype = true;

        } else if (std::regex_match(line, matches, patterns::commandLinePattern_4)) {

            cmd.type = stringToCommandType(matches[1].str());
            try {
                cmd.poly = parsePolygonLine(matches[2].str());
            } catch (...) {
                throw std::invalid_argument("Error: invalid polygon format");
            }
            cmd.has_polygon = true;

        } else {
            throw std::invalid_argument("Error: invalid command format in " + line);
        }
    } catch (const std::invalid_argument& ex) {
        throw std::invalid_argument("<INVALID COMMAND>");
    }

    return cmd;
}


double calculatePolygonArea(const Polygon& poly) {
    // метод трапеций
    double area = 0.0;
    size_t n = poly.points_.size();
    for (size_t i = 0; i < n; i++) {
        const Point& p1 = poly.points_[i];
        const Point& p2 = poly.points_[(i+1)%n];
        area += (p1.y + p2.y) * (p1.x - p2.x);
    }
    return std::abs(area) / 2.0;
}

bool isWithRightAngle(const Polygon& poly) {
    // Определяем, содержит ли многоугольник прямые углы
    size_t n = poly.points_.size();
    for (size_t i = 0; i < n; ++i) {
        const Point& prev = poly.points_[(i+n-1)%n];
        const Point& curr = poly.points_[i];
        const Point& next = poly.points_[(i+1)%n];

        int dx1 = curr.x - prev.x;
        int dy1 = curr.y - prev.y;
        int dx2 = next.x - curr.x;
        int dy2 = next.y - curr.y;

        if (dx1 * dx2 + dy1 * dy2 == 0) {
            return true;
        }
    }
    return false;
}

#endif // UTILS_H
