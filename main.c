#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "mongoose.h"

#include "libxml/HTMLparser.h"
#include "libxml/xpath.h"




#define SERVER "http://localhost:8080/flvParser"

xmlXPathObjectPtr filter990(htmlDocPtr* pdoc, xmlXPathContextPtr* pxpathCtx, char * path)
{
    xmlXPathObjectPtr xpathObj = NULL;
    
    if (path == NULL) {
        /// main page
        
        * pdoc = htmlReadFile ("http://990.ro", NULL, 0);
        if (*pdoc == NULL) {
            printf("Error: cannot read file\n");
            return(NULL);        
        }
        printf("File was read\n");
        
        *pxpathCtx = xmlXPathNewContext(*pdoc);
        if(*pxpathCtx == NULL) {
            printf("Error: unable to create new XPath context\n");
            xmlFreeDoc(*pdoc); 
            return(NULL);
        }
        /* Evaluate xpath expression */
        xmlChar* xpathExpr = "//*[@id='ddtopmenubar']//a";
        xpathObj = xmlXPathEvalExpression(xpathExpr, *pxpathCtx);
        if(xpathObj == NULL) {
            fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", xpathExpr);
            xmlXPathFreeContext(*pxpathCtx); 
            xmlFreeDoc(*pdoc); 
            return(NULL);
        }
        
    }
    else if (strcmp(path,"seriale.html") == 0) {
        /// main page
        
        * pdoc = htmlReadFile ("http://990.ro/seriale.html", NULL, 0);
        if (*pdoc == NULL) {
            printf("Error: cannot read file\n");
            return(NULL);        
        }
        printf("File was read\n");
        
        *pxpathCtx = xmlXPathNewContext(*pdoc);
        if(*pxpathCtx == NULL) {
            printf("Error: unable to create new XPath context\n");
            xmlFreeDoc(*pdoc); 
            return(NULL);
        }
        /* Evaluate xpath expression */
        xmlChar* xpathExpr = "//a[@class='titlu']";
        xpathObj = xmlXPathEvalExpression(xpathExpr, *pxpathCtx);
        if(xpathObj == NULL) {
            fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", xpathExpr);
            xmlXPathFreeContext(*pxpathCtx); 
            xmlFreeDoc(*pdoc); 
            return(NULL);
        }
        
    }

    return(xpathObj);
    
}


xmlXPathObjectPtr handleInput(htmlDocPtr* pdoc, xmlXPathContextPtr* pxpathCtx, const struct mg_request_info *request_info)
{
    xmlXPathObjectPtr xpathObj = NULL;
    char* query_string = NULL;
    char* path = NULL;
    char* server = NULL;    
    /// detecting server, to select right input plugin
    printf("Query string: %s\n", request_info->query_string);
    query_string = strdup(request_info->query_string);
    if (server = strtok(query_string, "&"))
    {
        path = strtok(NULL, "&" );
    }
    else {
        server = query_string;
    }
    
    printf("Server: %s, path: %s\n", server, path);

    if (strcmp(server, "www.990.ro") == 0) {
        /// 990 server, so handle it there
        
        xpathObj = filter990( pdoc, pxpathCtx, path);
    }
    free(query_string);
    query_string = NULL;
    return xpathObj;

}

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



void handleFlvParserGetRequest(struct mg_connection *conn, const struct mg_request_info *request_info)
{

    int i;
    char content[2048];
    int content_length;
    htmlDocPtr doc;
    xmlXPathContextPtr xpathCtx;
    xmlXPathObjectPtr xpathObj;
    xpathObj = handleInput(&doc, &xpathCtx, request_info);
    if (xpathObj) {
        content_length = handleOutput(request_info, xpathObj->nodesetval, content);
        /* Cleanup */
        xmlXPathFreeObject(xpathObj);
        xmlXPathFreeContext(xpathCtx);
        xmlFreeDoc(doc); 

        mg_printf(conn,
                  "HTTP/1.1 200 OK\r\n"
                  "Content-Type: text/plain\r\n"
                  "Content-Length: %d\r\n"        // Always set Content-Length
                  "\r\n"
                  "%s",
                  content_length, content);
        
    }
}

//@{ this is the core part of application
static void *callback(enum mg_event event,
                      struct mg_connection *conn) {
    const struct mg_request_info *request_info;
    int i = 0;


    xmlXPathObjectPtr xpathObj;
    

    if (event == MG_NEW_REQUEST) {
        request_info = mg_get_request_info(conn);
        if (strcmp(request_info->request_method, "GET") == 0) {
            
            if (strstr(request_info->uri, "flvParser")) {
                handleFlvParserGetRequest(conn, request_info);
            }
        }
        return "";
    } else {
        return NULL;
    }
}

/** 
 * main function
 * 
 * @param argc 
 * @param argv 
 * 
 * @return 
 */
int main(int argc, char **argv)
{

    struct mg_context *ctx;
    const char *options[] = {"listening_ports", "8080", NULL};

    ctx = mg_start(&callback, NULL, options);//@}
    getchar();  // Wait until user hits "enter"
    mg_stop(ctx);
    
    return(0);
}
//@} 
