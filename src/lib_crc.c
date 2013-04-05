#include "lib_crc.h"

#ifdef __cplusplus
extern "C"{
#endif


    /*******************************************************************\
    *                                                                   *
    *   Library         : lib_crc                                       *
    *   File            : lib_crc.c                                     *
    *   Author          : Lammert Bies  1999-2008                       *
    *   E-mail          : info@lammertbies.nl                           *
    *   Language        : ANSI C                                        *
    *                                                                   *
    *                                                                   *
    *   Description                                                     *
    *   ===========                                                     *
    *                                                                   *
    *   The file lib_crc.c contains the private  and  public  func-     *
    *   tions  used  for  the  calculation of CRC-16, CRC-CCITT and     *
    *   CRC-32 cyclic redundancy values.                                *
    *                                                                   *
    *                                                                   *
    *   Dependencies                                                    *
    *   ============                                                    *
    *                                                                   *
    *   lib_crc.h       CRC definitions and prototypes                  *
    *                                                                   *
    *                                                                   *
    *   Modification history                                            *
    *   ====================                                            *
    *                                                                   *
    *   Date        Version Comment                                     *
    *                                                                   *
    *   2008-04-20  1.16    Added CRC-CCITT calculation for Kermit      *
    *                                                                   *
    *   2007-04-01  1.15    Added CRC16 calculation for Modbus          *
    *                                                                   *
    *   2007-03-28  1.14    Added CRC16 routine for Sick devices        *
    *                                                                   *
    *   2005-12-17  1.13    Added CRC-CCITT with initial 0x1D0F         *
    *                                                                   *
    *   2005-05-14  1.12    Added CRC-CCITT with start value 0          *
    *                                                                   *
    *   2005-02-05  1.11    Fixed bug in CRC-DNP routine                *
    *                                                                   *
    *   2005-02-04  1.10    Added CRC-DNP routines                      *
    *                                                                   *
    *   1999-02-21  1.01    Added FALSE and TRUE mnemonics              *
    *                                                                   *
    *   1999-01-22  1.00    Initial source                              *
    *                                                                   *
    \*******************************************************************/



    /*******************************************************************\
    *                                                                   *
    *   #define P_xxxx                                                  *
    *                                                                   *
    *   The CRC's are computed using polynomials. The  coefficients     *
    *   for the algorithms are defined by the following constants.      *
    *                                                                   *
    \*******************************************************************/

//#define                 P_16        0xA001
//#define                 P_32        0xEDB88320L
//#define                 P_CCITT     0x1021
//#define                 P_DNP       0xA6BC
//#define                 P_KERMIT    0x8408
//#define                 P_SICK      0x8005



    /*******************************************************************\
    *                                                                   *
    *   static int crc_tab...init                                       *
    *   static unsigned ... crc_tab...[]                                *
    *                                                                   *
    *   The algorithms use tables with precalculated  values.  This     *
    *   speeds  up  the calculation dramaticaly. The first time the     *
    *   CRC function is called, the table for that specific  calcu-     *
    *   lation  is set up. The ...init variables are used to deter-     *
    *   mine if the initialization has taken place. The  calculated     *
    *   values are stored in the crc_tab... arrays.                     *
    *                                                                   *
    *   The variables are declared static. This makes them  invisi-     *
    *   ble for other modules of the program.                           *
    *                                                                   *
    \*******************************************************************/

//static int              crc_tab16_init          = FALSE;
//static int              crc_tab32_init          = FALSE;
//static int              crc_tabccitt_init       = FALSE;
//static int              crc_tabdnp_init         = FALSE;
//static int              crc_tabkermit_init      = FALSE;

//static unsigned short   crc_tab16[256];
//static unsigned long    crc_tab32[256];
//static unsigned short   crc_tabccitt[256];
//static unsigned short   crc_tabdnp[256];
//static unsigned short   crc_tabkermit[256];

static unsigned short   crc_tabccitt[] = {
0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7, 0x8108, 0x9129,
0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF, 0x1231, 0x0210, 0x3273, 0x2252,
0x52B5, 0x4294, 0x72F7, 0x62D6, 0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 
0xF3FF, 0xE3DE, 0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D, 0x3653, 0x2672,
0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4, 0xB75B, 0xA77A, 0x9719, 0x8738, 
0xF7DF, 0xE7FE, 0xD79D, 0xC7BC, 0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861,
0x2802, 0x3823, 0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12, 0xDBFD, 0xCBDC,
0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A, 0x6CA6, 0x7C87, 0x4CE4, 0x5CC5,
0x2C22, 0x3C03, 0x0C60, 0x1C41, 0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B,
0x8D68, 0x9D49, 0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78, 0x9188, 0x81A9,
0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F, 0x1080, 0x00A1, 0x30C2, 0x20E3,
0x5004, 0x4025, 0x7046, 0x6067, 0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C,
0xE37F, 0xF35E, 0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D, 0x34E2, 0x24C3,
0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 0xA7DB, 0xB7FA, 0x8799, 0x97B8,
0xE75F, 0xF77E, 0xC71D, 0xD73C, 0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676,
0x4615, 0x5634, 0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3, 0xCB7D, 0xDB5C,
0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A, 0x4A75, 0x5A54, 0x6A37, 0x7A16,
0x0AF1, 0x1AD0, 0x2AB3, 0x3A92, 0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B,
0x9DE8, 0x8DC9, 0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8, 0x6E17, 0x7E36,
0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};
    /*******************************************************************\
    *                                                                   *
    *   static void init_crc...tab();                                   *
    *                                                                   *
    *   Three local functions are used  to  initialize  the  tables     *
    *   with values for the algorithm.                                  *
    *                                                                   *
    \*******************************************************************/

//static void             init_crc16_tab( void );
//static void             init_crc32_tab( void );
//static void             init_crcccitt_tab( void );
//static void             init_crcdnp_tab( void );
//static void             init_crckermit_tab( void );



    /*******************************************************************\
    *                                                                   *
    *   unsigned short update_crc_ccitt( unsigned long crc, char c );   *
    *                                                                   *
    *   The function update_crc_ccitt calculates  a  new  CRC-CCITT     *
    *   value  based  on the previous value of the CRC and the next     *
    *   byte of the data to be checked.                                 *
    *                                                                   *
    \*******************************************************************/

unsigned short update_crc_ccitt( unsigned short crc, char c ) {

    unsigned short tmp, short_c;

    short_c  = 0x00ff & (unsigned short) c;

//    if ( ! crc_tabccitt_init ) init_crcccitt_tab();

    tmp = (crc >> 8) ^ short_c;
    crc = (crc << 8) ^ crc_tabccitt[tmp];

    return crc;

}  /* update_crc_ccitt */


//
    /*******************************************************************\
    *                                                                   *
    *   unsigned short update_crc_sick(                                 *
    *             unsigned long crc, char c, char prev_byte );          *
    *                                                                   *
    *   The function  update_crc_sick  calculates  a  new  CRC-SICK     *
    *   value  based  on the previous value of the CRC and the next     *
    *   byte of the data to be checked.                                 *
    *                                                                   *
    \*******************************************************************/
//
//unsigned short update_crc_sick( unsigned short crc, char c, char prev_byte ) {
//
//    unsigned short short_c, short_p;
//
//    short_c  =   0x00ff & (unsigned short) c;
//    short_p  = ( 0x00ff & (unsigned short) prev_byte ) << 8;
//
//    if ( crc & 0x8000 ) crc = ( crc << 1 ) ^ P_SICK;
//    else                crc =   crc << 1;
//
//    crc &= 0xffff;
//    crc ^= ( short_c | short_p );
//
//    return crc;
//
//}  /* update_crc_sick */
//
//
//
    /*******************************************************************\
    *                                                                   *
    *   unsigned short update_crc_16( unsigned short crc, char c );     *
    *                                                                   *
    *   The function update_crc_16 calculates a  new  CRC-16  value     *
    *   based  on  the  previous value of the CRC and the next byte     *
    *   of the data to be checked.                                      *
    *                                                                   *
    \*******************************************************************/
//
//unsigned short update_crc_16( unsigned short crc, char c ) {
//
//    unsigned short tmp, short_c;
//
//    short_c = 0x00ff & (unsigned short) c;
//
////    if ( ! crc_tab16_init ) init_crc16_tab();
//
//    tmp =  crc       ^ short_c;
//    crc = (crc >> 8) ^ crc_tab16[ tmp & 0xff ];
//
//    return crc;
//
//}  /* update_crc_16 */
//
//
//
    /*******************************************************************\
    *                                                                   *
    *   unsigned short update_crc_kermit( unsigned short crc, char c ); *
    *                                                                   *
    *   The function update_crc_kermit calculates a  new  CRC value     *
    *   based  on  the  previous value of the CRC and the next byte     *
    *   of the data to be checked.                                      *
    *                                                                   *
    \*******************************************************************/
//
//unsigned short update_crc_kermit( unsigned short crc, char c ) {
//
//    unsigned short tmp, short_c;
//
//    short_c = 0x00ff & (unsigned short) c;
//
//    if ( ! crc_tabkermit_init ) init_crckermit_tab();
//
//    tmp =  crc       ^ short_c;
//    crc = (crc >> 8) ^ crc_tabkermit[ tmp & 0xff ];
//
//    return crc;
//
//}  /* update_crc_kermit */
//
//
//
    /*******************************************************************\
    *                                                                   *
    *   unsigned short update_crc_dnp( unsigned short crc, char c );    *
    *                                                                   *
    *   The function update_crc_dnp calculates a new CRC-DNP  value     *
    *   based  on  the  previous value of the CRC and the next byte     *
    *   of the data to be checked.                                      *
    *                                                                   *
    \*******************************************************************/
//
//unsigned short update_crc_dnp( unsigned short crc, char c ) {
//
//    unsigned short tmp, short_c;
//
//    short_c = 0x00ff & (unsigned short) c;
//
//    if ( ! crc_tabdnp_init ) init_crcdnp_tab();
//
//    tmp =  crc       ^ short_c;
//    crc = (crc >> 8) ^ crc_tabdnp[ tmp & 0xff ];
//
//    return crc;
//
//}  /* update_crc_dnp */
//
//
//
    /*******************************************************************\
    *                                                                   *
    *   unsigned long update_crc_32( unsigned long crc, char c );       *
    *                                                                   *
    *   The function update_crc_32 calculates a  new  CRC-32  value     *
    *   based  on  the  previous value of the CRC and the next byte     *
    *   of the data to be checked.                                      *
    *                                                                   *
    \*******************************************************************/
//
//unsigned long update_crc_32( unsigned long crc, char c ) {
//
//    unsigned long tmp, long_c;
//
//    long_c = 0x000000ffL & (unsigned long) c;
//
//    if ( ! crc_tab32_init ) init_crc32_tab();
//
//    tmp = crc ^ long_c;
//    crc = (crc >> 8) ^ crc_tab32[ tmp & 0xff ];
//
//    return crc;
//
//}  /* update_crc_32 */
//
//
//
    /*******************************************************************\
    *                                                                   *
    *   static void init_crc16_tab( void );                             *
    *                                                                   *
    *   The function init_crc16_tab() is used  to  fill  the  array     *
    *   for calculation of the CRC-16 with values.                      *
    *                                                                   *
    \*******************************************************************/
//
//static void init_crc16_tab( void ) {
//
//    int i, j;
//    unsigned short crc, c;
//
//    for (i=0; i<256; i++) {
//
//        crc = 0;
//        c   = (unsigned short) i;
//
//        for (j=0; j<8; j++) {
//
//            if ( (crc ^ c) & 0x0001 ) crc = ( crc >> 1 ) ^ P_16;
//            else                      crc =   crc >> 1;
//
//            c = c >> 1;
//        }
//
//        crc_tab16[i] = crc;
//    }
//
//    crc_tab16_init = TRUE;
//
//}  /* init_crc16_tab */
//
//
//
    /*******************************************************************\
    *                                                                   *
    *   static void init_crckermit_tab( void );                         *
    *                                                                   *
    *   The function init_crckermit_tab() is used to fill the array     *
    *   for calculation of the CRC Kermit with values.                  *
    *                                                                   *
    \*******************************************************************/
//
//static void init_crckermit_tab( void ) {
//
//    int i, j;
//    unsigned short crc, c;
//
//    for (i=0; i<256; i++) {
//
//        crc = 0;
//        c   = (unsigned short) i;
//
//        for (j=0; j<8; j++) {
//
//            if ( (crc ^ c) & 0x0001 ) crc = ( crc >> 1 ) ^ P_KERMIT;
//            else                      crc =   crc >> 1;
//
//            c = c >> 1;
//        }
//
//        crc_tabkermit[i] = crc;
//    }
//
//    crc_tabkermit_init = TRUE;
//
//}  /* init_crckermit_tab */
//
//
//
    /*******************************************************************\
    *                                                                   *
    *   static void init_crcdnp_tab( void );                            *
    *                                                                   *
    *   The function init_crcdnp_tab() is used  to  fill  the  array    *
    *   for calculation of the CRC-DNP with values.                     *
    *                                                                   *
    \*******************************************************************/
//
//static void init_crcdnp_tab( void ) {
//
//    int i, j;
//    unsigned short crc, c;
//
//    for (i=0; i<256; i++) {
//
//        crc = 0;
//        c   = (unsigned short) i;
//
//        for (j=0; j<8; j++) {
//
//            if ( (crc ^ c) & 0x0001 ) crc = ( crc >> 1 ) ^ P_DNP;
//            else                      crc =   crc >> 1;
//
//            c = c >> 1;
//        }
//
//        crc_tabdnp[i] = crc;
//    }
//
//    crc_tabdnp_init = TRUE;
//
//}  /* init_crcdnp_tab */
//
//
//
    /*******************************************************************\
    *                                                                   *
    *   static void init_crc32_tab( void );                             *
    *                                                                   *
    *   The function init_crc32_tab() is used  to  fill  the  array     *
    *   for calculation of the CRC-32 with values.                      *
    *                                                                   *
    \*******************************************************************/
//
//static void init_crc32_tab( void ) {
//
//    int i, j;
//    unsigned long crc;
//
//    for (i=0; i<256; i++) {
//
//        crc = (unsigned long) i;
//
//        for (j=0; j<8; j++) {
//
//            if ( crc & 0x00000001L ) crc = ( crc >> 1 ) ^ P_32;
//            else                     crc =   crc >> 1;
//        }
//
//        crc_tab32[i] = crc;
//    }
//
//    crc_tab32_init = TRUE;
//
//}  /* init_crc32_tab */
//
//
//
    /*******************************************************************\
    *                                                                   *
    *   static void init_crcccitt_tab( void );                          *
    *                                                                   *
    *   The function init_crcccitt_tab() is used to fill the  array     *
    *   for calculation of the CRC-CCITT with values.                   *
    *                                                                   *
    \*******************************************************************/
//
//static void init_crcccitt_tab( void ) {
//
//    int i, j;
//    unsigned short crc, c;
//
//    for (i=0; i<256; i++) {
//
//        crc = 0;
//        c   = ((unsigned short) i) << 8;
//
//        for (j=0; j<8; j++) {
//
//            if ( (crc ^ c) & 0x8000 ) crc = ( crc << 1 ) ^ P_CCITT;
//            else                      crc =   crc << 1;
//
//            c = c << 1;
//        }
//
//        crc_tabccitt[i] = crc;
//        printf("0x%0X, ",crc);
//    }
//
//    crc_tabccitt_init = TRUE;
//
//}  /* init_crcccitt_tab */

#ifdef __cplusplus
} // extern "C"
#endif
