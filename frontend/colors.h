/*
 *******************************************************************************
 *                                  Micrified
 * FILENAME:    colors.h
 * PROGRAMMER:  Randolph, Charles
 * CREATED:     23/07/2017
 * DESCRIPTION: Header for minimal color-based printing facilities. Based on
 *              Peter Varo's Rainicorn: https://github.com/petervaro/rainicorn
 *******************************************************************************
 */


/******************************** ATTRIBUTES  **********************************/

#define RST                     "0"
#define BOL                     "1"
#define DIM                     "2"
#define UND                     "4"
#define BLI                     "5"
#define REV                     "7"
#define HID                     "8"


/********************************** COLORS  ************************************/

#define BLK                     "0"
#define RED                     "1"
#define GRN                     "2"
#define YEL                     "3"
#define BLU                     "4"
#define MAG                     "5"
#define CYN                     "6"
#define WTE                     "7"

/******************************* CONSTRUCTORS  *********************************/

#define FGCOLOR(C)              "3" C
#define BGCOLOR(C)              "4" C

#define SEQ(S)                  "\x1B[" S "m"
#define CONFIG_A(A)             SEQ(A)
#define CONFIG_F(F)             SEQ(FGCOLOR(F))
#define CONFIG_B(B)             SEQ(BGCOLOR(B))
#define CONFIG_AF(A,F)          SEQ(FGCOLOR(F) ";" A)
#define CONFIG_AB(A,B)          SEQ(BGCOLOR(B) ";" A)
#define CONFIG_FB(F,B)          SEQ(FGCOLOR(F) ";" BGCOLOR(B))
#define CONFIG_AFB(A,F,B)       SEQ(A ";" FGCOLOR(F) ";" BGCOLOR(B))
#define RESET                   SEQ(RST)


/************************** TERMINAL FORMAT SETTERS  ***************************/

#define C_A(A,T)                CONFIG_A(A)         T
#define C_F(F,T)                CONFIG_F(F)         T
#define C_B(B,T)                CONFIG_B(B)         T
#define C_AF(A,F,T)             CONFIG_AF(A,F)      T
#define C_AB(A,B,T)             CONFIG_AB(A,B)      T
#define C_FB(F,B,T)             CONFIG_FB(F,B)      T
#define C_AFB(A,F,B,T)          CONFIG_AFB(A,F,B)   T

/************************ TEXT-ONLY FORMAT SETTERS   ***************************/

#define C_TA(A,T)               C_A(A,T)        RESET
#define C_TF(F,T)               C_F(F,T)        RESET
#define C_TB(B,T)               C_B(B,T)        RESET
#define C_TAF(A,F,T)            C_AF(A,F,T)     RESET
#define C_TAB(A,B,T)            C_AB(A,B,T)     RESET
#define C_TFB(F,B,T)            C_FB(F,B,T)     RESET
#define C_TAFB(A,F,B,T)         C_AFB(A,F,B,T)  RESET