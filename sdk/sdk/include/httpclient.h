#ifndef __HTTPCLIENT_H__
#define __HTTPCLIENT_H__

#include <curl/curl.h>

#ifdef  __cplusplus                                                                                                                                                    
extern "C" {
#endif

typedef enum httpoption{
    OPTION_NONE,
    TIMEOUT,
    CONNECT_TIMEOUT
}HttpOption;

typedef enum httpmethod{
    NONE,
    GET,
    POST,
    PUT,
	DEL,
	HEAD
}HttpMethod;

typedef struct httpheader{
    struct curl_slist *headers;
}HttpHeader;

HttpHeader *httpHeader_create();
void httpHeader_set_option(HttpHeader *header, char *option);

#define PATH_MAX_SIZE 1024
#define URL_MAX_SIZE 512

typedef struct request{

    CURL *curl_handle;
    HttpHeader *httpHeader;
    HttpMethod method;
    char *body;
    char path[PATH_MAX_SIZE];
    char url[URL_MAX_SIZE];
    char host[64];
    char port[16];
    char endpoint[80];

}Request;

int request_init();
Request *request_create(const char *host, const char *port);
Request *request_make(Request *request, HttpHeader *httpHeader,HttpMethod httpMethod, char *url, char *body);
void request_set_header(Request *request, HttpHeader *httpHeader);
void request_set_endpoint(const char *host, const char *port);
void request_set_url(Request *request, char *url);
void request_set_body(Request *request, char *body);
void request_set_method(Request *request, HttpMethod method);
void request_set_option(Request *request, HttpOption option, int value);
void request_send(Request *request, void *userData);
void request_destory(Request *request);

typedef struct response{
	long statusCode;
	char *body;
}Response;

typedef struct httpclient{
    Request *request;
    Response *response;

}HttpClient;

int httpclient_init();
HttpClient *httpclient_create(const char *host, const char *port);
int httpclient_set_header(HttpClient* httpClient, HttpHeader *httpHeader);
int httpclient_set_method(HttpClient* httpClient, HttpMethod httpMethod);
int httpclient_set_url(HttpClient* httpClient,const char *index,const char *doctype, char *keyWord);
int httpclient_set_url_direct(HttpClient* httpClient,char *url);
int httpclient_set_body(HttpClient* httpClient, char *jsonBody);
int httpclient_set_request(HttpClient* httpClient, Request *request);
Response *httpclient_send_request(HttpClient *httpClient);
void httpclient_close(HttpClient *client);

char* indices_get(HttpClient *httpClient, Request *request, char *index);
char* indices_get_mappings(HttpClient *httpClient, Request *request, char *index);
char* indices_get_settings(HttpClient *httpClient, Request *request, char *index);
char *indices_get_doctype_mappings(HttpClient *httpClient, Request *request, char *index, char *doctype);
char *indices_get_field_mappings(HttpClient *httpClient, Request *request, char *index, char *doctype, char *field);

char *search(HttpClient *httpClient, Request *request, char *index, char *doctype, char* json_body);

char *make_path(char *field);
char *join_path(char *pre_field, char *pos_field);
char *join_multipath(char *args, ...);

#ifdef  __cplusplus
}
#endif

#endif
