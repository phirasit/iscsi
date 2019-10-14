#ifndef __ISCSI_PDU_H__
#define __ISCSI_PDU_H__

#include "iscsi_byte.h"
#include "iscsi_connection.h"
#include "iscsi_type.h"

#define BASIC_HEADER_SEGMENT_LENGTH 48
#define DEFAULT_TRANSFER_TAG 0xFFFFFFFF

enum OPCODE {
  NOP_OUT  = 0x00,
  SCSI_CMD = 0x01,
  SCSI_TASK_MANAGE_REQ = 0x02,
  LOGIN    = 0x03,
  TEXT     = 0x04,
  SCSI_DATA_OUT = 0x05,
  LOGOUT   = 0x06,
  SNACK    = 0x07,

  NOP_IN    = 0x20,
  SCSI_RES  = 0x21,
  SCSI_TASK_MANAGE_RES = 0x22,
  LOGIN_RES = 0x23,
  TEXT_RES  = 0x24,
  SCSI_DATA_IN = 0x25,
  LOGOUT_RES = 0x26,

  R2T = 0x31,
  ASYNC_MSG = 0x32,

  REJECT = 0x3f
};

// PDU information getter

static int iscsi_pdu_immediate(byte* buffer) {
  return iscsi_byte_bit(buffer[0], 7);
}

static enum OPCODE iscsi_pdu_opcode(byte* buffer) {
  return buffer[0] & 0x3F;
}

static inline int iscsi_pdu_padded_length(int length) {
  return (length + 0x03) & ~0x03;
}

static inline int iscsi_pdu_calc_size(int total_ahs_length, int data_segment_length) {
  int length = BASIC_HEADER_SEGMENT_LENGTH + total_ahs_length + data_segment_length;
  return iscsi_pdu_padded_length(length);
}

static inline int iscsi_pdu_ahs_length(byte* buffer) {
  return buffer[4];
}

static inline int iscsi_pdu_data_segment_length(byte* buffer) {
  return (int) buffer[5] << 16 | (int) buffer[6] << 8 | buffer[7];
}

static int iscsi_pdu_length(byte* buffer) {
  return iscsi_pdu_calc_size(
    iscsi_pdu_ahs_length(buffer),
    iscsi_pdu_data_segment_length(buffer)
  );
}

static inline int iscsi_pdu_cmdSN(byte* buffer) {
  return iscsi_byte_byte2int(buffer + 24);
}

static inline byte* iscsi_pdu_data(byte* buffer) {
  return buffer + BASIC_HEADER_SEGMENT_LENGTH;
}

static inline int iscsi_pdu_initiator_task_tag(byte* buffer) {
  return iscsi_byte_byte2int(buffer + 16);
}
static inline int iscsi_pdu_target_transfer_tag(byte* buffer) {
  return iscsi_byte_byte2int(buffer + 20);
}

// PDU information setter

static inline void iscsi_pdu_set_bit(byte* buffer, int bit, int val) {
  buffer[0] = (buffer[0] & ~(1 << bit)) | (val << bit);
}

static inline void iscsi_pdu_set_immediate(byte* buffer, int i) {
  buffer[0] = ((i & 0x01) << 6) | (buffer[0] & 0xBF);
}

static inline void iscsi_pdu_set_opcode(byte* buffer, int opcode) {
  buffer[0] = (opcode & 0x3F) | (buffer[0] & 0xC0);
}

static inline void iscsi_pdu_set_final(byte* buffer, int f) {
  buffer[1] = ((f & 0x01) << 7) | (buffer[1] & 0x7F); 
}

static inline void iscsi_pdu_set_ahs_length(byte* buffer, int ahs_length) {
  buffer[4] = ahs_length & 0xFF;
}

static inline void iscsi_pdu_set_data_segment_length(byte* buffer, int segment_length) {
  buffer[5] = (segment_length >> 16) & 0xFF;
  buffer[6] = (segment_length >>  8) & 0xFF;
  buffer[7] = (segment_length      ) & 0xFF;
}

static inline void iscsi_pdu_set_initiator_task_tag(byte* buffer, int initiator_task_tag) {
  iscsi_byte_int2byte(buffer + 16, initiator_task_tag);
}

static inline void iscsi_pdu_set_target_transfer_tag(byte* buffer, int target_transfer_tag) {
  iscsi_byte_int2byte(buffer + 20, target_transfer_tag);
}

// helper functions

static inline int iscsi_pdu_has_cmdSN(byte* buffer) {
  int opcode = iscsi_pdu_opcode(buffer);
  return opcode == NOP_OUT
    || opcode == SCSI_CMD
    || opcode == LOGIN
    || opcode == TEXT
    || opcode == LOGOUT;
}

static inline void iscsi_pdu_generate_from_buffer(byte* response, byte* request) {
  memcpy(response, request, BASIC_HEADER_SEGMENT_LENGTH);
}

// function prototype

int iscsi_pdu_valid(byte* pdu, int length);
void iscsi_pdu_pad0(byte* pdu, int length);

#endif // __ISCSI_PDU_H__
