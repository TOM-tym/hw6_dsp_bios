/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */
/*
 *  ======== mailbox.c ========
 *  Use a MBX mailbox to send messages from multiple writer() 
 *  tasks to a single reader() task.
 *  The mailbox, reader task, and three writer tasks are created by the
 *  Configuration Tool.
 *
 *  This example is similar to semtest.c.  The major differences 
 *    are:
 *    - MBX is used in place of QUE and SEM.
 *    - the 'elem' field is removed from MsgObj.
 *    - reader() task is *not* higher priority than writer task.
 *    - reader() looks at return value from MBX_pend() for timeout
 */
#include <stdlib.h>

#include <std.h>

#include <log.h>
#include <mbx.h>
#include <tsk.h>
#include <time.h>


#include "mailboxcfg.h"


#define NUMMSGS         5000       /* number of messages */

#define TIMEOUT         20

typedef struct MsgObj {
    Int         id;             /* writer task id */
    Char        val;            /* message value */
} MsgObj, *Msg;
typedef struct MsgObj2 {
    Int         id;             /* writer task id */
    int        val;            /* message value */
} MsgObj2, *Msg2;

Void reader(Void);
Void reader_counter(Void);
Void writer(Arg id_arg);
Void writer_timer(Arg id_arg);
Void timer_counter_swi();
/*
 *  ======== main ========
 */
Void main()
{
    /* Does nothing */
}

/*
 *  ======== reader ========
 */
Void reader(Void)
{
    MsgObj      msg;
    Int         i;

    for (i=0; ;i++) {

        /* wait for mailbox to be posted by writer() */
        if (MBX_pend(&mbx, &msg, TIMEOUT) == 0) {
            LOG_printf(&trace, "timeout expired for MBX_pend()");
            break;
        }

        /* print value */
        LOG_printf(&trace, "read '%c' from (%d).", msg.val, msg.id);
        TSK_yield();
    }
    LOG_printf(&trace, "reader done.");
}

/*
 *  ======== writer ========
 */
Void writer(Arg id_arg)
{
    MsgObj      msg;
    Int         i;
    Int id =    ArgToInt (id_arg);
    Int a = 0;
    Int b = 0;
    Int a_or_b = 0;
    for (i=0; i < NUMMSGS; i++) {
        /* fill in value */
        msg.id = id;
        a = rand() % 26 + 65;
        b = rand() % 26 + 97;
        a_or_b = rand() % 2;
        msg.val = a_or_b==0?a:b;
        /* enqueue message */
        MBX_post(&mbx, &msg, TIMEOUT);

        /* what happens if you call TSK_yield() here? */
        TSK_yield();
    }

    LOG_printf(&trace, "writer (%d) done.", id);
}

Void writer_timer(Arg id_arg)
{
		MsgObj2      msg;
	    Int         i;
	    Int id =    ArgToInt (id_arg);
	    msg.id = id;
	    msg.val = 0;
	    LOG_printf(&trace, " [int_writer](%d) waiting. ", id);
	    int t = time(0);
	    for (;;) {
	    	int t2 = time(0);
	    	if (t2-t>0){
	    		msg.val += (t2-t);
	    		t = time(0);
	    		MBX_post(&MBX0, &msg, TIMEOUT);
	    	}
	    	TSK_yield();
	    	}

	    }

	    //LOG_printf(&trace, "writer (%d) done.", id);

Void reader_counter(Void)
{
    MsgObj2      msg;
    Int         i;

    for (i=0; ;i++) {

        /* wait for mailbox to be posted by writer() */
        if (MBX_pend(&MBX0, &msg, TIMEOUT) == 0) {
            LOG_printf(&trace, "timeout expired for MBX_pend()");
            break;
        }

        /* print value */
        LOG_printf(&trace, "[counter] '%d' from (%d).", msg.val, msg.id);
        TSK_yield();
    }
    LOG_printf(&trace, "reader done.");    
}
