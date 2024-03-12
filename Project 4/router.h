#ifndef router_h
#define router_h

#include <vector>
#include "base_classes.h"


class GeoPoint;

class Router: public RouterBase
{
public:
    Router(const GeoDatabaseBase& geo_db);
    virtual ~Router();
    virtual std::vector<GeoPoint> route(const GeoPoint& pt1, const GeoPoint& pt2) const;
private:
    const GeoDatabaseBase& m_geo_db;
    double calculatePriority(const GeoPoint& point, const GeoPoint& destination) const;
};


#endif /* router_h */
