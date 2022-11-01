#ifndef CONSTS
#define CONSTS

/**************************************************************************** 
 *
 * This header file contains utility constants & macro definitions.
 * 
 ****************************************************************************/

/* Hardware & software constants */
#define PAGESIZE		  4096			/* page size in bytes	*/
#define WORDLEN			  4				  /* word size in bytes	*/
#define MAXPROC			  20			/* pandos can only support this pool of concurrent*/
							/* processes */
#define INF			  0x7FFFFFFF		/* infinity */
#define BIGNUMBER		  999999		/* big value for loading timer wwith */


/* timer, timescale, TOD-LO and other bus regs */
#define RAMBASEADDR		0x10000000
#define RAMBASESIZE		0x10000004
#define RAMTOP			0x2000.1000
#define TODLOADDR		  0x1000001C
#define INTERVALTMR		0x10000020	
#define TIMESCALEADDR	0x10000024


/* utility constants */
#define	TRUE			    1
#define	FALSE			    0
#define HIDDEN			  static
#define EOS				    '\0'
#define NULL 			    ((void *)0xFFFFFFFF)

/* masks */
#define IPMASK            8

/* device interrupts */
#define DISKINT			  3
#define FLASHINT 		  4
#define NETWINT 		  5
#define PRNTINT 		  6
#define TERMINT			  7

#define DEVINTNUM		  5		  /* interrupt lines used by devices */
#define DEVPERINT		  8		  /* devices per interrupt line */
#define DEVREGLEN		  4		  /* device register field length in bytes, and regs per dev */	
#define DEVREGSIZE	  16 		/* device register size in bytes */
#define INSDEVBITMAP		0x1000.002C	/*installed devices bitmap (5words) */
#define IntDEVBITMAP		0x1000.0040	/*interrupt devices bitmap (5words) */
#define LOWMEM			0x1000.0054

/* device register field number for non-terminal devices */
#define STATUS			  0
#define COMMAND			  1
#define DATA0			    2
#define DATA1			    3

/* device register field number for terminal devices */
#define RECVSTATUS  	0
#define RECVCOMMAND 	1
#define TRANSTATUS  	2
#define TRANCOMMAND 	3

/* device common STATUS codes */
#define UNINSTALLED		0
#define READY			    1
#define BUSY			    3

/* device common COMMAND codes */
#define RESET			    0
#define ACK				    1

/* Memory related constants */
#define KSEG0           0x00000000
#define KSEG1           0x20000000
#define KSEG2           0x40000000
#define KUSEG           0x80000000
#define RAMSTART        0x20000000
#define BIOSDATAPAGE    0x0FFFF000
#define	PASSUPVECTOR	  0x0FFFF900
/*adding*/
#define Cause		0x0000007C
#define CAUSESHIFT	0x00000002
#define QUANTUM		.005
/* #define INTERVALTMR	.1       This is redifined again here for some reason... */
#define IPSHIFT		8

/*bit stuff*/
#define ALLOFF		0x00000000
#define IEPBITON	0x00000004
#define IMON      	0x0000FF00
#define TEBITON   	0x08000000
#define IECON	  	0x00000001
#define KUPON	  	0x00000008

#define LINEONEON	0x00000080
#define LINETWOON	0x00000100
#define LINETHREEON	0x00000200
#define LINEFOURON	0x00000400
#define LINEFIVEON	0x00000800
#define LINESIXON	0x00001000
#define LINESEVENON	0x00002000

/*ours added*/
/*#define 		0x2000.1000		top of nucleus stack page*/

/* Exceptions related constants */
#define	PGFAULTEXCEPT	  0
#define GENERALEXCEPT	  1


/* operations */
#define	MIN(A,B)		((A) < (B) ? A : B)
#define MAX(A,B)		((A) < (B) ? B : A)
#define	ALIGNED(A)		(((unsigned)A & 0x3) == 0)

/* Macro to load the Interval Timer */
#define LDIT(T)	((* ((cpu_t *) INTERVALTMR)) = (T) * (* ((cpu_t *) TIMESCALEADDR))) 

/* Macro to read the TOD clock */
#define STCK(T) ((T) = ((* ((cpu_t *) TODLOADDR)) / (* ((cpu_t *) TIMESCALEADDR))))

/* Macro to get device */

#endif
