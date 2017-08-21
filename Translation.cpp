#include "stdafx.h"
#include <stdio.h>
#include "UI.h"
//#include "Objects.h"
#include "Dispatch.h"
#include "CSB.h"



int numXlate = 0;
char **xlate = NULL; // translation strings come in pairs. English then translation.


void ReadTranslationFile(void)
{
  char buf[200];
  int handle, len, len1, len2;
  int dquote[4];
  numXlate = 0;
  xlate = NULL;
  handle = OPEN("Translation.txt", "r");
  if (handle < 0) return;
  while (GETS(buf, 199, (i16)handle) != NULL)
  {
    int i = 0, j;
    len = strlen(buf);
    for (j=0; j<4; j++) // Find 4 double-quotes
    {
      dquote[j] = -1;
      for (; i<len; i++)
      {
        if (buf[i] == '"')
        {
          dquote[j] = i++;
          break;
        };
      };
    };
    if (dquote[3] < 0)
    {
      UI_MessageBox("Bad Translation Line","Warning",MESSAGE_OK);
    };
    xlate = (char **)UI_realloc(xlate, 2*(numXlate+1)*sizeof(*xlate), MALLOC108);
    if (xlate == NULL) break;
    len1 = dquote[1]-dquote[0];
    len2 = dquote[3]-dquote[2];
    xlate[2*numXlate] = (char *)UI_malloc(len1, MALLOC108);
    if (xlate[2*numXlate] == NULL) break;
    xlate[2*numXlate+1] = (char *)UI_malloc(len2, MALLOC108);
    if (xlate[2*numXlate+1] == NULL) break;
    numXlate++;
    buf[dquote[1]] = 0;
    buf[dquote[3]] = 0;
    memcpy(xlate[2*numXlate-2], buf+dquote[0]+1, len1);
    memcpy(xlate[2*numXlate-1], buf+dquote[2]+1, len2);
  };
  CLOSE(handle);
}




const char *TranslateLanguage(const char *text)
{
  int i;
  for (i=0; i<numXlate; i++)
  {
    if (strcmp(xlate[2*i], text) == 0) return xlate[2*i+1];
  };
  return text;
}

void CleanupTranslations(void)
{
  int i;
  for (i=0; i<numXlate; i++)
  {
    UI_free(xlate[2*i]);
    UI_free(xlate[2*i+1]);
  };
  if (xlate != NULL) UI_free(xlate);
  xlate = NULL;
  numXlate = 0;
}