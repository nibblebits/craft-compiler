
/* 
 * File:   MagicOMF.h
 * Author: dansb
 *
 * Created on 06 December 2016, 11:34
 */

#ifndef MAGICOMF_H
#define MAGICOMF_H

#include "mdef.h"
#include "types.h"
#include "record.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif
    
struct MagicOMFHandle* MagicOMFTranslate(char* buf, uint32 size, bool skip_unimplemented_records);
struct MagicOMFHandle* MagicOMFCreateHandle();
void MagicOMFSetupBuffer(struct MagicOMFHandle* handle, uint32 size, char* buf);
void MagicOMFAddRecord(struct RECORD* record);
void MagicOMFAddTHEADR(struct MagicOMFHandle* handle, const char* name);
void MagicOMFAddCOMENT(struct MagicOMFHandle* handle, COMMENT_TYPE type, uint8 _class, const char* str);
struct RECORD* MagicOMFNewLNAMESRecord(struct MagicOMFHandle* handle);
void MagicOMFAddLNAME(struct RECORD* record, const char* name);
void MagicOMFFinishLNAMES(struct RECORD* record);
struct RECORD* MagicOMFNewEXTDEFRecord(struct MagicOMFHandle* handle);
void MagicOMFAddEXTDEF(struct RECORD* record, const char* name, int type_index);
void MagicOMFFinishEXTDEF(struct RECORD* record);
void MagicOMFAddSEGDEF16(struct MagicOMFHandle* handle, const char* name, struct Attributes attributes, uint16 size);
void MagicOMFAddLEDATA16(struct MagicOMFHandle* handle, const char* seg_name, uint16 data_offset, int data_size, char* data);
struct RECORD* MagicOMFNewFIXUP16Record(struct MagicOMFHandle* handle);
void MagicOMFAddFIXUP16_SubRecord_Segment_Fixup(struct RECORD* record, const char* referring_to_segment_name, uint16 offset, LOCATION_TYPE location_type, FIXUP_MODE fixup_mode);
void MagicOMFAddFIXUP16_SubRecord_External_Fixup(struct RECORD* record, const char* extern_ref_name, uint16 offset, LOCATION_TYPE location_type, FIXUP_MODE fixup_mode);
void MagicOMFFinishFIXUP16(struct RECORD* record);
struct RECORD* MagicOMFNewPUBDEF16Record(struct MagicOMFHandle* handle, const char* seg_name);
void MagicOMFAddPUBDEF16Identifier(struct RECORD* record, const char* pub_def_name, uint16 offset, uint8 type_index);
void MagicOMFFinishPUBDEF16(struct RECORD* record);
void MagicOMFAddMODEND16(struct MagicOMFHandle* handle);
int MagicOMFCalculateBufferSize(struct MagicOMFHandle* handle);
void MagicOMFGenerateBuffer(struct MagicOMFHandle* handle);
void MagicOMFCloseHandle(struct MagicOMFHandle* handle);
char* MagicOMFGetLNAMESNameByIndex(struct MagicOMFHandle* handle, uint8 index);
struct SEGDEF_16* MagicOMFGetSEGDEF16ByIndex(struct MagicOMFHandle* handle, uint8 index);
struct LEDATA_16* MagicOMFGetLEDATABySegmentIndex(struct MagicOMFHandle* handle, uint8 index);
struct EXTDEF* MagicOMFGetEXTDEFByIndex(struct MagicOMFHandle* handle, uint8 index);
int MagicOMFGetLNAMESIndex(struct MagicOMFHandle* handle, const char* name);
int MagicOMFGetSEGDEFIndex(struct MagicOMFHandle* handle, const char* name);
int MagicOMFGetEXTDEFIndex(struct MagicOMFHandle* handle, const char* name);
const char* MagicOMFErrorMessage(MAGIC_OMF_ERROR_CODE error_id);


#ifdef __cplusplus
}
#endif

#endif /* MAGICOMF_H */

