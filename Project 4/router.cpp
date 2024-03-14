#include "router.h"
#include <queue>
#include <stack>
#include "base_classes.h"
#include "HashMap.h"
#include <cmath>
#include <unordered_map>

Router::Router(const GeoDatabaseBase& geo_db) : m_geo_db(geo_db){
    //construct router object
}

Router::~Router(){
    
}

//struct ComparePairs {
//    bool operator()(const std::pair<double, GeoPoint>& p1, const std::pair<double, GeoPoint>& p2) const {
//        return p1.first > p2.first; // Compare based on the first element of the pair (priority)
//    }
//};
//
//std::vector<GeoPoint> Router::route(const GeoPoint& pt1, const GeoPoint& pt2) const{
//    
//    std::vector<GeoPoint> path;
//    path.push_back(pt1);
//    if (pt1.latitude == pt2.latitude && pt1.longitude == pt2.longitude){
//        path.push_back(pt1);
//        return path;
//    }
//    
//    std::priority_queue<std::pair<double, GeoPoint>, std::vector<std::pair<double, GeoPoint>>, ComparePairs> pq; //creates a min heap priority queue, and double represents the calculated priority (we want it to be based on SMALLEST priority)
//    HashMap<GeoPoint> locationOfPreviousWayPoint; //associates a given GeoPoint G (string representations) to the previous GeoPoint P in the route
//    
//    pq.push({calculatePriority(pt1, pt2), pt1}); //push the starting point with its priority
//    //drop breadcrumb
//    
//    while (!pq.empty()){
//        GeoPoint curr = pq.top().second;
//        pq.pop();
//        
//        if (curr.latitude == pt2.latitude && curr.longitude == pt2.longitude){
//            std::stack<GeoPoint> stack;
//            stack.push(curr);
//            
//            GeoPoint* add = locationOfPreviousWayPoint.find(curr.to_string());
// 
//            while (add != nullptr && (add->latitude != pt1.latitude && add->longitude != pt1.longitude)){ //ERROR: add never became nullptr
//                stack.push(*add);
//                add = locationOfPreviousWayPoint.find(add->to_string());
//            }
//            while (!stack.empty()){
//                path.push_back(stack.top());
//                stack.pop();
//            }
//            return path;
//
//        }
//         std::vector<GeoPoint> connectedPoints = m_geo_db.get_connected_points(curr);
//        for (int i = 0; i < connectedPoints.size(); i++){
//            if (locationOfPreviousWayPoint.find(connectedPoints[i].to_string()) == nullptr){
//                
//                locationOfPreviousWayPoint.insert(connectedPoints[i].to_string(), curr);
//                pq.push({calculatePriority(connectedPoints[i], pt2), connectedPoints[i]}); //push connected points with their priority
//            }
//        }
//        
//    }
//    return {}; //return empty vector if the path was not found
//}
//
//double Router::calculatePriority(const GeoPoint& point, const GeoPoint& destination) const{
//    //calculate priority based on distance to destination based on euclidian distance
//    double dx = destination.longitude - point.longitude;
//    double dy = destination.latitude - point.latitude;
//    double euclideanDistance = std::sqrt(dx * dx + dy * dy);
//    
//    // Return the calculated Euclidean distance as the priority
//    return euclideanDistance;
//}


//redo with the A* algorithm
double heuristic(const GeoPoint& pt1, const GeoPoint& pt2){
    double dx = pt2.longitude - pt1.longitude;
    double dy = pt2.latitude - pt1.latitude;
    return std::sqrt(dx * dx + dy * dy);
}

struct ComparePairs {
    bool operator()(const std::pair<double, GeoPoint>& p1, const std::pair<double, GeoPoint>& p2) const {
        return p1.first > p2.first; // Compare based on the first element of the pair (priority)
    }
};

std::vector<GeoPoint> Router::route(const GeoPoint& pt1, const GeoPoint& pt2) const{
    
    std::vector<GeoPoint> path;
    path.push_back(pt1);
    if (pt1.latitude == pt2.latitude && pt1.longitude == pt2.longitude){
        path.push_back(pt1);
        return path;
    }
    
    std::priority_queue<std::pair<double, GeoPoint>, std::vector<std::pair<double, GeoPoint>>, ComparePairs> pq; //creates a min heap priority queue, and double represents the calculated priority (we want it to be based on SMALLEST priority)
    HashMap<GeoPoint> locationOfPreviousWayPoint; //associates a given GeoPoint G (string representations) to the previous GeoPoint P in the route
    
    //map to store the cost from the source to each vertex
    HashMap<double> cost;
    cost.insert(pt1.to_string(), 0.0);
    
    pq.push({heuristic(pt1, pt2), pt1}); //push the starting point with its priority
    
    
    while (!pq.empty()){
        GeoPoint curr = pq.top().second;
        pq.pop();
        
        if (curr.latitude == pt2.latitude && curr.longitude == pt2.longitude){
            std::stack<GeoPoint> stack;
            stack.push(curr);
            
            GeoPoint* add = locationOfPreviousWayPoint.find(curr.to_string());
 
            while (add != nullptr && (add->latitude != pt1.latitude && add->longitude != pt1.longitude)){ //ERROR: add never became nullptr
                stack.push(*add);
                add = locationOfPreviousWayPoint.find(add->to_string());
            }
            while (!stack.empty()){
                path.push_back(stack.top());
                stack.pop();
            }
            return path;

        }
         std::vector<GeoPoint> connectedPoints = m_geo_db.get_connected_points(curr);
        for (int i = 0; i < connectedPoints.size(); i++){
            double new_cost = 1.0e9;
            if (cost.find(curr.to_string()) != nullptr){
                new_cost = cost[curr.to_string()] + heuristic(curr, connectedPoints[i]);

            }
            

            if (locationOfPreviousWayPoint.find(connectedPoints[i].to_string()) == nullptr || (cost.find(connectedPoints[i].to_string()) != nullptr && new_cost < cost[connectedPoints[i].to_string()])){
                
                cost[connectedPoints[i].to_string()] = new_cost;
                double priority = new_cost + heuristic(connectedPoints[i], pt2);
                locationOfPreviousWayPoint.insert(connectedPoints[i].to_string(), curr);
                pq.push({priority, connectedPoints[i]}); //push connected points with their priority
            }
        }
        
    }
    return {}; //return empty vector if the path was not found
}

//double Router::calculatePriority(const GeoPoint& point, const GeoPoint& destination) const{
//    //calculate priority based on distance to destination based on euclidian distance
//    double dx = destination.longitude - point.longitude;
//    double dy = destination.latitude - point.latitude;
//    double euclideanDistance = std::sqrt(dx * dx + dy * dy);
//    
//    // Return the calculated Euclidean distance as the priority
//    return euclideanDistance;
//}

