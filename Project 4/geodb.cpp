#include <iostream>
#include <fstream>
#include <sstream>  // needed in addition to <iostream> for string stream I/O
using namespace std;
#include "geodb.h"
#include "geotools.h"

GeoDatabase::GeoDatabase() {
//    connectedPointsMap = std::unordered_map<GeoPoint, std::vector<GeoPoint>>();
}

GeoDatabase::~GeoDatabase(){
    
}

/*mapdata:
 10th Helena Drive
 34.0547000 -118.4794734 34.0544590 -118.4801137
 0
 11th Helena Drive
 34.0549825 -118.4795629 34.0547778 -118.4802585
 0
 12th Helena Drive
 34.0552554 -118.4796392 34.0550808 -118.4803801
 0
 */
bool GeoDatabase::load(const std::string& map_data_file){
    cout << "Loading map data..." << endl;
    ifstream infile(map_data_file);    // infile is a name of our choosing
    if ( ! infile )                // Did opening the file fail?
    {
        cerr << "Error: Cannot open " << map_data_file << endl;
        return false;
    }
    
    string streetName;
    while (getline(infile, streetName)){
        string start_lat;
        string start_long;
        string end_lat;
        string end_long;
        infile >> start_lat >> start_long >> end_lat >> end_long;
        int numPOI;
        infile >> numPOI;
        infile.ignore(10000, '\n');
        
        vector<pair<string, GeoPoint>> pointsOfInterest;
        for (int i = 0; i < numPOI; i++){
            string poiLine; //gets the entire poi line with the name, latitude, and longitude
            string poiName;
            getline(infile, poiLine);
            istringstream poi_stream(poiLine);
            getline(poi_stream, poiName, '|'); //reads characters from the stream until it encounters the '|' delimiter, and stores the extracted characters in poiName
            
            string latitude;
            string longitude;
            poi_stream >> latitude >> longitude;
            pointsOfInterest.push_back({poiName, GeoPoint(latitude, longitude)});
            poiLocations.insert(poiName, GeoPoint(latitude, longitude));
            cout << "POI name: " << poiName << ", Latitude: " << latitude << ", Longitude: " << longitude << endl;
        }
        
        GeoPoint startPoint(start_lat, start_long);
        GeoPoint endPoint(end_lat, end_long);
        StreetSegment segment(startPoint, endPoint, pointsOfInterest);
        connectedPointsMap[startPoint.to_string()].push_back(endPoint);
        connectedPointsMap[endPoint.to_string()].push_back(startPoint);
        streetNameLookup[startPoint.to_string() + endPoint.to_string()] = streetName;
        streetNameLookup[endPoint.to_string() + startPoint.to_string()] = streetName;
        
        database.insert(streetName, segment);
        
        //connect the points of interest
        if (numPOI > 0){
            GeoPoint mid = midpoint(startPoint, endPoint);
            database.insert(streetName, StreetSegment(startPoint, mid, {}));
            database.insert(streetName, StreetSegment(mid, endPoint, {}));
            connectedPointsMap[startPoint.to_string()].push_back(mid);
            connectedPointsMap[mid.to_string()].push_back(startPoint);
            connectedPointsMap[mid.to_string()].push_back(endPoint);
            connectedPointsMap[endPoint.to_string()].push_back(mid);
            
            streetNameLookup[startPoint.to_string() + mid.to_string()] = streetName;
            streetNameLookup[mid.to_string() + startPoint.to_string()] = streetName;
            streetNameLookup[mid.to_string() + endPoint.to_string()] = streetName;
            streetNameLookup[endPoint.to_string() + mid.to_string()] = streetName;
            
            for (const auto& poi : pointsOfInterest) {
                database.insert("a path", StreetSegment(poi.second, mid, {}));
                connectedPointsMap[poi.second.to_string()].push_back(mid);
                connectedPointsMap[mid.to_string()].push_back(poi.second);
                
                streetNameLookup[poi.second.to_string() + mid.to_string()] = "a path";
                streetNameLookup[mid.to_string() + poi.second.to_string()] = "a path";
            }
        }
    }
    cout << "Map data loaded successfully" << endl;
    return true;
}

bool GeoDatabase::get_poi_location(const std::string& poi, GeoPoint& point) const {
    cout << "Searching for POI: " << poi << endl;
    const GeoPoint* poiPtr = poiLocations.find(poi);
    if (poiPtr == nullptr) {
        cout << "POI not found." << endl;
        return false;
    }
    point = *poiPtr; // Dereference the pointer to assign the GeoPoint object
    return true;
}

std::vector<GeoPoint> GeoDatabase::get_connected_points(const GeoPoint& pt) const{
    const std::vector<GeoPoint>* ptr = connectedPointsMap.find(pt.to_string());
    if (ptr != nullptr) return *ptr;
    return {};
}

std::string GeoDatabase::get_street_name(const GeoPoint& pt1, const GeoPoint& pt2) const{
    const string* combo1 = streetNameLookup.find(pt1.to_string() + pt2.to_string());
    const string* combo2 = streetNameLookup.find(pt2.to_string() + pt1.to_string());
    if (*combo1 != "") return *combo1;
    return *combo2;
}
