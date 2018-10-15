/****************************************Copyright (c)*****************************************************
**                            Guangzhou ZHIYUAN electronics Co.,LTD.
**                                      
**                                 http://www.embedtools.com
**
**--------------File Info----------------------------------------------------------------------------------
** File name:               vi_fix.c
** Last modified Date:      2011-6-1 13:47:06
** Last Version:            1.0.0
** Descriptions:            This file implement some funtions that VxWorks is pressed for.
**
**---------------------------------------------------------------------------------------------------------
** Created by:              WangDongfang
** Last modified Date:      2011-6-1 13:47:06
** Version:                 1.0.0
** Descriptions:            This file implement some funtions that VxWorks is pressed for.
**
**********************************************************************************************************/

#include "vi_fix.h"

/*********************************************************************************************************
** ��������: get_terminal_width_height
** ��������: ����ն˴��ڵĴ�С
** �䡡��  : fd            �ն�
**           width         ����
**           height        �߶�
** �䡡��  : 0: OK
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
int  get_terminal_width_height (const int  fd, int  *width, int  *height)
{
    if (width) {
        *width = __VI_TERMINAL_WIDTH;
    }
    if (height) {
        *height = __VI_TERMINAL_HEIGHT;
    }
    
    return  (0);
}
/*********************************************************************************************************
** ��������: last_char_is
** ��������: �ж�ָ���ַ��Ƿ����ַ��������һ���ַ�
** �䡡��  : s             �ַ���
**           c             ���һ���ַ�
** �䡡��  : ����������ͬ����ָ��, �����ͬ���� NULL
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
char *last_char_is (const char *s, int c)
{
    if (s && *s) {
        size_t sz = lib_strlen(s) - 1;
        s += sz;
        if ((unsigned char)*s == c) {
            return  (char*)s;
        }
    }
    return  NULL;
}
/*********************************************************************************************************
** ��������: lib_xzalloc
** ��������: �ڴ����(����)
** �䡡��  : s             ��С
** �䡡��  : ���ٳɹ������ڴ��ַ, ����ֱ���˳��߳�.
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
void  *lib_xzalloc (size_t s)
{
    return lib_memset(lib_xmalloc(s), 0, s);
}
/*********************************************************************************************************
** ��������: full_write
** ��������: �����л������ڵ�����д���ļ�
** �䡡��  : fd        �ļ�
**           buf       ������
**           len       ����
** �䡡��  : д��ĳ���
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
ssize_t full_write (int fd, const void *buf, size_t len)
{
    ssize_t temp;
    ssize_t total = 0;
    
    while (len) {
        temp = safe_write(fd, (char *)buf, len);
        if (temp < 0) {
            return  (temp);
        }
        total += temp;
        buf    = ((const char *)buf) + temp;
        len   -= (size_t)temp;
    }

	return  (total);
}
/*********************************************************************************************************
** ��������: bb_putchar
** ��������: ���һ���ַ�
** �䡡��  : c     �ַ�
** �䡡��  : 
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
int  bb_putchar (int ch)
{
    return  (fputc(ch, stdout));
}

/* wdf */
/**********************************************************************************************************
  find a char in string from left, return the char addr if find or the '\0' addr
**********************************************************************************************************/
char *strchrnul(const char *str, char c)
{
    char *p = strchr(str, c);

    if (p == NULL) {
        return (char *)(str + strlen(str));
    } else {
        return p;
    }
}

/**********************************************************************************************************
  count of a string chars, return strlen if it's little than maxlen or maxlen
**********************************************************************************************************/
size_t strnlen (const char *s,
				size_t imaxlen)
{
    const char *save = s + 1;

    while (*s++ != EOS && (s - save) < imaxlen)
        ;

    return (s - save);
}

/**********************************************************************************************************
  copy a string to a new malloc memory, return the new string address
**********************************************************************************************************/
char * xstrdup (const char *string)
{
	return strcpy (xmalloc (strlen (string) + 1), string);
}

/**********************************************************************************************************
  copy a string to a new malloc memory, if source strlen > n, only copy front n chars
**********************************************************************************************************/
char * xstrndup (const char *s, size_t n)  
{  
    char *result;  
    size_t len = strlen (s);  

    if (n < len)  
        len = n;  

    result = memH_malloc(len + 1);   

    result[len] = '\0';  
    return (char *) memcpy (result, s, len);  
}

/**********************************************************************************************************
  alloc memory, if failed warning caller
**********************************************************************************************************/
void * xmalloc (size_t size)
{
    register void *value = memH_malloc(size);
    if (value == 0)
        printf("virtual memory exhausted");

    return value;
}
/* wdf */

/*********************************************************************************************************
  END
*********************************************************************************************************/