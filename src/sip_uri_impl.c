/*
 * SipUri.cpp
 *
 *  Created on: 18 sept. 2010
 *      Author: jehanmonnier
 */

#include "sip_uri.h"
#include <stdlib.h>
#include <string.h>
 #include <stdarg.h>
#include "sipuriParser.h"
#include "sipuriLexer.h"

#define GET_SET_STRING(object_type,attribute) \
	const char* object_type##_get_##attribute (object_type* obj) {\
		return obj->attribute;\
	}\
	void object_type##_set_##attribute (object_type* obj,const char* value) {\
		if (obj->attribute != NULL) free((void*)obj->attribute);\
		obj->attribute=malloc(strlen(value)+1);\
		strcpy((char*)(obj->attribute),value);\
	}

#define SIP_URI_GET_SET_STRING(attribute) GET_SET_STRING(sip_uri,attribute)

#define GET_SET_UINT(object_type,attribute) \
	unsigned int* object_type##_get_##attribute (object_type* obj) {\
		return obj->attribute;\
	}\
	void object_type##_set_##attribute (object_type* obj,unsigned int value) {\
		obj->attribute=value;\
	}

#define SIP_URI_GET_SET_UINT(attribute) GET_SET_UINT(sip_uri,attribute)






struct _sip_uri {
	const char* user;
	const char* host;
	const char* transport_param;
};
sip_uri* sip_uri_parse (const char* uri) {
	pANTLR3_INPUT_STREAM           input;
	psipuriLexer               lex;
	pANTLR3_COMMON_TOKEN_STREAM    tokens;
	psipuriParser              parser;
	input  = antlr3NewAsciiStringCopyStream	(
			(pANTLR3_UINT8)uri,
			(ANTLR3_UINT32)strlen(uri),
			NULL);
	lex    = sipuriLexerNew                (input);
	tokens = antlr3CommonTokenStreamSourceNew  (ANTLR3_SIZE_HINT, TOKENSOURCE(lex));
	parser = sipuriParserNew               (tokens);

	sip_uri* l_parsed_uri = parser->an_sip_uri(parser);

	// Must manually clean up
	//
	parser ->free(parser);
	tokens ->free(tokens);
	lex    ->free(lex);
	input  ->close(input);
	return l_parsed_uri;
}
sip_uri* sip_uri_new () {
	sip_uri* lUri = (sip_uri*)malloc(sizeof(sip_uri));
	memset(lUri,0,sizeof(sip_uri));
	return lUri;
}
void sip_uri_delete(sip_uri* uri) {
	free(uri);
}

static char * concat (const char *str, ...) {
  va_list ap;
  size_t allocated = 100;
  char *result = (char *) malloc (allocated);

  if (result != NULL)
    {
      char *newp;
      char *wp;
      const char* s;

      va_start (ap, str);

      wp = result;
      for (s = str; s != NULL; s = va_arg (ap, const char *)) {
          size_t len = strlen (s);

          /* Resize the allocated memory if necessary.  */
          if (wp + len + 1 > result + allocated)
            {
              allocated = (allocated + len) * 2;
              newp = (char *) realloc (result, allocated);
              if (newp == NULL)
                {
                  free (result);
                  return NULL;
                }
              wp = newp + (wp - result);
              result = newp;
            }
          memcpy (wp, s, len);
          wp +=len;
        }

      /* Terminate the result string.  */
      *wp++ = '\0';

      /* Resize memory to the optimal size.  */
      newp = realloc (result, wp - result);
      if (newp != NULL)
        result = newp;

      va_end (ap);
    }

  return result;
}
char*	sip_uri_to_string(sip_uri* uri)  {
	return concat(	"sip:"
					,(uri->user?uri->user:"")
					,(uri->user?"@":"")
					,uri->host
					,(uri->transport_param?";transport=":"")
					,(uri->transport_param?uri->transport_param:"")
					,NULL);
}

SIP_URI_GET_SET_STRING(user)
SIP_URI_GET_SET_STRING(host)
SIP_URI_GET_SET_STRING(transport_param)

