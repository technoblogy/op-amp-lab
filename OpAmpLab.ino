/* Pocket Op-Amp Lab - see http://www.technoblogy.com/show?3CHT

   David Johnson-Davies - www.technoblogy.com - 12th January 2021
   AVR128DB28 @ 24 MHz (internal oscillator; BOD disabled)
   
   CC BY 4.0
   Licensed under a Creative Commons Attribution 4.0 International license: 
   http://creativecommons.org/licenses/by/4.0/
*/

#include <SPI.h>
#include <EEPROM.h>

// Adafruit 1.14" 240x135 display
int const ysize = 135, xsize = 240, yoff = 53, xoff = 40, invert = 1, rotate = 6;

// AliExpress 1.14" 240x135 display
//int const ysize = 135, xsize = 240, yoff = 52, xoff = 40, invert = 1, rotate = 0;

// Arduino pins
int const dc = 2;
int const cs = 3;

// Character set for upper case - stored in program memory
const uint8_t CharMap[64][6] PROGMEM = {
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 
{ 0x00, 0x00, 0x5F, 0x00, 0x00, 0x00 }, 
{ 0x00, 0x07, 0x00, 0x07, 0x00, 0x00 }, 
{ 0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00 }, 
{ 0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x00 }, 
{ 0x23, 0x13, 0x08, 0x64, 0x62, 0x00 }, 
{ 0x36, 0x49, 0x56, 0x20, 0x50, 0x00 }, 
{ 0x00, 0x08, 0x07, 0x03, 0x00, 0x00 }, 
{ 0x00, 0x1C, 0x22, 0x41, 0x00, 0x00 }, 
{ 0x00, 0x41, 0x22, 0x1C, 0x00, 0x00 }, 
{ 0x2A, 0x1C, 0x7F, 0x1C, 0x2A, 0x00 }, 
{ 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00 }, 
{ 0x00, 0x80, 0x70, 0x30, 0x00, 0x00 }, 
{ 0x08, 0x08, 0x08, 0x08, 0x08, 0x00 }, 
{ 0x00, 0x00, 0x60, 0x60, 0x00, 0x00 }, 
{ 0x20, 0x10, 0x08, 0x04, 0x02, 0x00 }, 
{ 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00 }, 
{ 0x00, 0x42, 0x7F, 0x40, 0x00, 0x00 }, 
{ 0x72, 0x49, 0x49, 0x49, 0x46, 0x00 }, 
{ 0x21, 0x41, 0x49, 0x4D, 0x33, 0x00 }, 
{ 0x18, 0x14, 0x12, 0x7F, 0x10, 0x00 }, 
{ 0x27, 0x45, 0x45, 0x45, 0x39, 0x00 }, 
{ 0x3C, 0x4A, 0x49, 0x49, 0x31, 0x00 }, 
{ 0x41, 0x21, 0x11, 0x09, 0x07, 0x00 }, 
{ 0x36, 0x49, 0x49, 0x49, 0x36, 0x00 }, 
{ 0x46, 0x49, 0x49, 0x29, 0x1E, 0x00 }, 
{ 0x00, 0x00, 0x14, 0x00, 0x00, 0x00 }, 
{ 0x00, 0x40, 0x34, 0x00, 0x00, 0x00 }, 
{ 0x00, 0x08, 0x14, 0x22, 0x41, 0x00 }, 
{ 0x14, 0x14, 0x14, 0x14, 0x14, 0x00 }, 
{ 0x00, 0x41, 0x22, 0x14, 0x08, 0x00 }, 
{ 0x02, 0x01, 0x59, 0x09, 0x06, 0x00 }, 
{ 0x3E, 0x41, 0x5D, 0x59, 0x4E, 0x00 }, 
{ 0x7C, 0x12, 0x11, 0x12, 0x7C, 0x00 }, 
{ 0x7F, 0x49, 0x49, 0x49, 0x36, 0x00 }, 
{ 0x3E, 0x41, 0x41, 0x41, 0x22, 0x00 }, 
{ 0x7F, 0x41, 0x41, 0x41, 0x3E, 0x00 }, 
{ 0x7F, 0x49, 0x49, 0x49, 0x41, 0x00 }, 
{ 0x7F, 0x09, 0x09, 0x09, 0x01, 0x00 }, 
{ 0x3E, 0x41, 0x41, 0x51, 0x73, 0x00 }, 
{ 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00 }, 
{ 0x00, 0x41, 0x7F, 0x41, 0x00, 0x00 }, 
{ 0x20, 0x40, 0x41, 0x3F, 0x01, 0x00 }, 
{ 0x7F, 0x08, 0x14, 0x22, 0x41, 0x00 }, 
{ 0x7F, 0x40, 0x40, 0x40, 0x40, 0x00 }, 
{ 0x7F, 0x02, 0x1C, 0x02, 0x7F, 0x00 }, 
{ 0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00 }, 
{ 0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00 }, 
{ 0x7F, 0x09, 0x09, 0x09, 0x06, 0x00 }, 
{ 0x3E, 0x41, 0x51, 0x21, 0x5E, 0x00 }, 
{ 0x7F, 0x09, 0x19, 0x29, 0x46, 0x00 }, 
{ 0x26, 0x49, 0x49, 0x49, 0x32, 0x00 }, 
{ 0x03, 0x01, 0x7F, 0x01, 0x03, 0x00 }, 
{ 0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00 }, 
{ 0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00 }, 
{ 0x3F, 0x40, 0x38, 0x40, 0x3F, 0x00 }, 
{ 0x63, 0x14, 0x08, 0x14, 0x63, 0x00 }, 
{ 0x03, 0x04, 0x78, 0x04, 0x03, 0x00 }, 
{ 0x61, 0x59, 0x49, 0x4D, 0x43, 0x00 }, 
{ 0x00, 0x7F, 0x41, 0x41, 0x41, 0x00 }, 
{ 0x02, 0x04, 0x08, 0x10, 0x20, 0x00 }, 
{ 0x00, 0x41, 0x41, 0x41, 0x7F, 0x00 }, 
{ 0x04, 0x02, 0x01, 0x02, 0x04, 0x00 }, 
{ 0x40, 0x40, 0x40, 0x40, 0x40, 0x00 }
};

// Points **********************************************

typedef struct {
  int x;
  int y;
} point;

// Make + and - work with points

point operator+ (const point &lhs, const point &rhs) {
  point temp;
  temp.x = lhs.x + rhs.x;
  temp.y = lhs.y + rhs.y;
  return temp;
}

point operator- (const point &lhs, const point &rhs) {
  point temp;
  temp.x = lhs.x - rhs.x;
  temp.y = lhs.y - rhs.y;
  return temp;
}

// The grid **********************************************

const int cx = 6, cy = 9; // Character cells sizes

#define Grid(opn, x, y) (point){(opn*20+(x))*cx, (y)*cy}
#define GridX(opn, x, y) (point){(opn*20+(x))*cx+2, (y)*cy+4}

// Constant points
const point origin = {0, 0};
const point oneline = {0, cy}; 

// Current plot position
point pt0 = {0, 0};

// TFT colour display **********************************************

int const CASET = 0x2A; // Define column address
int const RASET = 0x2B; // Define row address
int const RAMWR = 0x2C; // Write to display RAM

const int WhiteColour = 0xFFFF;
const int BlackColour = 0;
const int RedColour   = 0xF800; // 0x07E0;

// Colours
int fore = WhiteColour;
int back = BlackColour;
int scale = 1;     // Text scale

// Send a byte to the display
void Data (uint8_t d) {
  digitalWrite(cs, LOW);
  SPI.transfer(d);
  digitalWrite(cs, HIGH);
}

// Send a command to the display
void Command (uint8_t c) {
  digitalWrite(dc, LOW);
  Data(c);
  digitalWrite(dc, HIGH);
}

// Send a command followed by two data words
void Command2 (uint8_t c, uint16_t d1, uint16_t d2) {
  digitalWrite(dc, LOW);
  Data(c);
  digitalWrite(dc, HIGH);
  Data(d1>>8); Data(d1); Data(d2>>8); Data(d2);
}
  
void InitDisplay () {
  pinMode(dc, OUTPUT);
  pinMode(cs, OUTPUT);
  digitalWrite(cs, HIGH);  
  digitalWrite(dc, HIGH);                  // Data
  SPI.begin();
  Command(0x01);                           // Software reset
  delay(150);                              // delay 150 ms
  Command(0x11);                           // Out of sleep mode
  delay(500);                              // delay 500 ms
  Command(0x3A); Data(0x55);               // Set color mode - 16-bit color
  Command(0x20+invert);                    // Invert
  Command(0x36); Data(rotate<<5);          // Set orientation
}

void DisplayOn () {
  Command(0x29);                           // Display on
  delay(10);
}

void ClearDisplay () {
  Command2(CASET, yoff, yoff + ysize - 1);
  Command2(RASET, xoff, xoff + xsize - 1);
  Command(0x3A); Data(0x03);               // 12-bit colour
  Command(RAMWR);
  for (int i=0; i<xsize/2; i++) {
    for (int j=0; j<ysize * 3; j++) {
      Data(0);
    }
  }
  Command(0x3A); Data(0x05);               // Back to 16-bit colour
}

// Move current plot position
void MoveTo (point pt) {
  pt0 = pt;
}

// Plot point at x,y
void PlotPoint (point pt) {
  int x = pt.x, y = pt.y;
  Command2(CASET, yoff+y, yoff+y);
  Command2(RASET, xoff+x, xoff+x);
  Command(RAMWR); Data(fore>>8); Data(fore & 0xff);
}

// Draw a line to x,y
void DrawTo (point pt) {
  int sx, sy, e2, err;
  int dx = abs(pt.x - pt0.x);
  int dy = abs(pt.y - pt0.y);
  if (pt0.x < pt.x) sx = 1; else sx = -1;
  if (pt0.y < pt.y) sy = 1; else sy = -1;
  err = dx - dy;
  for (;;) {
    PlotPoint(pt0);
    if (pt0.x==pt.x && pt0.y==pt.y) return;
    e2 = err<<1;
    if (e2 > -dy) { err = err - dy; pt0.x = pt0.x + sx; }
    if (e2 < dx) { err = err + dx; pt0.y = pt0.y + sy; }
  }
}

// Plot an ASCII character with bottom left corner at pt0
void PlotChar (char c) {
  int colour;
  Command2(CASET, yoff+pt0.y, yoff+pt0.y+8*scale-1);
  Command2(RASET, xoff+pt0.x, xoff+pt0.x+6*scale-1);
  Command(RAMWR);
  for (int xx=0; xx<6; xx++) {
    int bits = pgm_read_byte(&CharMap[c-32][xx]);
    for (int xr=0; xr<scale; xr++) {
      for (int yy=0; yy<8; yy++) {
        if (bits>>(7-yy) & 1) colour = fore; else colour = back;
        for (int yr=0; yr<scale; yr++) {
          Data(colour>>8); Data(colour & 0xFF);
        }
      }
    }
  }
  pt0.x = pt0.x + 6*scale;
}

// Plot text starting at the current plot position
void PlotText(const char *c) {
  while (1) {
    if (*c == 0) return;
    PlotChar(*c++);
  }
}

// Op amp designer **********************************************

const int nMenus = 6;
const int nOpAmps = 2;

uint8_t State[nOpAmps][nMenus];

typedef void (*fn_ptr_t)(uint8_t opn, uint8_t index);

typedef struct {
  const char name[7];
  uint8_t options;
  register8_t *reg;
  uint8_t mask;
  uint8_t position;
  const char **values;
  fn_ptr_t drawfn;
} menu_t;

// Key points on the diagram

point Inp (int opn) { return GridX(opn, 4, 13); }
point Inn (int opn) { return GridX(opn, 4, 9); }
point Out (int opn) { return GridX(opn, 16, 11); }
point Bot (int opn) { return GridX(opn, 4, 5); }
point Wip (int opn) { return GridX(opn, 10, 5); }
point Top (int opn) { return GridX(opn, 16, 5); }
point InpLabel (int opn) { return Grid(opn, 1, 13); }
point InnLabel (int opn) { return Grid(opn, 1, 9); }
point OutLabel (int opn) { return Grid(opn, 17, 11); }
point TopLabel (int opn) { return Grid(opn, 17, 5); }
point BotLabel (int opn) { return Grid(opn, 1, 5); }

void DrawGnd (point pt) {
  MoveTo(pt); DrawTo(pt - (point){cx, 0});
  DrawTo(pt - (point){cx, cy});
  for (int i=3; i>0; i--) {
    MoveTo(pt - (point){cx+i, (6-2*i+cy)});
    DrawTo(pt - (point){cx-i, (6-2*i+cy)});
  }
}

void LabelInp (point pt, int opn) {
  MoveTo(pt); PlotText("INP");
  MoveTo(pt - oneline); PlotText("PD"); PlotChar('0'+1+3*opn);
}

void LabelInn (point pt, int opn) {
  MoveTo(pt); PlotText("INN");
  MoveTo(pt - oneline); PlotText("PD"); PlotChar('0'+3+4*opn);
}

void LabelOut (point pt, int opn) {
  MoveTo(pt); PlotText("OUT");
  MoveTo(pt - oneline); PlotText("PD"); PlotChar('0'+2+3*opn);
}

void LabelOff (point pt) {
  MoveTo(pt); PlotText("   ");
  MoveTo(pt - oneline); PlotText("   ");
}

void DrawOpAmp (int opn) {
  MoveTo(GridX(opn, 6, 14)); DrawTo(GridX(opn, 6, 8)); 
  DrawTo(GridX(opn, 14, 11)); DrawTo(GridX(opn, 6, 14));
  // Inputs
  MoveTo(Inp(opn)); DrawTo(Inp(opn) + (point){2*cx, 0});
  MoveTo(Inn(opn)); DrawTo(Inn(opn) + (point){2*cx, 0});
  MoveTo(Out(opn)); DrawTo(Out(opn) - (point){2*cx, 0});
  // Labels
  MoveTo(InpLabel(opn) + (point){6*cx, -1}); PlotChar('+');
  MoveTo(InnLabel(opn) + (point){6*cx, 1}); PlotChar('-');
  MoveTo((point){opn*20*cx+8*cx, 11*cy-4}); scale = 2; PlotChar('0'+opn); scale = 1;
  // Label output
  LabelOut(OutLabel(opn), opn);
}

void DrawResistors (int opn) {
  MoveTo(Bot(opn));
  for (int j=5; j<=11; j=j+6) {
    DrawTo((point){opn*20*cx+j*cx+2, 5*cy+4});
    for (int i=0; i<2; i++) {
      DrawTo((point){opn*20*cx+j*cx+2+3+12*i, 5*cy+4+3});
      DrawTo((point){opn*20*cx+j*cx+2+9+12*i, 5*cy+4-3});
      DrawTo((point){opn*20*cx+j*cx+2+12+12*i, 5*cy+4});
    }
  }
  DrawTo(Top(opn));
}

// These functions draw the circuit corresponding to each of the menus

enum menus { OP, MUXPOS, MUXNEG, MUXBOT, MUXWIP, MUXTOP };

void MuxPosFn (uint8_t opn, uint8_t index) {
  if (index == 0) { // INP
    if (State[opn][1] != 1) LabelInp(InpLabel(opn), opn);
  } else if (index == 1) { // WIP
    MoveTo(Inp(opn)); DrawTo(Inp(opn) - (point){4*cx, 0}); DrawTo(Inp(opn) - (point){4*cx, 7*cy});
    DrawTo(Wip(opn) + (point){0, 1*cy}); DrawTo(Wip(opn) + (point){0, 1});
    if (State[opn][MUXNEG] == 1 && fore == BlackColour) { // Redraw MusNeg WIP
      fore = WhiteColour;
      MuxNegFn(opn, 1);
    }
  } else if (index == 2) { // DAC
    MoveTo(InpLabel(opn)); PlotText("DAC");
  } else if (index == 3) { // GND
    DrawGnd(Inp(opn));
  } else if (index == 4) { // VDDDIV2
    MoveTo(InpLabel(opn)); PlotText("V/2");
  } else if (index == 5 && opn == 1) { // LINKOUT
    LabelOff(OutLabel(0));
    MoveTo(Out(0)); DrawTo(Out(0) + point{4*cx-2, 0});
    DrawTo(Inp(1) - point{4*cx+2, 0}); DrawTo(Inp(1));
    if (fore == BlackColour) {
      fore = WhiteColour;
      if (State[1][MUXBOT] == 4) { // Redraw MusPos LINOUT
        MuxBotFn(opn, 4);
      } else { // Kludge to reinstate label
        LabelOut(OutLabel(0), 0);
      }
    }
  }
}

void MuxNegFn (uint8_t opn, uint8_t index) {
  if (index == 0) { // OFF
    LabelInn(InnLabel(opn), opn);
  } else if (index == 1) { // WIP
    MoveTo(Inn(opn)); DrawTo(Inn(opn) - (point){0, 2*cy});
    DrawTo(Wip(opn) + (point){0, 2*cy}); DrawTo(Wip(opn) + (point){0, 1});
    if (State[opn][MUXPOS] == 1 && fore == BlackColour) { // Redraw MusPos WIP
      fore = WhiteColour;
      MuxPosFn(opn, 1);
    }
  } else if (index == 2) { // OUT
    MoveTo(Inn(opn)); DrawTo(Inn(opn) - (point){0, 2*cy});
    DrawTo(Out(opn) - (point){0, 4*cy}); DrawTo(Out(opn));
    if (State[opn][MUXTOP] == 1 && fore == BlackColour) { // Redraw MuxTop OUT
      fore = WhiteColour;
      MuxTopFn(opn, 1);
    }
  } else if (index == 3) { // DAC
    MoveTo(InnLabel(opn)); PlotText("DAC");
  }
}

void MuxBotFn (uint8_t opn, uint8_t index) {
  if (index == 1) { // INP
    LabelInp(BotLabel(opn), opn);
  } else if (index == 2) { // INN
    LabelInn(BotLabel(opn), opn);
  } else if (index == 3) { // DAC
    MoveTo(BotLabel(opn)); PlotText("DAC");
  } else if (index == 4 && opn == 1) { // LINKOUT
    LabelOff(OutLabel(0));
    MoveTo(Out(0)); DrawTo(Out(0) + (point){4*cx-2, 0});
    DrawTo((point){20*cx+2-2, 5*cy+4});
    DrawTo(Bot(1) - (point){2, 0}); DrawTo(Bot(1));
    if (fore == BlackColour) {
      fore = WhiteColour;
      if (State[1][MUXPOS] == 5) { // Redraw MusPos LINOUT
        MuxPosFn(opn, 5);
      } else { // Kludge to reinstate label
        LabelOut(OutLabel(0), 0);
      }
    }
  } else if (index == 5) { // GND
    DrawGnd(Bot(opn));
  }
}

const int WipR1[8] = {15, 14, 12, 8, 6, 4, 2, 1};

void MuxWipFn (uint8_t opn, uint8_t index) {
  int r = WipR1[index];
  for (int i=0; i<2; i++) {
    int two = (r > 9);
    MoveTo((point){opn*20*cx+6*cx+6*i*cx-3*two+3, 4*cy});
    if (two) PlotChar('1');
    PlotChar('0'+r%10);
    PlotChar('R'); PlotChar(' ');
    r = 16-r;
  }
}

void MuxTopFn (uint8_t opn, uint8_t index) {
  if (index == 1) { // OUT
    MoveTo(Top(opn) + (point){0, 1}); DrawTo(Out(opn) - (point){0, 1});
    if (State[opn][MUXNEG] == 2 && fore == BlackColour) { // Redraw MuxTop OUT
      fore = WhiteColour;
      MuxNegFn(opn, 2);
    }
  } else if (index == 2) { // VDD
    MoveTo(TopLabel(opn)); PlotText("VDD");
  }
}

// These are the options on each menu

const char *OpAmps[2] = {"0", "1"};
const char *MuxPos[7] = {"INP    ", "WIP    ", "DAC    ", "GND    ", "VDDDIV2", "LINKOUT", "LINKWIP"};
const char *MuxNeg[4] = {"INN    ", "WIP    ", "OUT    ", "DAC    "};
const char *MuxBot[6] = {"OFF    ", "INP    ", "INN    ", "DAC    ", "LINKOUT", "GND    "};
const char *MuxWip[8] = {"WIP0   ", "WIP1   ", "WIP2   ", "WIP3   ", "WIP4   ", "WIP5   ", "WIP6   ", "WIP7   "};
const char *MuxTop[3] = {"OFF   ",  "OUT   ",  "VDD   "};

// These are the main menus

const menu_t Menus[nMenus] = {
//Name  Options Reg               Mask             Position         Values  Drawfn
  {"",       2, 0,                0,               0,               OpAmps, 0},
  {"MUXPOS", 7, &OPAMP.OP0INMUX,  OPAMP_MUXPOS_gm, OPAMP_MUXPOS_gp, MuxPos, MuxPosFn},
  {"MUXNEG", 4, &OPAMP.OP0INMUX,  OPAMP_MUXNEG_gm, OPAMP_MUXNEG_gp, MuxNeg, MuxNegFn},
  {"MUXBOT", 6, &OPAMP.OP0RESMUX, OPAMP_MUXBOT_gm, OPAMP_MUXBOT_gp, MuxBot, MuxBotFn},
  {"MUXWIP", 8, &OPAMP.OP0RESMUX, OPAMP_MUXWIP_gm, OPAMP_MUXWIP_gp, MuxWip, MuxWipFn},
  {"MUXTOP", 3, &OPAMP.OP0RESMUX, OPAMP_MUXTOP_gm, OPAMP_MUXTOP_gp, MuxTop, MuxTopFn}
};

const int OPAMPOFFSET = &OPAMP.OP1CTRLA - &OPAMP.OP0CTRLA;

void DrawMenu (int opamp, int menu, int option, boolean selected) {
  if (menu == 0) {
    MoveTo((point){0, cy*(1-opamp)}); 
  } else {
    MoveTo((point){12 + (menu-1)*6*8, cy*(1-opamp)});
  }
  if (selected) fore = RedColour;
  PlotText((Menus[menu].values)[option]);
  fore = WhiteColour;
}

void DrawMenus () {
  for (int m=0; m<nMenus; m++) {
    // Headings
    MoveTo((point){12 + (m-1)*6*8, 2*cy});
    PlotText(Menus[m].name);
    for (int i=0; i<nOpAmps; i++) {    
      DrawMenu(i, m, State[i][m], false);
      if (m != 0) (Menus[m].drawfn)(i, State[i][m]);
    }
  }
}

void InitState () {
  EEPROM.get(0, State);
  for (int i=0; i<nOpAmps; i++) {
    for (int j=0; j<nMenus; j++) {
      if (State[i][j] >= Menus[j].options) State[i][j] = 0;
    }
  }
  State[1][0] = 1;
}

// Op-Amp Configuration **********************************************

void InitOpAmp () {
  // Set up the timebase
  OPAMP.TIMEBASE = 23; // Number of CLK_PER cycles in 1 us, minus one
  OPAMP.OP0CTRLA = OPAMP_OP0CTRLA_OUTMODE_NORMAL_gc | OPAMP_ALWAYSON_bm;
  OPAMP.OP1CTRLA = OPAMP_OP1CTRLA_OUTMODE_NORMAL_gc | OPAMP_ALWAYSON_bm;
  OPAMP.CTRLA = OPAMP_ENABLE_bm;
}

void ConfigOpAmp (uint8_t opamp, uint8_t menu, uint8_t state) {
  register8_t *Address;
  Address = Menus[menu].reg + OPAMPOFFSET * opamp;
  *Address = (*Address & ~(Menus[menu].mask)) | state<<(Menus[menu].position);
}

void ConfigOpAmps () {
  for (int i=0; i<nOpAmps; i++) {
    for (int j=1; j<nMenus; j++) {
      ConfigOpAmp(i, j, State[i][j]);
    }
  }
}

// Controls **********************************************

// Global UI settings
volatile int OpAmp = 0, Menu = 0;

const int LeftBtn = 8;   // PC0
const int RightBtn = 9;  // PC1
const int UpBtn = 20;    // PF0
const int DownBtn = 21;  // PF1

void InitButtons () {
  pinMode(LeftBtn, INPUT_PULLUP);
  pinMode(RightBtn, INPUT_PULLUP);
  pinMode(UpBtn, INPUT_PULLUP);
  pinMode(DownBtn, INPUT_PULLUP);
  // Trigger buttons on falling level
  PORTC.PIN0CTRL = PORTC.PIN0CTRL | PORT_ISC_FALLING_gc;
  PORTC.PIN1CTRL = PORTC.PIN1CTRL | PORT_ISC_FALLING_gc;
  PORTF.PIN0CTRL = PORTF.PIN0CTRL | PORT_ISC_FALLING_gc;
  PORTF.PIN1CTRL = PORTF.PIN1CTRL | PORT_ISC_FALLING_gc;
}

// Up and Down buttons are in PORTF
ISR(PORTF_PORT_vect) {
  // Debounce buttons
  static unsigned long lastvisit;
  if (millis() - lastvisit < 250) {
    PORTF.INTFLAGS = PORT_INT0_bm | PORT_INT1_bm; // Clear interrupt flags
    return;
  }
  lastvisit = millis();
  // Change a menu option
  int ud = (1 - digitalRead(UpBtn)) - (1 - digitalRead(DownBtn));
  if (Menu == 0) {
    DrawMenu(OpAmp, Menu, State[OpAmp][Menu], false);
    OpAmp = (OpAmp + nOpAmps + ud) % nOpAmps;
    DrawMenu(OpAmp, Menu, State[OpAmp][Menu], true);
  } else {
    fore = BlackColour;
    (Menus[Menu].drawfn)(OpAmp, State[OpAmp][Menu]);
    fore = WhiteColour;
    State[OpAmp][Menu] = (State[OpAmp][Menu] + Menus[Menu].options + ud) % Menus[Menu].options;
    DrawMenu(OpAmp, Menu, State[OpAmp][Menu], true);
    // Update the diagram
    (Menus[Menu].drawfn)(OpAmp, State[OpAmp][Menu]);
    // Update the op-amp configuration to the new state
    ConfigOpAmp(OpAmp, Menu, State[OpAmp][Menu]);
    // Save state
    EEPROM.put(0, State);
  }
  PORTF.INTFLAGS = PORT_INT0_bm | PORT_INT1_bm; // Clear interrupt flags
}

// Left and Right buttons are in PORTC
ISR(PORTC_PORT_vect) {
  // Debounce buttons
  static unsigned long lastvisit;
  if (millis() - lastvisit < 250) {
    PORTC.INTFLAGS = PORT_INT0_bm | PORT_INT1_bm; // Clear interrupt flags
    return;
  }
  lastvisit = millis();
  // Step between menus
  int lr = (1 - digitalRead(RightBtn)) - (1 - digitalRead(LeftBtn));
  DrawMenu(OpAmp, Menu, State[OpAmp][Menu], false);
  Menu = (Menu + nMenus + lr) % nMenus;
  DrawMenu(OpAmp, Menu, State[OpAmp][Menu], true);
  PORTC.INTFLAGS = PORT_INT0_bm | PORT_INT1_bm; // Clear interrupt flags
}

// Setup **********************************************

void setup() {
  delay(100);
  InitDisplay();
  ClearDisplay();
  DisplayOn();
  InitButtons();
  // Initialise the diagram
  DrawOpAmp(0); DrawOpAmp(1); DrawResistors(0); DrawResistors(1);
  InitState();
  DrawMenus();
  // Start up the op amps
  InitOpAmp();
  ConfigOpAmps();
  // Highlight current menu option
  DrawMenu(OpAmp, Menu, State[OpAmp][Menu], true);
}

void loop () {
}
