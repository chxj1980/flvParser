#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "mongoose.h"

#include "libxml/HTMLparser.h"
#include "libxml/xpath.h"



/**
 * print_xpath_nodes:
 * @nodes:		the nodes set.
 * @output:		the output file handle.
 *
 * Prints the @nodes content to @output.
 */
void
print_xpath_nodes(xmlNodeSetPtr nodes, char* output) {
    xmlNodePtr cur;
    int size;
    int i;
    
    assert(output);
    size = (nodes) ? nodes->nodeNr : 0;
    
    sprintf(output, "Result (%d nodes):\n", size);
    for(i = 0; i < size; ++i) {
        assert(nodes->nodeTab[i]);
	
        if(nodes->nodeTab[i]->type == XML_NAMESPACE_DECL) {
            xmlNsPtr ns;
	    
            ns = (xmlNsPtr)nodes->nodeTab[i];
            cur = (xmlNodePtr)ns->next;
            if(cur->ns) { 
                sprintf(output, "= namespace \"%s\"=\"%s\" for node %s:%s\n", 
                        ns->prefix, ns->href, cur->ns->href, cur->name);
            } else {
                sprintf(output, "= namespace \"%s\"=\"%s\" for node %s\n", 
                        ns->prefix, ns->href, cur->name);
            }
        } else if(nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
            cur = nodes->nodeTab[i];   	    
            if(cur->ns) { 
    	        sprintf(output, "= element node \"%s:%s\"\n", 
                        cur->ns->href, cur->name);
            } else {
    	        sprintf(output, "= element node \"%s\"\n", 
                        cur->name);
            }
        } else {
            cur = nodes->nodeTab[i];    
            sprintf(output, "= node \"%s\": type %d, value: %s\n", cur->name, cur->type, cur->content);
        }
    }
}


  /** 
   * parse a web page and return the serialsList
   * 
   * 
   * @return 
   */
int parseSerials(char * content)
{
    htmlDocPtr	doc;
    xmlXPathContextPtr xpathCtx; 
    xmlXPathObjectPtr xpathObj;
    
    doc = htmlReadFile ("http://990.ro", NULL, 0);
    if (doc == NULL) {
        printf("Error: cannot read file\n");
        return(-1);        
    }
    printf("File was read\n");
    
    xpathCtx = xmlXPathNewContext(doc);
    if(xpathCtx == NULL) {
        printf("Error: unable to create new XPath context\n");
        xmlFreeDoc(doc); 
        return(-1);
    }
    /* Evaluate xpath expression */
    xmlChar* xpathExpr = "//ul[@id='ddsubmenu1']//a";
    xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
    if(xpathObj == NULL) {
        fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", xpathExpr);
        xmlXPathFreeContext(xpathCtx); 
        xmlFreeDoc(doc); 
        return(-1);
    }

    /* Print results */
    print_xpath_nodes(xpathObj->nodesetval, content);

    /* Cleanup */
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx); 
    xmlFreeDoc(doc); 
}


  /** 
   * print nodex in a format understood by RTD1073 devices
   * 
   * @param nodes 
   */

void rtd_print_xpath_nodes(xmlNodeSetPtr nodes)
{

}


  //@{ this is the core part of application
static void *callback(enum mg_event event,
                      struct mg_connection *conn) {
    const struct mg_request_info *request_info;
    int i = 0;
    char* query_string = NULL;
    char* server = NULL;
    char* path = NULL;


    char content[1024];
    int content_length;
    
    if (event == MG_NEW_REQUEST) {
        request_info = mg_get_request_info(conn);

        if (strcmp(request_info->request_method, "GET") == 0) {
            
            if (strstr(request_info->uri, "flvParser")) {
            


                for (i = 0; i < request_info->num_headers;++i) {
                    if (strcmp("User-Agent", request_info->http_headers[i].name) == 0) {
                        printf("Value: %s\n", request_info->http_headers[i].value);
                        if (strstr(request_info->http_headers[i].value, "Mozilla")) {
                            printf("Mozilla browser\n");
                        }
                        else {
                            printf("Unknown browser\n");
                    
                        }
                    }
                }
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
                    content_length = parseSerials(content);
                }
                else {
                    content_length = snprintf(content, sizeof(content),
                                              "Hello from flvParser, no plugin available for your site %s \n", server);
                }

                mg_printf(conn,
                          "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/plain\r\n"
                          "Content-Length: %d\r\n"        // Always set Content-Length
                          "\r\n"
                          "%s",
                          content_length, content);
                free(query_string);
                query_string = NULL;
                // Mark as processed
                return "";
            }
        }
        return "";
    } else {
        return NULL;
    }
}

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
