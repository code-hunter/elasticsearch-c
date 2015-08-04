#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <iostream>
#include <string>
#include "httpjsonbody.h"
#include "httpclient.h"

using namespace std;

#ifdef SDK_CLIENT_DLL
	class _declspec(dllexport) Client 
#else
	class _declspec(dllimport) Client
#endif
{

public:
	HttpHeader *httpHeader;
	HttpJsonBody httpJsonBody;
	HttpClient *httpClient;
	void *response;

public:
	Client();

	int conn(string host, string port);
	Client search(string index, string doctype);
	Client range(int from, int size);
	Client sort(string field, int order);
	Client filterByField(string field, string value);
	Client filterByGeoBox(string topLeft, string bottomRight);
	Client filterByGeoDistance(string point, string distance);
	Client filterByGeoDistanceRange(string point, string from, string to);
	Client filterByGeoPolygon(vector<string> points);
	Client filterByGeoShape(string type, vector<string> points);
	Response *send();

	Response *getFields(string index, string doctype);
	Response *createFields(string index, string doctype, map<string, string> fields);
	Response *createFields(string index, string doctype, Json::Value fields);
	Response *deleteFields(string index, string doctype);
	
	Response *getIndex(string index);
	Response* Client::createIndex(string index, int shards, int replicas);
	Response* deleteIndex(string index);

	Response *createDoc(string index, string doctype, map<string, string> doc);
	Response *createDoc(string index, string doctype, Json::Value doc);
	Response *updateDoc(string index, string doctype, string id, map<string, string>doc);
	Response *updateDoc(string index, string doctype, string id, Json::Value doc);
	Response *deleteDoc(string index, string doctype, string id);

	long getResponseCode(Response *response);
	char *getResponseBody(Response *response);
	
};

#endif