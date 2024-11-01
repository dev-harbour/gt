/*
 *
 */

#include "gt.h"

int main( int argc, char *argv[] )
{
   UNUSED( argc );
   UNUSED( argv );

   bool quit = F;
   GT *gt = gt_createWindow( 830, 450, "Test Unicode", "FFFFFF" );

   const char *text[] =
   {
      "",
      "Mathematics and sciences:",
      "",
      "  ∮ E⋅da = Q,  n → ∞, ∑ f(i) = ∏ g(i),      ⎧⎡⎛┌─────┐⎞⎤⎫",
      "                                            ⎪⎢⎜│a²+b³ ⎟⎥⎪",
      "  ∀x∈ℝ: ⌈x⌉ = −⌊−x⌋, α ∧ ¬β = ¬(¬α ∨ β),    ⎪⎢⎜│───── ⎟⎥⎪",
      "                                            ⎪⎢⎜⎷ c₈   ⎟⎥⎪",
      "  ℕ ⊆ ℕ₀ ⊂ ℤ ⊂ ℚ ⊂ ℝ ⊂ ℂ,                   ⎨⎢⎜       ⎟⎥⎬",
      "                                            ⎪⎢⎜ ∞     ⎟⎥⎪",
      "  ⊥ < a ≠ b ≡ c ≤ d ≪ ⊤ ⇒ (⟦A⟧ ⇔ ⟪B⟫),      ⎪⎢⎜ ⎲     ⎟⎥⎪",
      "                                            ⎪⎢⎜ ⎳aⁱ-bⁱ⎟⎥⎪",
      "  2H₂ + O₂ ⇌ 2H₂O, R = 4.7 kΩ, ⌀ 200 mm     ⎩⎣⎝i=1    ⎠⎦⎭",
      "",
      "Linguistics and dictionaries:",
      "",
      "  ði ıntəˈnæʃənəl fəˈnɛtık əsoʊsiˈeıʃn",
      "  Y [ˈʏpsilɔn], Yen [jɛn], Yoga [ˈjoːgɑ]",
      "",
      "APL:",
      "",
      "  ((V⍳V)=⍳⍴V)/V←,V    ⌷←⍳→⍴∆∇⊃‾⍎⍕⌈"
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
      gt_endDraw( gt );
   }

   gt_clearAll( gt );
   return 0;
}
