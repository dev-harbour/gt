/*
 *
 */

#include "gt.h"

/* ------------------------------------------------------------------------- */
const char *gt_os( void )
{
   return "Linux";
}

const char *gt_pathSeparator( void )
{
   return "/";
}

FileInfo *gt_directory( const char *currentDir, int *size )
{
   FileInfo *files = NULL;
   int count = 0;
   int parentIndex = -1;

   DIR *pDir;
   struct dirent *entry;
   struct stat fileInfo;

   // Open the directory
   pDir = opendir( currentDir );
   if( pDir == NULL )
   {
      fprintf( stderr, "Directory cannot be opened: %s\n", currentDir );
      return NULL;
   }

   // Read the directory entries
   while( ( entry = readdir( pDir ) ) != NULL )
   {
      // Skipping the current directory
      if( strcmp( entry->d_name, "." ) == 0 )
      {
         continue;
      }

      // Parent directory support
      if( strcmp( entry->d_name, ".." ) == 0 )
      {
         parentIndex = count;
      }

      // Construct the full path for each entry
      char fullPath[ PATH_MAX ];
      snprintf( fullPath, sizeof( fullPath ), "%s/%s", currentDir, entry->d_name );

      // Get file information using stat
      if( stat( fullPath, &fileInfo ) == -1 )
      {
         perror( "Error getting file info" );
         continue;
      }

      // Allocate memory for the file array dynamically
      FileInfo *temp = realloc( files, ( count + 1 ) * sizeof( FileInfo ) );
      if( !temp )
      {
         closedir( pDir );
         free( files ); // Free previously allocated memory
         return NULL;
      }
      files = temp;

      // Initialize FileInfo structure
      files[ count ].state = F;
      strncpy( files[ count ].name, entry->d_name, sizeof( files[ count ].name ) - 1 );
      files[ count ].name[ sizeof( files[ count ].name ) - 1 ] = '\0';

      // Set the file size
      snprintf( files[ count ].size, sizeof( files[ count ].size ), "%ld", fileInfo.st_size );

      // Set the date and time
      struct tm *tm = localtime( &fileInfo.st_mtime );
      strftime( files[ count ].date, sizeof( files[ count ].date ), "%d-%m-%Y", tm );
      strftime( files[ count ].time, sizeof( files[ count ].time ), "%H:%M:%S", tm );

      // Set file attributes
      strcpy( files[ count ].attr, "" );
      if( S_ISREG( fileInfo.st_mode ) )
      {
         strcat( files[ count ].attr, "A" ); // Regular file
         if( fileInfo.st_mode & S_IXUSR )
         {
            strcat( files[ count ].attr, "E" ); // Executable file
         }
      }
      if( S_ISDIR( fileInfo.st_mode ) )
      {
         strcat( files[ count ].attr, "D" ); // Directory
      }
      if( entry->d_name[ 0 ] == '.' )
      {
         strcat( files[ count ].attr, "H" ); // Hidden file
      }

      count++;
   }

   closedir( pDir );

   // Move the parent directory ("..") to the first position if found
   if( parentIndex > 0 )
   {
      FileInfo temp = files[ parentIndex ];
      memmove( &files[ 1 ], &files[ 0 ], sizeof( FileInfo ) * parentIndex );
      files[ 0 ] = temp;
   }

   *size = count; // Set the number of files/directories found
   return files;
}

void gt_freeDirectory( FileInfo *files )
{
   if( files )
   {
      free( files );
   }
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

   snprintf( command, sizeof( command ), "%s", commandLine );

   int result = system( command );
   if( result != 0 )
   {
      fprintf( stderr, "Failed to run executable \n" );
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

   snprintf( command, sizeof( command ), "xdg-open %s", commandLine );

   int result = system( command );
   if( result != 0 )
   {
      fprintf( stderr, "Failed to open file \n" );
      return F;
   }

   return T;
}

bool gt_isExecutable( const char *commandLine )
{
   const int commandBufferSize = PATH_MAX;
   const int commandPrefixMaxSize = 10;

   if( strlen( commandLine ) > ( commandBufferSize - commandPrefixMaxSize -1 ) )
   {
      fprintf( stderr, "Command is too long\n" );
      return F;
   }

   char command[ commandBufferSize + 50 ];
   snprintf( command, sizeof( command ), "file --mime-type -b %s", commandLine );

   FILE *fp = popen( command, "r" );
   if( fp == NULL )
   {
      perror( "popen" );
      return F;
   }

   char mimeType[ 100 ];
   if( fgets( mimeType, sizeof( mimeType ), fp ) != NULL )
   {
      mimeType[ strcspn( mimeType, "\n" )] = 0;

      if( strcmp( mimeType, "application/x-executable" ) == 0 || strcmp( mimeType, "application/x-pie-executable" ) == 0 )
      {
         pclose( fp );
         return T;
      }
   }

   pclose( fp );
   return F;
}

void gt_convertToSystemPath( char *path )
{
   UNUSED( path );
}
