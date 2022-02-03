/*
 *	u2STRING.h	03/17/11	Release 7.3.1 
 *
 *	Rocket Software Confidential
 *	OCO Source Materials
 *	Copyright (C) Rocket Software, Inc.  2011-2011
 *
*/
/*
 * This file should contain commonly used defines for UV SRTING functions
 *
*******************************************************************************
*
*       Maintenence log - insert most recent change descriptions at top
*
*       Date.... GTAR#... WHO Description......................................
*   	08/01/13 UNV-15051 MYAN standard function declaration for STRtol64
*   07/08/13 UNV-15051 MBM Fixed build error for UniData 7.4 Windows.
*	06/27/13 UNV-15051 WMY add args.
*	08/01/11          CUI STRchar() might be already defined somewhere else.
*	03/17/11 UDT-3864 CUI created for UD 7.3.
*
*******************************************************************************
*/
#ifndef U2STRING_H
#define U2STRING_H

typedef struct  icstring
{
        int     len;
        unsigned char *text;
}       STRING;

extern STRING	STRcat(STRING, STRING); 
extern STRING	STRncat(STRING, STRING, int); 
extern STRING	STRcpy(STRING, STRING); 
extern STRING	STRncpy(STRING,STRING, int); 
extern STRING	STRstr(STRING, STRING) ; 
extern STRING 	strtSTR(char *data);
extern STRING	strScpy(STRING, char *); 
extern STRING	strScat(STRING, char *); 
extern STRING	STRtok(STRING *,STRING); 
extern STRING	STRtoa(STRING *,STRING); 
extern STRING	STRtoe(STRING *,STRING) ;
extern STRING STRtoq(STRING *str1, STRING str2, STRING str3, unsigned char *c);
extern int	STRcmp(STRING, STRING); 
extern int	STRncmp(STRING, STRING, int); 
extern int	STRdiff(STRING, STRING); 
extern int	STRtoi(STRING); 
extern int	STRspn(STRING, STRING) ;
extern int	STRcspn(STRING str1, STRING str2);
extern	STRING  STRextract(STRING, unsigned char, int, int);
extern	STRING  STRreplace(STRING, STRING, unsigned char, int);
extern	STRING	STRinsert(STRING, STRING, unsigned char, int);
extern	int     STRpathncmp(STRING, STRING, int);
extern	long	STRtol(STRING);
extern	long long STRtol64(STRING);

#ifndef STRchr
extern unsigned char *STRchr(STRING s, int c);
extern unsigned char *STRrchr(STRING s, int c);
#endif
extern char *STRtstr(STRING), *STRscpy(char *,STRING);
extern char *STRscpy(char *dest, STRING srce);

#endif	/* U2STRING_H */
