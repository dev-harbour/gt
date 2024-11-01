/*
 *
 */

#include "gt.h"

int main( int argc, char *argv[] )
{
   UNUSED( argc );
   UNUSED( argv );

   bool quit = F;
   GT *gt = gt_createWindow( 830, 450, "Box drawing alignment tests", "FFFFFF" );

   const char *text[] =
   {
      "",
      "                                                                    █",
      "                                                                    ▉",
      "╔══╦══╗  ┌──┬──┐  ╭──┬──╮  ╭──┬──╮  ┏━━┳━━┓  ┎┒┏┑   ╷  ╻ ┏┯┓ ┌┰┐    ▊ ╱╲╱╲╳╳╳",
      "║┌─╨─┐║  │╔═╧═╗│  │╒═╪═╕│  │╓─╁─╖│  ┃┌─╂─┐┃  ┗╃╄┙  ╶┼╴╺╋╸┠┼┨ ┝╋┥    ▋ ╲╱╲╱╳╳╳",
      "║│╲ ╱│║  │║   ║│  ││ │ ││  │║ ┃ ║│  ┃│ ╿ │┃  ┍╅╆┓   ╵  ╹ ┗┷┛ └┸┘    ▌ ╱╲╱╲╳╳╳",
      "╠╡ ╳ ╞╣  ├╢   ╟┤  ├┼─┼─┼┤  ├╫─╂─╫┤  ┣┿╾┼╼┿┫  ┕┛┖┚     ┌┄┄┐ ╎ ┏┅┅┓ ┋ ▍ ╲╱╲╱╳╳╳",
      "║│╱ ╲│║  │║   ║│  ││ │ ││  │║ ┃ ║│  ┃│ ╽ │┃  ░░▒▒▓▓██ ┊  ┆ ╎ ╏  ┇ ┋ ▎",
      "║└─╥─┘║  │╚═╤═╝│  │╘═╪═╛│  │╙─╀─╜│  ┃└─╂─┘┃  ░░▒▒▓▓██ ┊  ┆ ╎ ╏  ┇ ┋ ▏",
      "╚══╩══╝  └──┴──┘  ╰──┴──╯  ╰──┴──╯  ┗━━┻━━┛  ▗▄▖▛▀▜   └╌╌┘ ╎ ┗╍╍┛ ┋  ▁▂▃▄▅▆▇█",
      "                                             ▝▀▘▙▄▟"
   };

   int num_lines = LEN( text );

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

         for( int i = 0; i < num_lines; i++ )
         {
            gt_drawText( 0, i, text[ i ], "FFFFFF/000000" );
         }

         gt_dispBox( 0, 11, 77, 5, BOX_SINGLE, "FFFFFF/000000" );
         gt_dispBox( 0, 16, 77, 5, BOX_DOUBLE, "FFFFFF/000000" );
         gt_dispBox( 0, 21, 77, 5, BOX_SINGLE_DOUBLE, "FFFFFF/000000" );
         gt_dispBox( 0, 26, 77, 5, BOX_DOUBLE_SINGLE, "FFFFFF/000000" );

      gt_endDraw( gt );
   }

   gt_clearAll( gt );
   return 0;
}
