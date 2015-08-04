#ifndef __HTTPJSONBODY__
#define __HTTPJSONBODY__

#include <iostream>
#include <map>
#include <string>
#include <json/json.h>

using namespace std;

typedef enum{
	ASC=1,
	DESC=-1
};

class HttpJsonBody{

private:
	Json::Value queryJson;

	Json::Value mustValue;
	Json::Value boolValue;
	Json::Value queryInValue;
	Json::Value filteredValue;

	Json::Value filterValue;
	Json::Value fromValue;
	Json::Value sizeValue;
	Json::Value sortValue;

	Json::Value indexValue;

public:

	Json::Value getQueryJson(){
		return queryJson;
	}

	string getQueryJsonString(){
		Json::FastWriter writer;

		return writer.write(queryJson);
	}


	void initQueryJson(){
		mustValue["must"].resize(0);
		boolValue["bool"] = mustValue;
		queryInValue["query"] = boolValue;
		queryInValue["filter"] = Json::Value(Json::objectValue);
		filteredValue["filtered"] = queryInValue;	
		queryJson["query"] = filteredValue;
		
		queryJson["from"] = 0;
		queryJson["size"] = 10;
		queryJson["sort"].resize(0);		
	}

	
	void setMust(Json::Value must){
		queryJson["query"]["filtered"]["query"]["bool"]["must"].append(must);
	}

	void addMustField(string key, string value){
		Json::Value tmpField;
		Json::Value tmpTerm;

		tmpField[key] = value;
		tmpTerm["term"] = tmpField;

		setMust(tmpTerm);
	}

	void setSize(int size){
		queryJson["size"] = size;
	}

	void setFrom(int from){
		queryJson["from"] = from;
	}

	void setSort(string field, int order){
		Json::Value tmpValue;
		tmpValue[field] = (order == ASC)?"asc":"desc";

		queryJson["sort"].append(tmpValue);
	}

	void setFilterField(string key, string value){
		addMustField(key, value);
	}

	void setFilterGeoBox(string topLeft, string bottomRight){
		Json::Value tmpBox;
		Json::Value tmpGeom;
		Json::Value tmpCood;

		tmpCood["top_left"] = topLeft;
		tmpCood["bottom_right"] = bottomRight;

		//fixed me the_geom is right for geotools
		tmpGeom["the_geom"] = tmpCood;
		tmpBox["geo_bounding_box"] = tmpGeom;

		queryJson["query"]["filtered"]["filter"].clear();
		queryJson["query"]["filtered"]["filter"] = tmpBox;
	}

	void setFilterGeoDistance(string point, string distance){		 
		Json::Value tmpCood;
		Json::Value tmpDis;

		//fixed me the_geom is right for geotools
		tmpCood["the_geom"] = point;
		tmpCood["distance"] = distance;

		tmpDis["geo_distance"] = tmpCood;

		queryJson["query"]["filtered"]["filter"].clear();
		queryJson["query"]["filtered"]["filter"] = tmpDis;

	}

	void setFilterGeoDisRange(string point, string from, string to){		 
		Json::Value tmpCood;
		Json::Value tmpDis;

		//fixed me the_geom is right for geotools
		tmpCood["the_geom"] = point;
		tmpCood["from"] = from;
		tmpCood["to"] = to;

		tmpDis["geo_distance_range"] = tmpCood;

		queryJson["query"]["filtered"]["filter"].clear();
		queryJson["query"]["filtered"]["filter"] = tmpDis;
	}

	void setFilterGeoPolygon(vector<string> points){
		Json::Value tmpCood;
		Json::Value tmpGeom;
		Json::Value tmpPoly;

		tmpCood["points"].resize(0);

		if(points.size() <= 0){
			printf("points must not be null\n");
			return;
		}

		vector<string>::iterator it;
		
		for(it = points.begin(); it!=points.end(); it++){
			tmpCood["points"].append(*it);
		}

		tmpGeom["the_geom"] = tmpCood;
		tmpPoly["geo_polygon"] = tmpGeom;

		queryJson["query"]["filtered"]["filter"].clear();
		queryJson["query"]["filtered"]["filter"] = tmpPoly;

	}

	string& trim(string &in){
		if(in.empty()){
			return in;
		}

		in.erase(0, in.find_first_not_of(" "));
		in.erase(in.find_last_not_of(" ") + 1);

		return in;
	}

	void split(string &in, string &delim, vector<string> *result){
		int last = 0;
		int index = in.find_first_of(delim, 0);
		while(index != string::npos){
			result->push_back(trim(in.substr(last, index-last)));
			last = index + 1;
			index = in.find_first_of(delim, last);
		}
		result->push_back(trim(in.substr(last)));
	}

	void setFilterGeoShape(string type, vector<string> points){
		Json::Value tmpShp;
		Json::Value tmpCood;
		
		Json::Value tmpGeom;
		Json::Value tmpGeo;
		Json::Value point = Json::Value(Json::arrayValue);

		
		int size = points.size();
		if(size <= 0){
			printf("points must not be null\n");
			return;
		}

		vector<string>::iterator it;
		tmpCood["coordinates"].resize(0);
		
		vector<string> splits;
		vector<string>::iterator iter;
		string delim = ",";
		
		for(it = points.begin(); it!=points.end(); it++){
			point.clear();
			splits.clear();
			if(size == 1){
				split(*it, delim, &splits);
				for(iter = splits.begin(); iter!=splits.end(); iter++){
					tmpCood["coordinates"].append(atof(((string)*iter).c_str()));
				}			
			}else{
				split(*it, delim, &splits);
				for(iter = splits.begin(); iter!=splits.end(); iter++){
					point.append(atof(((string)*iter).c_str()));
				}
				tmpCood["coordinates"].append(point);
			}
			
		}

		tmpCood["type"] = type;

		tmpShp["shape"] = tmpCood;
		tmpGeom["the_geom"] = tmpShp;
		tmpGeo["geo_shape"] = tmpGeom;

		queryJson["query"]["filtered"]["filter"].clear();
		queryJson["query"]["filtered"]["filter"] = tmpGeo;
	}

	void dumpQueryJson(){
		Json::FastWriter writer;

		cout<< writer.write(queryJson) <<endl;
	}

	Json::Value getJsonObject(string jsonStr){
		Json::Reader reader;
		Json::Value result;

		reader.parse(jsonStr, result);

		return result;
	}

	string getJsonString(Json::Value jsonObj){
		Json::FastWriter writer;

		return writer.write(jsonObj);
	}

	Json::Value getHitsJson(string jsonStr){
		Json::Reader reader;
		Json::Value result;

		reader.parse(jsonStr, result);

		return result["hits"];
	}

	string getHitsString(string jsonStr){
		Json::Reader reader;
		Json::FastWriter writer;
		Json::Value result;

		reader.parse(jsonStr, result);
		return writer.write(result["hits"]);
	}

	string createIndex(string index, int shards, int replicas){
		Json::Value setValue;
		setValue["number_of_shards"] = shards;
		setValue["number_of_replicas"] = replicas;

		indexValue.clear();
		indexValue["settings"] = setValue;
		
		Json::FastWriter writer;
		return writer.write(indexValue);
	}

	string createMapping(string doctype, map<string, string>fields){
		if( fields.size() <= 0){
			return NULL;
		}

		Json::Value typeValue;
		Json::Value fieldValue;
		Json::Value propValue;
		Json::Value doctypeValue;

		map<string, string>::iterator it =  fields.begin();

		while(it != fields.end()){ 
			typeValue.clear();

			typeValue["type"] = it->second;
			if(!strcmp("the_geom", it->first.c_str())){
				typeValue["store"] = true;
			}
			fieldValue[it->first] = typeValue;
			it++;
		}

		if(!fieldValue.isNull()){
			propValue["properties"] = fieldValue;
			doctypeValue[doctype] = propValue;
		}

		Json::FastWriter writer;
		return writer.write(doctypeValue);

	}

	string createMapping(string doctype, Json::Value fieldsValue){
		Json::Value propValue;
		Json::Value doctypeValue;		

		if(!fieldsValue.isNull()){
			propValue["properties"] = fieldsValue;
			doctypeValue[doctype] = propValue;
		}

		Json::FastWriter writer;
		return writer.write(doctypeValue);

	}

	string createDoc(map<string, string>doc){
		if( doc.size() <= 0){
			return NULL;
		}

		Json::Value docValue;

		map<string, string>::iterator it =  doc.begin();

		while(it != doc.end()){ 

			docValue[it->first] = it->second;
			it++;
		}

		Json::FastWriter writer;
		return writer.write(docValue);

	}

};

#endif