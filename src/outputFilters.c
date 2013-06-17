#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "outputFilters.h"

const char* SERVER="http://localhost:8080/flvParser";


int handleWgetOutput(xmlNodeSetPtr nodes, char* content, char* server)
{
    assert(nodes);
    xmlNodePtr cur;
    int size;
    int i;
    int length = 0;
    size = (nodes) ? nodes->nodeNr : 0;
    length = sprintf(content, "Result (%d nodes):\n", size);
    for(i = 0; i < size; ++i) {
        assert(nodes->nodeTab[i]);
        
        if(nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
            cur = nodes->nodeTab[i];   	    
            length += sprintf(content + length, "= Title \"%s\", ", 
                   cur->children->content);
            xmlAttrPtr  pxAttr = cur->properties;
            while (pxAttr) {
                if (strcmp(pxAttr->name, "href") == 0) {
                    length += sprintf(content + length, "= href \"%s?%s&%s\"\n", 
                                      SERVER, server, pxAttr->children->content);
                }
                 pxAttr = pxAttr->next;
            }
        }
        
    }
    return length;

}


int handleOutput(const struct mg_request_info *request_info, xmlNodeSetPtr nodes, char * content)
{

    char* query_string = NULL;
    char* server = NULL;
    query_string = strdup(request_info->query_string);
    if ((server = strtok(query_string, "&")) == NULL)
    {
        server = query_string;
    }
    printf("Server: %s\n", server);

    int i;
    for (i = 0; i < request_info->num_headers;++i) {
        if (strcmp("User-Agent", request_info->http_headers[i].name) == 0) {
            printf("Value: %s\n", request_info->http_headers[i].value);
            if (strstr(request_info->http_headers[i].value, "Mozilla")) {
                printf("Mozilla browser\n");
            }
            if (strstr(request_info->http_headers[i].value, "Wget")) {
                printf("Wget browser, output simple text\n");
                return handleWgetOutput(nodes, content, server);
            }
            else {
                printf("Unknown browser\n");
                
            }
        }
    }
    free(query_string);
    query_string = NULL;

}
