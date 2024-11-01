/*
 *
 */

#include "gt.h"

/* ------------------------------------------------------------------------- */
// static
static void gt_clearColor( const char *hex )
{
   int r, g, b, a = 255;

   if( strlen( hex ) == 6 ) // Format: RRGGBB
   {
      sscanf( hex, "%02x%02x%02x", &r, &g, &b );
      glClearColor( r / 255.0f, g / 255.0f, b / 255.0f, 1.0f );
   }
   else if( strlen( hex ) == 8 ) // Format: RRGGBBAA
   {
      sscanf( hex, "%02x%02x%02x%02x", &r, &g, &b, &a );
      glClearColor( r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f );
   }
   else
   {
      fprintf( stderr, "Invalid hex color format: gt_clearColor()\n" );
      glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
   }
}

static void gt_drawPoint( float x, float y )
{
   glPointSize( 1.0 );
   glBegin( GL_POINTS );
      glVertex2f( x - 0.5, y + 1 - FONT_CELL_HEIGHT );
   glEnd();
}

static void gt_drawChar( float x, float y, unsigned int codepoint, const char *colorString )
{
   unsigned int *bitmap = gtFontMatrix[ codepoint ];

   SDL_Color bgColor = { 255, 255, 255, 255 };
   SDL_Color fgColor = { 0, 0, 0, 255};

   if( colorString && strlen( colorString ) > 0 )
   {
      const char *separator = strchr( colorString, '/' );
      if( separator && ( separator - colorString == 6 ) && strlen( separator + 1 ) == 6 )
      {
         char bgColorStr[ 7 ];
         strncpy( bgColorStr, colorString, 6 );
         bgColorStr[ 6 ] = '\0';

         bgColor = gt_hexToColor( bgColorStr );
         fgColor = gt_hexToColor( separator + 1 );
      }
   }

   glColor4ub( bgColor.r, bgColor.g, bgColor.b, bgColor.a );
   glBegin( GL_QUADS );
      glVertex2f( x - 1, y );
      glVertex2f( x + FONT_CELL_WIDTH, y );
      glVertex2f( x + FONT_CELL_WIDTH, y - FONT_CELL_HEIGHT );
      glVertex2f( x - 1, y - FONT_CELL_HEIGHT );
   glEnd();

   glColor4ub( fgColor.r, fgColor.g, fgColor.b, fgColor.a );
   for( int row = 0; row < FONT_CELL_HEIGHT; row++ )
   {
      unsigned int value = bitmap[ row ];
      for( int col = 0; col < FONT_CELL_WIDTH; col++ )
      {
         if( value & ( 1 << ( 15 - col ) ) )
         {
            gt_drawPoint( x + col, y + row );
         }
      }
   }
}

/* ------------------------------------------------------------------------- */
// internal
void check_open_gl_error( const char *stmt, const char *fname, int line, GLenum *errCode )
{
   GLenum err = glGetError();
   if( err != GL_NO_ERROR )
   {
      printf( "OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt );
      *errCode = err;
   }
   else
   {
     *errCode = GL_NO_ERROR;
   }
}

/* ------------------------------------------------------------------------- */
// API functions
GT *gt_createWindow( int width, int height, const char *title, const char *hexColor )
{
   setlocale( LC_ALL, "en_US.UTF-8" );

   GT *gt = malloc( sizeof( GT ) );
   if( !gt )
   {
      fprintf( stderr, "Memory allocation failed for GT structure.\n" );
      return NULL;
   }

   memset( gt, 0, sizeof( GT ) );
   gt->width = width;
   gt->height = height;
   gt->background = hexColor;

   if( SDL_Init( SDL_INIT_VIDEO ) != 0 )
   {
      fprintf( stderr, "Unable to initialize SDL: %s\n", SDL_GetError() );
      free( gt );
      return NULL;
   }

   gt->window = SDL_CreateWindow( title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
   if( !gt->window )
   {
      fprintf( stderr, "Could not create window: %s\n", SDL_GetError() );
      SDL_Quit();
      free( gt );
      return NULL;
   }

   gt->glContext = SDL_GL_CreateContext( gt->window );
   if( !gt->glContext )
   {
      fprintf( stderr, "SDL_GL_CreateContext failed: %s\n", SDL_GetError() );
      SDL_DestroyWindow( gt->window );
      SDL_Quit();
      free( gt );
      return NULL;
   }

   SDL_SetWindowMinimumSize( gt->window, width, height );

   return gt;
}

void gt_clearAll( GT *gt )
{
   SDL_GL_DeleteContext( gt->glContext );
   SDL_DestroyWindow( gt->window );
   gt->window = NULL;
   SDL_Quit();

   free( gt );
   gt = NULL;
}

void gt_beginDraw( GT *gt )
{
   int newWidth, newHeight;
   int newWidthPos, newHeightPos;
   int newMouseX, newMouseY;

   SDL_GL_GetDrawableSize( gt->window, &newWidth, &newHeight );
   gt->width = newWidth;
   gt->height = newHeight;

   SDL_GetWindowPosition( gt->window, &newWidthPos, &newHeightPos ); // ?!
   gt->widthPos = newWidthPos;
   gt->heightPos = newHeightPos;

   SDL_GetMouseState( &newMouseX, &newMouseY );
   gt->mouseX = newMouseX;
   gt->mouseY = newMouseY;

   glViewport( 0, 0, newWidth, newHeight );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
   gt_clearColor( gt->background );

   glDisable( GL_CULL_FACE );
   glDisable( GL_DEPTH_TEST );
   glEnable( GL_BLEND );
   glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
   glOrtho( 0, newWidth, newHeight, 0, -1, 1 );
   glMatrixMode( GL_MODELVIEW );
   glLoadIdentity();
}

void gt_endDraw( GT *gt )
{
   REPORT_OPENGL_ERROR( "End drawing: " );
   SDL_GL_SwapWindow( gt->window );
}

/* ------------------------------------------------------------------------- */
SDL_Color gt_hexToColor( const char *hexColor )
{
   SDL_Color color = { 0, 0, 0, 255 };
   int r, g, b, a = 255;

   if( strlen( hexColor ) == 6 ) // Format: RRGGBB
   {
      sscanf( hexColor, "%02x%02x%02x", &r, &g, &b );
      color.r = r;
      color.g = g;
      color.b = b;
      color.a = 255;
   }
   else if( strlen( hexColor ) == 8 ) // Format: RRGGBBAA
   {
      sscanf( hexColor, "%02x%02x%02x%02x", &r, &g, &b, &a );
      color.r = r;
      color.g = g;
      color.b = b;
      color.a = a;
   }
   else
   {
      fprintf( stderr, "Invalid hex color format: gt_hexToColor() - defaulting to gray.\n" );
      color.r = 128;
      color.g = 128;
      color.b = 128;
      color.a = 255;
   }

   return color;
}

/* ------------------------------------------------------------------------- */
void gt_drawText( int x, int y, const char *string, const char *hexColor )
{
   unsigned int codepoint;
   int bytes;
   unsigned char ch;
   int i;

   x = x * FONT_CELL_WIDTH + 1;
   y = y * FONT_CELL_HEIGHT + FONT_CELL_HEIGHT;

   while( *string != '\0' )
   {
      bytes = 0;
      ch = ( unsigned char )( *string++ );
      if( ch <= 0x7F )
      {
         codepoint = ch;
         bytes = 1;
      }
      else if( ch <= 0xDF )
      {
         codepoint = ch & 0x1F;
         bytes = 2;
      }
      else if( ch <= 0xEF )
      {
         codepoint = ch & 0x0F;
         bytes = 3;
      } else
      {
         codepoint = ch & 0x07;
         bytes = 4;
      }

      for( i = 1; i < bytes; i++ )
      {
         ch = ( unsigned char )( *string++ );
         codepoint = ( codepoint << 6 ) | ( ch & 0x3F );
      }

      gt_drawChar( x, y, codepoint, hexColor );

      x += FONT_CELL_WIDTH;
   }
}


/* ------------------------------------------------------------------------- */
int gt_maxCol( GT *gt )
{
   return gt->width / FONT_CELL_WIDTH;
}

int gt_maxRow( GT *gt )
{
   return gt->height / FONT_CELL_HEIGHT;
}

int gt_maxWidth( GT *gt )
{
   return gt->width;
}

int gt_maxHeight( GT *gt )
{
   return gt->height;
}

/* ------------------------------------------------------------------------- */
char *gt_addStr( const char *firstString, ... )
{
   if( firstString == NULL )
   {
      return NULL;
   }

   size_t totalLength = 0;
   va_list args;

   va_start( args, firstString );
   const char *string = firstString;
   while( string != NULL )
   {
      totalLength += strlen( string );
      string = va_arg( args, const char * );
   }
   va_end( args );

   char *result = ( char * ) malloc( totalLength + 1 );
   if( !result )
   {
      return NULL;
   }

   result[ 0 ] = '\0';

   va_start( args, firstString );
   string = firstString;
   while( string != NULL )
   {
      strcat( result, string );
      string = va_arg( args, const char * );
   }
   va_end( args );

   return result;
}

int gt_at( const char *search, const char *string )
{
   char *occurrence = strstr( string, search );
   if( occurrence == NULL )
   {
      return -1;
   }

   int utf8Position = gt_utf8LenUpTo( string, occurrence );

   return utf8Position;
}

int gtAt( const char *search, const char *target )
{
   char *occurrence = strstr( target, search );
   if( occurrence == NULL )
   {
      return 0;
   }
   return ( int )( occurrence - target + 1 );
}


char *gt_left( const char *string, int count )
{
   int len = gt_utf8Len( string );

   if( count <= 0 )
   {
      return strdup( "" );
   }
   if( count >= len )
   {
      count = len;
   }

   const char *byteEnd = gt_utf8CharPtrAt( string, count );
   int byteCount = byteEnd - string;

   char *result = ( char * ) malloc( byteCount + 1 );
   if( result == NULL )
   {
      return NULL;
   }

   memcpy( result, string, byteCount );
   result[ byteCount ] = '\0';

   return result;
}

char *gt_right( const char *string, int count )
{
   int len = gt_utf8Len( string );

   if( count <= 0 )
   {
      return strdup( "" );
   }
   if( count > len )
   {
      count = len;
   }

   const char *byteStart = gt_utf8CharPtrAt( string, len - count );
   int byteCount = string + strlen( string ) - byteStart;

   char *result = ( char * ) malloc( byteCount + 1 );
   if( result == NULL )
   {
      return NULL;
   }

   memcpy( result, byteStart, byteCount );
   result[ byteCount ] = '\0';

   return result;
}

char *gt_padL( const char *string, int length )
{
   int len = gt_utf8Len( string );
   int byteLen = strlen( string );

   if( len >= length )
   {
      char *result = ( char * ) malloc( byteLen + 1 );
      if( !result )
      {
         return NULL;
      }

      memcpy( result, string, byteLen );
      result[ byteLen ] = '\0';
      return result;
   }
   else
   {
      int padding = length - len;

      char *result = ( char * ) malloc( padding + byteLen + 1 );
      if( !result )
      {
         return NULL;
      }

      memset( result, ' ', padding );
      memcpy( result + padding, string, byteLen );
      result[ padding + byteLen ] = '\0';
      return result;
   }
}

char *gt_padC( const char *string, int length )
{
   int len = gt_utf8Len( string );
   int byteLen = strlen( string );

   if( len >= length )
   {
      char *result = ( char * ) malloc( byteLen + 1 );
      if( !result )
      {
         return NULL;
      }

      memcpy( result, string, byteLen );
      result[ byteLen ] = '\0';
      return result;
   }
   else
   {
      int totalPadding = length - len;
      int paddingLeft  = totalPadding / 2;
      int paddingRight = totalPadding - paddingLeft;

      char *result = ( char * ) malloc( paddingLeft + byteLen + paddingRight + 1 );
      if( !result )
      {
         return NULL;
      }

      memset( result, ' ', paddingLeft );
      memcpy( result + paddingLeft, string, byteLen );
      memset( result + paddingLeft + byteLen, ' ', paddingRight );
      result[ paddingLeft + byteLen + paddingRight ] = '\0';
      return result;
   }
}

char *gt_padR( const char *string, int length )
{
   int len = gt_utf8Len( string );
   int byteLen = strlen( string );

   if( len >= length )
   {
      char *result = ( char * ) malloc( byteLen + 1 );
      if( !result )
      {
         return NULL;
      }

      memcpy( result, string, byteLen );
      result[ byteLen ] = '\0';
      return result;
   }
   else
   {
      int padding = length - len;

      char *result = ( char * ) malloc( byteLen + padding + 1 );
      if( !result )
      {
         return NULL;
      }

      memcpy( result, string, byteLen );
      memset( result + byteLen, ' ', padding );
      result[ byteLen + padding ] = '\0';
      return result;
   }
}

void gt_safeCopy( char *dest, const char *src, size_t destSize )
{
   if( destSize == 0 ) return;

   strncpy( dest, src, destSize - 1 );
   dest[ destSize - 1 ] = '\0';
}

void gt_safeCopyUtf8( char *dest, const char *src, size_t destSize )
{
   if( destSize == 0 ) return;

   size_t copiedBytes = 0;
   const char *srcPtr = src;

   while( *srcPtr && copiedBytes < destSize - 1 )
   {
      size_t charSize = 1;

      unsigned char byte = ( unsigned char ) *srcPtr;
      if( ( byte & 0x80 ) == 0 )         // ASCII (0xxxxxxx)
         charSize = 1;
      else if( ( byte & 0xE0 ) == 0xC0 ) // (110xxxxx)
         charSize = 2;
      else if( ( byte & 0xF0 ) == 0xE0 ) // (1110xxxx)
         charSize = 3;
      else if( ( byte & 0xF8 ) == 0xF0 ) // (11110xxx)
         charSize = 4;

      if( copiedBytes + charSize >= destSize - 1 )
         break;

      for( size_t i = 0; i < charSize; ++i )
      {
         dest[ copiedBytes++ ] = *srcPtr++;
      }
   }

   dest[copiedBytes] = '\0';
}

char *gt_space( int count )
{
   if( count <= 0 )
   {
      char *result = ( char * ) malloc( 1 );
      if( !result )
      {
         return NULL;
      }
      result[ 0 ] = '\0';
      return result;
   }

   char *result = ( char * ) malloc( count + 1 );
   if( !result )
   {
      return NULL;
   }

   memset( result, ' ', count );
   result[ count ] = '\0';
   return result;
}

char *gt_subStr( const char *string, int start, int count )
{
   int nSize = gt_utf8Len( string );

   if( start > nSize )
   {
      count = 0;
   }

   if( count > 0 )
   {
      if( start < 0 )
      {
         start = nSize + start;
      }
      if( start < 0 )
      {
         start = 0;
      }
      if( start + count > nSize )
      {
         count = nSize - start;
      }
   }
   else
   {
      if( start < 0 )
      {
         start = nSize + start;
      }
      if( start < 0 )
      {
         start = 0;
      }
      count = nSize - start;
   }

   const char *byteStart = gt_utf8CharPtrAt( string, start );
   const char *byteEnd   = gt_utf8CharPtrAt( byteStart, count );

   int byteCount = byteEnd - byteStart;

   char *result = ( char * ) malloc( byteCount + 1 );
   if( !result )
   {
      return NULL;
   }

   memcpy( result, byteStart, byteCount );
   result[ byteCount ] = '\0';
   return result;
}

/* ------------------------------------------------------------------------- */
void gt_dispBox( int x, int y, int width, int height, const char *boxString, const char *hexColor )
{
   if( strlen( boxString ) < 6 )
   {
      fprintf( stderr, "Error: boxString must contain at least 6 UTF-8 characters.\n" );
      return;
   }

   // Buffers for individual UTF-8 box-drawing characters (maximum 4 bytes + 1 for '\0')
   char topLeft[ 5 ]     = { 0 };
   char horizontal[ 5 ]  = { 0 };
   char topRight[ 5 ]    = { 0 };
   char vertical[ 5 ]    = { 0 };
   char bottomRight[ 5 ] = { 0 };
   char bottomLeft[ 5 ]  = { 0 };

   // Extract each UTF-8 character from boxString
   size_t index = 0;
   gt_Utf8CharExtract( boxString, topLeft, &index );
   gt_Utf8CharExtract( boxString, horizontal, &index );
   gt_Utf8CharExtract( boxString, topRight, &index );
   gt_Utf8CharExtract( boxString, vertical, &index );
   gt_Utf8CharExtract( boxString, bottomRight, &index );
   gt_Utf8CharExtract( boxString, bottomLeft, &index );

   gt_drawText( x, y, topLeft, hexColor );                  // top-left corner
   gt_drawText( x, y + height - 1, bottomLeft, hexColor );  // bottom-left corner

   for( int i = 1; i < width - 1; i++ )
   {
      gt_drawText( x + i, y, horizontal, hexColor );               // top edge
      gt_drawText( x + i, y + height - 1, horizontal, hexColor );  // bottom edge
   }

   for( int i = 1; i < height - 1; i++ )
   {
      gt_drawText( x, y + i, vertical, hexColor );              // left edge
      gt_drawText( x + width - 1, y + i, vertical, hexColor );  // right edge
   }

   gt_drawText( x + width - 1, y, topRight, hexColor );                  // top-right corner
   gt_drawText( x + width - 1, y + height - 1, bottomRight, hexColor );  // bottom-right corner
}

/* ------------------------------------------------------------------------- */
void gt_Utf8CharExtract( const char *source, char *dest, size_t *index )
{
   unsigned char firstByte = source[ *index ];
   size_t charLen = 1;

   if( ( firstByte & 0x80 ) == 0x00 )       // ASCII (0xxxxxxx)
      charLen = 1;
   else if( ( firstByte & 0xE0 ) == 0xC0 )  // (110xxxxx)
      charLen = 2;
   else if( ( firstByte & 0xF0 ) == 0xE0 )  // (1110xxxx)
      charLen = 3;
   else if( ( firstByte & 0xF8 ) == 0xF0 )  // (11110xxx)
      charLen = 4;

   strncpy( dest, source + *index, charLen );
   dest[ charLen ] = '\0';

   *index += charLen;
}

const char *gt_utf8CharPtrAt( const char *utf8String, int characterOffset )
{
   while( characterOffset > 0 && *utf8String )
   {
      unsigned char byte = *utf8String;

      // Determine how many bytes the current character uses
      if( ( byte & 0x80 ) == 0x00 )       // ASCII (0xxxxxxx)
         utf8String += 1;
      else if( ( byte & 0xE0 ) == 0xC0 )  // (110xxxxx)
         utf8String += 2;
      else if( ( byte & 0xF0 ) == 0xE0 )  // (1110xxxx)
         utf8String += 3;
      else if( ( byte & 0xF8 ) == 0xF0 )  // (11110xxx)
         utf8String += 4;
      else
         utf8String += 1;

      characterOffset--;
   }

   return utf8String;
}

size_t gt_utf8Len( const char *utf8String )
{
   size_t len = 0;
   while( *utf8String )
   {
      unsigned char byte = *utf8String;

      if( ( byte & 0x80 ) == 0 )         // ASCII (0xxxxxxx)
         utf8String += 1;
      else if( ( byte & 0xE0 ) == 0xC0 ) // (110xxxxx)
         utf8String += 2;
      else if( ( byte & 0xF0 ) == 0xE0 ) // (1110xxxx)
         utf8String += 3;
      else if( ( byte & 0xF8 ) == 0xF0 ) // (11110xxx)
         utf8String += 4;
      else
         utf8String += 1;

      len++;
   }

   return len;
}

size_t gt_utf8LenUpTo( const char *utf8String, const char *endPosition )
{
   size_t len = 0;
   while( utf8String < endPosition && *utf8String )
   {
      unsigned char byte = *utf8String;

      if( ( byte & 0x80 ) == 0 )         // ASCII (0xxxxxxx)
         utf8String += 1;
      else if( ( byte & 0xE0 ) == 0xC0 ) // (110xxxxx)
         utf8String += 2;
      else if( ( byte & 0xF0 ) == 0xE0 ) // (1110xxxx)
         utf8String += 3;
      else if( ( byte & 0xF8 ) == 0xF0 ) // (11110xxx)
         utf8String += 4;
      else
         utf8String += 1;

      len++;
   }

   return len;
}

/* ------------------------------------------------------------------------- */
const char *gt_cwd( void )
{
   static char result[ PATH_MAX ];
   size_t len;

   if( GET_CURRENT_DIR( result, sizeof( result ) ) != NULL )
   {
      len = strlen( result );
      char separator = gt_pathSeparator()[ 0 ];

      // Checking if the path length is within the buffer limits
      if( len >= ( sizeof( result ) - 2 ) )
      {
         fprintf( stderr, "Error: Path exceeds the allowed buffer size. \n" );
         return NULL;
      }

      // Checking if there is already a separator at the end of the path
      if( result[ len - 1 ] != separator )
      {
         result[ len ] = separator;
         result[ len + 1 ] = '\0';
      }

      return result;
   }
   else
   {
      fprintf( stderr, "Error: gt_cwd. \n" );
      return NULL;
   }
}

bool gt_isValidPath( const char *path )
{
   if( path == NULL || path[ 0 ] == '\0' )
   {
      return F;
   }

   if( strlen( path ) >= PATH_MAX )
   {
      return F;
   }

   return T;
}

const char *gt_dirDeleteLastPath( const char *path )
{
   static char result[ PATH_MAX ];
   char *lastPath;

   strncpy( result, path, sizeof( result ) - 1 );
   result[ sizeof( result ) - 1 ] = '\0';

   char separator = gt_pathSeparator()[0];

   lastPath = strrchr( result, separator );
   if( lastPath != NULL )
   {
      char *secondLastPath = NULL;
      *lastPath = '\0';
      secondLastPath = strrchr( result, separator );

      if( secondLastPath != NULL )
      {
         *lastPath = separator;
         *( secondLastPath + 1 ) = '\0';
      }
      else
      {
         *lastPath = separator;
      }
   }

   return result;
}

const char *gt_dirDeleteLastSeparator( const char *path )
{
   static char result[ PATH_MAX ];
   int maxBytes = PATH_MAX - 1;

   strncpy( result, path, maxBytes );
   result[ maxBytes ] = '\0';

   char separator = gt_pathSeparator()[0];

   char *lastSeparator = strrchr( result, separator );
   if( lastSeparator != NULL )
   {
      if( lastSeparator != result )
      {
         *lastSeparator = '\0';
      }
   }

   return result;
}

const char *gt_dirLastName( const char *path )
{
   static char result[ PATH_MAX ];
   int pathLength;

   if( path == NULL || ( pathLength = strlen( path ) ) == 0 )
   {
      result[ 0 ] = '\0';
      return result;
   }

   char separator = gt_pathSeparator()[ 0 ];

   if( path[ pathLength - 1 ] == separator )
   {
      pathLength--;
   }

   for( int i = pathLength - 1; i >= 0; i-- )
   {
      if( path[ i ] == separator )
      {
         int length = pathLength - i - 1;
         strncpy( result, path + i + 1, length );
         result[ length ] = '\0';
         return result;
      }
   }

   strncpy( result, path, pathLength );
   result[ pathLength ] = '\0';

   return result;
}

/* ------------------------------------------------------------------------- */
char *gt_strdup( const char *string )
{
   size_t len = strlen( string ) + 1;
   char *dup = malloc( len );
   if( dup )
   {
      memcpy( dup, string, len );
   }
   return dup;
}
