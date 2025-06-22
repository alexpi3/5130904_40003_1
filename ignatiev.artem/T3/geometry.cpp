#include "geometry.h"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <tuple>

using namespace std::placeholders;

const std::string GeometryConstants::INVALID_COMMAND_MSG = "<INVALID COMMAND>\n";

bool Point::operator==(const Point& other) const
{
  return x == other.x && y == other.y;
}

bool Point::operator<(const Point& other) const
{
  return std::tie(x, y) < std::tie(other.x, other.y);
}

bool Polygon::operator==(const Polygon& other) const
{
  if (points.size() != other.points.size()) {
    return false;
  }

  std::multiset<Point> thisPoints(points.begin(), points.end());
  std::multiset<Point> otherPoints(other.points.begin(), other.points.end());

  return thisPoints == otherPoints;
}

double calculateArea(const Polygon& polygon)
{
  if (polygon.points.size() < 3) {
    return 0.0;
  }

  std::vector<std::pair<Point, Point>> coordsVec;

  auto getArea = [](double sum, const std::pair<Point, Point>& coords) {
    const Point& i = coords.first;
    const Point& j = coords.second;
    return sum + (i.x * j.y) - (j.x * i.y);
  };

  if (!polygon.points.empty()) {
    std::transform(polygon.points.begin(), std::prev(polygon.points.end()),
                   std::next(polygon.points.begin()), std::back_inserter(coordsVec),
                   [](const Point& point, const Point& otherPoint) {
                     return std::make_pair(point, otherPoint);
                   });

    coordsVec.push_back(std::make_pair(polygon.points.back(), polygon.points.front()));
  }

  return std::abs(std::accumulate(coordsVec.begin(), coordsVec.end(), 0.0, getArea)) / 2.0;
}

bool isNumber(const std::string& str)
{
  return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

bool parsePoint(const std::string& token, Point& point)
{
  if (token.size() < GeometryConstants::POINT_TOKEN_MIN_LENGTH ||
      token.front() != '(' || token.back() != ')') {
    return false;
  }

  std::string inner = token.substr(1, token.size() - 2);
  std::replace(inner.begin(), inner.end(), ';', ' ');
  std::istringstream iss(inner);
  return (iss >> point.x && iss >> point.y);
}

bool parseToken(std::istringstream& iss, std::vector<Point>& points, size_t count, bool isFirst)
{
  if (count < 3 && isFirst) {
    return false;
  }

  if (count < 1) {
    return (iss).eof();
  }

  std::string token;
  Point point;

  if (!(iss >> token) || !parsePoint(token, point)) {
    return false;
  }

  points.push_back(point);
  return parseToken(iss, points, --count, false);
}

bool parsePolygon(std::istringstream& iss, Polygon& polygon)
{
  std::string countStr;
  if (!(iss >> countStr) || !isNumber(countStr)) {
    return false;
  }

  size_t count = static_cast<size_t>(std::stoul(countStr));
  std::vector<Point> points;
  points.reserve(count);

  if (!parseToken(iss, points, count, true)) {
    return false;
  }

  polygon.points = std::move(points);
  return true;
}

int getIntParam(const std::string& option)
{
  if (option == "EVEN") {
    return GeometryConstants::EVEN;
  } else if (option == "ODD") {
    return GeometryConstants::ODD;
  } else if (option == "MEAN") {
    return GeometryConstants::MEAN;
  } else if (option == "AREA") {
    return GeometryConstants::AREA;
  } else if (option == "VERTEXES") {
    return GeometryConstants::VERTEXES;
  } else {
    return GeometryConstants::INVALID_COMMAND;
  }
}

void handleArea(const std::vector<Polygon>& polygons, std::istringstream& iss)
{
  std::string param;
  if (!(iss >> param)) {
    std::cout << GeometryConstants::INVALID_COMMAND_MSG;
    return;
  }

  auto areaAccumulator = [](double sum, const Polygon& poly) {
    return sum + calculateArea(poly);
  };

  if (isNumber(param)) {
    if (std::stoi(param) < 3) {
      std::cout << GeometryConstants::INVALID_COMMAND_MSG;
      return;
    }

    size_t vtxCount = static_cast<size_t>(std::stoul(param));
    double total = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                                   [vtxCount](double sum, const Polygon& poly) {
                                     return poly.points.size() == vtxCount
                                                ? sum + calculateArea(poly)
                                                : sum;
                                   });
    std::cout << std::fixed << std::setprecision(1) << total << '\n';
  } else {
    int option = getIntParam(param);

    switch (option) {
      case GeometryConstants::EVEN: {
        double total = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                                       [](double sum, const Polygon& poly) {
                                         return poly.points.size() % 2 == 0
                                                    ? sum + calculateArea(poly)
                                                    : sum;
                                       });
        std::cout << std::fixed << std::setprecision(1) << total << '\n';
        break;
      }

      case GeometryConstants::ODD: {
        double total = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                                       [](double sum, const Polygon& poly) {
                                         return poly.points.size() % 2 != 0
                                                    ? sum + calculateArea(poly)
                                                    : sum;
                                       });
        std::cout << std::fixed << std::setprecision(1) << total << '\n';
        break;
      }

      case GeometryConstants::MEAN: {
        if (polygons.empty()) {
          std::cout << GeometryConstants::INVALID_COMMAND_MSG;
          return;
        }

        double total = std::accumulate(polygons.begin(), polygons.end(), 0.0, areaAccumulator);
        std::cout << std::fixed << std::setprecision(1) << total / polygons.size() << '\n';
        break;
      }

      default:
        std::cout << GeometryConstants::INVALID_COMMAND_MSG;
    }
  }
}

void handleMaxMin(const std::vector<Polygon>& polygons, const std::string& command,
                  std::istringstream& iss)
{
  if (polygons.size() < 1) {
    std::cout << GeometryConstants::INVALID_COMMAND_MSG;
    return;
  }

  std::string param;
  if (!(iss >> param) || polygons.empty()) {
    std::cout << GeometryConstants::INVALID_COMMAND_MSG;
    return;
  }

  int option = getIntParam(param);

  switch (option) {
    case GeometryConstants::AREA: {
      auto compareAreas = [](const Polygon& a, const Polygon& b) {
        return calculateArea(a) < calculateArea(b);
      };
      auto it = command == "MAX"
                   ? std::max_element(polygons.begin(), polygons.end(), compareAreas)
                   : std::min_element(polygons.begin(), polygons.end(), compareAreas);
      std::cout << std::fixed << std::setprecision(1) << calculateArea(*it) << '\n';
      break;
    }

    case GeometryConstants::VERTEXES: {
      auto compareVtxCount = [](const Polygon& a, const Polygon& b) {
        return a.points.size() < b.points.size();
      };
      auto it = command == "MAX"
                   ? std::max_element(polygons.begin(), polygons.end(), compareVtxCount)
                   : std::min_element(polygons.begin(), polygons.end(), compareVtxCount);
      std::cout << it->points.size() << '\n';
      break;
    }

    default:
      std::cout << GeometryConstants::INVALID_COMMAND_MSG;
  }
}

void handleCount(const std::vector<Polygon>& polygons, std::istringstream& iss)
{
  std::string param;
  if (!(iss >> param)) {
    std::cout << GeometryConstants::INVALID_COMMAND_MSG;
    return;
  }

  if (isNumber(param)) {
    if (std::stoi(param) < 3) {
      std::cout << GeometryConstants::INVALID_COMMAND_MSG;
      return;
    }

    size_t vtxCount = static_cast<size_t>(std::stoul(param));
    size_t count = std::count_if(polygons.begin(), polygons.end(),
                                 [vtxCount](const Polygon& poly) {
                                   return poly.points.size() == vtxCount;
                                 });
    std::cout << count << '\n';
    return;
  }

  int option = getIntParam(param);
  switch (option) {
    case GeometryConstants::EVEN: {
      size_t count = std::count_if(polygons.begin(), polygons.end(),
                                   [](const Polygon& poly) {
                                     return poly.points.size() % 2 == 0;
                                   });
      std::cout << count << '\n';
      break;
    }

    case GeometryConstants::ODD: {
      size_t count = std::count_if(polygons.begin(), polygons.end(),
                                   [](const Polygon& poly) {
                                     return poly.points.size() % 2 != 0;
                                   });
      std::cout << count << '\n';
      break;
    }

    default:
      std::cout << GeometryConstants::INVALID_COMMAND_MSG;
  }
}

void handlePerms(const std::vector<Polygon>& polygons, std::istringstream& iss)
{
  Polygon target;
  if (!parsePolygon(iss, target) || target.points.empty()) {
    std::cout << GeometryConstants::INVALID_COMMAND_MSG;
    return;
  }

  size_t count = std::count_if(polygons.begin(), polygons.end(),
                               [&target](const Polygon& poly) {
                                 if (poly.points.size() != target.points.size()) {
                                   return false;
                                 }

                                 std::multiset<Point> targetPoints(target.points.begin(),
                                                                   target.points.end());
                                 std::multiset<Point> polyPoints(poly.points.begin(),
                                                                 poly.points.end());
                                 return targetPoints == polyPoints;
                               });

  std::cout << count << '\n';
}

void handleMaxSeq(const std::vector<Polygon>& polygons, std::istringstream& iss)
{
  Polygon target;
  if (!parsePolygon(iss, target) || target.points.empty()) {
    std::cout << GeometryConstants::INVALID_COMMAND_MSG;
    return;
  }

  size_t maxSequence = 0;
  size_t currentSequence = 0;

  std::for_each(polygons.begin(), polygons.end(),
                [&target, &maxSequence, &currentSequence](const Polygon& poly) {
                  if (poly == target) {
                    currentSequence++;
                    maxSequence = std::max(maxSequence, currentSequence);
                  } else {
                    currentSequence = 0;
                  }
                });

  std::cout << maxSequence << '\n';
}

void processFileInput(std::vector<Polygon>& polygons,
                      std::unordered_map<std::string,
                                         std::function<void(const std::vector<Polygon>&, std::istringstream&)>> handlers,
                      std::unordered_map<std::string,
                                         std::function<void(const std::vector<Polygon>&,
                                                            const std::string&, std::istringstream&)>> maxMinHandlers,
                      std::string& line)
{
  if (std::getline(std::cin, line)) {
    std::istringstream iss(line);
    std::string command;
    if (!(iss >> command)) {
      std::cout << GeometryConstants::INVALID_COMMAND_MSG;
      return processFileInput(polygons, handlers, maxMinHandlers, line);
    }

    auto it = handlers.find(command);
    if (it != handlers.end()) {
      it->second(polygons, iss);
      return processFileInput(polygons, handlers, maxMinHandlers, line);
    }

    auto maxMinIt = maxMinHandlers.find(command);
    if (maxMinIt != maxMinHandlers.end()) {
      maxMinIt->second(polygons, command, iss);
      return processFileInput(polygons, handlers, maxMinHandlers, line);
    }

    std::cout << GeometryConstants::INVALID_COMMAND_MSG;
    processFileInput(polygons, handlers, maxMinHandlers, line);
  }
}
