/*
 *
 */

#ifndef HC_H_
#define HC_H_

#include "gt.h"

typedef struct _HC   HC; // Harbour Commander
typedef struct _AS   AS; // Application Settings

struct _AS
{
   int  width;
   int  height;
   bool saveWindowSize;
   int  widthPos;
   int  heightPos;
   bool saveWindowPosition;
   const char *aPanel;
};

struct _HC
{
   int         col;
   int         row;
   int         maxCol;
   int         maxRow;
   char        currentDir[ PATH_MAX ];

   FileInfo   *files;
   int         filesCount;

   int         rowBar;
   int         rowNo;

   const char *cmdLine;
   int         cmdCol;
   int         cmdColNo;

   bool isFirstDirectory;
   bool isHiddenDirectory;
   bool isFirstFile;
   bool isHiddenFile;

   bool sizeVisible;
   bool attrVisible;
   bool dateVisible;
   bool timeVisible;
};

/* ------------------------------------------------------------------------- */
static HC         *hc_init( void );
void               hc_loadSettings( GT *gt, HC *hc, const char *configFilePath );
void               hc_saveSettings( GT *gt, HC *lPanel, HC *rPanel, const char *configFilePath );
static void        hc_free( HC *hc );
static void        hc_fetchList( HC *hc, const char *currentDir );
static int         hc_compareFiles( const void *a, const void *b );
static void        hc_autosize( GT *gt );
static void        hc_resize( HC *hc, int col, int row, int maxCol, int maxRow );
static void        hc_drawPanel( HC *hc );
static int         hc_findLongestName( HC *hc );
static int         hc_findLongestSize( HC *hc );
static int         hc_findLongestAttr( HC *hc );
static const char *hc_paddedString( HC *hc, int longestName, int longestSize, int longestAttr, const char *name, const char *size, const char *date, const char *time, const char *attr );
static const char *hc_selectColor( const char *attr, bool state );
static void        hc_refreshPanel( HC *hc );
static void        hc_drawComdLine( GT *gt, HC *hc );
static void        hc_changeDir( HC *hc );
static void        hc_updateFetchList( HC *hc, const char *newDir );
static int         hc_dirIndexName( HC *hc, const char *tmpDir );

#endif /* End HC_H_ */