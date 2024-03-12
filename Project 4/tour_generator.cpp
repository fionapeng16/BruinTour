#include "tour_generator.h"
#include "tourcmd.h"
#include "geotools.h"

TourGenerator::TourGenerator(const GeoDatabaseBase& geodb, const RouterBase& router) : m_geodb(geodb), m_router(router) {
}

TourGenerator::~TourGenerator(){
    
}

std::vector<TourCommand> TourGenerator::generate_tour(const Stops& stops) const{
    std::vector<TourCommand> instructions;
    
    GeoPoint prevPOIGeoPoint;
    
    //iterate through each point of interest
    for (int i = 0; i < stops.size(); i++){
        std::string poi;
        std::string talking_points;
        stops.get_poi_data(i, poi, talking_points);
        
        if (i != 0){
            GeoPoint currGeoPoint;
            m_geodb.get_poi_location(poi, currGeoPoint);
            //use router to generate a route betwen the current and next point of interest
            std::vector<GeoPoint> route = m_router.route(prevPOIGeoPoint, currGeoPoint);
            
            for (int j = 0; j < route.size()-1; j++){
                GeoPoint p1 = route[j];
                GeoPoint p2 = route[j+1];
                
                //calculate the direction based on angle betwen p1 and p2
                double angle = angle_of_line(p1, p2);
                std::string direction;
                if (angle >= 337.5 || angle < 22.5) {
                    direction = "east";
                } else if (angle >= 22.5 && angle < 67.5) {
                    direction = "northeast";
                } else if (angle >= 67.5 && angle < 112.5) {
                    direction = "north";
                } else if (angle >= 112.5 && angle < 157.5) {
                    direction = "northwest";
                } else if (angle >= 157.5 && angle < 202.5) {
                    direction = "west";
                } else if (angle >= 202.5 && angle < 247.5) {
                    direction = "southwest";
                } else if (angle >= 247.5 && angle < 292.5) {
                    direction = "south";
                } else {
                    direction = "southeast";
                }
                
                //distance between p1 and p2
                double distance = distance_earth_miles(p1, p2);
                
                //proceed tourcommand
                TourCommand proceedCmd;
                proceedCmd.init_proceed(direction, m_geodb.get_street_name(p1, p2), distance, p1, p2);
                instructions.push_back(proceedCmd);
                
                if (j < route.size() - 2){ //check if we need a turn
                    GeoPoint p3 = route[j+2];
                    double angle = angle_of_turn(p1, p2, p3);
                    std::string turnDir = (angle >= 180) ? "right" : "left";
                    
                    if (angle != 0 && m_geodb.get_street_name(p1, p2) != m_geodb.get_street_name(p2, p3)){
                        TourCommand turnCmd;
                        turnCmd.init_turn(direction, m_geodb.get_street_name(p2, p3));
                        instructions.push_back(turnCmd);
                    }
                }
            }
        }
        
        //generate commentary command for the current point of interest if there's another point of interest following it
        TourCommand commentaryCmd;
        commentaryCmd.init_commentary(poi, talking_points);
        instructions.push_back(commentaryCmd);
        
        if (i != stops.size()-1){ //there is another point of interest following it
            m_geodb.get_poi_location(poi, prevPOIGeoPoint);
        }
    }
    return instructions;
}

