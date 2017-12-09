/*
   C array source code example:
   -  read text file into a simple array
   -  with fixed maximum number of lines
   -  with fixed maximum line size
   -  sort the array
   -  write array content into another text file.

   from http://stahlworks.com/dev/article-02-01-c-code-array-fixed-size-from-text-file.html
*/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// how many text lines can this program read?
#define MAX_TEXT_LINES 10000   // up to 1000 lines
#define MAX_LINE_LENGTH 200   // with up to 200 chars each

// very simple static array for text storage:
char aTextData[MAX_TEXT_LINES+10][MAX_LINE_LENGTH+10];
// +10 are safety spaces, to avoid crash on off-by-one errors.

// tell that these functions exist further below:
int perr(const char *pszFormat, ...);
int processData(char aText[MAX_TEXT_LINES+10][MAX_LINE_LENGTH+10],
    int iLinesUsed, int iLinesAlloced);

int main(int argc, char *argv[])
{
   if (argc < 3)
      return 9+perr("specify input and output filename.\n");

   char *pszInFile  = argv[1];
   char *pszOutFile = argv[2];

   int  iTextSize = MAX_TEXT_LINES;  // how large is the array
   int  iTextUsed = 0;               // how many lines contained

   // safety: clean the memory with zeros
   memset(aTextData, 0, sizeof(aTextData));

   // line buffer for reading
   char szLineBuf[MAX_LINE_LENGTH+10];
   memset(szLineBuf, 0, sizeof(szLineBuf));

   // read input file into the array. use binary mode "rb"
   // to avoid CR/LF line end conversions.
   FILE *fin = fopen(pszInFile , "rb");
   if (!fin)
      return 9+perr("cannot read %s\n", pszInFile);

   // while not end of file, read another line
   while (fgets(szLineBuf, sizeof(szLineBuf)-10, fin) != 0)
   {
      // check if array has space for another line
      if (iTextUsed >= iTextSize)
         { perr("overflow: too many text lines\n"); break; }

      // strip CR/LF from line endings so we get pure text
      char *psz = strchr(szLineBuf, '\r'); if (psz) *psz = '\0';
            psz = strchr(szLineBuf, '\n'); if (psz) *psz = '\0';

      // store the line
      strncpy(aTextData[iTextUsed], szLineBuf, MAX_LINE_LENGTH);
      aTextData[iTextUsed][MAX_LINE_LENGTH-1] = '\0'; // safety

      iTextUsed++;
   }

   fclose(fin);

   // do some processing on the array contents
   processData(aTextData, iTextUsed, iTextSize);

   // write the whole array to output file. use text mode "w"
   // so under windows it will create CRLF line endings.
   FILE *fout = fopen(pszOutFile, "w");
   if (!fout)
      return 9+perr("cannot write %s\n", pszOutFile);

   int iLine;
   for (iLine=0; iLine<iTextUsed; iLine++)
   {
      strncpy(szLineBuf, aTextData[iLine], MAX_LINE_LENGTH);
      szLineBuf[MAX_LINE_LENGTH-2] = '\0'; // safety w/ space for LF

      strcat(szLineBuf, "\n"); // produces CRLF under windows

      int nlen = strlen(szLineBuf);
      if (fwrite(szLineBuf, 1, nlen, fout) != nlen)
         return 9+perr("failed to fully write %s\n", pszOutFile);
   }

   fclose(fout);

   return 0;
}

// processing example: sort array text lines alphabetically
// with the simplest possible sorting algorithm (bubblesort).
int processData(char aText[MAX_TEXT_LINES+10][MAX_LINE_LENGTH+10],
   int iLinesUsed, int iLinesAlloced)
{
   int iOuterIdx, iInnerIdx;

   char aBuffer[MAX_LINE_LENGTH+10];

   for (iOuterIdx=0; iOuterIdx<iLinesUsed; iOuterIdx++)
   {
      for (iInnerIdx=iOuterIdx+1; iInnerIdx<iLinesUsed; iInnerIdx++)
      {
         char *pszLine1 = aText[iOuterIdx];
         char *pszLine2 = aText[iInnerIdx];
         if (strcmp(pszLine1, pszLine2) > 0)
         {
            // swap both lines
            strcpy(aBuffer, pszLine1);
            strcpy(pszLine1, pszLine2);
            strcpy(pszLine2, aBuffer);
         }
      }
   }

   printf("sorted %d lines.\n", iLinesUsed);

   return 0;
}

// helper function: print error with variable parameters.
int perr(const char *pszFormat, ...)
{
   va_list argList;
   va_start(argList, pszFormat);
   char szBuf[1024];
   vsprintf(szBuf, pszFormat, argList);
   fprintf(stderr, "error: %s", szBuf);
   return 0;
}
