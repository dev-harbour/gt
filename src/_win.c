/*
 *
 */

#include "gt.h"

/* ------------------------------------------------------------------------- */
const char *gt_os( void )
{
   return "Windows";
}

const char *gt_pathSeparator( void )
{
   return "\\";
}

struct tm ConvertSystemTimeToTm( const SYSTEMTIME *st )
{
   struct tm tm;
   memset( &tm, 0, sizeof( tm ) );
   tm.tm_year = st->wYear - 1900; // Years since 1900
   tm.tm_mon  = st->wMonth - 1;   // The months are from 0 to 11
   tm.tm_mday = st->wDay;
   tm.tm_hour = st->wHour;
   tm.tm_min  = st->wMinute;
   tm.tm_sec  = st->wSecond;
   return tm;
}

FileInfo *gt_directory( const char *currentDir, int *size )
{
   FileInfo *files = NULL;
   int count = 0;
   int parentIndex = -1;

   WIN32_FIND_DATA findFileData;
   char fullPath[ PATH_MAX ];
   snprintf( fullPath, sizeof( fullPath ), "%s\\*", currentDir );
   HANDLE hFind = FindFirstFile( fullPath, &findFileData );

   if( hFind == INVALID_HANDLE_VALUE )
   {
      fprintf( stderr, "Directory cannot be opened: %s\n", currentDir );
      return NULL;
   }

   // Initial allocation for the file array
   files = malloc( sizeof( FileInfo ) * INITIAL_FPCSR );
   if( !files )
   {
      fprintf( stderr, "Memory allocation error.\n" );
      FindClose( hFind );
      return NULL;
   }

   do
   {
      // Skipping the current directory
      if( strcmp( findFileData.cFileName, "." ) == 0 )
      {
         continue;
      }

      // Mark the parent directory
      if( strcmp( findFileData.cFileName, ".." ) == 0 )
      {
         parentIndex = count;
      }

      // Reallocate memory if needed
      if( count >= INITIAL_FPCSR )
      {
         FileInfo *temp = realloc( files, sizeof( FileInfo ) * ( count + 1 ) );
         if( !temp )
         {
            fprintf( stderr, "Memory allocation error.\n" );
            FindClose( hFind );
            free( files ); // Free previously allocated memory
            return NULL;
         }
         files = temp;
      }

      // Initialize FileInfo structure
      files[ count ].state = F;
      strncpy( files[ count ].name, findFileData.cFileName, sizeof( files[ count ].name ) - 1 );
      files[ count ].name[ sizeof( files[ count ].name ) - 1 ] = '\0';

      // Set file size
      LARGE_INTEGER fileSize;
      fileSize.LowPart = findFileData.nFileSizeLow;
      fileSize.HighPart = findFileData.nFileSizeHigh;
      snprintf( files[ count ].size, sizeof( files[ count ].size ), "%lld", fileSize.QuadPart );

      // Date and time conversion
      FILETIME ft = findFileData.ftLastWriteTime;
      SYSTEMTIME st;
      FileTimeToLocalFileTime( &ft, &ft );
      FileTimeToSystemTime( &ft, &st );

      struct tm tm = ConvertSystemTimeToTm( &st );
      strftime( files[ count ].date, sizeof( files[ count ].date ), "%d-%m-%Y", &tm );
      strftime( files[ count ].time, sizeof( files[ count ].time ), "%H:%M:%S", &tm );

      // Set file attributes
      strcpy( files[ count ].attr, "" );
      if( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
      {
         strcat( files[ count ].attr, "D" ); // Directory
      }
      if( findFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN )
      {
         strcat( files[ count ].attr, "H" ); // Hidden file
      }

      count++;
   }
   while( FindNextFile( hFind, &findFileData ) != 0 );

   FindClose( hFind );

   // Move the parent directory ("..") to the first position
   if( parentIndex > 0 )
   {
      FileInfo temp = files[ parentIndex ];
      memmove( &files[ 1 ], &files[ 0 ], sizeof( FileInfo ) * parentIndex );
      files[ 0 ] = temp;
   }

   *size = count; // Record the number of found files/directories
   return files;
}

void gt_freeDirectory( FileInfo *files )
{
   free( files );
}

bool gt_runApp( const char *commandLine )
{
   const int commandBufferSize = PATH_MAX;
   const int commandPrefixMaxSize = 10;

   if( strlen( commandLine ) > ( commandBufferSize - commandPrefixMaxSize -1 ) )
   {
      fprintf( stderr, "Command is too long\n" );
      return F;
   }

   char command[ commandBufferSize ];

   snprintf( command, sizeof( command ), "start %s", commandLine );

   int result = system( command );
   if( result != 0 )
   {
      fprintf( stderr, "Failed to open command \n" );
      return F;
   }

   return T;
}

bool gt_run( const char *commandLine )
{
   const int commandBufferSize = PATH_MAX;
   const int commandPrefixMaxSize = 10;

   if( strlen( commandLine ) > ( commandBufferSize - commandPrefixMaxSize -1 ) )
   {
      fprintf( stderr, "Command is too long\n" );
      return F;
   }

   char command[ commandBufferSize ];

   snprintf( command, sizeof( command ), "start %s", commandLine );

   // Execute the command and check the result
   int result = system( command );
   if( result != 0 )
   {
      fprintf( stderr, "Failed to open command \n" );
      return F;
   }

   return T;
}

bool gt_isExecutable( const char *filePath )
{
   const char *extensions[] = { ".exe", ".bat", ".com", ".cmd" };
   const char *ext = strrchr( filePath, '.' );

   if( ext != NULL )
   {
      for( size_t i = 0; i < sizeof( extensions) / sizeof( extensions[ 0 ] ); i++ )
      {
         if( strcmp( ext, extensions[i] ) == 0 )
         {
            return T;
         }
      }
   }

   return F;
}

void gt_convertToSystemPath( char *path )
{
   for( int i = 0; path[ i ] != '\0'; i++ )
   {
      if( path[ i ] == '/' )
      {
         path[ i ] = '\\';
      }
   }
}
