#include "core.h"
#include "inputFilters.h"
#include "outputFilters.h"

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
