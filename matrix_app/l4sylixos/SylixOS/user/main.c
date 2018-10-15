#include "unistd.h"
#include "sys/utsname.h"

static INT  __tshellCmdReadMem (INT  iArgC, PCHAR  ppcArgV[]);

int  t_main (void)
{
    struct utsname  name;
    
    uname(&name);
    
    printf("sysname  : %s\n", name.sysname);
    printf("nodename : %s\n", name.nodename);
    printf("release  : %s\n", name.release);
    printf("version  : %s\n", name.version);
    printf("machine  : %s\n", name.machine);
    
    Lw_TShell_Create(STDOUT_FILENO, LW_OPTION_TSHELL_PROMPT_FULL | LW_OPTION_TSHELL_VT100);
    
    API_TShellKeywordAdd("readmem", __tshellCmdReadMem);

    return  (0);
}


static INT  __tshellCmdReadMem (INT  iArgC, PCHAR  ppcArgV[])
{
    CHAR    cParam[64];
    ULONG   ulParam;

    lib_strlcpy(cParam, ppcArgV[1], sizeof(cParam));

    ulParam = lib_strtoul(cParam, LW_NULL, 16);

    printf("0x%X : ", (UINT32)ulParam);
    printf("%X\n", *(UINT32 *)ulParam);

    return  (ERROR_NONE);
}
