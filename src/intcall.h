/******************************************************************************
*
*	intercall 2.1 - Intercall header file.
 *      Rocket Software Confidential
 *      OCO Source Materials
 *      Copyright Rocket Software, Inc.  1994-2012
*
*	Module	intcall.h	Version	2.1.0.1 	Date	11/21/94
*
*       IBM Confidential
*       OCO Source Materials
*       Copyright (C) IBM Corp.  1994, 2008
*
*	(c) Copyright 1998 Ardent Software Inc. - All Rights Reserved
*	This is unpublished proprietary source code of Ardent Software Inc.
*	The copyright notice above does not evidence any actual or intended
*	publication of such source code.
*
*******************************************************************************
*
*	Maintenence log - insert most recent change descriptions at top
*
*	Date.... GTAR# WHO Description.........................................
*	03/28/16 UCC-3046 WMY take out FAR PASCAL
*	06/24/14 UCC-3009 add undef FAR and PASCAL 
*	06/19/14 UCC-3009 WMY add PASCAL FAR for newly added funcs.
*	06/03/14 UCC-2973 WMY add ic_set_ipv() and ic_get_ipv().
*	ic_get_sessionipv()
*	06/21/12 W.YEH UCC-1546 merge from ud/icsdk.
*	09/30/08 W.YEH combine uv and changes. 64bit ud and ssl uv.
*	05/04/04       CUI Added SSL error code.
*       01/20/99 24251 DJD Added support for multiple databases.
*       11/04/98 23801 DJD Changed copyright.
*	11/05/98 23617 OGO Change comments from uvrpc to unirpc.
*	10/23/98 23765 CSM Change prototype of opensession
*	11/24/97 22201 AGM Port to UNIX
*	07/22/97 20403 DW  Added NLS Locale category tokens.
*	07/22/97 20104 DW  Add I_SQLNULL
*	07/01/97 20403 DW  Match NLS tokens to revised UniVerse NLS tokens.
*	05/27/97 20403 DW  Match NLS tokens to UniVerse NLS.
*	02/27/97 20104 DW  Added NLS functions.
*       06/03/96 18553 DJD Added new server and lan pipe error codes.
*	07/17/96 18856 ALC Modified comments for codes 80011 and 80019
*       03/26/96 18189 DJD Added ic_set_comms_timeout.
*       02/12/96 18025 DJD Added new InterCall codes for IC 2.1.
*       11/13/95 17162 DJD Added new intercall codes
*       10/26/95 17162 DJD Added new error code for UNIX server
*       07/17/95 16924 DJD Added ICSTRING struct for users.
*       05/16/95   -   DJD Added Control Code error for trans
*	11/17/94 16333 DJD Created for InterCALL 2.0 from InterCALL 1.0.
*
******************************************************************************/

#include "u2STRING.h"

#ifdef	__cplusplus
extern "C" {
#endif

extern  void *calloc(size_t __nmemb, size_t __size);
extern  void *malloc(size_t __size);
extern  void *realloc(void *__ptr, size_t __size);

/* Mark characters */

#define I_IM                   255
#define I_FM                   254
#define I_VM                   253
#define I_SM                   252
#define I_TM                   251
#define I_SQLNULL              128

/* ICSTRING Struct def. */
typedef STRING ICSTRING ;


/* Limits */

#define IC_MAX_RECID_LENGTH    1024    /* Maximum length of a record id*/
#define IC_MAX_SELECT_LIST     10      /* Highest select list number   */

/* Parameters (key values) */

#define IK_DATA                0      /* Open the data portion        */
#define IK_DICT                1      /* Open the dictionary portion  */

#define IK_DELETE              0      /* Release any locks on that record during DELETE*/
#define IK_DELETEW             1      /* Release any locks on that record during DELETE + wait for lock in the first place */
#define IK_DELETEU             3      /* Retain any locks during DELETE*/

#define IK_READ                0      /* READ without locking         */
#define IK_READU               2      /* READU lock                   */
#define IK_READUW	       3      /* READU lock + wait for lock to clear */
#define IK_READL               4      /* READL lock                   */
#define IK_READLW	       5      /* READL lock + wait for lock to clear */
#define IK_WAIT                1      /* An additive value to IK$READU and IK$READL*/

#define IK_WRITE               0      /* Release the lock during WRITE*/
#define IK_WRITEW              1      /* Release the lock during WRITE + wait for lock in the first place*/
#define IK_WRITEU              5      /* Retain the lock during WRITE */

#define IK_HCNONE              0      /* No conversions forced to the host*/
#define IK_HCDATE              1      /* Force date conversions to be done on the host*/
#define IK_HCTIME              2      /* Force time conversions to be done on the host*/
#define IK_HCMD                4      /* Force MD conversions to be done on the host*/

/* Transaction support */

#define IK_START		1	/* Starts a transaction */
#define IK_COMMIT		2	/* Commits a transaction */
#define IK_ROLLBACK		3	/* Rolls back a transaction */

/* Session Info Keys */

#define IK_HOSTNAME		1	/* Hostname of conection */
#define IK_HOSTTYPE		2	/* Host type, either UNIX or NT */
#define IK_TRANSPORT		3	/* Transport used by session */
#define IK_USERNAME		4	/* Username used by session */
#define IK_STATUS		5	/* Status of connection */
#define IK_NLS			6	/* Status of NLS */

/* NLS Mark keys */

#define IK_IM			6
#define IK_AM			5
#define IK_VM			4
#define IK_SM			3
#define IK_TM			2
#define IK_NULL			1
#define IE_NLS_DEFAULT		1	/* NLS status: default used */
#define IE_NLS_BAD_MARKS	2	/* NLS status: marks are not distinct */

/* NLS Locale Category keys */

#define IK_LC_ALL		0
#define IK_LC_COLLATE		1
#define IK_LC_CTYPE		2
#define IK_LC_MONETARY		3
#define IK_LC_NUMERIC		4
#define IK_LC_TIME		5

/* FILEINFO keys (copied from fileinfo.it in PI) */

#define FINFO_IS_FILEVAR       0      /* Anything                     */
#define FINFO_VOCNAME          1      /* PI only                      */
#define FINFO_PATHNAME         2      /* ALL                          */
#define FINFO_TYPE             3      /* ALL                          */
#define FINFO_HASHALG          4      /* LH, SH                       */
#define FINFO_MODULUS          5      /* LH, SH                       */
#define FINFO_MINMODULUS       6      /* LH                           */
#define FINFO_GROUPSIZE        7      /* LH                           */
#define FINFO_LARGERECORDSIZE  8      /* LH                           */
#define FINFO_MERGELOAD        9      /* LH                           */
#define FINFO_SPLITLOAD        10     /* LH                           */
#define FINFO_CURRENTLOAD      11     /* LH (percentage)              */
#define FINFO_NODENAME         12     /* ALL. Null if local, else nodename*/
#define FINFO_IS_AKFILE        13     /* LH                           */
#define FINFO_CURRENTLINE      14     /* SEQ                          */
#define FINFO_PARTNUM          15     /* Distributed, Multivolume     */
#define FINFO_STATUS           16     /* Distributed, Multivolume     */
#define FINFO_RECOVERYTYPE     17     /* ALL                          */
#define FINFO_RECOVERYID       18     /* LH                           */
#define FINFO_IS_FIXED_MODULUS 19     /* LH                           */
/* Next one depends on whether Primos or PI/open really */
#define FINFO_MAXOPTION        19     /* Maximum key                  */


/* AT Variable tocken's for GET/SET */

#define IK_AT_LOGNAME			1
#define IK_AT_PATH			2
#define IK_AT_USERNO			3
#define IK_AT_WHO			4
#define	IK_AT_TRANSACTION		5
#define IK_AT_DATA_PENDING		6
#define IK_AT_USER_RETURN_CODE		7
#define	IK_AT_SYSTEM_RETURN_CODE	8
#define	IK_AT_NULL_STR			9
#define	IK_AT_SCHEMA			10
#define IK_AT_TRANSACTION_LEVEL		11

/* File type values as returned by fileinfo. Taken from filetypes.it. */

#define FILETYPE_MEMORY        1
#define FILETYPE_HASHED        2
#define FILETYPE_DYNAMIC       3
#define FILETYPE_TYPE1         4
#define FILETYPE_SEQ           5
#define FILETYPE_MULTIVOLUME   6
#define FILETYPE_DISTRIBUTED   7

/* Lock status values returned by RECORDLOCKED().  Taken from rec_locked.it. */

#define LOCK_MY_FILELOCK       3      /* this user has filelock       */
#define LOCK_MY_READU          2      /* this user has readu lock     */
#define LOCK_MY_READL          1      /* this user has readl lock     */
#define LOCK_NO_LOCK           0      /* record not locked            */
#define LOCK_OTHER_READL       -1     /* another user has readl lock  */
#define LOCK_OTHER_READU       -2     /* another user has readu lock  */
#define LOCK_OTHER_FILELOCK    -3     /* another user has filelock    */

/* Error numbers. */

/* Numbers relating to the C library on the PC. */
/* These are adapted from the file errno.h. */

#define IE_PC_CLIB_FIRST       14000  /* First error number in range  */
#define IE_ENOENT              14002  /* No such file or directory    */
#define IE_EIO                 14005  /* I/O error                    */
#define IE_EBADF               14009  /* Bad file number              */
#define IE_ENOMEM              14012  /* No memory available          */
#define IE_EACCES              14013  /* Permission denied            */
#define IE_EINVAL              14022  /* Invalid argument             */
#define IE_ENFILE              14023  /* File table overflow          */
#define IE_EMFILE              14024  /* Too many open files          */
#define IE_ENOSPC              14028  /* No space left on device      */

/* Numbers relating to the Virtual Socket Library on the PC. */

#define IE_BW_START            14200  /* Beginning of range           */
#define IE_BW_BASE             14500  /* Offset for error mapping     */

#define IE_VSL_BADNETKEY       14495  /* VSL network type not specified correctly*/
#define IE_VSL_NONETMODULE     14499  /* VSL network module not loaded*/

#define IE_BW_WOULDBLOCK       14535  /* Operation would block        */
#define IE_BW_INPROGRESS       14536  /* Operation now in progress    */
#define IE_BW_ALREADY          14537  /* Operation already in progress*/
#define IE_BW_NOTSOCK          14538  /* Socket operation on non-socket*/
#define IE_BW_DESTADDRREQ      14539  /* Destination address required */
#define IE_BW_MSGSIZE          14540  /* Message too long             */
#define IE_BW_PROTOTYPE        14541  /* Protocol wrong type for socket*/
#define IE_BW_NOPROTOOPT       14542  /* Bad protocol option          */
#define IE_BW_PROTONOSUPPORT   14543  /* Protocol not supported       */
#define IE_BW_SOCKTNOSUPPORT   14544  /* Socket type not supported    */
#define IE_BW_OPNOTSUPP        14545  /* Operation not supported on socket*/
#define IE_BW_PFNOSUPPORT      14546  /* Protocol family not supported*/
#define IE_BW_AFNOSUPPORT      14547  /* Addr family not supported by prot family*/
#define IE_BW_ADDRINUSE        14548  /* Address already in use       */
#define IE_BW_ADDRNOTAVAIL     14549  /* Can't assign requested address*/
#define IE_BW_NETDOWN          14550  /* Network is down              */
#define IE_BW_NETUNREACH       14551  /* Network is unreachable       */
#define IE_BW_NETRESET         14552  /* Network dropped connection or reset*/
#define IE_BW_CONNABORTED      14553  /* Software caused connection abort*/
#define IE_BW_CONNRESET        14554  /* Connection reset by peer     */
#define IE_BW_NOBUFS           14555  /* No buffer space available    */
#define IE_BW_ISCONN           14556  /* Socket is already connected  */
#define IE_BW_NOTCONN          14557  /* Socket is not connected      */
#define IE_BW_SHUTDOWN         14558  /* Can't send after socket shutdown*/
#define IE_BW_TOOMANYREFS      14559  /* Too many references: can't splice*/
#define IE_BW_TIMEDOUT         14560  /* Connection timed out         */
#define IE_BW_CONNREFUSED      14561  /* Connection refused           */
#define IE_BW_LOOP             14562  /* Too many levels of symbolic links*/
#define IE_BW_NAMETOOLONG      14563  /* File name too long           */
#define IE_BW_HOSTDOWN         14564  /* Host is down                 */
#define IE_BW_HOSTUNREACH      14565  /* Host is unreachable          */
#define IE_BW_NOTEMPTY         14566  /* Directory not empty          */
#define IE_BW_PROCLIM          14567  /* Too many processes           */
#define IE_BW_USERS            14568  /* Too many users               */
#define IE_BW_DQUOT            14569  /* Disc quota exceeded          */
#define IE_BW_STALE            14570  /* Stale NFS file handle        */
#define IE_BW_REMOTE           14571  /* Too many levels of remote in path*/
#define IE_BW_NOSTR            14572  /* Device is not a stream       */
#define IE_BW_TIME             14573  /* Timer expired                */
#define IE_BW_NOSR             14574  /* Out of streams resources     */
#define IE_BW_NOMSG            14575  /* No message of desired type   */
#define IE_BW_BADMSG           14576  /* Trying to read unreadable message*/
#define IE_BW_IDRM             14577  /* Identifier removed           */
#define IE_BW_DEADLK           14578  /* Deadlock condition.          */
#define IE_BW_NOLCK            14579  /* No record locks available.   */
#define IE_BW_BADVERSION       14580  /* Library/driver version mismatch*/
#define IE_BW_INVALSOCK        14581  /* Invalid argument             */
#define IE_BW_TOOMANYSOCK      14582  /* Too many open sockets        */
#define IE_BW_FAULTSOCK        14583  /* Bad address in sockets call  */
#define IE_BW_RESET            14584  /* The socket has reset         */
#define IE_BW_NOTUNIQUE        14585  /* Unique parameter required    */
#define IE_BW_NOGATEADDR       14586  /* Gateway address required     */
#define IE_BW_SENDERR          14587  /* The packet could not be sent */
#define IE_BW_NOETHDRVR        14588  /* No driver or card failed init*/
#define IE_BW_WRITPENDING      14589  /* Queued write operation       */
#define IE_BW_READPENDING      14590  /* Queued read operation        */
#define IE_BW_NOTCPIP          14591  /* TCPIP not loaded             */
#define IE_BW_DRVBUSY          14592  /* TCPIP busy                   */
#define IE_BW_END              14999  /* End of range                 */

/* Numbers in this group are derived from INFORMATION itself. */
/* DO NOT CHANGE THESE!!! except for changes in INFORMATION... */

#define IE_FRST                10000  /* First PI-specific error number*/

/* Errors generated from library routine calls */

#define IE_NOACCESS            11000  /* Requested access denied      */
#define IE_NOSUPPORT           11001  /* Function not supported on this system*/
#define IE_NOTRELATIVE         11002  /* Relative pathname expected and not given*/
#define IE_PATHNOTFOUND        11003  /* Pathname could not be found  */
#define IE_NOTASSIGNED         11004  /* Device not assigned          */
#define IE_NODEVICE            11005  /* Device not known             */
#define IE_ROFS                11006  /* Device assigned with Read Only access*/
#define IE_BADSTTY             11007  /* Bad stty option when device assigned*/
#define IE_UNKNOWN_USER        11008  /* Attempting to send message to user not in PI*/
#define IE_SND_REQ_REC         11009  /* Sender requires receive enabled*/
#define IE_MSG_REJECTED        11010  /* Message rejected by recipient*/

#define IE_ALI                 22000  /* Already initialised          */
#define IE_BFN                 22001  /* bad field number (READV, WRITEV...)*/
#define IE_BTS                 22002  /* buffer.size too small or not valid number*/
#define IE_IID                 22003  /* Illegal record ID            */
#define IE_LRR                 22004  /* last record read (READNEXT)  */
#define IE_NFI                 22005  /* file.tag is not a file identifier*/
#define IE_NIN                 22006  /* Client library not initialised for this task*/
#define IE_NLK                 22007  /* The file was not locked by your process*/
#define IE_NPI                 22008  /* Prime INFORMATION is not available*/
#define IE_STR                 22009  /* The FILEINFO result is a string.*/
#define IE_MEM                 22010  /* no memory to DIM an array (OPEN)*/
#define IE_RLS                 22011  /* Error releasing memory       */
#define IE_BPF                 22012  /* Bad Partitioned file         */
#define IE_ALG                 22013  /* Bad Partitioning algorithm   */
#define IE_NUP                 22014  /* Non-unique Part number       */
#define IE_DNA                 22015  /* Dynt not available           */

/* Generic and visible file system errors */

#define IE_RNF                 30001  /* Record not found             */
#define IE_LCK                 30002  /* File or record is locked by another user*/
#define IE_FITF                30007  /* File table (ie smm) full     */
#define IE_PAR                 30010  /* Bad parameter                */
#define IE_KEY                 30011  /* Bad key                      */
#define IE_EXS                 30012  /* File already exists in an attempt to create*/
#define IE_WFT                 30013  /* Wrong file type: not segdir or dir*/
#define IE_EXCL                30014  /* File opened exclusively by another user*/
#define IE_BRWL                30015  /* Rwlock on file is wrong and can't be fixed*/
#define IE_WSFT                30016  /* Wrong subfile type           */
#define IE_SFNF                30018  /* Subfile not found when expected*/
#define IE_BMF                 30019  /* Bad header in memory file    */
#define IE_UFT                 30020  /* Unsupported file type        */
#define IE_UNKN                30021  /* Unknown file type detected   */
#define IE_IOF                 30031  /* Illegal operation on file    */
#define IE_BSHR                30049  /* Shared data for file doesn't match file*/
#define IE_BLHP                30052  /* Bad header in LH primary subfile*/
#define IE_BLHO                30053  /* Bad header in LH overflow subfile*/
#define IE_NAM                 30075  /* Bad file name                */
#define IE_UFI                 30086  /* Unimplemented FILEINFO request*/
#define IE_BIL                 30094  /* Bad ID length                */
#define IE_FIFS		       30095  /* Fileid is incorrect for session */
#define IE_USC                 30096  /* Unsupport Server command, functions not availble let */
#define IE_SELFAIL	       30097  /* Select Failed */
#define IE_LOCKINVALID         30098  /* Lock number provided is invalid */
#define IE_SEQOPENED           30099  /* Filed opened for sequential access and hashed access tried */
#define IE_HASHOPENED	       30100  /* Filed opened for hashed access and sequential access tried */
#define IE_SEEKFAILED	       30101  /* Seek command failed */
#define IE_DATUMERROR	       30102  /* Internal datum error */
#define IE_INVALIDATKEY	       30103  /* Invalid Key used for GET/SET at variables */
#define IE_INVALIDFILEINFOKEY  30104  /* FILEINFO Key out of range */
#define IE_UNABLETOLOADSUB     30105  /* Unable to load subroutine on host */
#define IE_BADNUMARGS	       30106  /* Bad number of arguments for subroutine, either too many or not enough */
#define IE_SUBERROR	       30107  /* Subroutine failed to complete suceesfully */
#define IE_ITYPEFTC	       30108  /* IType failed to complete correctly */
#define IE_ITYPEFAILEDTOLOAD   30109  /* IType failed to load */
#define IE_ITYPENOTCOMPILED    30110  /* The IType has not been compiled */
#define IE_BADITYPE	       30111  /* It is not an itype or the itype is corrupt */
#define IE_INVALIDFILENAME     30112  /* Filename is null */
#define IE_WEOFFAILED	       30113  /* Weofseq failed */
#define IE_EXECUTEISACTIVE     30114  /* An execute is currently active */
#define IE_EXECUTENOTACTIVE    30115  /* An execute is currently active */
#define IE_BADEXECUTESTATUS    30116  /* Internal execute error, execute has not return an expected status */
#define IE_INVALIDBLOCKSIZE    30117  /* Blocksize is invalid for call */
#define IE_BAD_CONTROL_CODE    30118  /* Bad trans control code */
#define IE_BAD_EXEC_CODE       30119  /* Execute did not send returncodes bad to client correctly */
#define IE_BAD_TTY_DUP         30120  /* failure to dup ttys */
#define IE_BAD_TX_KEY          30121  /* Bad Transaction Key */
#define IE_TX_COMMIT_FAILED    30122  /* Transaction commit has failed */
#define IE_TX_ROLLBACK_FAILED  30123  /* Transaction rollback has failed */
#define IE_TX_ACTIVE           30124  /* A Transaction is active so this action is forbidden */
#define IE_CANT_ACCESS_PF      30125  /* Can not access part files */
#define IE_FAIL_TO_CANCEL      30126  /* failed to cancel an execute */
#define IE_INVALID_INFO_KEY    30127  /* Bad key for ic_session_info */
#define IE_CREATE_FAILED       30128  /* create of sequential file failed */
#define IE_DUPHANDLE_FAILED    30129  /* Failed to duplicate a pipe handle */

/* Errors for OPEN */
#define IE_NVR                 31000  /* No VOC record                */
#define IE_NPN                 31001  /* No pathname in VOC record    */
#define IE_VNF                 31002  /* VOC file record not a File record*/

/* Errors for CLEARFILE */

#define IE_CFNEA               31100  /* Clear file no exclusive access*/

/* Errors for client library, taken from ICI */

#define IE_LNA                 33200  /* Select list not active       */
#define IE_PAR1                33201  /* Bad parameter 1              */
#define IE_PAR2                33202  /* Bad parameter 2              */
#define IE_PAR3                33203  /* Bad parameter 3              */
#define IE_PAR4                33204  /* Bad parameter 4              */
#define IE_PAR5                33205  /* Bad parameter 5              */
#define IE_PAR6                33206  /* Bad parameter 6              */
#define IE_PAR7                33207  /* Bad parameter 7              */
#define IE_PAR8                33208  /* Bad parameter 8              */
#define IE_PAR9                33209  /* Bad parameter 9              */
#define IE_BSLN                33211  /* Bad select list number       */
#define IE_BPID                33212  /* Bad partfile id              */
#define IE_BAK                 33213  /* Bad AK file                  */

/* Error numbers generated explicitly by the interCALL server: */

#define IE_BAD_COMMAND         39000  /* command not recognized by server*/
#define IE_NO_LOGOUT           39001  /* no way to perform a LOGOUT command*/
#define IE_BAD_LENGTH          39002  /* data.length not a valid number*/
#define IE_NO_VOC              39003  /* can't open the VOC file      */
#define IE_CLIENT_RESET        39004  /* internal - client RESET received OK*/
#define IE_INVALID_SRC         39005  /* @SYSTEM.RETURN.CODE non-numeric after EXECUTE*/
#define IE_TOOLONG_SRC         39006  /* @SYSTEM.RETURN.CODE has more than 2 fields*/
#define IE_KEY_NOT_IMP         39007  /* interCALL server key not implemented*/
#define IE_WRITE_FAILURE       39008  /* WRITE failed and taken ELSE clause*/

/* Errors for the Client/Server interface */

#define IE_NODATA              39101  /* Host not responding          */
#define IE_SYNC_TIMEOUT        39102  /* Synchroniser not received    */
#define IE_RCV_TIMEOUT         39103  /* Timeout on receving packets  */
#define IE_HOSTERROR           39104  /* Host length error on receive */
#define IE_NOT_READY           39105  /* Host does not give "ready" prompt*/
#define IE_NO_ACK              39106  /* Packet not acknowledged      */
#define IE_NUM_TASKS           39107  /* Too many concurrent user tasks*/
#define IE_UDATA_LOCK          39108  /* Could not lock user data block*/
#define IE_LIBINUSE            39109  /* Library in use               */
#define IE_DATA_LOSS           39110  /* Host got incorrect length from PC*/
#define IE_HOST_NNUM           39111  /* Host response non-numeric    */
#define IE_HOST_DATA           39112  /* Host length error on receive */
#define IE_HOST_RESPONSE       39113  /* No data in host response     */
#define IE_NO_HOST_NAME        39114  /* Host name missing from script file*/
#define IE_SOCKET_CLOSED       39115  /* Host has closed socket       */
#define IE_BAD_HOST_NAME       39116  /* Failed to get address for this host*/
#define IE_FATAL               39117  /* Fatal error */
#define IE_BAD_ERROR           39118  /* Bad error number from host, i.e. error 0*/
#define IE_AT_INPUT            39119  /* Server waiting for input */
#define IE_SESSION_NOT_OPEN    39120  /* Session is not opened when an action has be tried on it */
#define IE_UVEXPIRED	       39121  /* The Universe license has expired */
#define IE_CSVERSION	       39122  /* Client or server is out of date Client/server functions have been updated*/
#define IE_COMMSVERSION	       39123  /* Client or server is out of date comms support has been updated */
#define IE_BADSIG	       39124  /* Incorrect client/server being commuincated with */
#define IE_BADDIR	       39125  /* The dicteroy you are connecting to, either is not a universe account or does not exist */
#define IE_SERVERERR	       39126  /* An error has occurred on the server when trying to transmit an error code to the client */
#define IE_BAD_UVHOME         39127  /* Unable to get the uv home coorectly */
#define IE_INVALIDPATH        39128  /* Bad path found UV.ACCUNTS file */
#define IE_INVALIDACCOUNT      39129  /* Account name given is not an account */
#define IE_BAD_UVACCOUNT_FILE  39130  /* UV.ACCOUNT file could not be found to opened */
#define IE_FTA_NEW_ACCOUNT     39131  /* Failed to attach to the account specified */
#define IE_NOT_UVACCOUNT       39132  /* not a valid universe account */
#define IE_FTS_TERMINAL        39133  /* failed to setup the terminal for server */
#define IE_ULR                 39134  /* user limited reached */     

#define IE_NO_NLS	       		39135  /* NLS support not available */
#define IE_MAP_NOT_FOUND		39136  /* NLS map not found */
#define IE_NO_LOCALE			39137  /* NLS locale support not available */
#define IE_LOCALE_NOT_FOUND		39138  /* NLS locale not found */
#define IE_CATEGORY_NOT_FOUND	39139  /* NLS locale category not found */

#define IE_SR_CREATE_PIPE_FAIL 39200  /* Server failed to create the slave pipes */
#define IE_SR_SOCK_CON_FAIL    39201  /* Server failed to connect to socket */
#define IE_SR_GA_FAIL          39202  /* Slave failed to give server the Go Ahead message */
#define IE_SR_MEMALLOC_FAIL    39203  /* Failed to allocate memory for the message from the slave */
#define IE_SR_SLAVE_EXEC_FAIL  39204  /* The slave failed to start correctly */
#define IE_SR_PASS_TO_SLAVE_FAIL 39205 /* Failed to the pass the message to the slave correctly */
#define IE_SR_EXEC_ALLOC_FAIL  39206  /* Server failed to allocate the memory for the execute buffer correctly */
#define IE_SR_SLAVE_READ_FAIL   39207 /* Failed to read from the slave correctly */
#define IE_SR_REPLY_WRITE_FAIL 39208  /* Failed to write the reply to the slave (ic_inputreply) */
#define IE_SR_SIZE_READ_FAIL   39209  /* Failed to read the size of the message from the slave */
#define IE_SR_SELECT_FAIL      39210  /* Server failed to select() on input channel */
#define IE_SR_SELECT_TIMEOUT   39211  /* The select has timed out */

#define IE_BAD_LOGINNAME       80011  /* login name or password provided is incorrect */
#define IE_BAD_PASSWORD        80019  /* password provided has expired */
#define IE_REM_AUTH_FAILED     80036  /* Remote authorisation failed */
#define IE_ACCOUNT_EXPIRED     80144  /* The account has expired */
#define IE_RUN_REMOTE_FAILLED  80147  /* Unable to run on the remote machine as the given user. */
#define IE_UPDATE_USER_FAILED  80148  /* Unable to update user details. */

#define UVRPC_BAD_CONNECTION        81001  /* connection is bad */
#define UVRPC_NO_CONNECTION         81002  /* connection is down */
#define UVRPC_NOT_INITED            81003  /* the rpc has not be initialised */
#define UVRPC_INVALID_ARG_TYPE      81004  /* argument for message is not a valid type */
#define UVRPC_WRONG_VERSION         81005  /* rpc version mismatch */
#define UVRPC_BAD_SEQNO             81006  /* packet message out of step */
#define UVRPC_NO_MORE_CONNECTIONS   81007  /* not more connections available */
#define UVRPC_BAD_PARAMETER         81008  /* bad parameter passed to the rpc */
#define UVRPC_FAILED                81009  /* rpc failed */
#define UVRPC_ARG_COUNT             81010  /* bad number pf arguments for message */
#define UVRPC_UNKNOWN_HOST          81011  /* bad hostname, or host not responding */
#define UVRPC_FORK_FAILED           81012  /* rpc failed to fork service correctly */
#define UVRPC_CANT_OPEN_SERV_FILE   81013  /* cannot find or open the unirpcserices file */
#define UVRPC_CANT_FIND_SERVICE     81014  /* unable to find the service in the unirpcservices file */
#define UVRPC_TIMEOUT               81015  /* connection has timed out */
#define UVRPC_REFUSED               81016  /* connection refused, unirpcd not running */
#define UVRPC_SOCKET_INIT_FAILED    81017  /* Failed to initialise the socket */
#define UVRPC_SERVICE_PAUSED        81018  /* The unirpcd service on the server has been paused */
#define UVRPC_BAD_TRANSPORT         81019  /* Invalid transport type */
#define UVRPC_BAD_PIPE              81020  /* Bad pipe */
#define UVRPC_PIPE_WRITE_ERROR      81021  /* Error writing to pipe */
#define UVRPC_PIPE_READ_ERROR       81022  /* Error reading from pipe */
#define UVRPC_CONNECTION             81023
#define UVRPC_NO_MULTIPLEX_SUPPORT   81024
#define UVRPC_NO_ENCRYPTION_SUPPORT  81025
#define UVRPC_NO_COMPRESSION_SUPPORT 81026
#define UVRPC_BAD_ENCRYPTION         81027
#define UVRPC_BAD_COMPRESSION        81028
#define UVRPC_BAD_SSL_FLAG           81029 /* Bad SSL mode flag */
#define UVRPC_SSL_NOT_SUPPORTED      81030 /* SSL mode not supported */
#define UVRPC_BAD_SSL_PROPERTY       81031 /* Bad SSL property list name/password */
#define UVRPC_BAD_SSL_HANDSHAKE      81032 /* SSL handshake failed */
#define UVRPC_BAD_SSL_IO             81033 /* Error SSL read/write operation */

/* Function declarations */

#ifdef FAR
#undef	FAR
#define FAR
#else
#define	FAR
#endif

#ifdef PASCAL
#undef PASCAL
#define PASCAL
#else
#define PASCAL
#endif

#ifndef LPSTR
#define LPSTR char *
#endif

#ifndef LPLONG
#define LPLONG long *
#endif

#if defined(__STDC__) || defined(WIN32) || defined(MSWIN)
#define ic_proto(x) x
#else
#define ic_proto(x) ()
#endif

void ic_alpha ic_proto((LPSTR, LPLONG, LPLONG));
void * ic_calloc ic_proto((LPLONG));
void ic_cancel ic_proto((LPLONG));
void ic_cleardata ic_proto((LPLONG));
void ic_clearfile ic_proto((LPLONG, LPLONG));
void ic_clearselect ic_proto((LPLONG, LPLONG));
void ic_close ic_proto((LPLONG, LPLONG));
void ic_closeseq ic_proto((LPLONG, LPLONG));
void ic_col1 ic_proto((LPLONG, LPLONG));
void ic_col2 ic_proto((LPLONG, LPLONG));
void ic_convert ic_proto((LPSTR, LPLONG, LPSTR, LPLONG, LPSTR, LPLONG, LPLONG));
void ic_count ic_proto((LPSTR, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));
void ic_data ic_proto((LPSTR, LPLONG, LPLONG));
void ic_date ic_proto((LPLONG, LPLONG));
void ic_dcount ic_proto((LPSTR, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));
void ic_delete ic_proto((LPLONG, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));
void ic_execute ic_proto((LPSTR, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG, LPLONG, LPLONG));
void ic_executecontinue ic_proto((LPSTR, LPLONG, LPLONG, LPLONG, LPLONG, LPLONG));
void ic_extract ic_proto((LPSTR, LPLONG, LPLONG, LPLONG, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));
void ic_field ic_proto((LPSTR, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));
void ic_fieldstore ic_proto((LPSTR, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG, LPSTR, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));
void ic_fileinfo ic_proto((LPLONG, LPLONG, LPLONG, LPSTR, LPLONG, LPLONG));
void ic_filelock ic_proto((LPLONG, LPLONG, LPLONG));
void ic_fileunlock ic_proto((LPLONG, LPLONG, LPLONG));
void ic_fmt ic_proto((LPSTR, LPLONG, LPSTR, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));
void ic_formlist ic_proto((LPSTR, LPLONG, LPLONG, LPLONG));
void ic_free ic_proto((void *));
void ic_getlist ic_proto((LPSTR, LPLONG, LPLONG, LPLONG));
void ic_get_locale ic_proto((LPLONG,LPSTR,LPLONG,LPLONG,LPLONG));
void ic_get_map ic_proto((LPSTR,LPLONG,LPLONG,LPLONG));
void ic_get_mark_value ic_proto((LPLONG,LPSTR,LPLONG,LPLONG,LPLONG));
void ic_getvalue ic_proto((LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));
void ic_iconv ic_proto((LPSTR, LPLONG, LPSTR, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));
void ic_indices ic_proto((LPLONG, LPSTR, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));
void ic_inputreply ic_proto((LPSTR, LPLONG, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG, LPLONG, LPLONG));
void ic_insert ic_proto((LPSTR, LPLONG, LPLONG, LPLONG, LPLONG, LPLONG, LPSTR, LPLONG, LPLONG));
void ic_itype ic_proto((LPSTR, LPLONG, LPSTR, LPLONG, LPSTR, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));
void ic_locate ic_proto((LPSTR, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG, LPLONG)); 
void ic_lock ic_proto((LPLONG, LPLONG));
void ic_lower ic_proto((LPSTR, LPLONG, LPLONG));
void * ic_malloc ic_proto((LPLONG));
void ic_matbuild ic_proto((LPSTR, LPLONG, LPSTR, LPLONG, LPLONG, LPSTR, LPLONG, LPLONG));
void ic_matparse ic_proto((LPSTR, LPLONG, LPLONG, LPSTR, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));
void ic_oconv ic_proto((LPSTR, LPLONG, LPSTR, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));
void ic_open ic_proto((LPLONG, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));
void ic_openseq ic_proto((LPLONG, LPSTR, LPLONG, LPSTR,  LPLONG, LPLONG, LPLONG));
long ic_opensession ic_proto((LPSTR, LPSTR, LPSTR, LPSTR, LPLONG, LPSTR));
long ic_universe_session ic_proto((LPSTR, LPSTR, LPSTR, LPSTR, LPLONG, LPSTR));
long ic_unidata_session ic_proto((LPSTR, LPSTR, LPSTR, LPSTR, LPLONG, LPSTR, LPSTR));
void ic_quit ic_proto((LPLONG));
void ic_quitall ic_proto((LPLONG));
void ic_raise ic_proto((LPSTR, LPLONG, LPLONG));
void ic_read ic_proto((LPLONG, LPLONG, LPSTR, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG, LPLONG));
void ic_readblk ic_proto((LPLONG, LPSTR, LPLONG, LPLONG, LPLONG, LPLONG));
void ic_readcontinue ic_proto((LPSTR, LPLONG, LPLONG, LPLONG));
void ic_readlist ic_proto((LPLONG, LPSTR, LPLONG, LPLONG, LPLONG, LPLONG));
void ic_readnext ic_proto((LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));
void ic_readseq ic_proto((LPLONG, LPSTR, LPLONG, LPLONG, LPLONG, LPLONG));
void ic_readv ic_proto((LPLONG, LPLONG, LPSTR, LPLONG, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG, LPLONG));
void ic_recordlock ic_proto((LPLONG, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));
void ic_recordlocked ic_proto((LPLONG, LPSTR, LPLONG, LPLONG, LPLONG, LPLONG));
void ic_release ic_proto((LPLONG, LPSTR, LPLONG, LPLONG));
void ic_remove ic_proto((LPSTR, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG, LPLONG, LPLONG));
void ic_replace ic_proto((LPSTR, LPLONG, LPLONG, LPLONG, LPLONG, LPLONG, LPSTR, LPLONG, LPLONG));
void ic_seek ic_proto((LPLONG, LPLONG, LPLONG, LPLONG));
void ic_select ic_proto((LPLONG, LPLONG, LPLONG));
void ic_selectindex ic_proto((LPLONG, LPLONG, LPSTR, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));
void ic_session_info ic_proto((LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));
void ic_set_comms_timeout ic_proto((LPLONG, LPLONG));
void ic_set_locale ic_proto((LPLONG,LPSTR,LPLONG,LPLONG,LPLONG));
void ic_set_map ic_proto((LPSTR,LPLONG,LPLONG,LPLONG));
void ic_setsession ic_proto((LPLONG, LPLONG));
void ic_setvalue ic_proto((LPLONG, LPSTR, LPLONG, LPLONG));
void ic_status ic_proto((LPLONG, LPLONG));
void ic_strdel ic_proto((LPSTR, LPLONG, LPLONG, LPLONG, LPLONG, LPLONG));
/*
#if defined(__STDC__) || defined(MSWIN) || defined(WIN32)
*/
void ic_subcall ic_proto((LPSTR, LPLONG, LPLONG, LPLONG, ...));
/*
#else
void ic_subcall();
#endif
*/
void ic_systemreturncode ic_proto((LPLONG, LPLONG));
void ic_time ic_proto((LPLONG, LPLONG));
void ic_timedate ic_proto((LPSTR, LPLONG, LPLONG, LPLONG));
void ic_timeout ic_proto((LPLONG, LPLONG, LPLONG));
void ic_trans ic_proto((LPSTR, LPLONG, LPLONG, LPSTR, LPLONG, LPLONG, LPSTR, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG, LPLONG));
void ic_transaction ic_proto((LPLONG, LPLONG, LPLONG));
void ic_trim ic_proto((LPSTR, LPLONG, LPSTR, LPSTR, LPLONG));
void ic_unlock ic_proto((LPLONG, LPLONG));
void ic_weofseq ic_proto((LPLONG, LPLONG));
void ic_write ic_proto((LPLONG, LPLONG, LPSTR, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));
void ic_writeblk ic_proto((LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));
void ic_writecontinue ic_proto((LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));
void ic_writeseq ic_proto((LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));
void ic_writestart ic_proto((LPLONG, LPLONG, LPSTR, LPLONG, LPLONG));
void ic_writev ic_proto((LPLONG, LPLONG, LPSTR, LPLONG, LPLONG, LPSTR, LPLONG, LPLONG, LPLONG));

/* UCC-3009
void ic_set_ipv(char *);
void * ic_get_ipv();
void * ic_get_sessionipv(int);
*/
void ic_set_ipv ic_proto((LPSTR));
void * ic_get_ipv ();
void * ic_get_sessionipv ic_proto((LPLONG));

#undef ic_proto

#ifdef __cplusplus
}
#endif

/*  END-CODE */


