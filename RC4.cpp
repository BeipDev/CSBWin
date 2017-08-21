#include "stdafx.h"

//*********WARNING*************************
// This is NOT the RC4 encryption algorithm!!!!
// Just the key preparation is from RC4.
// The rest is garbage that I invented and is
// not secure in any way!!!!

#define SIZE 8192

struct RC4_KEY
{      
   unsigned char  m_state[SIZE];
   unsigned char x;        
   unsigned char y;
};
void prepare_key(unsigned char *key_data_ptr,i32 key_data_len);
void rc4(unsigned char *buffer_ptr,i32 buffer_len);

#define swap_byte(a,b) (a)^=(b);(b)^=(a);(a)^=(b);


static RC4_KEY rc4_key;

void RC4_prepare_key(unsigned char *key_data_ptr, i32 key_data_len)
{
   unsigned char index1;
   unsigned char index2;
   i32 xorIndex;
   unsigned char* state;
   short counter;     

   state = &rc4_key.m_state[0];         
   for(counter = 0; counter < SIZE; counter++)              
   state[counter] = (unsigned char)counter;               
   rc4_key.x = 0;     
   rc4_key.y = 0;     
   index1 = 0;     
   index2 = 0;             
   for(counter = 0; counter < 256; counter++)      
   {               
      index2 = (unsigned char)((key_data_ptr[index1] + state[counter] +
                index2) % 256);                
      swap_byte(state[counter], state[index2]);            
      
      index1 = (unsigned char)((index1 + 1) % key_data_len);  
   }       

   
   // Create a few more random numbers.
   for(counter = 256; counter < SIZE; counter ++)      
   {               
      rc4_key.x = (unsigned char)((rc4_key.x + 1) % 256);                      
      rc4_key.y = (unsigned char)((state[rc4_key.x] + rc4_key.y) % 256);               
      swap_byte(state[rc4_key.x], state[rc4_key.y]);                        
      
      xorIndex = (state[rc4_key.x] + state[rc4_key.y]) % 256;                 
      
      state[counter] ^= state[xorIndex];         
   }               
}

void RC4_encipher(unsigned char *buffer_ptr, 
                  i32 buffer_len,
                  i32 position)
{ 
   i32 i;
   
   position = position % SIZE;

   for (i=0; i<buffer_len; i++)
   {
     buffer_ptr[i] ^= rc4_key.m_state[position];
     position = (position + 1) % SIZE;
   };
}

