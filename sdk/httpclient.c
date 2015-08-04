#include "httpclient.h"
#include <malloc.h>

int httpclient_init(){
    return request_init();
}

Response *response_create(){
	Response *response = (Response *)malloc(sizeof(Response));
	if(response == NULL){
		printf("failed to create response instance\n");
		return NULL;
	}
	response->statusCode = -1;
	response->body = NULL;
}

void response_release(Response *response){
	if(response->body != NULL){
		free(response->body);
		response->body = NULL;
	}
	
	if(response != NULL){
		free(response);
		response = NULL;
	}
}

HttpClient *httpclient_create(char *host, char *port){
	HttpClient *httpClient;
    //request_init();

    httpClient = (HttpClient *)malloc(sizeof(HttpClient));
    if(httpClient == NULL){
        printf("failed to create httpClient instance\n");
        return  NULL;
    }
    httpClient->request = request_create(host, port);;
    httpClient->response = response_create();

    return  httpClient;
}


int httpclient_set_header(HttpClient* httpClient, HttpHeader *httpHeader){
    if(httpClient == NULL || httpHeader == NULL){
        printf("httpHeader and httpClient must not be null\n");
        return -1;
    }

    request_set_header(httpClient->request, httpHeader);
}

int httpclient_set_method(HttpClient* httpClient, HttpMethod httpMethod){
	if(httpClient == NULL){
		printf("httpClient must not be null\n");
		return;
	}
	request_set_method(httpClient->request, httpMethod);
}

int httpclient_set_url_direct(HttpClient* httpClient,char *url){
	request_set_url(httpClient->request, url);

    return 0;
}

int httpclient_set_body(HttpClient* httpClient, char *jsonBody){

    Request *request;
    request = httpClient->request;

    if( request == NULL){
        printf("request must not be null\n");
        return -1;
    }

    request_set_body(request, jsonBody);
}

int httpclient_set_request(HttpClient* httpClient, Request *request){
    if(request == NULL){
        printf("request is invalid\n");
        return -1;
    }
    httpClient->request = request;

    return 0;
}


Response *httpclient_send_request(HttpClient *httpClient){
    Request *request = httpClient->request;

    if(request == NULL){
        printf("request must not be null\n");
        return NULL;
    }

    if(request->curl_handle == NULL){
        printf("curl_handle must not be null\n");
        return NULL;
    }

    request_send(request, httpClient);

    return  httpClient->response;
}

void httpclient_close(HttpClient *httpClient){
    if(httpClient == NULL)
        return;

    if(httpClient->request != NULL){
        request_destory(httpClient->request);
		httpClient->request = NULL;
    }

	if(httpClient->response != NULL){
		response_release(httpClient->response);
		httpClient->response = NULL;
	}

    if(httpClient != NULL){
        free(httpClient);
		httpClient = NULL;
    }
}

void httpclient_deinit(){
	request_deinit();
}

