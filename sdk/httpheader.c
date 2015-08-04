#include "httpclient.h"
#include <malloc.h>

HttpHeader *httpHeader_create(){
    HttpHeader *header = (HttpHeader *)malloc(sizeof(HttpHeader));
    if(header == NULL){
        printf("failed to malloc httpheader");
        return  NULL;
    }

    header->headers = NULL;
    return  header;
}

void httpHeader_set_option(HttpHeader *httpHeader, char *option){
    httpHeader->headers = curl_slist_append(httpHeader->headers, option);
}

void httpHeader_close(HttpHeader *httpHeader){
	if(httpHeader != NULL){
		if(httpHeader->headers != NULL){
			curl_slist_free_all(httpHeader->headers);
			httpHeader->headers = NULL;
		}
		free(httpHeader);
		httpHeader = NULL;
	}
}
