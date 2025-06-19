#pragma once

#include <cstdint>
#include <vector>
#include <string>

// Basic point structure for PCB coordinates (in mils/thou)
struct BRDPoint {
    int x = 0;
    int y = 0;

    BRDPoint() = default;
    BRDPoint(int x, int y) : x(x), y(y) {}

    bool operator==(const BRDPoint& point) const {
        return x == point.x && y == point.y;
    }
    bool operator!=(const BRDPoint& point) const {
        return x != point.x || y != point.y;
    }
};

// PCB part mounting sides
enum class BRDPartMountingSide { Both, Bottom, Top };
enum class BRDPartType { SMD, ThroughHole };

// PCB Part structure
struct BRDPart {
    std::string name;
    std::string mfgcode;
    BRDPartMountingSide mounting_side = BRDPartMountingSide::Top;
    BRDPartType part_type = BRDPartType::SMD;
    unsigned int end_of_pins = 0;
    BRDPoint p1{0, 0};
    BRDPoint p2{0, 0};
};

// Pin sides
enum class BRDPinSide { Both, Bottom, Top };

// PCB Pin structure
struct BRDPin {
    BRDPoint pos;
    int probe = 0;
    unsigned int part = 0;
    BRDPinSide side = BRDPinSide::Top;
    std::string net = "UNCONNECTED";
    double radius = 0.5f;
    std::string snum;
    std::string name;
    std::string comment;

    bool operator<(const BRDPin &p) const {
        return part == p.part ? (snum < p.snum) : (part < p.part);
    }
};

// PCB Nail structure
struct BRDNail {
    unsigned int probe = 0;
    BRDPoint pos;
    BRDPartMountingSide side = BRDPartMountingSide::Top;
    std::string net = "UNCONNECTED";
};
