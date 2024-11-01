/*
 *
 */

#include "gt.h"

int main( int argc, char *argv[] )
{
   UNUSED( argc );
   UNUSED( argv );

   bool quit = F;
   GT *gt = gt_createWindow( 830, 450, "Test window", "" );

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
                  printf( "Window closed\n" );
                  quit = T;
                  break;

               case SDL_WINDOWEVENT:
                  if( event.window.event == SDL_WINDOWEVENT_CLOSE )
                  {
                     printf( "Window close requested\n" );
                     quit = T;
                  }
                  break;

               case SDL_KEYDOWN:
                  if( event.key.keysym.sym == SDLK_ESCAPE )
                  {
                     printf( "Esc pressed\n" );
                     quit = T;
                  }
                  else
                  {
                     printf( "Other key pressed (key: %d)\n", event.key.keysym.sym );
                  }
                  break;

               default:
                  printf( "Unhandled event (type: %d)\n", event.type );
                  break;
            }

         }
         while( SDL_PollEvent( &event ) );
      }

      gt_beginDraw( gt );
         // ...
      gt_endDraw( gt );
   }

   gt_clearAll( gt );
   return 0;
}
