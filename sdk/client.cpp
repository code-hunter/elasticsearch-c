#define SDK_CLIENT_API

#include "client.h"

#ifdef SDK_CLIENT_API
    extern "C" __declspec( dllexport)
#else
    extern "C" __declspec( dllimport)
#endif
void global_init(){
                httpclient_init();
}

#ifdef SDK_CLIENT_API
    extern "C" __declspec( dllexport)
#else
    extern "C" __declspec( dllimport)
#endif
void global_deinit(){
                httpclient_deinit();
}


Client::Client(){}

int Client::conn(string host, string port){
	//create httpHeader
	httpHeader = httpHeader_create();
	if(httpHeader == NULL){
		return -1;
	}
    httpHeader_set_option(httpHeader, "Content-Type: application/json");

	//create httpClient
    httpClient = httpclient_create(host.c_str(), port.c_str());
	if(httpClient == NULL){
		return -1;
	}

	return 0;
}

Client Client::search(string index, string doctype){
	
	httpclient_set_method(httpClient, POST);
	string url = "/" + index + "/" + doctype + "/" + "_search";
	httpclient_set_url_direct(httpClient, (char *)url.c_str());
	httpJsonBody.initQueryJson();

	return (*this);
}

Client Client::range(int from, int size){
	httpJsonBody.setFrom(from);
	httpJsonBody.setSize(size);

	return (*this);
}

Client Client::sort(string field, int order){
	httpJsonBody.setSort(field, order);

	return (*this);
}

Client Client::filterByField(string field, string value){
	httpJsonBody.setFilterField(field, value);
	return (*this);
}

Client Client::filterByGeoBox(string topLeft, string bottomRight){
	httpJsonBody.setFilterGeoBox(topLeft, bottomRight);
	return (*this);
}

Client Client::filterByGeoDistance(string point, string distance){
	httpJsonBody.setFilterGeoDistance(point, distance);
	return (*this);
}

Client Client::filterByGeoDistanceRange(string point, string from, string to){
	httpJsonBody.setFilterGeoDisRange(point, from, to);
	return (*this);
}

Client Client::filterByGeoPolygon(vector<string> points){
	httpJsonBody.setFilterGeoPolygon(points);
	return (*this);
}

Client Client::filterByGeoShape(string type, vector<string> points){
	httpJsonBody.setFilterGeoShape(type, points);
	return (*this);
}

Response *Client::getFields(string index, string doctype){
	HttpMethod method = GET;
	string url = "/" + index + "/" + "_mapping" + "/" + doctype;
	string body;
	
	return send_request(method, url, body);
}

Response *Client::createFields(string index, string doctype, map<string, string>fields){
	HttpMethod method = PUT;
	string url = "/" + index + "/" + "_mapping" + "/" + doctype;
	string body = httpJsonBody.createMapping(doctype, fields);
	
	return send_request(method, url, body);
}

Response *Client::createFields(string index, string doctype, Json::Value fields){
	HttpMethod method = PUT;
	string url = "/" + index + "/" + "_mapping" + "/" + doctype;
	string body = httpJsonBody.createMapping(doctype, fields);
	
	return send_request(method, url, body);
}

Response *Client::deleteFields(string index, string doctype){
	HttpMethod method = DEL;
	string url = "/" + index + "/" + "_mapping" + "/" + doctype;
	string body;
	
	return send_request(method, url, body);
}

Response *Client::getIndex(string index){
	HttpMethod method = GET;
	string url = "/" + index;
	string body;
	
	return send_request(method, url, body);
}

	
Response* Client::createIndex(string index, int shards, int replicas){	
	HttpMethod method = PUT;
	string url = "/" + index;
	string body = httpJsonBody.createIndex(index, shards, replicas);
	
	return send_request(method, url, body);
}

Response* Client::deleteIndex(string index){

	HttpMethod method = DEL;
	string url = "/" + index;
	string body;
	
	return send_request(method, url, body);
}

Response *Client::createDoc(string index, string doctype, string id, map<string, string> doc){

	HttpMethod method = POST;
	string url = "/" + index + "/" + doctype + "/" + id;
	string body = httpJsonBody.createDoc(doc);
	
	return send_request(method, url, body);
}

Response *Client::createDoc(string index, string doctype, map<string, string> doc){

	HttpMethod method = POST;
	string url = "/" + index + "/" + doctype;
	string body = httpJsonBody.createDoc(doc);
	
	return send_request(method, url, body);
}

Response *Client::createDoc(string index, string doctype, string id, Json::Value doc){
	HttpMethod method = POST;
	string url = "/" + index + "/" + doctype + "/" + id;
	string body = httpJsonBody.getJsonString(doc);
	
	return send_request(method, url, body);
}

Response *Client::createDoc(string index, string doctype, Json::Value doc){
	HttpMethod method = POST;
	string url = "/" + index + "/" + doctype;
	string body = httpJsonBody.getJsonString(doc);
	
	return send_request(method, url, body);
}
Response *Client::updateDoc(string index, string doctype, string id, map<string, string>doc){
	HttpMethod method = PUT;
	string url = "/" + index + "/" + doctype + "/" +id;
	string body = httpJsonBody.createDoc(doc);
	
	return send_request(method, url, body);
}
Response *Client::updateDoc(string index, string doctype, string id, Json::Value doc){
	HttpMethod method = PUT;
	string url = "/" + index + "/" + doctype + "/" +id;
	string body = httpJsonBody.getJsonString(doc);
	
	return send_request(method, url, body);
}
Response *Client::deleteDoc(string index, string doctype, string id){
	HttpMethod method = DEL;
	string url = "/" + index + "/" + doctype + "/" +id;
	string body ;
	
	return send_request(method, url, body);
}


Response* Client::send(){
	string jsonBody = httpJsonBody.getQueryJsonString();
	cout << httpJsonBody.getJsonObject(jsonBody);
	httpclient_set_body(httpClient, (char *)jsonBody.c_str());

	Response *response = httpclient_send_request(httpClient);
	return response;
}

Response* Client::send_request(HttpMethod method, string url, string body){

	httpclient_set_method(httpClient, method);
	httpclient_set_url_direct(httpClient, (char *)url.c_str());

	if(!body.empty()){
		httpclient_set_body(httpClient,(char*)body.c_str());
	}

	Response *response = httpclient_send_request(httpClient);
	return response;

}

long Client::getResponseCode(Response *response){
	if(response == NULL){
		return -1;
	}

	return response->statusCode;
}

char *Client::getResponseBody(Response *response){
	if(response == NULL){
		return NULL;
	}

	return response->body;
}

void Client::close(){
	if(httpHeader != NULL){
		httpHeader_close(httpHeader);
	}

	if(httpClient != NULL){
		httpclient_close(httpClient);
	}
}

