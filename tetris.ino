/*
Ported to use LedControll 2015 AnneJan IJhack

Author: Jae Yeong Bae
        UBC ECE
        jocker.tistory.com       
Date:   Jan.18.2013
File:   Tetris
Purpose:
        killing time.. + for fun
   
Comment:
        This is my second Arduino Project. 
        Code may be messy and inefficient.
        References from Arduino Library and datasheets.
*/

#include "LedControl.h"

long delays = 0;
short delay_ = 500;
long bdelay = 0;
short buttondelay = 150;
short btdowndelay = 30;
short btsidedelay = 80;
unsigned char blocktype;
unsigned char blockrotation;

boolean  block[8][18]; //2 extra for rotation
boolean  pile[8][16];

boolean disp[8][16];

static const int DATA_PIN = 20;
static const int CLK_PIN  = 5;
static const int CS_PIN   = 21;
static const int DISPLAYS = 2;

static const int X_PIN = 4;
static const int Y_PIN = 5;

LedControl lc=LedControl(DATA_PIN, CLK_PIN, CS_PIN, DISPLAYS);

void setup() {
  Serial.begin(9600);
  Serial.println("Booted Tetris");
  
//  pinMode(A7, INPUT);
//  pinMode(A6, INPUT);

  int seed = 
  (analogRead(0)+1)*
  (analogRead(1)+1)*
  (analogRead(2)+1)*
  (analogRead(3)+1);
  randomSeed(seed);
  random(10,9610806);
  seed = seed *random(3336,15679912)+analogRead(random(4)) ;
  randomSeed(seed);  
  random(10,98046);

  // The MAX72XX is in power-saving mode on startup,
  // we have to do a wakeup call
  lc.shutdown(0, false);
  lc.shutdown(1, false);
  // Set the brightness
  lc.setIntensity(0, 15);
  lc.setIntensity(1, 15);
  // and clear the display
  lc.clearDisplay(0);
  lc.clearDisplay(1);
    
  newBlock();
  updateLED();    
}

void loop() {
  
  if (delays < millis())
   {
     delays = millis() + delay_;
     movedown();
   }
   
   

   //buttun actions
  int button = readDirection();
  
  if (button == 1) //up=rotate
    rotate();
  if (button == 2) //right=moveright
    moveright();    
  if (button == 3) //left=moveleft
    moveleft();
  if (button == 4) //down=movedown
    movedown();  
  
  Serial.print(analogRead(X_PIN));
  Serial.print("|");
  Serial.println(analogRead(Y_PIN));
   
  LEDRefresh();
}

boolean moveleft()
{  
  if (space_left())
  {
    int i;
    int j;
    for (i=0;i<7;i++)
    {
      for (j=0;j<16;j++)      
      {
        block[i][j]=block[i+1][j];
      }
    }
    
    for (j=0;j<16;j++)      
    {
      block[7][j]=0;
    }    

    updateLED();
    return 1;
  }

  return 0;
}

boolean moveright()
{
  if (space_right())
  {
    int i;
    int j;
    for (i=7;i>0;i--)
    {
      for (j=0;j<16;j++)      
      {
        block[i][j]=block[i-1][j];
      }
    }

    for (j=0;j<16;j++)      
    {
      block[0][j]=0;
    }    
    
   updateLED(); 
   return 1;   
  
  }
  return 0;
}

int readDirection()
{
  if (bdelay > millis())
  {
    return 0;
  }
  if (analogRead(X_PIN) > 750)
  {
    //left
    bdelay = millis() + btsidedelay;    
    return 3;
  }
  
  if (analogRead(Y_PIN) > 750)
  {
    //down
    bdelay = millis() + btdowndelay;    
    return 4;
  }    
  if (analogRead(X_PIN) < 225)
  {
    //right
    bdelay = millis() + btsidedelay;
    return 2;
  }  
  if (analogRead(Y_PIN) < 225)
  {
    //up
    bdelay = millis() + buttondelay;
    return 1;
  }  
  
  return 0;
}

void updateLED()
{
  int i;
  int j;  
  for (i=0;i<8;i++)
  {
    for (j=0;j<16;j++)
    {
      disp[i][j] = block[i][j] | pile[i][j];
    }
  }
}

void rotate()
{
  
  //skip for square block(3)
  if (blocktype == 3) return;
  
  int xi;
  int yi;
  int i;
  int j;
  //detect left
  for (i=7;i>=0;i--)
  {
    for (j=0;j<16;j++)
    {
      if (block[i][j])
      {
        xi = i;
      }
    }
  }
  
  //detect up
  for (i=15;i>=0;i--)
  {
    for (j=0;j<8;j++)
    {
      if (block[j][i])
      {
        yi = i;
      }
    }
  }  
    
  if (blocktype == 0)
  {
    if (blockrotation == 0) 
    {
      
      
      if (!space_left())
      {
        if (space_right3())
        {
          if (!moveright())
            return;
          xi++;
        }
        else return;
      }     
      else if (!space_right())
      {
        if (space_left3())
        {
          if (!moveleft())
            return;
          if (!moveleft())
            return;          
          xi--;
          xi--;        
        }
        else
          return;
      }
      else if (!space_right2())
      {
        if (space_left2())
        {
          if (!moveleft())
            return;          
          xi--;      
        }
        else
          return;
      }   
   
      
   
      
      
      block[xi][yi]=0;
      block[xi][yi+2]=0;
      block[xi][yi+3]=0;      
      
      block[xi-1][yi+1]=1;
      block[xi+1][yi+1]=1;
      block[xi+2][yi+1]=1;      

      blockrotation = 1;
    }
    else
    {
      block[xi][yi]=0;
      block[xi+2][yi]=0;
      block[xi+3][yi]=0;
      
      block[xi+1][yi-1]=1;
      block[xi+1][yi+1]=1;
      block[xi+1][yi+2]=1;

      blockrotation = 0;
    }    
  }
  
  //offset to mid
  xi ++;  
  yi ++;  
  
  if (blocktype == 1)
  {
    if (blockrotation == 0)
    {
      block[xi-1][yi-1] = 0;
      block[xi-1][yi] = 0;
      block[xi+1][yi] = 0;

      block[xi][yi-1] = 1;
      block[xi+1][yi-1] = 1;
      block[xi][yi+1] = 1;      
      
      blockrotation = 1;
    }
    else if (blockrotation == 1)
    {
      if (!space_left())
      {
        if (!moveright())
          return;
        xi++;
      }        
      xi--;
      
      block[xi][yi-1] = 0;
      block[xi+1][yi-1] = 0;
      block[xi][yi+1] = 0;      
      
      block[xi-1][yi] = 1;
      block[xi+1][yi] = 1;
      block[xi+1][yi+1] = 1;      
      
      blockrotation = 2;      
    }
    else if (blockrotation == 2)
    {
      yi --;
      
      block[xi-1][yi] = 0;
      block[xi+1][yi] = 0;
      block[xi+1][yi+1] = 0;      
      
      block[xi][yi-1] = 1;
      block[xi][yi+1] = 1;
      block[xi-1][yi+1] = 1;      
      
      blockrotation = 3;            
    }
    else
    {
      if (!space_right())
      {
        if (!moveleft())
          return;
        xi--;
      }
      block[xi][yi-1] = 0;
      block[xi][yi+1] = 0;
      block[xi-1][yi+1] = 0;        

      block[xi-1][yi-1] = 1;
      block[xi-1][yi] = 1;
      block[xi+1][yi] = 1;
      
      blockrotation = 0;          
    }  
  }



  if (blocktype == 2)
  {
    if (blockrotation == 0)
    {
      block[xi+1][yi-1] = 0;
      block[xi-1][yi] = 0;
      block[xi+1][yi] = 0;

      block[xi][yi-1] = 1;
      block[xi+1][yi+1] = 1;
      block[xi][yi+1] = 1;      
      
      blockrotation = 1;
    }
    else if (blockrotation == 1)
    {
      if (!space_left())
      {
        if (!moveright())
          return;
        xi++;
      }              
      xi--;
      
      block[xi][yi-1] = 0;
      block[xi+1][yi+1] = 0;
      block[xi][yi+1] = 0;      
      
      block[xi-1][yi] = 1;
      block[xi+1][yi] = 1;
      block[xi-1][yi+1] = 1;      
      
      blockrotation = 2;      
    }
    else if (blockrotation == 2)
    {
      yi --;
      
      block[xi-1][yi] = 0;
      block[xi+1][yi] = 0;
      block[xi-1][yi+1] = 0;      
      
      block[xi][yi-1] = 1;
      block[xi][yi+1] = 1;
      block[xi-1][yi-1] = 1;      
      
      blockrotation = 3;            
    }
    else
    {
      if (!space_right())
      {
        if (!moveleft())
          return;
        xi--;
      }      
      block[xi][yi-1] = 0;
      block[xi][yi+1] = 0;
      block[xi-1][yi-1] = 0;        

      block[xi+1][yi-1] = 1;
      block[xi-1][yi] = 1;
      block[xi+1][yi] = 1;
      
      blockrotation = 0;          
    }  
  }
  
  if (blocktype == 4)
  {
    if (blockrotation == 0)
    {
      block[xi+1][yi-1] = 0;
      block[xi-1][yi] = 0;

      block[xi+1][yi] = 1;
      block[xi+1][yi+1] = 1;      
      
      blockrotation = 1;
    }
    else
    {
      if (!space_left())
      {
        if (!moveright())
          return;
        xi++;
      }              
      xi--;
      
      block[xi+1][yi] = 0;
      block[xi+1][yi+1] = 0;      
      
      block[xi-1][yi] = 1;
      block[xi+1][yi-1] = 1;
      
      blockrotation = 0;          
    }  
  }  


  if (blocktype == 5)
  {
    if (blockrotation == 0)
    {
      block[xi][yi-1] = 0;
      block[xi-1][yi] = 0;
      block[xi+1][yi] = 0;

      block[xi][yi-1] = 1;
      block[xi+1][yi] = 1;
      block[xi][yi+1] = 1;      
      
      blockrotation = 1;
    }
    else if (blockrotation == 1)
    {
      if (!space_left())
      {
        if (!moveright())
          return;
        xi++;
      }              
      xi--;
      
      block[xi][yi-1] = 0;
      block[xi+1][yi] = 0;
      block[xi][yi+1] = 0;
      
      block[xi-1][yi] = 1;
      block[xi+1][yi] = 1;
      block[xi][yi+1] = 1;
      
      blockrotation = 2;      
    }
    else if (blockrotation == 2)
    {
      yi --;
      
      block[xi-1][yi] = 0;
      block[xi+1][yi] = 0;
      block[xi][yi+1] = 0;     
      
      block[xi][yi-1] = 1;
      block[xi-1][yi] = 1;
      block[xi][yi+1] = 1;      
      
      blockrotation = 3;            
    }
    else
    {
      if (!space_right())
      {
        if (!moveleft())
          return;
        xi--;
      }      
      block[xi][yi-1] = 0;
      block[xi-1][yi] = 0;
      block[xi][yi+1] = 0;      
      
      block[xi][yi-1] = 1;
      block[xi-1][yi] = 1;
      block[xi+1][yi] = 1;
      
      blockrotation = 0;          
    }  
  }
  
  if (blocktype == 6)
  {
    if (blockrotation == 0)
    {
      block[xi-1][yi-1] = 0;
      block[xi][yi-1] = 0;

      block[xi+1][yi-1] = 1;
      block[xi][yi+1] = 1;      
      
      blockrotation = 1;
    }
    else
    {
      if (!space_left())
      {
        if (!moveright())
          return;
        xi++;
      }              
      xi--;
      
      block[xi+1][yi-1] = 0;
      block[xi][yi+1] = 0;      
      
      block[xi-1][yi-1] = 1;
      block[xi][yi-1] = 1;
      
      blockrotation = 0;          
    }  
  }  
  






  //if rotating made block and pile overlap, push rows up
  while (!check_overlap())
  {
    for (i=0;i<18;i++)
    {
      for (j=0;j<8;j++)
      {
         block[j][i] = block[j][i+1];
      }
    }
    delays = millis() + delay_;
  }
  
  
  updateLED();


    
}

void movedown()
{
  if (space_below())
  {
    //move down
    int i;
    for (i=15;i>=0;i--)
    {
      int j;
      for (j=0;j<8;j++)
      {
        block[j][i] = block[j][i-1];
      }
    }
    for (i=0;i<7;i++)
    {
      block[i][0] = 0;
    }
  }
  else
  {
    //merge and new block
    int i;
    int j;    
    for (i=0;i<8;i++)
    {
     for(j=0;j<16;j++)
     {
       if (block[i][j])
       {
         pile[i][j]=1;
         block[i][j]=0;
       }
     }
    }
    newBlock();   
  }
  updateLED();  
}

boolean check_overlap()
{
  int i;
  int j;  
  for (i=0;i<16;i++)
  {
    for (j=0;j<7;j++)
    {
       if (block[j][i])
       {
         if (pile[j][i])
           return false;
       }        
    }
  }
  for (i=16;i<18;i++)
  {
    for (j=0;j<7;j++)
    {
       if (block[j][i])
       {
         return false;
       }        
    }
  }  
  return true;
}

void check_gameover()
{
  int i;
  int j;
  int cnt=0;;
  
  for(i=15;i>=0;i--)
  {
    cnt=0;
    for (j=0;j<8;j++)
    {
      if (pile[j][i])
      {
        cnt ++;
      }
    }    
    if (cnt == 8)
    {
      for (j=0;j<8;j++)
      {
        pile[j][i]=0;
      }        
      updateLED();
      delay(50);
      
      int k;
      for(k=i;k>0;k--)
      {
        for (j=0;j<8;j++)
        {
          pile[j][k] = pile[j][k-1];
        }                
      }
      for (j=0;j<8;j++)
      {
        pile[j][0] = 0;
      }        
      updateLED();      
      delay(50);      
      i++;     
      
      
    
    }
  }  
  
  
  for(i=0;i<8;i++)
  {
    if (pile[i][0])
      gameover();
  }
  return;
}

void gameover()
{
  int i;
  int j;
  for (i=0;i<8;i++)
  {
     for (j=0;j<16;j++)
     {
       if (j%2)
       {
         disp[i][j]=1;
       }
       else
       {
         disp[7-i][j]=1;        
       }
     }
     delay(60);
  }
 
  for (i=0;i<8;i++)
  {
     for (j=0;j<16;j++)
     {
       if (j%2)
       {
         disp[i][j]=0;
       }
       else
       {
         disp[7-i][j]=0;       
       }
     }
     delay(60);
  }       
       
       
       
       
  
  while(true)
  {
  }
  
}

void newBlock()
{
  check_gameover();
  
  
  blocktype = random(7);

  
  if (blocktype == 0)
  // 0
  // 0
  // 0
  // 0
  {
    block[3][0]=1;
    block[3][1]=1;
    block[3][2]=1;
    block[3][3]=1;      
  }

  if (blocktype == 1)
  // 0
  // 0 0 0
  {
    block[2][0]=1;
    block[2][1]=1;
    block[3][1]=1;
    block[4][1]=1;        
  }
  
  if (blocktype == 2)
  //     0
  // 0 0 0
  {
    block[4][0]=1;
    block[2][1]=1;
    block[3][1]=1;
    block[4][1]=1;         
  }

  if (blocktype == 3)
  // 0 0
  // 0 0
  {
    block[3][0]=1;
    block[3][1]=1;
    block[4][0]=1;
    block[4][1]=1;          
  }    

  if (blocktype == 4)
  //   0 0
  // 0 0
  {
    block[4][0]=1;
    block[5][0]=1;
    block[3][1]=1;
    block[4][1]=1;         
  }    
  
  if (blocktype == 5)
  //   0
  // 0 0 0
  {
    block[4][0]=1;
    block[3][1]=1;
    block[4][1]=1;
    block[5][1]=1;       
  }        

  if (blocktype == 6)
  // 0 0
  //   0 0
  {
    block[3][0]=1;
    block[4][0]=1;
    block[4][1]=1;
    block[5][1]=1;         
  }    

  blockrotation = 0;
}

boolean space_below()
{ 
  int i;
  int j;  
  for (i=15;i>=0;i--)
  {
    for (j=0;j<8;j++)
    {
       if (block[j][i])
       {
         if (i == 15)
           return false;
         if (pile[j][i+1])
         {
           return false;
         }      
       }        
    }
  }
  return true;
}

boolean space_left2()
{ 
  int i;
  int j;  
  for (i=15;i>=0;i--)
  {
    for (j=0;j<8;j++)
    {
       if (block[j][i])
       {
         if (j == 0 || j == 1)
           return false;
         if (pile[j-1][i] | pile[j-2][i])
         {
           return false;
         }      
       }        
    }
  }
  return true;
}

boolean space_left3()
{ 
  int i;
  int j;  
  for (i=15;i>=0;i--)
  {
    for (j=0;j<8;j++)
    {
       if (block[j][i])
       {
         if (j == 0 || j == 1 ||j == 2 )
           return false;
         if (pile[j-1][i] | pile[j-2][i]|pile[j-3][i])
         {
           return false;
         }      
       }        
    }
  }
  return true;
}

boolean space_left()
{ 
  int i;
  int j;  
  for (i=15;i>=0;i--)
  {
    for (j=0;j<8;j++)
    {
       if (block[j][i])
       {
         if (j == 0)
           return false;
         if (pile[j-1][i])
         {
           return false;
         }      
       }        
    }
  }
  return true;
}

boolean space_right()
{ 
  int i;
  int j;  
  for (i=15;i>=0;i--)
  {
    for (j=0;j<8;j++)
    {
       if (block[j][i])
       {
         if (j == 7)
           return false;
         if (pile[j+1][i])
         {
           return false;
         }      
       }        
    }
  }
  return true;
}

boolean space_right3()
{ 
  int i;
  int j;  
  for (i=15;i>=0;i--)
  {
    for (j=0;j<8;j++)
    {
       if (block[j][i])
       {
         if (j == 7||j == 6||j == 5)
           return false;
         if (pile[j+1][i] |pile[j+2][i] | pile[j+3][i])
         {
           return false;
         }      
       }        
    }
  }
  return true;
}

boolean space_right2()
{ 
  int i;
  int j;  
  for (i=15;i>=0;i--)
  {
    for (j=0;j<8;j++)
    {
       if (block[j][i])
       {
         if (j == 7 || j == 6)
           return false;
         if (pile[j+1][i] |pile[j+2][i])
         {
           return false;
         }      
       }        
    }
  }
  return true;
}

void LEDRefresh()
{
    int i;
    int k;
    //////////////////////////////////////////////
  
    for(i=0;i<8;i++)
    {            
       byte upper = 0;
       int b;
       for(b = 0;b<8;b++)
       {
         upper <<= 1;
         if (disp[b][i]) upper |= 1;
       }
       
       lc.setRow(0, i, upper);
       
       //Serial.print(upper);
       
       byte lower = 0;
       for(b = 0;b<8;b++)
       {
         lower <<= 1;
         if (disp[b][i+8]) lower |= 1;
       }
       
      lc.setRow(1, i, lower);
      
      //Serial.println(lower);
    } 
}
