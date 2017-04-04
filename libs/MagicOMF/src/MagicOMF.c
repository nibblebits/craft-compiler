#include <stdlib.h>
#include <string.h>
#include "MagicOMF.h"
#include "translate.h"
#include "generator.h"
#include "IO.h"
#include "error.h"
#include "builder.h"

struct MagicOMFHandle* MagicOMFTranslate(char* buf, uint32 size, bool skip_unimplemented_records)
{
    struct MagicOMFHandle* handle = MagicOMFCreateHandle();
    MagicOMFSetupBuffer(handle, size, buf);
    handle->skip_unimplemented_records = skip_unimplemented_records;

    char* end = buf + size;
    
    /* OMF Files always expect either a THEADR or an LHEADR to begin with. 
     so we need to peak ahead and check its one of these*/
    uint8 type = ReadUnsignedByteNoNext(buf);
    if (type == THEADR_ID)
    {
        TranslatorReadTHEADR(handle);
    }
    else if (type == LHEADR_ID)
    {
        TranslatorReadLHEADR(handle);
    }
    else
    {
        error(EXPECTING_THEADR_OR_LHEADR, handle);
    }


    while (handle->next < end
            && !handle->has_error)
    {
        type = ReadUnsignedByteNoNext(handle->next);
        handle->record_type = type;
        switch (type)
        {
        case THEADR_ID:
            TranslatorReadTHEADR(handle);
            break;
        case LHEADR_ID:
            TranslatorReadLHEADR(handle);
            break;
        case COMENT_ID:
            TranslatorReadCOMENT(handle);
            break;
        case LNAMES_ID:
            TranslatorReadLNAMES(handle);
            break;
        case SEGDEF_16_ID:
            TranslatorReadSEGDEF16(handle);
            break;
        case PUBDEF_16_ID:
            TranslatorReadPUBDEF16(handle);
            break;        
        case LEDATA_16_ID:
            TranslatorReadLEDATA16(handle);
            break;    
        case FIXUPP_16_ID:
            TranslatorReadFIXUPP16(handle);
            break;
        case MODEND_16_ID:
            TranslatorReadMODEND16(handle);
            break;
        case EXTDEF_ID:
            TranslatorReadEXTDEF(handle);
            break;
      
        default:
            if (handle->skip_unimplemented_records)
            {
                TranslatorSkipRecord(handle);
            }
            else
            {
                error(INVALID_RECORD_TYPE, handle);
            }
            break;
        }
    }


    // Finalize some things.
    TranslatorFinalize(handle);

    return handle;
}

struct MagicOMFHandle* MagicOMFCreateHandle()
{
    // Setup a basic handle
    struct MagicOMFHandle* handle = (struct MagicOMFHandle*) malloc(sizeof (struct MagicOMFHandle));
    handle->buf = NULL;
    handle->next = NULL;
    handle->root = NULL;
    handle->last = NULL;
    handle->skip_unimplemented_records = false;
    handle->has_error = false;
    handle->last_error_code = -1;
    handle->last_ledata = NULL;

    return handle;
}

void MagicOMFSetupBuffer(struct MagicOMFHandle* handle, uint32 size, char* buf)
{
    handle->buf_size = size;
    handle->buf = buf;
    handle->next = buf;
}

void MagicOMFAddRecord(struct RECORD* record)
{
    struct MagicOMFHandle* handle = record->handle;
    // No root set so lets set it
    if (handle->root == NULL)
    {
        handle->root = record;
    }
    else
    {
        // Ok we already have a root so we know we have a last, lets append the last record's next pointer to point to us.
        handle->last->next = record;
    }
    handle->last = record;
}

void MagicOMFAddTHEADR(struct MagicOMFHandle* handle, const char* name)
{
    struct RECORD* record;
    struct THEADR* theadr = BuildTHEADR((char*) name);
    uint16 record_len = theadr->string_length + 2; // +2 for string length and for checksum
    record = BuildRecord(handle, THEADR_ID, record_len, 0);
    record->contents = theadr;
    MagicOMFAddRecord(record);
}

void MagicOMFAddCOMENT(struct MagicOMFHandle* handle, COMMENT_TYPE type, uint8 _class, const char* str)
{
    struct RECORD* record;
    struct COMENT* coment = BuildCOMENT(type, _class, (char*) str);
    uint16 record_len = strlen(str) + 3; // +3 for comment type, comment class and checksum
    record = BuildRecord(handle, COMENT_ID, record_len, 0);
    record->contents = coment;
    MagicOMFAddRecord(record);
}

struct RECORD* MagicOMFNewLNAMESRecord(struct MagicOMFHandle* handle)
{
    // Impossible to know the record length at this point
    struct RECORD* record = BuildRecord(handle, LNAMES_ID, 0, 0);
    record->contents = NULL;
    return record;
}

void MagicOMFAddLNAME(struct RECORD* record, const char* name)
{
    // Ok lets check that this is an LNAMES record
    if (!record->type == LNAMES_ID)
    {
        error(INVALID_LNAMES_PROVIDED, record->handle);
        return;
    }

    struct LNAMES* lnames = (struct LNAMES*) record->contents;
    if (lnames == NULL)
    {
        // This is the first LNAMES we have
        lnames = BuildLNAMES((char*) name);
        record->contents = lnames;
    }
    else
    {
        // Ok this is not the first LNAMES for this LNAME record, lets find out where we should add this new LNAME
        struct LNAMES* current = lnames;
        while (1)
        {
            if (current->next != NULL)
            {
                current = current->next;
            }
            else
            {
                break;
            }
        }

        // variable "current" contains the LNAMES we need to add the new LNAME to
        current->next = BuildLNAMES((char*) name);
    }
}

void MagicOMFFinishLNAMES(struct RECORD* record)
{
    // Calculate the size of this LNAMES record as we now need to set it
    int size = 1; // 1 for Checksum
    struct LNAMES* current = (struct LNAMES*) record->contents;
    while (current != NULL)
    {
        size += (current->s_len + 1); // +1 for the string length byte
        current = current->next;
    }
    record->length = size;

    // Finally lets add the record
    MagicOMFAddRecord(record);
}

struct RECORD* MagicOMFNewEXTDEFRecord(struct MagicOMFHandle* handle)
{
    // Impossible to know the record length at this point
    struct RECORD* record = BuildRecord(handle, EXTDEF_ID, 0, 0);
    record->contents = NULL;
    return record;
}

void MagicOMFAddEXTDEF(struct RECORD* record, const char* name, int type_index)
{
    // Ok lets check that this is an EXTDEF record
    if (!record->type == EXTDEF_ID)
    {
        error(INVALID_EXTDEF_PROVIDED, record->handle);
        return;
    }

    struct EXTDEF* extdef = (struct EXTDEF*) record->contents;
    if (extdef == NULL)
    {
        // This is the first EXTDEF we have
        extdef = BuildEXTDEF((char*) name, type_index);
        record->contents = extdef;
    }
    else
    {
        // Ok this is not the first EXTDEF record contents so lets find where we need to put the new record contents
        struct EXTDEF* current = extdef;
        while (1)
        {
            if (current->next != NULL)
            {
                current = current->next;
            }
            else
            {
                break;
            }
        }

        // variable "current" contains the EXTDEF we need to add the new EXTDEF to
        current->next = BuildEXTDEF((char*) name, type_index);
    }
}

void MagicOMFFinishEXTDEF(struct RECORD* record)
{
    // Calculate the size of this EXTDEF record as we now need to set it
    int size = 1; // 1 for Checksum
    struct EXTDEF* current = (struct EXTDEF*) record->contents;
    while (current != NULL)
    {
        size += (current->s_len + 2); // +2 for the string length byte + the type index
        current = current->next;
    }
    record->length = size;

    // Finally lets add the record
    MagicOMFAddRecord(record);
}

void MagicOMFAddSEGDEF16(struct MagicOMFHandle* handle, const char* name, struct Attributes attributes, uint16 size)
{
    struct RECORD* record;
    struct SEGDEF_16* segdef_16 = BuildSEGDEF16(handle, name, attributes, size);
    int record_len = 7;
    if (attributes.A == SEG_ATTR_ALIGNMENT_ABS_SEG)
    {
        // A == absolute segment so the frame number and offset are present
        record_len += 3;
    }
    record = BuildRecord(handle, SEGDEF_16_ID, record_len, 0);
    record->contents = segdef_16;
    MagicOMFAddRecord(record);
}

void MagicOMFAddLEDATA16(struct MagicOMFHandle* handle, const char* seg_name, uint16 data_offset, int data_size, char* data)
{
    struct RECORD* record;
    struct LEDATA_16* ledata_16 = BuildLEDATA16(handle, seg_name, data_offset, data_size, data);
    uint16 record_len = 4 + data_size; // +4 for segment index, enumerated data offset and checksum
    record = BuildRecord(handle, LEDATA_16_ID, record_len, 0);
    record->contents = ledata_16;
    MagicOMFAddRecord(record);
}

struct RECORD* MagicOMFNewFIXUP16Record(struct MagicOMFHandle* handle)
{
    // Impossible to know the record length at this point
    struct RECORD* record = BuildRecord(handle, FIXUPP_16_ID, 0, 0);
    record->contents = NULL;
    return record;
}

void MagicOMFAddFIXUP16_SubRecord_Fixup(struct RECORD* record, struct FIXUPP_16_FIXUP_SUBRECORD* subrecord)
{
    if (record->type != FIXUPP_16_ID)
    {
        error(INVALID_FIXUPP_16_PROVIDED, record->handle);
        return;
    }


    // lets create our new sub record and descriptor
    struct FIXUP_16_SUBRECORD_DESCRIPTOR* new_descriptor = BuildFIXUP16_RecordDescriptor(FIXUPP_FIXUP_SUBRECORD, (const void*) subrecord);


    // We need to find out where we are going to put this new FIXUP 
    struct FIXUP_16_SUBRECORD_DESCRIPTOR* record_descriptor = (struct FIXUP_16_SUBRECORD_DESCRIPTOR*) record->contents;
    if (record_descriptor == NULL)
    {
        // We will put it as the records contents as its the first of its kind
        record->contents = new_descriptor;
    }
    else
    {

        // Ok its not the first so we need to find the record that was added last
        while (1)
        {
            if (record_descriptor->next_subrecord_descriptor != NULL)
            {
                record_descriptor = record_descriptor->next_subrecord_descriptor;
            }
            else
            {
                break;
            }
        }

        // Lets add a pointer pointing to us.
        record_descriptor->next_subrecord_descriptor = new_descriptor;
    }
}

void MagicOMFAddFIXUP16_SubRecord_Segment_Fixup(struct RECORD* record, const char* referring_to_segment_name, uint16 offset, LOCATION_TYPE location_type, FIXUP_MODE fixup_mode)
{
    if (record->type != FIXUPP_16_ID)
    {
        error(INVALID_FIXUPP_16_PROVIDED, record->handle);
        return;
    }


    // lets create our new sub record and descriptor
    struct FIXUPP_16_FIXUP_SUBRECORD* subrecord = BuildFIXUP16_SubRecord_Segment_Fixup(record->handle, referring_to_segment_name, offset, location_type, fixup_mode);
    MagicOMFAddFIXUP16_SubRecord_Fixup(record, subrecord);

}

void MagicOMFAddFIXUP16_SubRecord_External_Fixup(struct RECORD* record, const char* extern_ref_name, uint16 offset, LOCATION_TYPE location_type, FIXUP_MODE fixup_mode)
{
    if (record->type != FIXUPP_16_ID)
    {
        error(INVALID_FIXUPP_16_PROVIDED, record->handle);
        return;
    }

    // Lets create our new sub record and descriptor
    struct FIXUPP_16_FIXUP_SUBRECORD* subrecord = BuildFIXUP16_SubRecord_External_Fixup(record->handle, extern_ref_name, offset, location_type, fixup_mode);
    MagicOMFAddFIXUP16_SubRecord_Fixup(record, subrecord);
}

void MagicOMFFinishFIXUP16(struct RECORD* record)
{
    // We must calculate the record size
    uint16 record_size = 1; // 1 for checksum
    struct FIXUP_16_SUBRECORD_DESCRIPTOR* record_descriptor = record->contents;
    while (record_descriptor != NULL)
    {
        if (record_descriptor->subrecord_type == FIXUPP_FIXUP_SUBRECORD)
        {
            struct FIXUPP_16_FIXUP_SUBRECORD* fixup_sub = (struct FIXUPP_16_FIXUP_SUBRECORD*) (record_descriptor->subrecord);
            record_size += 3; // +2 for locat + 1 for fix data ( spec states fix data is conditional but not how )
            if (!(fixup_sub->fix_data & FIXUPP_FIXUP_SET_F))
            {
                // F is not set which means we have a frame datum
                record_size += 1;
            }

            if (!(fixup_sub->fix_data & FIXUPP_FIXUP_SET_P))
            {
                // P is not set which means we have a target displacement
                record_size += 3;
            }
        }
        record_descriptor = record_descriptor->next_subrecord_descriptor;
    }

    // Ok we have the total record size so lets set it and add the record
    record->length = record_size;
    MagicOMFAddRecord(record);
}

struct RECORD* MagicOMFNewPUBDEF16Record(struct MagicOMFHandle* handle, const char* seg_name)
{
    // Impossible to know the record length at this point
    struct RECORD* record = BuildRecord(handle, PUBDEF_16_ID, 0, 0);
    struct PUBDEF_16* contents = BuildPUBDEF16(handle, seg_name);
    record->contents = contents;
    return record;
}

void MagicOMFAddPUBDEF16Identifier(struct RECORD* record, const char* pub_def_name, uint16 offset, uint8 type_index)
{
    struct PUBDEF_16* pubdef_16 = (struct PUBDEF_16*) record->contents;
    struct PUBDEF_16_IDEN* new_iden = BuildPUBDEF16_IDEN(pub_def_name, offset, type_index);
    if(pubdef_16->iden == NULL)
    {
        pubdef_16->iden = new_iden;
    }
    else
    {
        // We need to find where to put this new identifier
        struct PUBDEF_16_IDEN* current = pubdef_16->iden;
        while(current->next != NULL)
        {
            current = current->next;
        }
        
        // Last pubdef identifier is found so set its next to the new identifier created
        current->next = new_iden;
    }
        
}

void MagicOMFFinishPUBDEF16(struct RECORD* record)
{
    struct PUBDEF_16* pubdef_16 = (struct PUBDEF_16*) record->contents;
    // base group index + base segment index + checksum = 3
    uint16 record_size = 3; 
    
    if (pubdef_16->bs_index == 0)
    {
        // Base segment is zero so base frame is present, 2 bytes for base frame
        record_size += 2;
    }
    
    struct PUBDEF_16_IDEN* iden = pubdef_16->iden;
    while(iden != NULL)
    {
        // String length field + string length + offset + type index = 4 + string length
        record_size += 4 + iden->str_len;
        iden = iden->next;
    }
    
    // We have the record size
    record->length = record_size;
    MagicOMFAddRecord(record);
    
}

void MagicOMFAddMODEND16(struct MagicOMFHandle* handle)
{
    // Only very basic support is currently legal, we do not support start addresses
    int record_length = 2;
    struct RECORD* record = BuildRecord(handle, MODEND_16_ID, record_length, 0);
    struct MODEND_16* modend_16 = BuildMODEND16(handle);
    record->contents = modend_16;
    MagicOMFAddRecord(record);
}

int MagicOMFCalculateBufferSize(struct MagicOMFHandle* handle)
{
    /* We need to calculate the buffer size for all given records. 
     * the algorithm for this is (record size field + 3) */

    int size = 0;
    struct RECORD* current = handle->root;
    while (current != NULL)
    {
        size += current->length + 3;
        current = current->next;
    }

    return size;
}

void MagicOMFGenerateBuffer(struct MagicOMFHandle* handle)
{
    if (handle->buf != NULL)
    {
        // Buffer is currently present so lets free the memory as we will soon do another malloc
        free(handle->buf);
    }

    uint32 buf_size = MagicOMFCalculateBufferSize(handle);
    // Allocate the new memory for the buffer    
    char* buf = (char*) malloc(buf_size);

    // Setup the new buffer
    MagicOMFSetupBuffer(handle, buf_size, buf);

    // Generate the records into the buffer.
    struct RECORD* current = handle->root;
    while (current != NULL)
    {
        switch (current->type)
        {
        case THEADR_ID:
            GeneratorWriteTHEADR(&handle->next, current);
            break;
        case COMENT_ID:
            GeneratorWriteCOMENT(&handle->next, current);
            break;
        case LNAMES_ID:
            GeneratorWriteLNAMES(&handle->next, current);
            break;
        case EXTDEF_ID:
            GeneratorWriteEXTDEF(&handle->next, current);
            break;
        case SEGDEF_16_ID:
            GeneratorWriteSEGDEF16(&handle->next, current);
            break;
        case LEDATA_16_ID:
            GeneratorWriteLEDATA16(&handle->next, current);
            break;
        case FIXUPP_16_ID:
            GeneratorWriteFIXUPP16(&handle->next, current);
            break;
        case MODEND_16_ID:
            GeneratorWriteMODEND16(&handle->next, current);
            break;
        case PUBDEF_16_ID:
            GeneratorWritePUBDEF16(&handle->next, current);
            break;
        default:
            error(INVALID_RECORD_TYPE, handle);
        }
        current = current->next;
    }

}

void MagicOMFCloseHandle(struct MagicOMFHandle* handle)
{
    // TO BE IMPLEMENTED
    struct RECORD* current = handle->root;
    while (current != NULL)
    {

        current = current->next;
    }
}

char* MagicOMFGetLNAMESNameByIndex(struct MagicOMFHandle* handle, uint8 index)
{
    struct RECORD* record = handle->root;
    int c_index = 1;
    while (record != NULL)
    {
        if (record->type == LNAMES_ID)
        {
            struct LNAMES* lnames_record = (struct LNAMES*) (record->contents);
            while (lnames_record != NULL)
            {
                if (c_index == index)
                {
                    return lnames_record->n_string;
                }
                c_index++;
                lnames_record = lnames_record->next;
            }
        }
        record = record->next;
    }

    return NULL;
}

struct SEGDEF_16* MagicOMFGetSEGDEF16ByIndex(struct MagicOMFHandle* handle, uint8 index)
{
    struct RECORD* record = handle->root;
    int c_index = 1;
    while (record != NULL)
    {
        if (record->type == SEGDEF_16_ID)
        {
            if (c_index == index)
            {
                return record->contents;
            }
            c_index++;
        }
        record = record->next;
    }

    return NULL;
}

struct LEDATA_16* MagicOMFGetLEDATABySegmentIndex(struct MagicOMFHandle* handle, uint8 index)
{
    struct RECORD* record = handle->root;
    while (record != NULL)
    {
        if (record->type == LEDATA_16_ID)
        {
            struct LEDATA_16* ledata_contents = (struct LEDATA_16*) (record->contents);
            if (ledata_contents->seg_index == index)
            {
                return ledata_contents;
            }
        }
        record = record->next;
    }

    return NULL;
}

struct EXTDEF* MagicOMFGetEXTDEFByIndex(struct MagicOMFHandle* handle, uint8 index)
{
    struct RECORD* record = handle->root;
    int c_index = 1;
    while (record != NULL)
    {
        if (record->type == EXTDEF_ID)
        {
            struct EXTDEF* extdef_record = (struct EXTDEF*) (record->contents);
            while (extdef_record != NULL)
            {
                if (c_index == index)
                {
                    return extdef_record;
                }
                c_index++;
                extdef_record = extdef_record->next;
            }
        }
        record = record->next;
    }

    return NULL;
}

int MagicOMFGetLNAMESIndex(struct MagicOMFHandle* handle, const char* name)
{
    struct RECORD* record = handle->root;
    int c_index = 1;
    while (record != NULL)
    {
        if (record->type == LNAMES_ID)
        {
            struct LNAMES* lnames_record = (struct LNAMES*) (record->contents);
            while (lnames_record != NULL)
            {
                if (strcmp(name, lnames_record->n_string) == 0)
                {
                    return c_index;
                }
                c_index++;
                lnames_record = lnames_record->next;
            }
        }
        record = record->next;
    }

    return -1;
}

int MagicOMFGetSEGDEFIndex(struct MagicOMFHandle* handle, const char* name)
{
    struct RECORD* record = handle->root;
    int c_index = 1;
    while (record != NULL)
    {
        if (record->type == SEGDEF_16_ID)
        {
            struct SEGDEF_16* segdef_16 = (struct SEGDEF_16*) record->contents;
            int segname_index = segdef_16->seg_name_index;
            if (strcmp(MagicOMFGetLNAMESNameByIndex(handle, segname_index), name) == 0)
            {
                return segname_index;
            }
            c_index++;
        }
        record = record->next;
    }

    return -1;
}

int MagicOMFGetEXTDEFIndex(struct MagicOMFHandle* handle, const char* name)
{
    struct RECORD* record = handle->root;
    int c_index = 1;
    while (record != NULL)
    {
        if (record->type == EXTDEF_ID)
        {
            struct EXTDEF* extdef_record = (struct EXTDEF*) (record->contents);
            while (extdef_record != NULL)
            {
                if (strcmp(name, extdef_record->name_str) == 0)
                {
                    return c_index;
                }
                c_index++;
                extdef_record = extdef_record->next;
            }
        }
        record = record->next;
    }

    return -1;
}

const char* MagicOMFErrorMessage(MAGIC_OMF_ERROR_CODE error_id)
{
    return GetErrorMessage(error_id);
}