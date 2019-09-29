/*
 * This implementation is influenced by RFC7143
 */
#ifndef __ISCSI_H__
#define __ISCSI_H__

#include <string.h>

#include "iscsi_target.h"
#include "iscsi_buffer.h"
#include "iscsi_pdu.h"

/*
#define MaxMissingDPDU 10
#define MaxOutstandingR2T 3

// TransferContext

struct TansferContext {
  int TargetTransferTag;
  int ExpectedDataSN;
};

// TCB

struct TCB {
  int SoFarInOrder;
  int ExpectedDataSN;
  int MissingDataSNList[MaxMissingDPDU];
  int FbitReceived;
  int StatusXferd;
  int CurrentlyAllegiant;
  int ActiveR2Ts;
  int Response;
  char *Reason;
  struct TransferContext TransferContextList[MaxOutstandingR2T];
  int InitiatorTaskTag;
  int CmdSN;
  int SNACK_Tag;
};


// Everything else

// typedef byte* PDU;
// typedef void *(void) handler;

// void receive_pdu(struct Connection* con, PDU inbound);
// void check_basic_validity(PDU inbound);
// void start_timer(handler timeout_handler, int timeout_ms);
*/


#endif
