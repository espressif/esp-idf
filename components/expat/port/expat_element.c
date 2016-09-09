/* This is simple demonstration of how to use expat. This program
   reads an XML document from standard input and writes a line with
   the name of each element to standard output indenting child
   elements by one tab stop more than their parent element.
   It must be used with Expat compiled for UTF-8 output.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expat.h"

#define XML_FMT_INT_MOD "l"

static void XMLCALL
startElement(void *userData, const char *name, const char **atts)
{
  int i;
  int *depthPtr = (int *)userData;
  (void)atts;

  for (i = 0; i < *depthPtr; i++)
    putchar('\t');
  puts(name);
  *depthPtr += 1;
}

static void XMLCALL
endElement(void *userData, const char *name)
{
  int *depthPtr = (int *)userData;
  (void)name;

  *depthPtr -= 1;
}

int xml_main(int argc, const char *argv)
{
  char *buf = NULL;
  int depth = 0;
  if (argv == NULL){
	 printf("no element parse\n");
	 return 1;
  }
  
  XML_Parser parser = XML_ParserCreate(NULL);
  if (parser){
	  XML_SetUserData(parser, &depth);
	  XML_SetElementHandler(parser, startElement, endElement);
	  buf = (char*)malloc(argc + 1);
	  bzero(buf, argc + 1);
	  memcpy(buf, argv, argc);
	  if (XML_Parse(parser, buf, argc, 0) == XML_STATUS_ERROR) {
	      printf( "%s at line %" XML_FMT_INT_MOD "u\n",
	              XML_ErrorString(XML_GetErrorCode(parser)),
	              XML_GetCurrentLineNumber(parser));
	  }else{
		  printf("XML_Parse Sucessful\n ");
	  }	  
  }

  XML_ParserFree(parser);
  return 0;
}

