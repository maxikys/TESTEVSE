#ifndef DEFINE_H
#define DEFINE_H
#include "stm32f0xx.h"

#define SizeMass_Send_Buffer  8   /*массив МК -> ПК*/
#define SizeMass_Report_buf   26   /*массив ПК -> МК*/
#define SizeBuffTo595 2

typedef enum{ActivLow = 0, ActivHight = 1}TActiveState;

typedef enum{_10ms = 10,_20ms = 20,_30ms = 30,_40ms = 40,_50ms = 50,_60ms = 60,_70ms = 70,_80ms = 80}Tperiod;

/*Стуктура хранения данных для опроса клавиатуры*/
typedef struct 
      {
          Tperiod period_oprosa;
          Tperiod TimeDrebezg;
          uint8_t SizeRow;
          uint8_t SizeColumn;
          uint8_t *ScanCodeBuff;// указатель на буфер декодирования
          TActiveState ActiveState;
      }TKeyboardParametrs;
     
typedef struct
      {
          Tperiod period_oprosa;
          Tperiod TimeDrebezg;
          uint8_t CountKey;
          uint8_t *ScanCodeBuff;// указатель на буфер декодирования
          TActiveState ActiveState;
      }TKeySingleParametrs;

typedef   enum 
  {
    NoEventKey,                                       //0   00  Reserved (no event indicated)
    ErrorRollOver,                                    //1   01  Keyboard ErrorRollOver
    POSTFail,                                         //2   02  Keyboard POSTFail
    ErrorUndefined,                                   //3   03  Keyboard ErrorUndefined
    Key_A,                                            //4   04  Keyboard a and A
    Key_B,                                            //5   05  Keyboard b and B
    Key_C,                                            //6   06  Keyboard c and C
    Key_D,                                            //7   07  Keyboard d and D
    Key_E,                                            //8   08  Keyboard e and E
    Key_F,                                            //9   09  Keyboard f and F
    Key_G,                                            //10  0A  Keyboard g and G
    Key_H,                                            //11  0B  Keyboard h and H
    Key_I,                                            //12  0C  Keyboard i and I
    Key_J,                                            //13  0D  Keyboard j and J
    Key_K,                                            //14  0E  Keyboard k and K
    Key_L,                                            //15  0F  Keyboard l and L
    Key_M,                                            //16  10  Keyboard m and M
    Key_N,                                            //17  11  Keyboard n and N
    Key_O,                                            //18  12  Keyboard o and O
    Key_P,                                            //19  13  Keyboard p and P
    Key_Q,                                            //20  14  Keyboard q and Q
    Key_R,                                            //21  15  Keyboard r and R
    Key_S,                                            //22  16  Keyboard s and S
    Key_T,                                            //23  17  Keyboard t and T
    Key_U,                                            //24  18  Keyboard u and U
    Key_V,                                            //25  19  Keyboard v and V
    Key_W,                                            //26  1A  Keyboard w and W
    Key_X,                                            //27  1B  Keyboard x and X
    Key_Y,                                            //28  1C  Keyboard y and Y
    Key_Z,                                            //29  1D  Keyboard z and Z
    Key_1,                                            //30  1E  Keyboard 1 and !
    Key_2,                                            //31  1F  Keyboard 2 and @
    Key_3,                                            //32  20  Keyboard 3 and #
    Key_4,                                            //33  21  Keyboard 4 and $
    Key_5,                                            //34  22  Keyboard 5 and %
    Key_6,                                            //35  23  Keyboard 6 and ^
    Key_7,                                            //36  24  Keyboard 7 and &
    Key_8,                                            //37  25  Keyboard 8 and *
    Key_9,                                            //38  26  Keyboard 9 and (
    Key_0,                                            //39  27  Keyboard 0 and )
    Key_Return,                                       //40  28  Keyboard Return (ENT
    Key_ESCAPE,                                       //41  29  Keyboard ESCAPE
    Key_Backspace,                                    //42  2A  Keyboard DELETE (Ba
    Key_Tab,                                          //43  2B  Keyboard Tab
    Key_Spacebar,                                     //44  2C  Keyboard Spacebar
    Key_Minus,                                        //45  2D  Keyboard - and (unders
    Key_Equal,                                        //46  2E  Keyboard = and +
    Key_SquareBracketL,                               //47  2F  Keyboard [ and {
    Key_SquareBracketR,                               //48  30  Keyboard ] and }
    Key_BackSlash,                                    //49  31  Keyboard \ and |
    Key_NonUS_Sharp,                                  //50  32  Keyboard Non-US # an
    Key_Colon,                                        //51  33  Keyboard ; and :
    Key_Quote,                                        //52  34  Keyboard ‘ and “
    Key_Grave,                                        //53  35  Keyboard Grave Accen
    Key_Comma,                                        //54  36  Keyboard, and <
    Key_Point,                                        //55  37  Keyboard . and >
    Key_Divide,                                       //56  38  Keyboard / and ?
    Key_CapsLock,                                     //57  39  Keyboard Caps Lock
    Key_F1,                                           //58  3A  Keyboard F1
    Key_F2,                                           //59  3B  Keyboard F2
    Key_F3,                                           //60  3C  Keyboard F3
    Key_F4,                                           //61  3D  Keyboard F4
    Key_F5,                                           //62  3E  Keyboard F5
    Key_F6,                                           //63  3F  Keyboard F6
    Key_F7,                                           //64  40  Keyboard F7
    Key_F8,                                           //65  41  Keyboard F8
    Key_F9,                                           //66  42  Keyboard F9
    Key_F10,                                          //67  43  Keyboard F10
    Key_F11,                                          //68  44  Keyboard F11
    Key_F12,                                          //69  45  Keyboard F12
    Key_PrintScreen,                                  //70  46  Keyboard PrintScreen
    Key_ScrollLock,                                   //71  47  Keyboard Scroll Lock
    Key_Pause,                                        //72  48  Keyboard Pause
    Key_Insert,                                       //73  49  Keyboard Insert
    Key_Home,                                         //74  4A  Keyboard Home
    Key_PageUp,                                       //75  4B  Keyboard PageUp
    Key_DeleteForward,                                //76  4C  Keyboard Delete Forward
    Key_End,                                          //77  4D  Keyboard End
    Key_PageDown,                                     //78  4E  Keyboard PageDown
    Key_RightArrow,                                   //79  4F  Keyboard RightArrow
    Key_LeftArrow,                                    //80  50  Keyboard LeftArrow
    Key_DownArrow,                                    //81  51  Keyboard DownArrow
    Key_UpArrow,                                      //82  52  Keyboard UpArrow
    Keypad_NumLock,                                   //83  53  Keypad Num Lock and C
    Keypad_Divide,                                    //84  54  Keypad /
    Keypad_Multiple,                                  //85  55  Keypad *
    Keypad_Minus,                                     //86  56  Keypad -
    Keypad_Plus,                                      //87  57  Keypad +
    Keypad_ENTER,                                     //88  58  Keypad ENTER
    Keypad_1,                                         //89  59  Keypad 1 and End
    Keypad_2,                                         //90  5A  Keypad 2 and Down Arro
    Keypad_3,                                         //91  5B  Keypad 3 and PageDn
    Keypad_4,                                         //92  5C  Keypad 4 and Left Arrow
    Keypad_5,                                         //93  5D  Keypad 5
    Keypad_6,                                         //94  5E  Keypad 6 and Right Arro
    Keypad_7,                                         //95  5F  Keypad 7 and Home
    Keypad_8,                                         //96  60  Keypad 8 and Up Arrow
    Keypad_9,                                         //97  61  Keypad 9 and PageUp
    Keypad_0,                                         //98  62  Keypad 0 and Insert
    Keypad_Point,                                     //99  63  Keypad . and Delete
    Key_NonUS_BackSlash,                              //100 64  Keyboard Non-US \ and |
    Key_Application,                                  //101 65  Keyboard Application
    Key_Power,                                        //102 66  Keyboard Power
    Keypad_Equal,                                     //103 67  Keypad =
                                                      //
    Key_LeftControl = 224 ,                           //224 E0 Keyboard LeftControl
    Key_LeftShift,                                    //225 E1 Keyboard LeftShift
    Key_LeftAlt,                                      //226 E2 Keyboard LeftAlt
    Key_LeftGUI,                                      //227 E3 Keyboard Left GUI
    Key_RightControl,                                 //228 E4 Keyboard RightControl
    Key_RightShift,                                   //229 E5 Keyboard RightShift
    Key_RightAlt,                                     //230 E6 Keyboard RightAlt
    Key_RightGUI,                                      //231 E7 Keyboard Right GUI
    Key_Error = 0xE0
  }eScanCode;

     
void DecodeUsbData( uint8_t *Rep_buf, uint8_t size);
void WriteShiftReg(uint8_t value); 
void proc1(void);     
     
#endif
