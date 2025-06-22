#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <string>
#include <sstream>
#include <functional>
#include <unordered_map>

class GeometryConstants {
private:
    GeometryConstants() = delete;
public:
    static constexpr int MIN_POINTS_FOR_AREA = 3;
    static constexpr int POINT_TOKEN_MIN_LENGTH = 5;
    static constexpr int INVALID_COMMAND = -1;
    static constexpr int EVEN = -2;
    static constexpr int ODD = -3;
    static constexpr int MEAN = -4;
    static constexpr int AREA = -5;
    static constexpr int VERTEXES = -6;

    static const std::string INVALID_COMMAND_MSG;
};

struct Point
{
    int x = 0;
    int y = 0;

    bool operator==(const Point& other) const;
    bool operator<(const Point& other) const;
};

struct Polygon
{
    std::vector<Point> points;

    bool operator==(const Polygon& other) const;
};

int getIntParam(const std::string& option);
double calculateArea(const Polygon& polygon);
bool isNumber(const std::string& str);
bool parsePoint(const std::string& token, Point& point);
bool parseToken(std::istringstream& iss, std::vector<Point>& points, size_t count, bool isFirst);
bool parsePolygon(std::istringstream& iss, Polygon& polygon);

void handleArea(const std::vector<Polygon>& polygons, std::istringstream& iss);
void handleMaxMin(const std::vector<Polygon>& polygons, const std::string& command, std::istringstream& iss);
void handleCount(const std::vector<Polygon>& polygons, std::istringstream& iss);
void handlePerms(const std::vector<Polygon>& polygons, std::istringstream& iss);
void handleMaxSeq(const std::vector<Polygon>& polygons, std::istringstream& iss);

void processFileInput(std::vector<Polygon>& polygons,
    std::unordered_map<std::string, std::function<void(const std::vector<Polygon>&, std::istringstream&)>> handlers,
    std::unordered_map<std::string, std::function<void(const std::vector<Polygon>&, const std::string&, std::istringstream&)>> maxMinHandlers,
    std::string& line);

#endif
