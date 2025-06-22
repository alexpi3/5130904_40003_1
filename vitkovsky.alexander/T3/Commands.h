#ifndef COMMANDS_H
#define COMMANDS_H

#include "BaseStructs.h"
#include "Utils.h"

#include <vector>
#include <numeric>
#include <iomanip>
#include <cctype>

namespace commands {
    struct area {
        std::string parameter;
        double operator()(const std::vector<Polygon>& polygons) const {
            if (parameter == "MEAN") {
                if (polygons.empty()) {
                    throw std::invalid_argument("<INVALID COMMAND>");
                }
                double total = 0.0;
                for (const auto& poly : polygons) {
                    total += calculatePolygonArea(poly);
                }
                return total / polygons.size();
            }

            auto pred = [this](const Polygon& poly) {
                if (parameter == "EVEN") return poly.points_.size() % 2 == 0;
                if (parameter == "ODD") return poly.points_.size() % 2 == 1;
                try {
                    size_t num = std::stoul(parameter);
                    if (num < 3) throw std::invalid_argument("<INVALID COMMAND>");
                    return poly.points_.size() == num;
                } catch (...) {
                    throw std::invalid_argument("<INVALID COMMAND>");
                }
            };

            if (parameter == "EVEN" || parameter == "ODD") {
                double totalArea = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                    [&pred](double sum, const Polygon& poly) {
                        return pred(poly) ? sum + calculatePolygonArea(poly) : sum;
                    });
                return totalArea;
            }

            double totalArea = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                [&pred](double sum, const Polygon& poly) {
                    return pred(poly) ? sum + calculatePolygonArea(poly) : sum;
                });

            return totalArea;
        }
    };

    struct count {
        std::string parameter;
        int operator()(const std::vector<Polygon>& polygons) const {

            auto pred = [this](const Polygon& poly) {
                if (parameter == "EVEN") return poly.points_.size() % 2 == 0;
                else if (parameter == "ODD") return poly.points_.size() % 2 == 1;
                else {
                    try {
                        size_t num = std::stoul(parameter);
                        if (num < 3) {
                            throw std::invalid_argument("<INVALID COMMAND>");
                        }
                        return poly.points_.size() == num;
                    } catch (...) {
                        throw std::invalid_argument("<INVALID COMMAND>");
                    }
                }
            };

            return std::count_if(polygons.begin(), polygons.end(), pred);
        }
    };

    struct rightShapes {
        int operator()(const std::vector<Polygon>& polygons) const {
            return std::count_if(polygons.begin(), polygons.end(), isWithRightAngle);
        }
    };

    struct echo {
        Polygon poly;
        int operator()(std::vector<Polygon>& polygons) const {
            int counter = 0;
            auto curr = polygons.begin();

            while (curr != polygons.end()) {
                if (*curr == poly) {
                    curr = polygons.insert(curr + 1, poly);
                    counter++;
                    curr++;
                } else {
                    curr++;
                }
            }

            return counter;
        }
    };

    struct max_min {
        std::string subtype;
        bool isMax;

        double operator()(const std::vector<Polygon>& polygons) const {
            auto comparator = [this](const Polygon& p1, const Polygon& p2) {
                double val1 = (subtype == "AREA") ? calculatePolygonArea(p1) : p1.points_.size();
                double val2 = (subtype == "AREA") ? calculatePolygonArea(p2) : p2.points_.size();
                return val1 < val2;
            };

            if (polygons.empty()) {
                throw std::invalid_argument("Error: no polygons for MAX/MIN");
            }

            auto extreme = isMax
                ? std::max_element(polygons.begin(), polygons.end(), comparator)
                : std::min_element(polygons.begin(), polygons.end(), comparator);

            return (subtype == "AREA")
                ? calculatePolygonArea(*extreme)
                : extreme->points_.size();
        }
    };
}

void commandHandler(std::vector<Polygon>& polygons, const Command& cmd) {
    try {
        switch (cmd.type) {
            case CommandType::AREA: {
                if (!cmd.has_subtype && !cmd.has_nov) {
                    throw std::invalid_argument("Error: no necessary arguments for AREA");
                }
                if (cmd.has_nov && cmd.numOfVertexes < 3) {
                    throw std::invalid_argument("Error: incorrect number of vertexes");
                }

                std::string parameter = cmd.has_subtype
                    ? cmd.subtype
                    : std::to_string(cmd.numOfVertexes);

                double result = commands::area{parameter}(polygons);
                std::cout << std::fixed << std::setprecision(1) << result << std::endl;
                break;
            }

            case CommandType::COUNT: {
                if (!cmd.has_subtype && !cmd.has_nov) {
                    throw std::invalid_argument("Error: no necessary arguments for COUNT");
                }
                if (cmd.has_nov && cmd.numOfVertexes < 3) {
                    throw std::invalid_argument("Error: incorrect number of vertexes");
                }

                std::string parameter = cmd.has_subtype
                    ? cmd.subtype
                    : std::to_string(cmd.numOfVertexes);

                int result = commands::count{parameter}(polygons);
                std::cout << result << std::endl;
                break;
            }

            case CommandType::ECHO: {
                if (!cmd.has_polygon) {
                    throw std::invalid_argument("Error: no necessary arguments for ECHO");
                }

                if (cmd.poly.points_.size() < 3) {
                    throw std::invalid_argument("<INVALID COMMAND>");
                }

                int result = commands::echo{cmd.poly}(polygons);
                std::cout << result << std::endl;
                break;
            }

            case CommandType::RIGHTSHAPES: {
                int result = commands::rightShapes{}(polygons);
                std::cout << result << std::endl;
                break;
            }

            case CommandType::MAX:
            case CommandType::MIN: {
                if (!cmd.has_subtype) {
                    throw std::invalid_argument("Error: no necessary arguments for MIN/MAX");
                }

                bool isMax = (cmd.type == CommandType::MAX);
                double result = commands::max_min{cmd.subtype, isMax}(polygons);
                if (cmd.subtype == "AREA") {
                    std::cout << std::fixed << std::setprecision(1) << result << std::endl;
                } else {
                    std::cout << static_cast<int>(result) << std::endl;
                }
                break;
            }

            default:
                throw std::invalid_argument("<INVALID COMMAND>");

        }
    } catch (const std::invalid_argument&) {
        throw std::invalid_argument("<INVALID COMMAND>");
    }
}

#endif // COMMANDS_H
