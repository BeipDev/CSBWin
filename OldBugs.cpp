#include "stdafx.h"


/*

===========================================================
15 April 2001 Matti  DAGGERED MUMMY

When a dagger is thrown at a mummy and the mummy
dies then we get a assertion at GetDBRecordLink
  recName==RNeof
I can reproduce
15 April 2001  PRS
Fixed mal-translation of fffe -> RNempty.
Had to do with mummy absorbing the dagger.




===========================================================
15 April 2001 Matti   TWIRL IN STAIRWELL

When standing in a stairwell, go backward rapidly so that
you go up and down the stairs.  Repeat many times and
you leave the stairwell.  

I can reporduce this.
15 April 2001 PRS  

Fixed.  Kinda.  This was caused by the main loop
processing more than one mouse event per clock tick.
That in turn was done for the Record/Playback feature.
So I put things back as the Atari had it.  I will have
to fix the Record/Playback in some other way.  I cannot
even remember what is was that went wrong!  



===========================================================
15 April 2001 Matti   BREAKING UNDER LOAD

Excess load seems to sap energy from characters too fast.
A savegame was supplied.  I have not tried to reproduce.
15 April 2001 PRS
I found a place where the wrong number could be subtracted
and cause the load to go negative.  Therebye appearing to
be very large.  Hopefully this was the problem.
  While poking around I found that the Atari version causes
the weight of the object in hand to be added to the
character's load twice when the game is reloaded.  So
a character's load can grow to unlimited values by
saving and reloading again and again with a heavy object
in hand.
  I fixed this and patched the Atari code, too.


===========================================================
16 April 2001  Matti   NAME GAINS LEVEL
There is no space between the name and "gains
fighter level".
16 April 2001 PRS
Fixed by inserting a blank.


===========================================================
16 April 2001 Matti  FIRE SHIELD INDICATION
After casting a 4-5-4 spell a red rectangle
should appear around the portraits.
16 April 2001 PRS
Fixed.  Probably had other repercussions
because the definition of BITS4_9(x) was
missing bit 9!


*/
