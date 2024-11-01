/*
 * Harbour Commander (HC) Project
 * Copyright 2014 - 2024 Rafał Jopek
 * Website: https://harbour.pl
 *
 * This work has been migrated from my project,
 * which was originally developed in the Harbour language. The code is
 * inspired by previous functions and structures that were implemented
 * in that project. This note serves to highlight the development history
 * and the source of inspiration for the current project.
 *
 */

#include "hc.h"

static HC *lPanel = NULL;
static HC *rPanel = NULL;
static HC *aPanel = NULL;

static AS as;

int main( int argc, char *argv[] )
{
   UNUSED( argc );
   UNUSED( argv );

   bool quit = F;
   int index;

   GT *gt = gt_createWindow( 800, 450, "Harbour Commander", "F1F1F1" );

   lPanel = hc_init();
   rPanel = hc_init();

   hc_loadSettings( gt, lPanel, "hc.usr" );
   hc_loadSettings( gt, rPanel, "hc.usr" );

   if( lPanel->currentDir[ 0 ] != '\0' )
   {
      hc_fetchList( lPanel, lPanel->currentDir );
   }
   else
   {
      hc_fetchList( lPanel, gt_cwd() );
   }

   if( rPanel->currentDir[ 0 ] != '\0' )
   {
      hc_fetchList( rPanel, rPanel->currentDir );
   }
   else
   {
      hc_fetchList( rPanel, gt_cwd() );
   }
   // ---
   if( strcmp( as.aPanel, "lPanel" ) == 0 )
   {
      aPanel = lPanel;
   }
   else if( strcmp( as.aPanel, "rPanel" ) == 0 )
   {
      aPanel = rPanel;
   }

   while( !quit )
   {
      SDL_Event event;
      if( SDL_WaitEvent( &event ) )
      {
         do
         {
            switch( event.type )
            {
               case SDL_QUIT:
                  quit = T;
                  break;

               case SDL_WINDOWEVENT:
                  if( event.window.event == SDL_WINDOWEVENT_CLOSE )
                  {
                     quit = T;
                  }
                  break;

               case SDL_KEYDOWN:

                  switch( event.key.keysym.sym )
                  {
                     case SDLK_ESCAPE:
                        quit = T;
                        break;

                     case SDLK_RETURN:

                        if( strlen( aPanel->cmdLine ) == 0 )
                        {
                           index = aPanel->rowBar + aPanel->rowNo;
                           if( gt_at( "D", aPanel->files[ index ].attr ) == 0 )
                           {
                              hc_changeDir( aPanel );
                           }
                           else
                           {
                              char *commandLine = gt_addStr( "\"", aPanel->currentDir, aPanel->files[ index ].name, "\"", NULL );
                              if( gt_isExecutable( commandLine ) )
                              {
                                 gt_runApp( commandLine );
                              }
                              else
                              {
                                 gt_run( commandLine );
                              }
                              free( commandLine );
                           }
                        }
                        else
                        {
                           int result = system( aPanel->cmdLine );
                           if( result != 0 )
                           {
                              printf( "Error executing command" );
                           }

                           aPanel->cmdLine = "";
                           aPanel->cmdCol = 0;
                           hc_refreshPanel( aPanel );
                        }
                        break;

                     case SDLK_INSERT:

                        index = aPanel->rowBar + aPanel->rowNo;

                        if( strcmp( aPanel->files[ index ].name, ".." ) != 0 )
                        {
                           if( aPanel->files[ index ].state )
                           {
                              aPanel->files[ index ].state = F;
                           }
                           else
                           {
                              aPanel->files[ index ].state = T;
                           }

                           // Scroll through the list of files in the panel, if possible
                           if( aPanel->rowBar < aPanel->maxRow - 3 && aPanel->rowBar <= aPanel->filesCount - 2 )
                           {
                              ++aPanel->rowBar;
                           }
                           else
                           {
                              if( aPanel->rowNo + aPanel->rowBar <= aPanel->filesCount - 3 )
                              {
                                 ++aPanel->rowNo;
                              }
                           }
                        }
                        break;

                     case SDLK_TAB:

                        if( aPanel == lPanel )
                        {
                           aPanel = rPanel;
                           aPanel->cmdLine = lPanel->cmdLine;
                           aPanel->cmdCol  = lPanel->cmdCol;
                           lPanel->cmdLine = "";
                           lPanel->cmdCol = 0;
                           as.aPanel = "rPanel";
                        }
                        else
                        {
                           aPanel = lPanel;
                           aPanel->cmdLine = rPanel->cmdLine;
                           aPanel->cmdCol  = rPanel->cmdCol;
                           rPanel->cmdLine = "";
                           rPanel->cmdCol = 0;
                           as.aPanel = "lPanel";
                        }
                        break;

                     case SDLK_DOWN:

                        if( aPanel->rowBar < aPanel->maxRow - 3 && aPanel->rowBar <= aPanel->filesCount - 2 )
                        {
                           ++aPanel->rowBar;
                        }
                        else
                        {
                           if( aPanel->rowNo + aPanel->rowBar <= aPanel->filesCount - 2 )
                           {
                              ++aPanel->rowNo;
                           }
                        }
                        break;

                     case SDLK_UP:

                        if( aPanel->rowBar > 0 )
                        {
                           --aPanel->rowBar;
                        }
                        else
                        {
                           if( aPanel->rowNo > 0 )
                           {
                              --aPanel->rowNo;
                           }
                        }
                        break;

                     case SDLK_PAGEDOWN:

                        if( aPanel->rowBar >= gt_maxRow( gt ) - 4 )
                        {
                           if( aPanel->rowNo + gt_maxRow( gt ) <= aPanel->filesCount )
                           {
                              aPanel->rowNo += gt_maxRow( gt ) - aPanel->rowBar;
                           }
                        }
                        aPanel->rowBar = MIN( gt_maxRow( gt ) - 4, aPanel->filesCount - aPanel->rowNo - 1 );
                        break;

                     case SDLK_PAGEUP:

                        if( aPanel->rowBar <= 1 )
                        {
                           if( aPanel->rowNo - gt_maxRow( gt ) >= 0 )
                           {
                              aPanel->rowNo -= gt_maxRow( gt );
                           }
                           else
                           {
                              aPanel->rowNo = 0;
                           }
                        }
                        aPanel->rowBar = 0;
                        break;

                     default:
                        break;
                  }
                  break;

               case SDL_MOUSEWHEEL:
                  if( event.wheel.y > 0 )
                  {
                     if( aPanel->rowBar > 0 )
                     {
                        --aPanel->rowBar;
                     }
                     else if( aPanel->rowNo > 0 )
                     {
                        --aPanel->rowNo;
                     }
                  }
                  else if( event.wheel.y < 0 )
                  {
                     if( aPanel->rowBar < aPanel->maxRow - 3 && aPanel->rowBar <= aPanel->filesCount - 2 )
                     {
                        ++aPanel->rowBar;
                     }
                     else if( aPanel->rowNo + aPanel->rowBar <= aPanel->filesCount - 2 )
                     {
                        ++aPanel->rowNo;
                     }
                  }
                  break;

               case SDL_MOUSEMOTION:

                  if( FONT_CELL_WIDTH == 0 || FONT_CELL_HEIGHT == 0 )
                  {
                     fprintf( stderr, "Error: Font not properly loaded, cell size is zero.\n" );
                     break;
                  }

                  int mouseX = gt->mouseX / FONT_CELL_WIDTH;
                  int mouseY = gt->mouseY / FONT_CELL_HEIGHT;

                  if( mouseX >= lPanel->col && mouseX < ( lPanel->col + lPanel->maxCol ) &&
                      mouseY >= lPanel->row && mouseY < ( lPanel->row + lPanel->maxRow ) )
                  {
                     aPanel = lPanel;
                  }
                  else if( mouseX >= rPanel->col && mouseX < ( rPanel->col + rPanel->maxCol ) &&
                           mouseY >= rPanel->row && mouseY < ( rPanel->row + rPanel->maxRow ) )
                  {
                     aPanel = rPanel;
                  }
                  break;

               case SDL_MOUSEBUTTONDOWN:

                  if( FONT_CELL_WIDTH == 0 || FONT_CELL_HEIGHT == 0 )
                  {
                     fprintf( stderr, "Error: Font not properly loaded, cell size is zero.\n" );
                     break;
                  }

                  switch( event.button.button )
                  {
                     case SDL_BUTTON_LEFT:
                        if( event.button.clicks == 1 )
                        {
                           int mouseX = gt->mouseX / FONT_CELL_WIDTH;
                           int mouseY = gt->mouseY / FONT_CELL_HEIGHT;

                           int rowIndex = mouseY - 1;

                           // Sprawdzenie, czy mysz jest w obrębie lewego panelu
                           if( mouseX >= lPanel->col && mouseX < ( lPanel->col + lPanel->maxCol ) &&
                              mouseY >= lPanel->row && mouseY < ( lPanel->row + lPanel->maxRow ) )
                           {
                              aPanel = lPanel;
                           }
                           // Sprawdzenie, czy mysz jest w obrębie prawego panelu
                           else if( mouseX >= rPanel->col && mouseX < ( rPanel->col + rPanel->maxCol ) &&
                                    mouseY >= rPanel->row && mouseY < ( rPanel->row + rPanel->maxRow ) )
                           {
                              aPanel = rPanel;
                           }

                           // Sprawdzenie, czy rowIndex znajduje się w granicach panelu i aktualizacja wyboru wiersza
                           if( rowIndex >= 0 && rowIndex < aPanel->filesCount )
                           {
                              aPanel->rowBar = rowIndex;
                           }
                        }
                        else if( event.button.clicks == 2 )
                        {
                           index = aPanel->rowBar + aPanel->rowNo;
                           if( gt_at( "D", aPanel->files[ index ].attr ) == 0 )
                           {
                              hc_changeDir( aPanel );
                           }
                           else
                           {
                              char *commandLine = gt_addStr( "\"", aPanel->currentDir, aPanel->files[ index ].name, "\"", NULL );
                              if( gt_isExecutable( commandLine ) )
                              {
                                 gt_runApp( commandLine );
                              }
                              else
                              {
                                 gt_run( commandLine );
                              }
                              free( commandLine );
                           }
                        }
                        break;

                     default:
                        break;
                  }
                  break;

               default:
                  break;
            }
         }
         while( SDL_PollEvent( &event ) );
      }
      gt_beginDraw( gt );

         hc_autosize( gt );
         hc_drawPanel( lPanel );
         hc_drawPanel( rPanel );
         hc_drawComdLine( gt, aPanel );

      gt_endDraw( gt );
   }

   hc_saveSettings( gt, lPanel, rPanel, "hc.usr" );

   hc_free( lPanel );
   hc_free( rPanel );
   aPanel = NULL;

   gt_clearAll( gt );

   return 0;
}

static HC *hc_init( void )
{
   HC *hc = malloc( sizeof( HC ) );
   if( !hc )
   {
      fprintf( stderr, "Failed to allocate memory for HC. \n" );
      return NULL;
   }

   memset( hc, 0, sizeof( HC ) );

   hc->cmdLine = "";

   hc->isFirstDirectory  = T;
   hc->isHiddenDirectory = F;
   hc->isFirstFile       = F;
   hc->isHiddenFile      = F;

   return hc;
}

void hc_loadSettings( GT *gt, HC *hc, const char *configFilePath )
{
   FILE *file = fopen( configFilePath, "r" );
   if( file == NULL )
   {
      fprintf( stderr, "Failed to open config file: %s\n", configFilePath );
      return;
   }

   char line[ 256 ];
   bool inSection = F;

   while( fgets( line, sizeof( line ), file ) )
   {
      line[ strcspn( line, "\n" ) ] = 0;

      // Rozpoznawanie sekcji
      if( strcmp( line, "lPanel" ) == 0 && hc == lPanel )
      {
         inSection = T;
         continue;
      }
      else if( strcmp( line, "lEnd" ) == 0 && hc == lPanel )
      {
         inSection = F;
         continue;
      }
      else if( strcmp( line, "rPanel" ) == 0 && hc == rPanel )
      {
         inSection = T;
         continue;
      }
      else if( strcmp( line, "rEnd" ) == 0 && hc == rPanel )
      {
         inSection = F;
         continue;
      }

      if( inSection )
      {
         if( strstr( line, "sizeVisible" ) )
         {
            hc->sizeVisible = IIF( ( strstr( line, "T" ) ), T, F );
         }
         else if( strstr( line, "attrVisible" ) )
         {
            hc->attrVisible = IIF( ( strstr( line, "T" ) ), T, F );
         }
         else if( strstr( line, "dateVisible" ) )
         {
            hc->dateVisible = IIF( ( strstr( line, "T" ) ), T, F );
         }
         else if( strstr( line, "timeVisible" ) )
         {
            hc->timeVisible = IIF( ( strstr( line, "T" ) ), T, F );
         }
         else if( strstr( line, "currentDir" ) )
         {
            sscanf( line, "currentDir = %[^\n]", hc->currentDir );
            gt_convertToSystemPath( hc->currentDir );
         }
      }
      else
      {
         // Application-Settings
         if( strstr( line, "width" ) )
         {
            sscanf( line, "width = %d", &as.width );
         }
         else if( strstr( line, "height" ) )
         {
            sscanf( line, "height = %d", &as.height );
         }
         else if( strstr( line, "saveWindowSize" ) )
         {
            as.saveWindowSize = IIF( ( strstr( line, "T" ) ), T, F  );
         }
         if( as.saveWindowSize )
         {
            SDL_SetWindowSize( gt->window, as.width, as.height );
         }
         //---
         if( strstr( line, "widthPos" ) )
         {
            sscanf( line, "widthPos = %d", &as.widthPos );
         }
         else if( strstr( line, "heightPos" ) )
         {
            sscanf( line, "heightPos = %d", &as.heightPos );
         }
         else if( strstr( line, "saveWindowPosition" ) )
         {
            as.saveWindowPosition = IIF( ( strstr( line, "T" ) ), T, F  );
         }
         // ---
         if( as.saveWindowPosition )
         {
            SDL_SetWindowPosition( gt->window, as.widthPos, as.heightPos );
         }
         if( strstr( line, "aPanel" ) )
         {
            char tempPanel[ 8 ];                       // Tymczasowy bufor dla wartości aPanel
            sscanf( line, "aPanel = %7s", tempPanel ); // Wczytanie wartości do bufora

            if( strcmp( tempPanel, "lPanel" ) == 0 )
            {
               as.aPanel = "lPanel";
            }
            else if( strcmp( tempPanel, "rPanel" ) == 0 )
            {
               as.aPanel = "rPanel";
            }
         }
      }
   }

   // printf( "Check currentDir for %s = %s\n", hc == lPanel ? "lPanel" : "rPanel", hc->currentDir );

   fclose( file );
}

void hc_saveSettings( GT *gt, HC *lPanel, HC *rPanel, const char *configFilePath )
{
   FILE *file = fopen( configFilePath, "w" );
   if( file == NULL )
   {
      fprintf( stderr, "Failed to open config file for saving: %s\n", configFilePath );
      return;
   }

   // Zapis ustawień globalnych
   fprintf( file, "Global-settings:\n" );
   fprintf( file, "width = %d\n", gt->width );
   fprintf( file, "height = %d\n", gt->height );
   fprintf( file, "saveWindowSize = %s\n", IIF( as.saveWindowSize, "T", "F" ) );
   fprintf( file, "widthPos = %d\n", gt->widthPos );
   fprintf( file, "heightPos = %d\n", gt->heightPos );
   fprintf( file, "saveWindowPosition = %s\n", IIF( as.saveWindowPosition, "T", "F" ) );
   fprintf( file, "aPanel = %s\n", as.aPanel );

   fprintf( file, "\nlPanel\n" );
   fprintf( file, "sizeVisible = %s\n", IIF( lPanel->sizeVisible, "T", "F" ) );
   fprintf( file, "attrVisible = %s\n", IIF( lPanel->attrVisible, "T", "F" ) );
   fprintf( file, "dateVisible = %s\n", IIF( lPanel->dateVisible, "T", "F" ) );
   fprintf( file, "timeVisible = %s\n", IIF( lPanel->timeVisible, "T", "F" ) );
   fprintf( file, "currentDir = %s\n", lPanel->currentDir );
   fprintf( file, "lEnd\n" );

   // Zapis ustawień dla rPanel
   fprintf( file, "\nrPanel\n" );
   fprintf( file, "sizeVisible = %s\n", IIF( rPanel->sizeVisible, "T", "F" ) );
   fprintf( file, "attrVisible = %s\n", IIF( rPanel->attrVisible, "T", "F" ) );
   fprintf( file, "dateVisible = %s\n", IIF( rPanel->dateVisible, "T", "F" ) );
   fprintf( file, "timeVisible = %s\n", IIF( rPanel->timeVisible, "T", "F" ) );
   fprintf( file, "currentDir = %s\n", rPanel->currentDir );
   fprintf( file, "rEnd\n" );

   fclose( file );
}

static void hc_free( HC *hc )
{
   if( hc )
   {
      if( hc->files )
      {
         gt_freeDirectory( hc->files );
      }
      free( hc );
   }
}

void hc_fetchList( HC *hc, const char *currentDir )
{
   if( currentDir == NULL || ! gt_isValidPath( currentDir ) )
   {
      const char *dir = gt_cwd();
      gt_safeCopy( hc->currentDir, dir, sizeof(  hc->currentDir ) );
   }
   else
   {
      gt_safeCopy( hc->currentDir, currentDir, sizeof( hc->currentDir ) );
   }

   gt_freeDirectory( hc->files );
   hc->filesCount = 0;
   hc->files = gt_directory( hc->currentDir, &hc->filesCount );

   if( hc->isFirstDirectory )
   {
      qsort( hc->files, hc->filesCount, sizeof( FileInfo ), hc_compareFiles );
   }
}

static int hc_compareFiles( const void *a, const void *b )
{
   FileInfo *fileA = ( FileInfo * ) a;
   FileInfo *fileB = ( FileInfo * ) b;

   // The ".." Directory always comes first
   if( strcmp( fileA->name, ".." ) == 0 ) return -1;
   if( strcmp( fileB->name, ".." ) == 0 ) return 1;

   // Directories before files
   bool isDirA = strchr( fileA->attr, 'D' ) != NULL;
   bool isDirB = strchr( fileB->attr, 'D' ) != NULL;
   if( isDirA != isDirB )
   {
      return IIF( isDirA, -1, 1 );
   }

   // Hidden files/directories after regular ones
   bool isHiddenA = strchr( fileA->attr, 'H' ) != NULL;
   bool isHiddenB = strchr( fileB->attr, 'H' ) != NULL;
   if( isHiddenA != isHiddenB )
   {
      return IIF( isHiddenA, 1, -1 );
   }

   return strcmp( fileA->name, fileB->name );
}

static void hc_autosize( GT *gt )
{
   int maxCol = gt_maxCol( gt );
   int maxRow = gt_maxRow( gt );

   if( maxCol == 0 || maxRow == 0 )
   {
      fprintf( stderr, "Error: Invalid maxCol or maxRow.\n" );
      return;
   }

   hc_resize( lPanel, 0, 0, maxCol / 2, maxRow - 1 );
   hc_resize( rPanel, maxCol / 2, 0, maxCol / 2, maxRow - 1 );
}

static void hc_resize( HC *hc, int col, int row, int maxCol, int maxRow )
{
   hc->col    = col;
   hc->row    = row;
   hc->maxCol = maxCol;
   hc->maxRow = maxRow;
}

static void hc_drawPanel( HC *hc )
{
   int row, i = 0;
   int longestName = 4;
   int longestSize = 0;
   int longestAttr = 0;

   longestName = MAX( longestName, hc_findLongestName( hc ) );
   longestSize = hc_findLongestSize( hc );
   longestAttr = hc_findLongestAttr( hc );

   if( aPanel == hc )
   {
      gt_dispBox( hc->col, hc->row, hc->maxCol, hc->maxRow, BOX_DOUBLE, "F1F1F1/323232" );
   }
   else
   {
      gt_dispBox( hc->col, hc->row, hc->maxCol, hc->maxRow, BOX_SINGLE, "F1F1F1/323232" );
   }

   i += hc->rowNo;
   for( row = hc->row + 1; row < hc->maxRow -1; row++ )
   {
      if( i < hc->filesCount )
      {
         const char *paddedString = hc_paddedString( hc, longestName, longestSize, longestAttr,
                                                     hc->files[ i ].name,
                                                     hc->files[ i ].size,
                                                     hc->files[ i ].date,
                                                     hc->files[ i ].time,
                                                     hc->files[ i ].attr );

         char *paddedResult = gt_padR( paddedString, hc->maxCol - 2 );

         const char *selectedColor;

         if( aPanel == hc && i == hc->rowBar + hc->rowNo )
         {
            if( hc->files[ i ].state == T )
            {
               selectedColor = "323232/FF4D4D";
            }
            else
            {
               selectedColor = "323232/00FF00";
            }
         }
         else
         {
            selectedColor = hc_selectColor( hc->files[ i ].attr, hc->files[ i ].state );
         }

         gt_drawText( hc->col + 1, row, paddedResult, selectedColor );

         free( paddedResult );

         ++i;
      }
      else
      {
         break;
      }
   }
}

static int hc_findLongestName( HC *hc )
{
   int longestName = 0;

   for( int i = 0; i < hc->filesCount; i++ )
   {
      int currentNameLength = gt_utf8Len( hc->files[ i ].name );
      if( currentNameLength > longestName )
      {
         longestName = currentNameLength;
      }
   }

   return longestName;
}

static int hc_findLongestSize( HC *hc )
{
   int longestSize = 0;

   for( int i = 0; i < hc->filesCount; i++ )
   {
      int currentSizeLength = strlen( hc->files[ i ].size );
      if( currentSizeLength > longestSize )
      {
         longestSize = currentSizeLength;
      }
   }

   return longestSize;
}

static int hc_findLongestAttr( HC *hc )
{
   int longestAttr = 0;

   for( int i = 0; i < hc->filesCount; i++ )
   {
      int currentAttrLength = strlen( hc->files[ i ].attr );
      if( currentAttrLength > longestAttr )
      {
         longestAttr = currentAttrLength;
      }
   }

   return longestAttr;
}

static const char *hc_paddedString( HC *hc, int longestName, int longestSize, int longestAttr, const char *name, const char *size, const char *date, const char *time, const char *attr )
{
   int lengthName = longestName;
   int lengthSize;
   int lengthAttr;
   int lengthDate;
   int lengthTime;
   int parentDir = 4;
   int space     = 1;
   int border    = 2;
   int nullTerminator = 1;

   static char formattedLine[ PATH_MAX ];

   if( strchr( attr, 'D' ) != NULL )
   {
      size = "DIR";
   }

   IIF( hc->sizeVisible, lengthSize = longestSize, lengthSize = 0 );
   IIF( hc->attrVisible, lengthAttr = longestAttr, lengthAttr = 0 );
   IIF( hc->dateVisible, lengthDate = 11, lengthDate = space );
   IIF( hc->timeVisible, lengthTime = 10, lengthTime = space );

   char *padLAttr = gt_padL( attr, lengthAttr );
   char *padLSize = gt_padL( size, lengthSize );

   char sizeAttrDateTime[ lengthSize + lengthAttr + lengthDate + lengthTime + nullTerminator ];

   if( hc->sizeVisible && hc->attrVisible && hc->dateVisible && hc->timeVisible )
   {
      snprintf( sizeAttrDateTime, sizeof( sizeAttrDateTime ), "%s %s %s %s", padLSize, padLAttr, date, time );
   }
   else if( hc->sizeVisible && hc->attrVisible && hc->dateVisible )
   {
      snprintf( sizeAttrDateTime, sizeof( sizeAttrDateTime ), "%s %s %s", padLSize, padLAttr, date );
   }
   else if( hc->sizeVisible && hc->attrVisible && hc->timeVisible )
   {
      snprintf( sizeAttrDateTime, sizeof( sizeAttrDateTime ), "%s %s %s", padLSize, padLAttr, time );
   }
   else if( hc->sizeVisible && hc->attrVisible )
   {
      snprintf( sizeAttrDateTime, sizeof( sizeAttrDateTime ), "%s %s", padLSize, padLAttr );
   }
   else if( hc->sizeVisible && hc->dateVisible && hc->timeVisible)
   {
      snprintf( sizeAttrDateTime, sizeof( sizeAttrDateTime ), "%s %s %s", padLSize, date, time );
   }
   else if( hc->sizeVisible && hc->dateVisible )
   {
      snprintf( sizeAttrDateTime, sizeof( sizeAttrDateTime ), "%s %s", padLSize, date );
   }
   else if( hc->sizeVisible && hc->timeVisible )
   {
      snprintf( sizeAttrDateTime, sizeof( sizeAttrDateTime ), "%s %s", padLSize, time );
   }
   else if ( hc->sizeVisible)
   {
      snprintf( sizeAttrDateTime, sizeof( sizeAttrDateTime ), "%s", padLSize );
   }
   else if( hc->attrVisible && hc->dateVisible && hc->timeVisible )
   {
      snprintf( sizeAttrDateTime, sizeof( sizeAttrDateTime ), "%s %s %s", padLAttr, date, time );
   }
   else if( hc->attrVisible && hc->dateVisible )
   {
      snprintf( sizeAttrDateTime, sizeof( sizeAttrDateTime ), "%s %s", padLAttr, date );
   }
   else if( hc->attrVisible && hc->timeVisible )
   {
      snprintf( sizeAttrDateTime, sizeof( sizeAttrDateTime ), "%s %s", padLAttr, time );
   }
   else if( hc->attrVisible )
   {
      snprintf( sizeAttrDateTime, sizeof( sizeAttrDateTime ), "%s", padLAttr );
   }
   else if( hc->dateVisible && hc->timeVisible )
   {
      snprintf( sizeAttrDateTime, sizeof( sizeAttrDateTime ), "%s %s", date, time );
   }
   else if( hc->dateVisible )
   {
      snprintf( sizeAttrDateTime, sizeof( sizeAttrDateTime ), "%s", date );
   }
   else if( hc->timeVisible )
   {
      snprintf( sizeAttrDateTime, sizeof( sizeAttrDateTime ), "%s", time );
   }
   else
   {
      snprintf( sizeAttrDateTime, sizeof( sizeAttrDateTime ), " " );
   }

   if( strcmp( name, ".." ) == 0 )
   {
      const char *lBracket = "[";
      const char *rBracket = "]";

      char *padLSizeAttrDateTime = gt_padL( sizeAttrDateTime, hc->maxCol - border - parentDir );

      size_t max_len = sizeof( formattedLine ) - strlen( lBracket ) - strlen( name ) - strlen( rBracket ) - 1;
      if( strlen( padLSizeAttrDateTime ) > max_len )
      {
         padLSizeAttrDateTime[ max_len ] = '\0';
      }
      snprintf( formattedLine, sizeof( formattedLine ), "%s%s%s%s", lBracket, name, rBracket, padLSizeAttrDateTime );

      free( padLSizeAttrDateTime );
   }
   else
   {
      char *padLSizeAttrDateTime = gt_padL( sizeAttrDateTime, hc->maxCol - border - lengthName );

      int availableWidthForName = hc->maxCol - border - strlen( padLSizeAttrDateTime );

      char *cutName = gt_left( name, availableWidthForName );

      char *padRName = gt_padR( cutName, availableWidthForName );

      snprintf( formattedLine, sizeof( formattedLine ), "%s%s", padRName, padLSizeAttrDateTime );

      free( cutName );
      free( padRName );
      free( padLSizeAttrDateTime );
   }

   free( padLAttr );
   free( padLSize );

   // printf( "Debugging: '%s'\n", formattedLine );

   return formattedLine;
}

static const char *hc_selectColor( const char *attr, bool state )
{
   if( state == T )
   {
      return "EAEAEA/B30000"; // Kolor na zaznaczonych plikach
   }
   else if( strcmp( attr, "DH" ) == 0 || strcmp( attr, "AH" ) == 0 )
   {
      return "EAEAEA/72A0E5";
   }
   else
   {
      return "EAEAEA/323232"; // Kolor dla pozostałych plików
   }
}

static void hc_refreshPanel( HC *hc )
{
   if( lPanel->currentDir == rPanel->currentDir )
   {
      hc_fetchList( lPanel, lPanel->currentDir );
      hc_fetchList( rPanel, rPanel->currentDir );

      hc_drawPanel( lPanel );
      hc_drawPanel( rPanel );
   }
   else
   {
      hc_fetchList( hc, hc->currentDir );
      hc_drawPanel( hc );
   }
}

static void hc_drawComdLine( GT *gt, HC *hc )
{
   int maxCol = gt_maxCol( gt );
   int maxRow = gt_maxRow( gt );
   const char *promptEnd;

   if( strcmp( "Windows", gt_os() ) == 0 )
   {
      promptEnd = ">";
   }
   else
   {
      promptEnd = "$ ";
   }

   const char *currentDir = gt_dirDeleteLastSeparator( hc->currentDir );

   char *combinedDir = gt_addStr( currentDir, promptEnd, NULL );
   char *cmdLinePart = gt_subStr( hc->cmdLine, 1 + hc->cmdColNo, maxCol + hc->cmdColNo );
   char *combinedCmdLine = gt_addStr( combinedDir, cmdLinePart, NULL );
   char *paddedCmdLine = gt_padR( combinedCmdLine, maxCol + 1 );

   gt_drawText( 0, maxRow - 1, paddedCmdLine, "323232/00FF00" );

   free( combinedDir );
   free( cmdLinePart );
   free( combinedCmdLine );
   free( paddedCmdLine );
}

static void hc_changeDir( HC *hc )
{
   int i = hc->rowBar + hc->rowNo;

   if( strcmp( hc->files[ i ].name, ".." ) == 0 )
   {
      const char *tmpDir = gt_dirLastName( hc->currentDir );
      const char *newDir;
      newDir = gt_dirDeleteLastPath( hc->currentDir );

      hc_updateFetchList( hc, newDir );

      int lastPosition = MAX( hc_dirIndexName( hc, tmpDir ), 1 ) ;
      if( lastPosition > aPanel->maxRow -3 )
      {
         aPanel->rowNo  = lastPosition % ( aPanel->maxRow -3 );
         aPanel->rowBar = aPanel->maxRow -3;
      }
      else
      {
         aPanel->rowNo  = 0;
         aPanel->rowBar = lastPosition;
      }
   }
   else
   {
      char *newDir = gt_addStr( hc->currentDir, hc->files[ i ].name, gt_pathSeparator(), NULL );
      hc->rowBar = 0;
      hc->rowNo  = 0;

      hc_updateFetchList( hc, newDir );
      free( newDir );
   }
}

static void hc_updateFetchList( HC *hc, const char *newDir )
{
   strncpy( hc->currentDir, newDir, sizeof( hc->currentDir ) -1 );
   hc->currentDir[ sizeof( hc->currentDir ) -1 ] = '\0';
   hc_fetchList( hc, newDir );
}

static int hc_dirIndexName( HC *hc, const char *tmpDir )
{
   for( int i = 0; i < hc->filesCount; i++ )
   {
      if( strcmp( hc->files[ i ].name, tmpDir ) == 0 )
      {
         return i;
      }
   }
   return -1;
}
