#include "httpclient.h"
#include <malloc.h>
#include <string.h>

int request_init(){
    CURLcode ret;
    ret = curl_global_init(CURL_GLOBAL_WIN32);
    if (CURLE_OK != ret)
    {
        printf("failed to init libcurl.\n");
        return -1;
    }

    return 0;
}

Request *request_create(const char *host, const char *port){
    Request* request = (Request *)malloc(sizeof(Request));
    if(request == NULL){
        printf("failed to create request instance\n");
    }

    memset(request->host, 0, 64);
    memset(request->path, 0, PATH_MAX_SIZE);
    memset(request->url, 0, URL_MAX_SIZE);
    memset(request->port, 0, 16);
    memset(request->endpoint, 0, 80);

    strcpy(request->host, host);
    strcpy(request->port, port);
    request->method = GET;
    sprintf(request->endpoint, "http://%s:%s", host, port);
    request->curl_handle = curl_easy_init();
    request->httpHeader = NULL;

    return request;
}

Request *request_make(Request *request, HttpHeader *httpHeader,HttpMethod httpMethod, char *url, char *body){

    if(request == NULL){
        printf("failed to make request");
        return  NULL;
    }

    if(httpHeader != NULL){
        request_set_header(request, httpHeader);
    }
    if(httpMethod != NONE){
        request_set_method(request, httpMethod);
    }
    if(url != NULL){
        request_set_url(request,url);
    }
    if(body != NULL){
        request_set_body(request, body);
    }

    return request;
}

void request_set_header(Request *request, HttpHeader *httpHeader){
    request->httpHeader = httpHeader;
    curl_easy_setopt(request->curl_handle, CURLOPT_HTTPHEADER, httpHeader->headers);
}


void request_set_option(Request *request, HttpOption option, int value){
    CURL *curl_handle = request->curl_handle;
    if(curl_handle == NULL){
        printf("curl_handle must not be null\n");
        return;
    }

    switch (option){
        case TIMEOUT:
            curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, value);
            break;
        case CONNECT_TIMEOUT:
            curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, value);
            break;
        default:
            break;
    }

    return;
}

void request_set_url(Request *request, char *url){
    CURL *curl_handle = request->curl_handle;
    if(url == NULL){
        printf("url must not be null\n");
        return;
    }

    memset(request->url, 0, URL_MAX_SIZE);
    memset(request->path, 0, PATH_MAX_SIZE);
    strcpy(request->url, url);
    strcat(request->path, request->endpoint);
    strcat(request->path, request->url);

    curl_easy_setopt(curl_handle, CURLOPT_URL, request->path);

    return;
}

void request_set_body(Request *request, char *body){
	int body_size;
    CURL *curl_handle = request->curl_handle;
    if(body == NULL){
        printf("body must not be null\n");
        return;
    }

    body_size = strlen(body) + 1;
    request->body = (char *)malloc(body_size);
    memset(request->body, 0, body_size);
    strcpy(request->body, body);

    //fixed me, body field only supports post method
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, body);
    return;
}

void request_set_method(Request *request, HttpMethod method){
    CURL *curl_handle = request->curl_handle;

    if(method == NONE){
        return;
    }
    switch (method){
        case POST:
            curl_easy_setopt(curl_handle, CURLOPT_POST, 1 );
            break;
        case GET:
            break;
        case PUT:
			curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "PUT" );
            break;
        case DEL:
			curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "DELETE");
            break;
		case HEAD:
			curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "HEAD");
        default:
            break;
    }

    return;
}

void write_cb(void *buffer, size_t size, size_t nmemb, void *user_p)
{
	HttpClient *httpclient;
	long buffer_size;
    if(buffer == NULL){
        printf("write_cb buffer is null\n");
        return;
    }
    httpclient = (HttpClient *)user_p;
	if(httpclient == NULL){
		printf("write_cb() httpclient is null\n");
		return;
	}
    buffer_size = strlen((char *)buffer);

	if(httpclient->response != NULL && httpclient->response->body != NULL){
		free(httpclient->response->body);
		httpclient->response->body = NULL;
	}

    httpclient->response->body = (char *)malloc(buffer_size + 1);

    memset(httpclient->response->body, 0, buffer_size + 1);
    memcpy(httpclient->response->body, buffer, buffer_size);

    return;
}

void request_send(Request *request, void *userData){
	CURL *curl_handle;
	HttpClient *httpclient = (HttpClient *)userData;

    if(request == NULL){
        printf("request is invalid\n");
        return;
    }

    curl_handle = request->curl_handle;
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, &write_cb);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, httpclient);

    curl_easy_perform(curl_handle);
	
	curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE , &httpclient->response->statusCode);

    return;
}

void request_destory(Request *request){
	if(request != NULL){
		if(request->curl_handle != NULL){
			curl_easy_cleanup(request->curl_handle);
			request->curl_handle = NULL;
		}		
	}
    
	if(request->body != NULL){
		free(request->body);
		request->body = NULL;
	}

	if(request->httpHeader != NULL){
		free(request->httpHeader);
		request->httpHeader = NULL;
	}

    free(request);
	request = NULL;
}

void request_deinit(){
	curl_global_cleanup();
}


