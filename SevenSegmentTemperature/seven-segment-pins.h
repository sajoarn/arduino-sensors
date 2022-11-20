#ifndef SEVEN_SEGMENT_PINS_H
#define SEVEN_SEGMENT_PINS_H

/*
Common Anode Display 3461BS-1
Pinouts:
12 11 10 9  8  7
|  |  |  |  |  |
-----------------
 _   _   _   _
|_| |_| |_| |_|
|_|.|_|.|_|.|_|.
-----------------
|  |  |  |  |  |
1  2  3  4  5  6

Segment Names:
    A
   ---
F |   | B
  | G |
   ---
E |   | C
  |   |
   --- *(DP)
    D

digit selects (pulled HIGH to enable digit):
digit 1 = pin 12
digit 2 = pin 9
digit 3 = pin 8
digit 4 = pin 6

segment pins (pulled LOW to turn on segment, HIGH to turn off segment):
segment A = pin 11
segment B = pin 7
segment C = pin 4
segment D = pin 2
segment E = pin 1
segment F = pin 10
segment G = pin 5
decimal point = pin 3

*/

//Arduino pin assignments:
enum pins {
  D1 = 2,
  D2 = 3,
  D3 = 4,
  D4 = 5,
  segA = 6,
  segB = 7,
  segC = 8,
  segD = 9,
  segE = 10,
  segF = 11,
  segG = 12,
  decpt = 13,
  NUM_OF_PINS = 14
};

#endif