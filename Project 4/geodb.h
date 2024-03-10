#ifndef geodb_h
#define geodb_h

#include <string>
#include <vector>
#include "geopoint.h"
#include "base_classes.h"
#include "HashMap.h"
#include "geopoint.h"

class GeoDatabase: public GeoDatabaseBase
{
public:
    GeoDatabase();
    virtual ~GeoDatabase();
    virtual bool load(const std::string& map_data_file);
    virtual bool get_poi_location(const std::string& poi, GeoPoint& point) const;
    virtual std::vector<GeoPoint> get_connected_points(const GeoPoint& pt) const;
    virtual std::string get_street_name(const GeoPoint& pt1, const GeoPoint& pt2) const;

private:
    struct StreetSegment{
        GeoPoint startPoint;
        GeoPoint endPoint;
        std::vector<std::pair<std::string, GeoPoint>> pointsOfInterest;
        //constructor
        StreetSegment(GeoPoint startPoint, GeoPoint endPoint, std::vector<std::pair<std::string, GeoPoint>> pointsOfInterest) : startPoint(startPoint), endPoint(endPoint), pointsOfInterest(pointsOfInterest) {}
    };
    HashMap<StreetSegment> database;
    HashMap<GeoPoint> poiLocations; //map each point of interest name to its geopoint location
    HashMap<std::vector<GeoPoint>> connectedPointsMap; //keys are a string representation of a geopoint, and key is a vector or all the geopoints that are connected to that geopoint
    HashMap<std::string> streetNameLookup; //keys are the coordinates of two GeoPoints concatenated into a single string and value is the name of the street
};

#endif /* geodb_h */
