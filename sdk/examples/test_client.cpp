// test_libcurl.cpp : 定义控制台应用程序的入口点。
//

#include "client.h"


#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <json/json.h>
#include <map>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

void test_search(){

	global_init();
	Response *response;
	Client client = Client();

	client.conn("localhost", "9200");
	//client.search("testbinguan", "binguan_Point").filterByField("ID", "66").send();

	//response = client.search("testbinguan", "binguan_Point").filterByGeoDistance("118.8736, 31.32106","1km").send();

	//response = client.getFields("testbinguan", "binguan_Point");
	//response = client.getIndex("testbinguan");
	//response = client.createIndex("testworld",3,1);
	//response = client.deleteIndex("testworld");

	//map<string, string> testMap;
	//testMap.insert(make_pair("name", "string"));
	//testMap.insert(make_pair("the_geom", "geo_shape"));
	//testMap.insert(make_pair("age", "long"));
	//response = client.createFields("testworld", "tweet", testMap);

	//Json::Value testValue;
	//Json::Value field;
	//Json::Value fields;
	//field["type"] = "string";
	//field["store"] = true;
	//fields["name"] = field;
	//field.clear();
	//field["type"] = "geo_shape";
	//field["store"] = true;
	//fields["the_geom"] = field;
	//field.clear();
	//field["type"] = "long";
	//fields["age"] = field;

	//response = client.createFields("testworld", "tweet", fields);

	//response = client.deleteFields("testworld", "tweet");

	//client.indexExists("testpoi1");

	//map<string, string> testMap;

	//testMap.insert(make_pair("key1", "string"));
	//testMap.insert(make_pair("the_geom", "geo_shape"));
	//testMap.insert(make_pair("key3", "long"));
	//client.createFields("testworld", "tweet", testMap);

	//client.deleteFields("testworld", "tweet");

	//map<string, string> testMap;
	//testMap.insert(make_pair("name", "testname"));
	//testMap.insert(make_pair("age", "10"));
	//response = client.createDoc("testworld", "tweet", "1", testMap);

	//Json::Value doc;
	//doc["name"] = "hello";
	//doc["age"]  = 12;
	//response = client.createDoc("testworld", "tweet", "2", doc);

	/*response = client.deleteDoc("testworld", "tweet", "2");*/

	//response = client.search("testworld", "tweet").filterByField("age", "10").range(0,1).send();

	//response = client.search("testworld", "tweet").sort("age", DESC).send();
	/*vector<string> points;
	points.push_back("110,30");
	points.push_back("110,34");
	points.push_back("120,34");
	points.push_back("120,30");

	response = client.search("testbinguan", "binguan_Point").filterByGeoPolygon(points).send();*/
	/*vector<string> points;
	points.push_back("107.72497343887437, 35.09650063396617");
	response = client.search("testzb", "zb_MultiPolygon").filterByGeoShape("Point", points).send();*/

	vector<string> points;
	points.push_back("107.72497343887437, 35.09650063396617");
	points.push_back("107.90956985623001, 33.271047173449176");
	response = client.search("testzb", "zb_MultiPolygon").filterByGeoShape("envelope", points).send();

	printf("code = %d\n", response->statusCode);
	printf("body = %s\n", response->body);

	client.close();
	global_deinit();
}


int main(int argc, char* argv[])
{

	test_search();

	system("pause");

    return 0;
}





