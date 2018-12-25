/*==============================================================================
** nongli.c -- nongli calender.
**
** MODIFY HISTORY:
**
** 2012-03-08 wdf Create.
==============================================================================*/
#include <dfewos.h>    /* support some types for string.h */
#include "../string.h" /* strcpy strncat strlen */
int sprintf (char *buffer, const char *fmt, ...);

#include "nongli.h"

/*======================================================================
  ÿ�����ֽڴ���һ�꣺

  ��һ���ֽڸ���λ�������ũ���������·ݣ�
            ����λ�������ũ��һ�����·ݵĴ�С�������
  �ڶ����ֽڴ������ũ������ʮ���·ݵĴ�С�������
  �������ֽ����λ�������ũ�����ܵĵ�ʮ���·ݴ�С�������
            [6 5]��������д������ڹ����·ݣ�
            [4 0]��������д������ڹ������ڡ�
======================================================================*/
static unsigned char const _G_year_code [] =
{
0x04,0xAe,0x53, /*1901*/ 0x0A,0x57,0x48, /*1902*/ 0x55,0x26,0xBd, /*1903*/
0x0d,0x26,0x50, /*1904*/ 0x0d,0x95,0x44, /*1905*/ 0x46,0xAA,0xB9, /*1906*/
0x05,0x6A,0x4d, /*1907*/ 0x09,0xAd,0x42, /*1908*/ 0x24,0xAe,0xB6, /*1909*/
0x04,0xAe,0x4A, /*1910*/ 0x6A,0x4d,0xBe, /*1911*/ 0x0A,0x4d,0x52, /*1912*/
0x0d,0x25,0x46, /*1913*/ 0x5d,0x52,0xBA, /*1914*/ 0x0B,0x54,0x4e, /*1915*/
0x0d,0x6A,0x43, /*1916*/ 0x29,0x6d,0x37, /*1917*/ 0x09,0x5B,0x4B, /*1918*/
0x74,0x9B,0xC1, /*1919*/ 0x04,0x97,0x54, /*1920*/ 0x0A,0x4B,0x48, /*1921*/
0x5B,0x25,0xBC, /*1922*/ 0x06,0xA5,0x50, /*1923*/ 0x06,0xd4,0x45, /*1924*/
0x4A,0xdA,0xB8, /*1925*/ 0x02,0xB6,0x4d, /*1926*/ 0x09,0x57,0x42, /*1927*/
0x24,0x97,0xB7, /*1928*/ 0x04,0x97,0x4A, /*1929*/ 0x66,0x4B,0x3e, /*1930*/
0x0d,0x4A,0x51, /*1931*/ 0x0e,0xA5,0x46, /*1932*/ 0x56,0xd4,0xBA, /*1933*/
0x05,0xAd,0x4e, /*1934*/ 0x02,0xB6,0x44, /*1935*/ 0x39,0x37,0x38, /*1936*/
0x09,0x2e,0x4B, /*1937*/ 0x7C,0x96,0xBf, /*1938*/ 0x0C,0x95,0x53, /*1939*/
0x0d,0x4A,0x48, /*1940*/ 0x6d,0xA5,0x3B, /*1941*/ 0x0B,0x55,0x4f, /*1942*/
0x05,0x6A,0x45, /*1943*/ 0x4A,0xAd,0xB9, /*1944*/ 0x02,0x5d,0x4d, /*1945*/
0x09,0x2d,0x42, /*1946*/ 0x2C,0x95,0xB6, /*1947*/ 0x0A,0x95,0x4A, /*1948*/
0x7B,0x4A,0xBd, /*1949*/ 0x06,0xCA,0x51, /*1950*/ 0x0B,0x55,0x46, /*1951*/
0x55,0x5A,0xBB, /*1952*/ 0x04,0xdA,0x4e, /*1953*/ 0x0A,0x5B,0x43, /*1954*/
0x35,0x2B,0xB8, /*1955*/ 0x05,0x2B,0x4C, /*1956*/ 0x8A,0x95,0x3f, /*1957*/
0x0e,0x95,0x52, /*1958*/ 0x06,0xAA,0x48, /*1959*/ 0x7A,0xd5,0x3C, /*1960*/
0x0A,0xB5,0x4f, /*1961*/ 0x04,0xB6,0x45, /*1962*/ 0x4A,0x57,0x39, /*1963*/
0x0A,0x57,0x4d, /*1964*/ 0x05,0x26,0x42, /*1965*/ 0x3e,0x93,0x35, /*1966*/
0x0d,0x95,0x49, /*1967*/ 0x75,0xAA,0xBe, /*1968*/ 0x05,0x6A,0x51, /*1969*/
0x09,0x6d,0x46, /*1970*/ 0x54,0xAe,0xBB, /*1971*/ 0x04,0xAd,0x4f, /*1972*/
0x0A,0x4d,0x43, /*1973*/ 0x4d,0x26,0xB7, /*1974*/ 0x0d,0x25,0x4B, /*1975*/
0x8d,0x52,0xBf, /*1976*/ 0x0B,0x54,0x52, /*1977*/ 0x0B,0x6A,0x47, /*1978*/
0x69,0x6d,0x3C, /*1979*/ 0x09,0x5B,0x50, /*1980*/ 0x04,0x9B,0x45, /*1981*/
0x4A,0x4B,0xB9, /*1982*/ 0x0A,0x4B,0x4d, /*1983*/ 0xAB,0x25,0xC2, /*1984*/
0x06,0xA5,0x54, /*1985*/ 0x06,0xd4,0x49, /*1986*/ 0x6A,0xdA,0x3d, /*1987*/
0x0A,0xB6,0x51, /*1988*/ 0x09,0x37,0x46, /*1989*/ 0x54,0x97,0xBB, /*1990*/
0x04,0x97,0x4f, /*1991*/ 0x06,0x4B,0x44, /*1992*/ 0x36,0xA5,0x37, /*1993*/
0x0e,0xA5,0x4A, /*1994*/ 0x86,0xB2,0xBf, /*1995*/ 0x05,0xAC,0x53, /*1996*/
0x0A,0xB6,0x47, /*1997*/ 0x59,0x36,0xBC, /*1998*/ 0x09,0x2e,0x50, /*1999*/
0x0C,0x96,0x45, /*2000*/ 0x4d,0x4A,0xB8, /*2001*/ 0x0d,0x4A,0x4C, /*2002*/
0x0d,0xA5,0x41, /*2003*/ 0x25,0xAA,0xB6, /*2004*/ 0x05,0x6A,0x49, /*2005*/
0x7A,0xAd,0xBd, /*2006*/ 0x02,0x5d,0x52, /*2007*/ 0x09,0x2d,0x47, /*2008*/
0x5C,0x95,0xBA, /*2009*/ 0x0A,0x95,0x4e, /*2010*/ 0x0B,0x4A,0x43, /*2011*/
0x4B,0x55,0x37, /*2012*/ 0x0A,0xd5,0x4A, /*2013*/ 0x95,0x5A,0xBf, /*2014*/
0x04,0xBA,0x53, /*2015*/ 0x0A,0x5B,0x48, /*2016*/ 0x65,0x2B,0xBC, /*2017*/
0x05,0x2B,0x50, /*2018*/ 0x0A,0x93,0x45, /*2019*/ 0x47,0x4A,0xB9, /*2020*/
0x06,0xAA,0x4C, /*2021*/ 0x0A,0xd5,0x41, /*2022*/ 0x24,0xdA,0xB6, /*2023*/
0x04,0xB6,0x4A, /*2024*/ 0x69,0x57,0x3d, /*2025*/ 0x0A,0x4e,0x51, /*2026*/
0x0d,0x26,0x46, /*2027*/ 0x5e,0x93,0x3A, /*2028*/ 0x0d,0x53,0x4d, /*2029*/
0x05,0xAA,0x43, /*2030*/ 0x36,0xB5,0x37, /*2031*/ 0x09,0x6d,0x4B, /*2032*/
0xB4,0xAe,0xBf, /*2033*/ 0x04,0xAd,0x53, /*2034*/ 0x0A,0x4d,0x48, /*2035*/
0x6d,0x25,0xBC, /*2036*/ 0x0d,0x25,0x4f, /*2037*/ 0x0d,0x52,0x44, /*2038*/
0x5d,0xAA,0x38, /*2039*/ 0x0B,0x5A,0x4C, /*2040*/ 0x05,0x6d,0x41, /*2041*/
0x24,0xAd,0xB6, /*2042*/ 0x04,0x9B,0x4A, /*2043*/ 0x7A,0x4B,0xBe, /*2044*/
0x0A,0x4B,0x51, /*2045*/ 0x0A,0xA5,0x46, /*2046*/ 0x5B,0x52,0xBA, /*2047*/
0x06,0xd2,0x4e, /*2048*/ 0x0A,0xdA,0x42, /*2049*/ 0x35,0x5B,0x37, /*2050*/
0x09,0x37,0x4B, /*2051*/ 0x84,0x97,0xC1, /*2052*/ 0x04,0x97,0x53, /*2053*/
0x06,0x4B,0x48, /*2054*/ 0x66,0xA5,0x3C, /*2055*/ 0x0e,0xA5,0x4f, /*2056*/
0x06,0xB2,0x44, /*2057*/ 0x4A,0xB6,0x38, /*2058*/ 0x0A,0xAe,0x4C, /*2059*/
0x09,0x2e,0x42, /*2060*/ 0x3C,0x97,0x35, /*2061*/ 0x0C,0x96,0x49, /*2062*/
0x7d,0x4A,0xBd, /*2063*/ 0x0d,0x4A,0x51, /*2064*/ 0x0d,0xA5,0x45, /*2065*/
0x55,0xAA,0xBA, /*2066*/ 0x05,0x6A,0x4e, /*2067*/ 0x0A,0x6d,0x43, /*2068*/
0x45,0x2e,0xB7, /*2069*/ 0x05,0x2d,0x4B, /*2070*/ 0x8A,0x95,0xBf, /*2071*/
0x0A,0x95,0x53, /*2072*/ 0x0B,0x4A,0x47, /*2073*/ 0x6B,0x55,0x3B, /*2074*/
0x0A,0xd5,0x4f, /*2075*/ 0x05,0x5A,0x45, /*2076*/ 0x4A,0x5d,0x38, /*2077*/
0x0A,0x5B,0x4C, /*2078*/ 0x05,0x2B,0x42, /*2079*/ 0x3A,0x93,0xB6, /*2080*/
0x06,0x93,0x49, /*2081*/ 0x77,0x29,0xBd, /*2082*/ 0x06,0xAA,0x51, /*2083*/
0x0A,0xd5,0x46, /*2084*/ 0x54,0xdA,0xBA, /*2085*/ 0x04,0xB6,0x4e, /*2086*/
0x0A,0x57,0x43, /*2087*/ 0x45,0x27,0x38, /*2088*/ 0x0d,0x26,0x4A, /*2089*/
0x8e,0x93,0x3e, /*2090*/ 0x0d,0x52,0x52, /*2091*/ 0x0d,0xAA,0x47, /*2092*/
0x66,0xB5,0x3B, /*2093*/ 0x05,0x6d,0x4f, /*2094*/ 0x04,0xAe,0x45, /*2095*/
0x4A,0x4e,0xB9, /*2096*/ 0x0A,0x4d,0x4C, /*2097*/ 0x0d,0x15,0x41, /*2098*/
0x2d,0x92,0xB5, /*2099*/
};

/*======================================================================
  ÿ��1�ž൱��Ԫ��������
======================================================================*/
static unsigned short const _G_days_interval[12] = 
{0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

/*======================================================================
  ÿ�������е�����
======================================================================*/
static unsigned char const _G_days_month[12] =
{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, };

/*==============================================================================
 * - _is_leapyear()
 *
 * - �ж� <year> �Ƿ�������, �������Ϊ���� ���� 1, ���򷵻� 0
 */
static int _is_leapyear (int year)
{
    return (year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0));
}

/*==============================================================================
 * - _is_big_moon()
 *
 * - ����<_G_year_code>��õ�ũ���µĴ��»�С��, �������Ϊ�󷵻� 1, ΪС���� 0
 */
static unsigned char _is_big_moon(unsigned char moon, unsigned short table_addr)
{
    switch (moon) {
        case 1: case 2: case 3: case 4:
            return (_G_year_code[table_addr]&(0x10 >> moon));
        case 5: case 6: case 7: case 8:case 9: case 10: case 11: case 12:
            return (_G_year_code[table_addr+1]&(0x100 >> (moon - 4)));
        case 13:
            return (_G_year_code[table_addr+2]&0x80);
    }
    return(0);
}


/*==============================================================================
 * - _fill_nongli_str()
 *
 * - ����ũ���������ɵ�֧, ����, ��Ф
 */
static int _fill_nongli_str (NONGLI_DATE *pNongli)
{
#define MIN_JIA_ZI_NIAN     4  /* ��С�ļ����� */
    static char tian_gan[10][3]  ={"��","��","��","��","��","��","��","��","��","��"};
    static char di_zhi[12][3]    ={"��","��","��","î","��","��","��","δ","��","��","��","��"};
    static char sheng_xiao[12][3]={"��","ţ","��","��","��","��","��","��","��","��","��","��"};
    static char moon[12][3]={"��","��","��","��","��","��","��","��","��","ʮ","��","��"};
    static char shi_ri[4][3]={"��","ʮ","إ","��"};
    static char ge_ri[10][3]={"ʮ","һ","��","��","��","��","��","��","��","��"};
    int x; /* ����һ��������ľ��� */
    
    if (pNongli == NULL || pNongli->nian < MIN_JIA_ZI_NIAN) {
        return -1;
    }

    x = (pNongli->nian - MIN_JIA_ZI_NIAN) % 60;                

    strcpy (&pNongli->tian_di[0], tian_gan[x%10]);
    strcpy (&pNongli->tian_di[2], di_zhi[x%12]);
    strcpy (&pNongli->sheng_xiao[0], sheng_xiao[x%12]);

    strcpy (&pNongli->ri_qi[0], moon[pNongli->yue - 1]);
    strcpy (&pNongli->ri_qi[2], "��");
    strcpy (&pNongli->ri_qi[4], (pNongli->ri == 10) ? shi_ri[0] : shi_ri[pNongli->ri / 10]);
    strcpy (&pNongli->ri_qi[6], ge_ri[pNongli->ri % 10]);

    return 0;
}

/*======================================================================
  24�������ֵĻ�׼����
======================================================================*/
static unsigned char const _G_jie_qi_days[]=
{
    6,20,4,19,6,21,         /* һ�µ�����  �Ľ����������� */
    5,20,6,21,6,21,         /* ���µ�����  �Ľ����������� */
    7,23,8,23,8,23,         /* ���µ�����  �Ľ����������� */
    8,24,8,22,7,22,         /* ʮ�µ�ʮ���µĽ����������� */
};

/*======================================================================
  ÿ��24������־��:
  ÿ��bit����ĳ�������Ƿ���Ҫ�Ի�׼���ڵ���
======================================================================*/
static unsigned char const _G_jie_qi_fix[]=
{
    0x4E,0xA6,0x99,    /*2000*/
    0x9C,0xA2,0x98,    /*2001*/
    0x80,0x00,0x18,    /*2002*/
    0x00,0x10,0x24,    /*2003*/
    0x4E,0xA6,0x99,    /*2004*/
    0x9C,0xA2,0x98,    /*2005*/
    0x80,0x82,0x18,    /*2006*/
    0x00,0x10,0x24,    /*2007*/
    0x4E,0xA6,0xD9,    /*2008*/
    0x9E,0xA2,0x98,    /*2009*/

    0x80,0x82,0x18,    /*2010*/
    0x00,0x10,0x04,    /*2011*/
    0x4E,0xE6,0xD9,    /*2012*/
    0x9E,0xA6,0xA8,    /*2013*/
    0x80,0x82,0x18,    /*2014*/
    0x00,0x10,0x00,    /*2015*/
    0x0F,0xE6,0xD9,    /*2016*/
    0xBE,0xA6,0x98,    /*2017*/
    0x88,0x82,0x18,    /*2018*/
    0x80,0x00,0x00,    /*2019*/

    0x0F,0xEF,0xD9,    /*2020*/
    0xBE,0xA6,0x99,    /*2021*/
    0x8C,0x82,0x98,    /*2022*/
    0x80,0x00,0x00,    /*2023*/
    0x0F,0xEF,0xDB,    /*2024*/
    0xBE,0xA6,0x99,    /*2025*/
    0x9C,0xA2,0x98,    /*2026*/
    0x80,0x00,0x18,    /*2027*/
    0x0F,0xEF,0xDB,    /*2028*/
    0xBE,0xA6,0x99,    /*2029*/

    0x9C,0xA2,0x98,    /*2030*/
    0x80,0x00,0x18,    /*2031*/
    0x0F,0xEF,0xDB,    /*2032*/
    0xBE,0xA2,0x99,    /*2033*/
    0x8C,0xA0,0x98,    /*2034*/
    0x80,0x82,0x18,    /*2035*/
    0x0B,0xEF,0xDB,    /*2036*/
    0xBE,0xA6,0x99,    /*2037*/
    0x8C,0xA2,0x98,    /*2038*/
    0x80,0x82,0x18,    /*2039*/

    0x0F,0xEF,0xDB,    /*2040*/
    0xBE,0xE6,0xD9,    /*2041*/
    0x9E,0xA2,0x98,    /*2042*/
    0x80,0x82,0x18,    /*2043*/
    0x0F,0xEF,0xFB,    /*2044*/
    0xBF,0xE6,0xD9,    /*2045*/
    0x9E,0xA6,0x98,    /*2046*/
    0x80,0x82,0x18,    /*2047*/
    0x0F,0xFF,0xFF,    /*2048*/
    0xFC,0xEF,0xD9,    /*2049*/
    0xBE,0xA6,0x18,    /*2050*/
};

/*======================================================================
  ���������������ַ���
======================================================================*/
static char const _G_jie_qi_str[25][5]=  /* �Թ��������Ⱥ����� */
{
    "",
 /*  ����         �Ƕ�    ��������     ���� */
    "С��",     /* 285     1�� 6�� */
    "��",     /* 300     1��20��    29.5�� */
    "����",     /* 315     2�� 4�� */
    "��ˮ",     /* 330     2��19��    29.8�� */
    "����",     /* 345     3�� 6�� */
    "����",     /*   0     3��21��    30.2�� */
    "����",     /*  15     4�� 5�� */
    "����",     /*  30     4��20��    30.7�� */
    "����",     /*  45     5�� 6�� */
    "����",     /*  60     5��21��    31.2�� */
    "â��",     /*  75     6�� 6�� */
    "����",     /*  90     6��21��    31.4�� */
    "С��",     /* 105     7�� 7�� */
    "����",     /* 120     7��23��    31.4�� */
    "����",     /* 135     8�� 8�� */
    "����",     /* 150     8��23��    31.1�� */
    "��¶",     /* 165     9�� 8�� */
    "���",     /* 180     9��23��    30.7�� */
    "��¶",     /* 195    10�� 8�� */
    "˪��",     /* 210    10��24��    30.1�� */
    "����",     /* 225    11�� 8�� */
    "Сѩ",     /* 240    11��22��    29.7�� */
    "��ѩ",     /* 255    12�� 7�� */
    "����",     /* 270    12��22��    29.5�� */
};

/*==============================================================================
 * - _get_jie_qi()
 *
 * - �õ�ָ���������ڵĽ����ţ�1 -- 24
 *   ������첻�ǽ������߲������Ϸ������� 0
 */
static int _get_jie_qi (int year, int month, int day)
{
    int maybe_jieqi;  /* �����������п��ܵĽ��� */
    int table_val;
    int need_fix;
    int jie_qi_day; /* ���п��ܵĽ������ڵ����� */

    /*
     *  Ŀǰ�����ж� 2000 ���� 2050 ��Ľ���
     */
    if ((year < 2000) || (year > 2050) ||
        (month < 0) || (month > 12)) {
        return 0;
    }

    /*
     * ������п��ܵĽ������ [0, 23]
     * ���Ӧ�� <_G_jie_qi_days>
     */
    maybe_jieqi = (month - 1) * 2;
    if (day >= 15) {
        maybe_jieqi++;
    }

    /*
     * ������п��ܵĽ����Ļ�׼����
     */
    jie_qi_day = _G_jie_qi_days[maybe_jieqi];

    /*
     * ����Ƿ���Ҫ�Խ�����׼���ڽ��е���
     */
    table_val = _G_jie_qi_fix[(year - 2000) * 3 + maybe_jieqi / 8];
    need_fix = (table_val << (maybe_jieqi % 8)) & 0x80;

    /*
     * �Ի�׼���ڽ��е���
     */
    if (need_fix != 0) {
        if( (maybe_jieqi == 1 || maybe_jieqi == 11 || 
             maybe_jieqi == 18|| maybe_jieqi == 21)
             && (year < 2044)) {

            jie_qi_day++;
        } else {
            jie_qi_day--;
        }
    }

    /*
     * ���п��ܵĽ������ڵ������Ƿ�Ͳ������
     */
    if (day == jie_qi_day) {
        return maybe_jieqi + 1; /* ���ؽ����� [1, 24] */
    } else {
        return 0;
    }
}

/*==============================================================================
 * - _get_next_jie_qi()
 *
 * - �����һ�������Ľ����� [1, 24]�������ؼ������
 *   ������ܵõ��¸����������� 0
 */
static int _get_next_jie_qi (int year, int month, int day, int *interval)
{
    int i = 1;
    int next_jie_qi_num;
    int this_month_days;

    /*
     * ��ñ��µ�����
     */
    this_month_days = _G_days_month[month - 1];
    if ((month == 2) && _is_leapyear (year)) {
        this_month_days++;
    }

    /*
     * �����ʮ��֮�ڲ����ֽ����Ļ��� * ����Ϊ�����е�����û��ͳ�ƽ���
     */
    do {
        /*
         * �����һ��
         */
        day++;
        if (day > this_month_days) {
            month = (month == 12) ? 1 : (month + 1);
            day = 1;
        }

        /*
         * �����Ƿ��ǽ���
         */
        next_jie_qi_num = _get_jie_qi (year, month, day);
        if (next_jie_qi_num != 0) {
            break;
        }

    } while (i++ < 20);

    *interval = i;

    return next_jie_qi_num;
}

/*==============================================================================
 * - _fill_bei_zhu()
 *
 * - ��ӱ�ע
 */
static void _fill_bei_zhu (int year, int month, int day, NONGLI_DATE *pNongli)
{
    int max_append;
    int next_jie_qi_num;
    int interval;
    char temp_bei_zhu[BEI_ZHU_LEN];

    /*
     * ��ձ�ע
     */
    pNongli->bei_zhu[0] = '\0';

    /*
     * ���첻�ǽ��� ��ע "�ഺ�ֻ���1��"
     */
    if (pNongli->jie_qi[0] == '\0') {
        max_append = BEI_ZHU_LEN - strlen(pNongli->bei_zhu) - 1;
        next_jie_qi_num = _get_next_jie_qi (year, month, day, &interval);
        
        if (next_jie_qi_num != 0) {
            sprintf (temp_bei_zhu, "��%s����%d��",
                    _G_jie_qi_str[next_jie_qi_num], interval);
            strncat (pNongli->bei_zhu, temp_bei_zhu, max_append);
        }
    }
}


/*==============================================================================
 * - nongli_get_date()
 *
 * - ���ݹ����õ�ũ������
 */
int nongli_get_date (int year, int month, int day, NONGLI_DATE *pNongli)
{
    int chunjie_month, chunjie_day;
    int chunjie_interval; /* ���ھ�Ԫ������ */
    int today_interval;   /* �����Ԫ������ */

    int table_addr;

    int moon;
    int moon_days;
    int leap_moon;

    int jie_qi_num;

    /* ������ */
    if ((year < 1901) ||
        (year > 2099) ||
        (month < 1)   ||
        (month > 12)  ||
        (day < 1) || (day > 31)) {

        return -1;
    }

    /* ��λ���ݱ��ַ */
    table_addr = (year - 1901) * 3;

    /* ȡ���괺�����ڵĹ����·� */
    chunjie_month = (_G_year_code[table_addr+2] & 0x60) >> 5;    
    /* ȡ���괺�����ڵĹ����� */
    chunjie_day = _G_year_code[table_addr+2] & 0x1f; 

    /* ���㵱�괺���뵱��Ԫ��������, ����ֻ���ڹ���1�»�2�� */
    chunjie_interval = chunjie_day - 1 + ((chunjie_month == 2) ? 31 : 0);

    /* ��������������뵱��Ԫ�������� */
    today_interval = _G_days_interval[month-1] + day - 1;

    /* ��������´���2�²��Ҹ����2��Ϊ����,������1 */
    if ((month > 2) && _is_leapyear(year)) {
        today_interval++;
    }

    /*
     * ���ڣ�����֪������������ ���³�һ ��Ӧ�Ĺ������ڣ�
     * <chunjie_month>�� <chunjie_day>��
     * ���ǽ���� �˹������� �� ������������ ֮��ֵ������
     * ����ս��������������������յ�ũ������
     */
    if (today_interval >= chunjie_interval) { /* �������ڴ��ں� */
        pNongli->nian = year;
        pNongli->yue = 1;   /* �����¿�ʼ������ */
        pNongli->is_run = 0;

        /* ��������봺�ڵĲ�ֵ */
        today_interval -= chunjie_interval;

        moon = 1;  /* �����¿�ʼ����ÿũ���µ����� */
        moon_days = _is_big_moon(moon,table_addr) ? 30 : 29;

        /* �����ݱ���ȡ����������·�,��Ϊ 0 ����������� */
        leap_moon = _G_year_code[table_addr] >> 4;     

        while (today_interval >= moon_days) { /* ����������ũ���£��Ƿ��ѽ����ո��� */

            today_interval -= moon_days;

            if (pNongli->yue == leap_moon) {
                pNongli->is_run = 1 - pNongli->is_run;

                if (pNongli->is_run == 0) {
                    pNongli->yue++;
                }
            } else {
                pNongli->yue++;
            }

            moon++;
            moon_days = _is_big_moon(moon,table_addr) ? 30 : 29;
        }

        pNongli->ri = today_interval + 1;
    } else {                                 /* �������ڴ���ǰ */
        pNongli->nian = year - 1;
        pNongli->yue = 12;   /* ��ʮ���¿�ʼ��ǰ���� */
        pNongli->is_run = 0;

        /* ��������봺�ڵĲ�ֵ */
        today_interval = chunjie_interval - today_interval;

        table_addr -= 3;
        leap_moon = _G_year_code[table_addr] >> 4;     
        moon = leap_moon ? 13 : 12; /* ��ũ�����һ���¿�ʼ����ÿũ���µ����� */
        moon_days = _is_big_moon(moon,table_addr) ? 30 : 29;

        while (today_interval > moon_days) { /* �����ǰ�������ũ���£��Ƿ��ѽ����ո��� */

            today_interval -= moon_days;

            if (pNongli->is_run == 0) {
                pNongli->yue--;
            }

            if (pNongli->yue == leap_moon) {
                pNongli->is_run = 1 - pNongli->is_run;
            }

            moon--;
            moon_days = _is_big_moon(moon,table_addr) ? 30 : 29;
        }

        pNongli->ri = moon_days - today_interval + 1;
    }

    /*
     * �õ�ũ���ַ���
     */
    _fill_nongli_str (pNongli);

    /*
     * �õ������ַ���
     */
    jie_qi_num = _get_jie_qi (year, month, day);
    strcpy (pNongli->jie_qi, _G_jie_qi_str[jie_qi_num]);

    /*
     * ��ӱ�ע
     */
    _fill_bei_zhu (year, month, day, pNongli);

    return (0);
}

/*==============================================================================
** FILE END
==============================================================================*/

