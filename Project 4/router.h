#ifndef router_h
#define router_h

#include <vector>
#include "base_classes.h"

class GeoPoint;
class GeoDatabaseBase;

class Router: public RouterBase
{
public:
    Router(const GeoDatabaseBase& geo_db);
    virtual ~Router();
    virtual std::vector<GeoPoint> route(const GeoPoint& pt1, const GeoPoint& pt2) const;
private:
    
};


#endif /* router_h */
