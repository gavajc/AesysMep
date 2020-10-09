#include "aesys_mep.h"
//---------------------------------------------------------------------

#ifdef AESYS_MEP_EXPORTS
    #include "libver.h"
    DECLARE_LIB_VERSION(1,0,DRAFT-JCG);
#endif

#define K_CRC_MASK 0x8000
#define K_POLYGEN  0x1021
#define K_DINC     0x0080
#define K_DSTX     0x0082
#define K_DETX     0x0083
#define K_DDLE     0x0090

#define GETVAL16(d,s,p) { d = (s[p] << 8 | s[p+1]); p+=2; }
#define GETVAL32(d,s,p) { d = (((uint32_t) s[p+3] << 0)  | \
                               ((uint32_t) s[p+2] << 8)  | \
                               ((uint32_t) s[p+1] << 16) | \
                               ((uint32_t) s[p]   << 24)); p+=4; }
//---------------------------------------------------------------------

tAESYS_MEP_CODE_PROPERTIES records[] =
{
    { .code = MEP_STATUS                    , .nbne = 0, .iops = MEP_RD, .type = MEP_VOID    , },
    { .code = MEP_HARDWARE_MODEL            , .nbne = 0, .iops = MEP_RD, .type = MEP_STRING  , },
    { .code = MEP_FIRMWARE_MODEL            , .nbne = 0, .iops = MEP_RD, .type = MEP_STRING  , },
    { .code = MEP_FIRMWARE_VERSION          , .nbne = 0, .iops = MEP_RD, .type = MEP_STRING  , },
    { .code = MEP_FIRMWARE_RELEASE          , .nbne = 0, .iops = MEP_RD, .type = MEP_STRING  , },
    { .code = MEP_FIRMWARE_DEVICE_TYPE      , .nbne = 0, .iops = MEP_RW, .type = MEP_UINT8   , },
    { .code = MEP_DEVICE_ID                 , .nbne = 0, .iops = MEP_RW, .type = MEP_STRING  , },
    { .code = MEP_DEVICE_DESCRIPTION        , .nbne = 1, .iops = MEP_RW, .type = MEP_STRING  , },
    { .code = MEP_RESET                     , .nbne = 0, .iops = MEP_WR, .type = MEP_VOID    , },
    { .code = MEP_VIS_EXTENSIBLE            , .nbne = 0, .iops = MEP_RW, .type = MEP_BINARY  , },
    { .code = MEP_TEMP_1                    , .nbne = 0, .iops = MEP_RD, .type = MEP_INT8    , },
    { .code = MEP_TEMP_2                    , .nbne = 0, .iops = MEP_RD, .type = MEP_INT8    , },
    { .code = MEP_TEMP_3                    , .nbne = 0, .iops = MEP_RD, .type = MEP_INT8    , },
    { .code = MEP_TEMP_4                    , .nbne = 0, .iops = MEP_RD, .type = MEP_INT8    , },
    { .code = MEP_TEMP_5                    , .nbne = 0, .iops = MEP_RD, .type = MEP_INT8    , },
    { .code = MEP_TEMP_6                    , .nbne = 0, .iops = MEP_RD, .type = MEP_INT8    , },
    { .code = MEP_TEMP_7                    , .nbne = 0, .iops = MEP_RD, .type = MEP_INT8    , },
    { .code = MEP_TEMP_8                    , .nbne = 0, .iops = MEP_RD, .type = MEP_INT8    , },
    { .code = MEP_HUMIDITY_1                , .nbne = 0, .iops = MEP_RD, .type = MEP_UINT8   , },
    { .code = MEP_HUMIDITY_2                , .nbne = 0, .iops = MEP_RD, .type = MEP_UINT8   , },
    { .code = MEP_HUMIDITY_3                , .nbne = 0, .iops = MEP_RD, .type = MEP_UINT8   , },
    { .code = MEP_HUMIDITY_4                , .nbne = 0, .iops = MEP_RD, .type = MEP_UINT8   , },
    { .code = MEP_ENVIRONMENTAL_BRIGHTNESS_1, .nbne = 0, .iops = MEP_RD, .type = MEP_UINT8   , },
    { .code = MEP_ENVIRONMENTAL_BRIGHTNESS_2, .nbne = 0, .iops = MEP_RD, .type = MEP_UINT8   , },
    { .code = MEP_ENVIRONMENTAL_BRIGHTNESS_3, .nbne = 0, .iops = MEP_RD, .type = MEP_UINT8   , },
    { .code = MEP_ENVIRONMENTAL_BRIGHTNESS_4, .nbne = 0, .iops = MEP_RD, .type = MEP_UINT8   , },
    { .code = MEP_ENVIRONMENTAL_BRIGHTNESS_5, .nbne = 0, .iops = MEP_RD, .type = MEP_UINT8   , },
    { .code = MEP_ENVIRONMENTAL_BRIGHTNESS_6, .nbne = 0, .iops = MEP_RD, .type = MEP_UINT8   , },
    { .code = MEP_ENVIRONMENTAL_BRIGHTNESS_7, .nbne = 0, .iops = MEP_RD, .type = MEP_UINT8   , },
    { .code = MEP_ENVIRONMENTAL_BRIGHTNESS_8, .nbne = 0, .iops = MEP_RD, .type = MEP_UINT8   , },
    { .code = MEP_DEVICE_RESTARTED          , .nbne = 0, .iops = MEP_RD, .type = MEP_BOOL    , },
    { .code = MEP_DOORS_OPEN                , .nbne = 0, .iops = MEP_RD, .type = MEP_BOOL    , },
    { .code = MEP_INTERNAL_ERROR_CODE       , .nbne = 0, .iops = MEP_RD, .type = MEP_UINT16  , },
    { .code = MEP_POWER_SAVING_STATUS       , .nbne = 0, .iops = MEP_RD, .type = MEP_UINT8   , },
    { .code = MEP_BATTERY_LEVEL             , .nbne = 0, .iops = MEP_RW, .type = MEP_UINT8   , },
    { .code = MEP_FANS_ACTIVE               , .nbne = 0, .iops = MEP_RD, .type = MEP_BOOL    , },
    { .code = MEP_HEATING_ACTIVE            , .nbne = 0, .iops = MEP_RD, .type = MEP_BOOL    , },
    { .code = MEP_SIREN_ACTIVE              , .nbne = 0, .iops = MEP_RD, .type = MEP_BOOL    , },
    { .code = MEP_BROKEN_FANS_NUMBER        , .nbne = 0, .iops = MEP_RD, .type = MEP_UINT8   , },
    { .code = MEP_BROKEN_LEDS_NUMBER        , .nbne = 0, .iops = MEP_RD, .type = MEP_UINT32  , },
    { .code = MEP_BROKEN_BACKLIGHTS_NUMBER  , .nbne = 0, .iops = MEP_RD, .type = MEP_UINT8   , },
    { .code = MEP_NUM_BROKEN_LED_BOARDS     , .nbne = 0, .iops = MEP_RD, .type = MEP_UINT16  , },
    { .code = MEP_CLOCK                     , .nbne = 0, .iops = MEP_RW, .type = MEP_BINARY  , },
    { .code = MEP_COLORS_CALIBRATION        , .nbne = 0, .iops = MEP_RW, .type = MEP_BINARY  , },
    { .code = MEP_REMEMBER_LAST_PUBLICATION , .nbne = 0, .iops = MEP_RW, .type = MEP_BOOL    , },
    { .code = MEP_BRIGHTNESS_1              , .nbne = 0, .iops = MEP_RW, .type = MEP_UINT8   , },
    { .code = MEP_BRIGHTNESS_2              , .nbne = 0, .iops = MEP_RW, .type = MEP_UINT8   , },
    { .code = MEP_BRIGHTNESS_3              , .nbne = 0, .iops = MEP_RW, .type = MEP_UINT8   , },
    { .code = MEP_BRIGHTNESS_4              , .nbne = 0, .iops = MEP_RW, .type = MEP_UINT8   , },
    { .code = MEP_TRAFFIC_LIGHT_STATUS_1    , .nbne = 0, .iops = MEP_RW, .type = MEP_BINARY  , },
    { .code = MEP_TRAFFIC_LIGHT_STATUS_2    , .nbne = 0, .iops = MEP_RW, .type = MEP_BINARY  , },
    { .code = MEP_TRAFFIC_LIGHT_STATUS_3    , .nbne = 0, .iops = MEP_RW, .type = MEP_BINARY  , },
    { .code = MEP_TRAFFIC_LIGHT_STATUS_4    , .nbne = 0, .iops = MEP_RW, .type = MEP_BINARY  , },
};
//---------------------------------------------------------------------

///
/// \brief Private functions declarations.
///
static uint8_t isValidCommand(uint8_t cmd);
static void calculateCRCByte(uint8_t byte, uint16_t *crc);
static uint16_t cpydataToSendFrame(uint8_t *src, uint8_t *dst, uint16_t size, uint16_t *crc);
static void  swapStrBytes(uint8_t *src, uint32_t src_size, uint8_t element_size, uint32_t count);
static uint8_t encodeToUPTBFrame(uint8_t tx_bytes, uint8_t *buffer, uint16_t b_size, tAESYS_MEP_BUFFER *frame);
static int addTextProperties(uint8_t *buffer, uint16_t *offset, const tAESYS_MEP_MSG_DATA *msg, const tAESYS_MEP_PANEL_DATA *panel);
static uint8_t decodeData(const uint8_t *src, uint8_t *dest, uint16_t src_size, uint16_t dest_size, uint16_t *offset, uint16_t *crc);
static tAESYS_MEP_BUFFER * createSendMEPFrame(uint8_t type, uint16_t addrs, uint16_t dlen, uint16_t trans, uint8_t cmd, uint8_t *data);
//---------------------------------------------------------------------
/**********************************************************************
*****                      PRIVATE FUNCTIONS                      *****
*****                       Utility section                       *****
**********************************************************************/

void swapStrBytes(uint8_t *src, uint32_t src_size, uint8_t element_size, uint32_t count)
{
    uint8_t byte;
    uint32_t size = element_size * count;

    if (src == NULL || element_size < 2 || size > src_size)
        return;

    for (uint32_t i = 0; i < size; i+=element_size)
    {
         for (uint32_t j = i, k = element_size-1; j < i+k; j++, k--)
         {
              byte = src[j];
              src[j] = src[i+k];
              src[i+k] = byte;
         }
    }
}
//---------------------------------------------------------------------

uint8_t isValidCommand(uint8_t cmd)
{
   for (int iEnum = MEP_SET; iEnum != (MEP_DAT+1); iEnum++)
        if (cmd == iEnum)
            return 1;

   return 0;
}
//---------------------------------------------------------------------

void calculateCRCByte(uint8_t byte, uint16_t *crc)
{
    *crc = *crc ^ ((uint16_t)byte << 8);

    for (uint8_t j = 0; j < 8; j++)
    {
         if (*crc & K_CRC_MASK)
             *crc = (*crc << 1) ^ K_POLYGEN;
         else
             *crc <<= 1;
    }
}
//---------------------------------------------------------------------

uint16_t cpydataToSendFrame(uint8_t *src, uint8_t *dst, uint16_t size, uint16_t *crc)
{
    uint16_t i = 0;

    for (; i < size; i++)
    {
         if (crc != NULL)
             calculateCRCByte(src[i],crc);

         dst[i] = src[i];
    }

    return i;
}
//---------------------------------------------------------------------

uint8_t decodeData(const uint8_t *src, uint8_t *dest, uint16_t src_size,
                   uint16_t dest_size, uint16_t *offset, uint16_t *crc)
{
    uint16_t i = 0, j = 0;

    for (; j < dest_size && src[i] != K_MEP_ETX && i < src_size; i++)
    {
         if (src[i] == K_MEP_DLE)
         {
             if (i+1 == src_size)
                 return 0;
             if (src[i+1] == K_DSTX || src[i+1] == K_DETX || src[i+1] == K_DDLE)
             {
                 dest[j++] = src[++i] - K_DINC;
                 if (crc != NULL)
                     calculateCRCByte(dest[j-1],crc);
             }
             else
                 return 0;
         }
         else
         {
             dest[j++] = src[i];
             if (crc != NULL)
                 calculateCRCByte(src[i],crc);
         }
    }

    *offset += i;

    return (j == dest_size);
}
//---------------------------------------------------------------------

uint8_t encodeToUPTBFrame(uint8_t tx_bytes, uint8_t *buffer, uint16_t b_size, tAESYS_MEP_BUFFER *frame)
{
    uint8_t plus;
    uint16_t j = ((tx_bytes) ? 1 : 0);
    uint16_t size = 0, newElements = 0;

    if (buffer == NULL || frame == NULL || frame->data != NULL || b_size > K_MEP_MAX_FRAME_SIZE)
        return 0;

    // Check the num of elements to encode.
    for (uint16_t i = 0; i < b_size; i++)
         if (buffer[i] == K_MEP_STX || buffer[i] == K_MEP_ETX || buffer[i] == K_MEP_DLE)
             newElements++;

    // Check the final frame size.
    size = b_size + newElements;
    if (size > K_MEP_MAX_FRAME_SIZE)
        return 0;

    frame->size  = size+tx_bytes;
    frame->data = (uint8_t *) calloc(frame->size,sizeof(uint8_t));
    if (frame->data == NULL)
        return 0;

    for (uint16_t i = 0; i < b_size; i++, j++)
    {
        plus = 0;
        if (buffer[i] == K_MEP_STX || buffer[i] == K_MEP_ETX || buffer[i] == K_MEP_DLE)
        {
            frame->data[j] = K_MEP_DLE;
            plus = K_DINC;
            j++;
        }

        frame->data[j] = buffer[i] + plus;
    }

    // Check if add the TX bytes: STX and ETX
    if (tx_bytes)
    {
        frame->data[0] = K_MEP_STX;
        frame->data[j] = K_MEP_ETX;
    }

    return 1;
}
//---------------------------------------------------------------------

tAESYS_MEP_BUFFER * createSendMEPFrame(uint8_t type, uint16_t addrs, uint16_t dlen, uint16_t trans, uint8_t cmd, uint8_t *data)
{
    tAESYS_MEP_UPTB_FRAME uptbFrame = { .crc = 0, .addr = htons(addrs), .pptp.dlen = htons(dlen), .pptp.tran = htons(trans), .pptp.cmd = cmd, };
    uint16_t offset  = 0, headSize  = 5   , crc = 0xFFFF, *crcPtr = NULL, frameSize = headSize + dlen;
    uint8_t  txBytes = 0, *tBuffer  = NULL, *ptrFrame = (uint8_t *) &uptbFrame.pptp;
    tAESYS_MEP_BUFFER *frame  = NULL;

    if (type)
    {
        frameSize += 4;
        headSize  += 2;
        ptrFrame   = (uint8_t *) &uptbFrame;
        ptrFrame  += 2;
        crcPtr     = &crc;
        txBytes    = (type == MEP_UPTBNTX) ? 0 : 2;
    }

    if (frameSize > K_MEP_MAX_FRAME_SIZE || dlen > K_MEP_MAX_DATA_SIZE || type > 2)
        goto CSFRAME_ERROR;

    tBuffer = (uint8_t *) calloc(frameSize,sizeof(uint8_t));
    if (tBuffer == NULL)
        goto CSFRAME_ERROR;

    frame   = (tAESYS_MEP_BUFFER *) calloc(1,sizeof(tAESYS_MEP_BUFFER));
    if (frame == NULL)
        goto CSFRAME_ERROR;

    offset += cpydataToSendFrame(ptrFrame,&tBuffer[offset],headSize,crcPtr);
    offset += cpydataToSendFrame(data,&tBuffer[offset],dlen,crcPtr);

    switch (type)
    {
        case  MEP_PPTP:
            frame->size = frameSize;
            frame->data = tBuffer;
        break;
        case MEP_UPTB:
        case MEP_UPTBNTX:
            crc = htons(crc);
            memcpy(&tBuffer[offset],&crc,2);
            if (encodeToUPTBFrame(txBytes,tBuffer,frameSize,frame) == 0)
                goto CSFRAME_ERROR;

            free(tBuffer);
        break;
    }

    return frame;

    CSFRAME_ERROR:

    if (tBuffer != NULL)
        free(tBuffer);

    if (frame != NULL) {
        if (frame->data != NULL)
            free(frame->data);
        free(frame);
    }

    return frame;
}
//---------------------------------------------------------------------

int addTextProperties(uint8_t *buffer, uint16_t *offset, const tAESYS_MEP_MSG_DATA *msg, const tAESYS_MEP_PANEL_DATA *panel)
{
    char vat[4];
    char hat[4];
    char aux[10];
    uint16_t size;
    uint16_t lpdh, lpdv;
    uint16_t rows, cols;
    uint16_t vAlign, hAlign;
    int page_size = *offset;
    uint8_t  rowSpacing, colSpacing;

    #define APPENDV(cmd,nv,v1,v2,v3,v4) \
    {\
        if (nv == 0) \
            sprintf(aux,"%s",cmd); \
        if (nv == 1) \
            sprintf(aux,"%s%c",cmd,v1);   \
        if (nv == 2) \
            sprintf(aux,"%s%c%c",cmd,v1,v2); \
        if (nv == 3) \
            sprintf(aux,"%s%s",cmd,v3);   \
        if (nv == 4) \
            sprintf(aux,"%s%s%s",cmd,v3,v4); \
        \
        size = strlen(aux);  \
        if (((*offset) + size) > K_MEP_MAX_DATA_SIZE) \
             return -1; \
        \
        memcpy(&buffer[*offset],aux,size); \
        *offset += size; \
    }\

    // ### CONSTRUCT THE DRAWING COMMANDS ###
    // Blinking text
    if (msg->blinking_text == 1)
        APPENDV("\x17\x41",0,0,0,"","");

    // Antialiasing
    if (msg->antialiasing > 0 && msg->antialiasing <= 9)
        APPENDV("\x17\x42",1,msg->antialiasing+48,0,"","");

    // Get the vertical leds used by each letter
    // and calculate the max number of rows supported by the device.
    rowSpacing = (msg->row_spacing > 9 || msg->total_rows == 1) ? 0 : msg->row_spacing;
    lpdv = panel->font_size[1]+rowSpacing;

    if (msg->truncate)
    {
        rows = (panel->panel_size[1]+rowSpacing)/lpdv;
        if (rows == 0 || msg->total_rows < rows)
            rows = msg->total_rows;
    }
    else
        rows = msg->total_rows;

    // Calculate the vertical alignment for the first element.
    vAlign = panel->panel_size[1] - ((lpdv * rows) - rowSpacing);        // Bottom.

    if (vAlign > panel->panel_size[1] || !msg->v_alignment) vAlign  = 0;  // Top or invalid value.
    else if (msg->v_alignment == 1)                         vAlign /= 2;  // Center.

    for (uint16_t r = 0; r < rows; r++)
    {
         // Get the horizontal leds used by each letter
         // and calculate the aprox max number of cols supported by the device.
         colSpacing = (msg->rows[r].col_spacing > 9 ) ? 0 : msg->rows[r].col_spacing;
         lpdh = panel->font_size[0]+colSpacing;
         if (msg->truncate && !msg->rows[r].compact_font)
         {
             cols = (panel->panel_size[0]+colSpacing)/lpdh;
             if (cols == 0 || msg->rows[r].msg_size < cols)
                 cols = msg->rows[r].msg_size;
         }
         else
             cols = msg->rows[r].msg_size;

         // Calculate the horizontal alignment. If compactfont is true,
         // then disable horizontal aligment because the size of each letter is variable.
         hAlign = panel->panel_size[0] - ((lpdh * cols) - colSpacing);               // Right.

         if (hAlign > panel->panel_size[0] || !msg->rows[r].h_alignment || msg->rows[r].compact_font)  // Left, invalid value or compactFont is set.
             hAlign  = 0;
         else if (msg->rows[r].h_alignment == 1)                                                       // Center.
             hAlign /= 2;

         // Vertical and Horizontal alignment as hex value.
         sprintf(vat,"%.3X",vAlign);
         sprintf(hat,"%.3X",hAlign);

         // Basic color or RGBY color
         if (msg->rows[r].custom_colors[0] && msg->rows[r].custom_colors[0] <= 5) {
             APPENDV("\x17\x43",1,msg->rows[r].custom_colors[0]+47,0,"","");
         }
         else if (msg->rows[r].custom_colors[0] == 6 && isxdigit(msg->rows[r].custom_colors[1])      &&
                  isxdigit(msg->rows[r].custom_colors[2]) && isxdigit(msg->rows[r].custom_colors[3]) &&
                  isxdigit(msg->rows[r].custom_colors[4])) {
             APPENDV("\x17\x44",3,0,0,&msg->rows[r].custom_colors[1],"");
         }

         // Scrolling
         if (msg->rows[r].scrolling_speed >= 1 && msg->rows[r].scrolling_speed <= 9)
             APPENDV("\x17\x53\x48",1,msg->rows[r].scrolling_speed+48,0,"","");

         // Set Position, font, col & row Spacing
         APPENDV("\x17\x51",4,0,0,vat,hat);
         APPENDV("\x17\x46\x31",2,colSpacing+48,rowSpacing+48,"","");

         if (msg->rows[r].compact_font)
             APPENDV("\x17\x4F",0,0,0,"","");

         if (((*offset)+cols) > K_MEP_MAX_DATA_SIZE)
             return -1;

         memcpy(&buffer[*offset],msg->rows[r].msg,cols);
         *offset += cols;
         vAlign  += lpdv;
    }

    return ((*offset)-page_size);
}

//---------------------------------------------------------------------
/**********************************************************************
*****                       PUBLIC FUNCTIONS                      *****
*****                        Decode section                       *****
**********************************************************************/

const tAESYS_MEP_CODE_PROPERTIES * AesysMepGetCodeProperties(uint16_t code)
{
    uint16_t size = sizeof (records)/sizeof(tAESYS_MEP_CODE_PROPERTIES);

    for (uint16_t i = 0; i < size; i++)
         if (code == records[i].code)
             return &records[i];

    return NULL;
}
//---------------------------------------------------------------------

uint8_t * AesysMepDecodeUPTBFrame(const uint8_t *frame, uint16_t frame_size)
{
    uint8_t buffer[7], *decoded = NULL;
    uint16_t crc, size = 0, offset = 1, compute_crc = 0xFFFF;

    // Check frame data. Not null and have STX and ETX bytes.
    if (frame == NULL   || frame_size < K_MEP_MIN_SIZE_UPTB ||
        frame[0] != K_MEP_STX || frame[frame_size-1] != K_MEP_ETX)
        goto DEC_ERROR;

    // Decoded the first 7 bytes from frame. i.e. UoPTB header.
    if (!decodeData(&frame[1],buffer,frame_size-offset,7,&offset,&compute_crc))
        goto DEC_ERROR;

    // Check if have a valid command
    if (!isValidCommand(buffer[6]))
        goto DEC_ERROR;

    // Swap header bytes and check the data frame length
    swapStrBytes(buffer,6,2,3);
    memcpy(&size,&buffer[2],2);
    if (size > K_MEP_MAX_DATA_SIZE)
        goto DEC_ERROR;

    // Prepare memory for decoded frame. CRC,ADDR,DLEN,TRANS,CMD,DATA
    decoded = (uint8_t *) calloc(size+9,sizeof (uint8_t));
    if (decoded == NULL)
        goto DEC_ERROR;

    // Copy UoPTB header and decode data from frame
    memcpy(&decoded[2],buffer,7);

    if (!decodeData(&frame[offset],&decoded[9],frame_size-offset,size,&offset,&compute_crc))
        goto DEC_ERROR;

    // Decode the CRC
    if (!decodeData(&frame[offset],buffer,frame_size-offset,2,&offset,NULL))
        goto DEC_ERROR;

    // Check if the length is valid. Plus 1 that represents the ETX byte.
    if (offset+1 != frame_size)
        goto DEC_ERROR;

    // Validate the CRC.
    offset = 0;
    GETVAL16(crc,buffer,offset);
    if (crc != compute_crc)
        goto DEC_ERROR;

    // Copy CRC
    memcpy(&decoded[0],&crc,2);

    return decoded;

    DEC_ERROR:

    if (decoded != NULL)
        free(decoded);

    return NULL;
}
//---------------------------------------------------------------------

uint8_t * AesysMepCncopyPPTPFrame(const uint8_t *frame, uint16_t frame_size)
{
    uint16_t size, offset = 0;
    uint8_t *duplicate = NULL;

    if (frame != NULL && frame_size >= K_MEP_MIN_SIZE_PPTB)
    {
        // Check if have a valid command
        if (!isValidCommand(frame[4]))
            return NULL;

        // Check the data size is valid.
        GETVAL16(size,frame,offset);
        if (size > K_MEP_MAX_DATA_SIZE || size != frame_size-5)
            return NULL;

        // Alloc memory for the new frame, copy and transform the header.
        duplicate = (uint8_t *) calloc(frame_size,sizeof(uint8_t));

        if (duplicate != NULL)
        {
            memcpy(duplicate,frame,frame_size);
            swapStrBytes(duplicate,frame_size,2,2);
        }
    }

    return duplicate;
}
//---------------------------------------------------------------------

char AesysMepReadNextDelCMD(uint8_t *payload, uint16_t p_size, uint16_t *offset, uint16_t *code)
{
    uint16_t size = sizeof(uint16_t);

    if (payload == NULL || offset == NULL || code == NULL || p_size < size)
        return -1;
    if (*offset >= p_size)
        return 0;
    if ((*offset)+size > p_size)
        return -1;

    GETVAL16(*code,payload,*offset);

    return 1;
}
//---------------------------------------------------------------------

char AesysMepReadNextSetCMD(uint8_t *payload, uint16_t p_size, uint16_t *offset, tAESYS_MEP_SET_CMD *cmd)
{
    uint16_t size = sizeof(tAESYS_MEP_SET_CMD)-sizeof(uint8_t *);

    if (payload == NULL || offset == NULL || cmd == NULL || p_size < size)
        return -1;
    if (*offset >= p_size)
        return 0;
    if ((*offset)+size > p_size)
        return -1;

    GETVAL16(cmd->code,payload,*offset);
    GETVAL32(cmd->offset,payload,*offset);
    GETVAL16(cmd->length,payload,*offset);
    cmd->data = NULL;

    // Validate data length
    if (cmd->length)
    {
        if ((*offset)+cmd->length <= p_size)
            cmd->data = &payload[*offset];
        else
            return -1;

        *offset += cmd->length;
    }

    return 1;
}
//---------------------------------------------------------------------

char AesysMepReadNextGetCMD(uint8_t *payload, uint16_t p_size, uint16_t *offset, tAESYS_MEP_GET_CMD *cmd)
{
    uint16_t size = sizeof(tAESYS_MEP_GET_CMD);

    if (payload == NULL || offset == NULL || cmd == NULL || p_size < size)
        return -1;
    if (*offset >= p_size)
        return 0;
    if ((*offset)+size > p_size)
        return -1;

    GETVAL16(cmd->code,payload,*offset);
    GETVAL32(cmd->offset,payload,*offset);

    return 1;
}
//---------------------------------------------------------------------

char AesysMepReadNextDatCMD(uint8_t *payload, uint16_t p_size, uint16_t *offset, tAESYS_MEP_DAT_CMD *cmd)
{
    uint16_t size = sizeof(tAESYS_MEP_DAT_CMD)-sizeof(uint8_t *);

    if (payload == NULL || offset == NULL || cmd == NULL || p_size < size)
        return -1;
    if (*offset >= p_size)
        return 0;
    if ((*offset)+size > p_size)
        return -1;

    GETVAL16(cmd->code,payload,*offset);
    GETVAL32(cmd->offset,payload,*offset);
    cmd->flags = payload[(*offset)++];
    GETVAL16(cmd->length,payload,*offset);
    cmd->data  = NULL;

    // Validate data length
    if (cmd->length)
    {
        if ((*offset)+cmd->length <= p_size)
            cmd->data = &payload[*offset];
        else
            return -1;

        *offset += cmd->length;
    }

    return 1;
}
//---------------------------------------------------------------------

tAESYS_MEP_RESPONSE * AesysMepParseResponse(uint8_t *mep_frame, uint16_t mep_frame_size, uint8_t mep_frame_type)
{
    char ret;
    uint16_t offset = 1;
    tAESYS_MEP_DAT_CMD dat;
    tAESYS_MEP_PPTP_FRAME *pptp        = NULL;
    tAESYS_MEP_RESPONSE *response      = NULL;
    tAESYS_MEP_RESPONSE_DATA **current = NULL;
    uint8_t *payload = NULL, *vframe   = NULL;
    uint8_t unknown_types[] = {MEP_VOID,MEP_UINT8,MEP_UINT16,MEP_BINARY,MEP_UINT32};

    #define RESPONSE_ERROR(e)           \
    {                                   \
        free(vframe);                   \
        AesysMepFreeResponse(response); \
        errno = e;                      \
        return NULL;                    \
    }                                   \

    if (mep_frame == NULL || mep_frame_type > 1)
        RESPONSE_ERROR(EINVAL);

    if (mep_frame_type)
    {
        if ((vframe = AesysMepDecodeUPTBFrame(mep_frame,mep_frame_size)) == NULL)
            RESPONSE_ERROR(ENOEXEC);

        tAESYS_MEP_UPTB_FRAME *uptb = (tAESYS_MEP_UPTB_FRAME *) vframe;
        pptp = &uptb->pptp;
    }
    else
    {
        if ((vframe = AesysMepCncopyPPTPFrame(mep_frame,mep_frame_size)) == NULL)
            RESPONSE_ERROR(ENOEXEC);

        pptp = (tAESYS_MEP_PPTP_FRAME *) vframe;
    }

    if (pptp->cmd != MEP_DAT)
        RESPONSE_ERROR(EPERM);

    payload = &pptp->payload;
    if (payload[0] != 0)
        RESPONSE_ERROR(ENOEXEC);

    response = (tAESYS_MEP_RESPONSE *) calloc(1,sizeof (tAESYS_MEP_RESPONSE));
    if (response == NULL)
        RESPONSE_ERROR(ENOMEM);

    current = &response->data;
    response->tran = pptp->tran;
    while ((ret = AesysMepReadNextDatCMD(payload,pptp->dlen,&offset,&dat)) == 1)
    {
        if (response->type == 0)
             response->type = dat.code;

        if (dat.flags == 1) // Code not supported by device discard code response.
            continue;

        *current = (tAESYS_MEP_RESPONSE_DATA *) calloc(1,sizeof (tAESYS_MEP_RESPONSE_DATA));
        if (*current == NULL)
            RESPONSE_ERROR(ENOMEM);

        (*current)->code = dat.code;
        (*current)->flag = dat.flags;
        (*current)->size = dat.length;

        if (dat.length)
        {
            const tAESYS_MEP_CODE_PROPERTIES *code_prop = AesysMepGetCodeProperties(dat.code);

            if (code_prop != NULL)    (*current)->type = code_prop->type;
            else if (dat.length < 5)  (*current)->type = unknown_types[dat.length];
            else                      (*current)->type = MEP_BINARY;

            (*current)->resp_data = calloc(1,(*current)->size);
            if ((*current)->resp_data == NULL)
                RESPONSE_ERROR(ENOMEM);

            memcpy((*current)->resp_data,dat.data,dat.length);
        }

        current = &(*current)->next;
    }

    if (ret < 0)
        RESPONSE_ERROR(EILSEQ);

    free(vframe);

    return response;
}
//---------------------------------------------------------------------

char AesysMepReadNextVisExtData(uint8_t *payload, uint16_t p_size, uint16_t *offset, uint16_t *elements, tAESYS_MEP_VIS_EXT_DATA *p_data)
{
    uint8_t noe, nop;
    uint16_t size = sizeof (tAESYS_MEP_VIS_EXT_DATA) + 1;

    if (payload  == NULL || offset == NULL || elements == NULL || p_data  == NULL)
        return -1;

    if (payload[0] == 0)
    {
        if (p_size > 1)
            return -1;

        return 0;
    }

    p_data->page = NULL;
    noe = *elements >> 8;
    nop = *elements & 0xFF;

    if (*offset >= p_size)
    {
        if (noe != payload[0])
            return -1;

        return 0;
    }

    if (nop == 0)
    {
        if ((*offset)+2 > p_size)
            return -1;

        p_data->id = payload[(*offset)++];
        nop        = payload[(*offset)++];

        noe++;
        *elements = ((*elements)&0x00FF) | (noe<<8);

        if (nop == 0)
            return 1;
    }

    if ((*offset)+size > p_size)
        return -1;

    p_data->page = (tAESYS_MEP_VIS_EXT_PAGE *) &payload[*offset];

    // Validate the pageDef size according to property size
    size = ntohs(p_data->page->size);
    *offset += size+5;

    if (*offset > p_size)
        return -1;

    nop--;
    *elements = ((*elements)&0xFF00) | nop;

    return 1;
}
//---------------------------------------------------------------------
/**********************************************************************
*****                     Device Info section                     *****
**********************************************************************/

tAESYS_MEP_BUFFER * AesysMepBuildClockInfoMsg(uint8_t type, uint16_t trans_id)
{
    tAESYS_MEP_GET_CMD clock_info = { .code = htons(MEP_CLOCK), .offset = 0, };

    return createSendMEPFrame(type,0xFFFE,sizeof (clock_info),trans_id,MEP_GET,(uint8_t *) &clock_info);
}
//---------------------------------------------------------------------

tAESYS_MEP_BUFFER * AesysMepBuildDeviceInfoMsg(uint8_t type, uint16_t trans_id)
{
    tAESYS_MEP_GET_CMD hw_info[] = {
                                      { .code = htons(MEP_CUSTOM_DEVICE_INFO_DATA), .offset = 0, },
                                      { .code = htons(MEP_HARDWARE_MODEL)         , .offset = 0, },
                                      { .code = htons(MEP_FIRMWARE_MODEL)         , .offset = 0, },
                                      { .code = htons(MEP_FIRMWARE_VERSION)       , .offset = 0, },
                                      { .code = htons(MEP_FIRMWARE_RELEASE)       , .offset = 0, },
                                      { .code = htons(MEP_FIRMWARE_DEVICE_TYPE)   , .offset = 0, },
                                      { .code = htons(MEP_DEVICE_ID)              , .offset = 0, },
                                      { .code = htons(MEP_DEVICE_DESCRIPTION)     , .offset = 0, },
                                  };

    return createSendMEPFrame(type,0xFFFE,sizeof (hw_info),trans_id,MEP_GET,(uint8_t *) hw_info);
}
//---------------------------------------------------------------------

tAESYS_MEP_BUFFER * AesysMepBuildDevStatusInfoMsg(uint8_t type, uint16_t trans_id)
{
    tAESYS_MEP_GET_CMD dev_status [] = {
                                         { .code = htons(MEP_CUSTOM_STATUS_INFO_DATA), .offset = 0, },
                                         { .code = htons(MEP_STATUS)                 , .offset = 0, },
                                       };

    return createSendMEPFrame(type,0xFFFE,sizeof (dev_status),trans_id,MEP_GET,(uint8_t *) dev_status);
}
//---------------------------------------------------------------------

tAESYS_MEP_BUFFER * AesysMepBuildDiagnosticInfoMsg(uint8_t type, uint16_t trans_id)
{
    tAESYS_MEP_GET_CMD diag_info[] = {
                                       { .code = htons(MEP_CUSTOM_DIAGNOSTIC_INFO_DATA), .offset = 0, },
                                       { .code = htons(MEP_DOORS_OPEN)                 , .offset = 0, },
                                       { .code = htons(MEP_POWER_SAVING_STATUS)        , .offset = 0, },
                                       { .code = htons(MEP_BATTERY_LEVEL)              , .offset = 0, },
                                       { .code = htons(MEP_FANS_ACTIVE)                , .offset = 0, },
                                       { .code = htons(MEP_SIREN_ACTIVE)               , .offset = 0, },
                                       { .code = htons(MEP_HEATING_ACTIVE)             , .offset = 0, },
                                       { .code = htons(MEP_BROKEN_FANS_NUMBER)         , .offset = 0, },
                                       { .code = htons(MEP_BROKEN_BACKLIGHTS_NUMBER)   , .offset = 0, },
                                       { .code = htons(MEP_INTERNAL_ERROR_CODE)        , .offset = 0, },
                                       { .code = htons(MEP_NUM_BROKEN_LED_BOARDS)      , .offset = 0, },
                                       { .code = htons(MEP_BROKEN_LEDS_NUMBER)         , .offset = 0, },
                                    };

    return createSendMEPFrame(type,0xFFFE,sizeof (diag_info),trans_id,MEP_GET,(uint8_t *) diag_info);
}
//---------------------------------------------------------------------

tAESYS_MEP_BUFFER * AesysMepBuildDevRestartedInfoMsg(uint8_t type, uint16_t trans_id)
{
    tAESYS_MEP_GET_CMD dev_restarted = { .code = htons(MEP_DEVICE_RESTARTED), .offset = 0, };

    return createSendMEPFrame(type,0xFFFE,sizeof (dev_restarted),trans_id,MEP_GET,(uint8_t *) &dev_restarted);
}
//---------------------------------------------------------------------

tAESYS_MEP_BUFFER * AesysMepBuildLastPublicationInfoMsg(uint8_t type, uint16_t trans_id)
{
    tAESYS_MEP_GET_CMD pub_info = { .code = htons(MEP_REMEMBER_LAST_PUBLICATION), .offset = 0, };

    return createSendMEPFrame(type,0xFFFE,sizeof (pub_info),trans_id,MEP_GET,(uint8_t *) &pub_info);
}
//---------------------------------------------------------------------

tAESYS_MEP_BUFFER * AesysMepBuildTempInfoMsg(uint8_t type, uint16_t trans_id, uint16_t code)
{
    uint16_t temp_codes[] = {MEP_TEMP_1,MEP_TEMP_2,MEP_TEMP_3,MEP_TEMP_4,MEP_TEMP_5,MEP_TEMP_6,MEP_TEMP_7,MEP_TEMP_8};
    tAESYS_MEP_GET_CMD temp_info[] = {
                                       { .code = htons(MEP_CUSTOM_TEMPERATURE_INFO_DATA), .offset = 0, },
                                       { .code = htons(temp_codes[0])                   , .offset = 0, },
                                       { .code = htons(temp_codes[1])                   , .offset = 0, },
                                       { .code = htons(temp_codes[2])                   , .offset = 0, },
                                       { .code = htons(temp_codes[3])                   , .offset = 0, },
                                       { .code = htons(temp_codes[4])                   , .offset = 0, },
                                       { .code = htons(temp_codes[5])                   , .offset = 0, },
                                       { .code = htons(temp_codes[6])                   , .offset = 0, },
                                       { .code = htons(temp_codes[7])                   , .offset = 0, },
                                    };

    if (code != 0)
    {
        for (uint16_t i = 0, size = sizeof(temp_codes)/2; i < size; i++)
        {
             if (code == temp_codes[i])
                 return createSendMEPFrame(type,0xFFFE,sizeof (tAESYS_MEP_GET_CMD),trans_id,MEP_GET,(uint8_t *)&temp_info[i+1]);
        }

        return NULL;
    }

    return createSendMEPFrame(type,0xFFFE,sizeof (temp_info),trans_id,MEP_GET,(uint8_t *) temp_info);
}
//---------------------------------------------------------------------

tAESYS_MEP_BUFFER * AesysMepBuildHumidityInfoMsg(uint8_t type, uint16_t trans_id, uint16_t code)
{
    uint16_t hum_codes[] = {MEP_HUMIDITY_1,MEP_HUMIDITY_2,MEP_HUMIDITY_3,MEP_HUMIDITY_4};
    tAESYS_MEP_GET_CMD hum_info[] = {
                                       { .code = htons(MEP_CUSTOM_HUMIDITY_INFO_DATA), .offset = 0, },
                                       { .code = htons(hum_codes[0])                 , .offset = 0, },
                                       { .code = htons(hum_codes[1])                 , .offset = 0, },
                                       { .code = htons(hum_codes[2])                 , .offset = 0, },
                                       { .code = htons(hum_codes[3])                 , .offset = 0, },
                                    };

    if (code != 0)
    {
        for (uint16_t i = 0, size = sizeof(hum_codes)/2; i < size; i++)
        {
             if (code == hum_codes[i])
                 return createSendMEPFrame(type,0xFFFE,sizeof (tAESYS_MEP_GET_CMD),trans_id,MEP_GET,(uint8_t *)&hum_info[i+1]);
        }

        return NULL;
    }

    return createSendMEPFrame(type,0xFFFE,sizeof (hum_info),trans_id,MEP_GET,(uint8_t *) hum_info);
}
//---------------------------------------------------------------------

tAESYS_MEP_BUFFER * AesysMepBuildBrightnessInfoMsg(uint8_t type, uint16_t trans_id, uint16_t code)
{
    uint16_t bright_codes[] = {MEP_BRIGHTNESS_1,MEP_BRIGHTNESS_2,MEP_BRIGHTNESS_3,MEP_BRIGHTNESS_4};
    tAESYS_MEP_GET_CMD bright_info[] = {
                                         { .code = htons(MEP_CUSTOM_BRIGHTNESS_INFO_DATA), .offset = 0, },
                                         { .code = htons(bright_codes[0])                , .offset = 0, },
                                         { .code = htons(bright_codes[1])                , .offset = 0, },
                                         { .code = htons(bright_codes[2])                , .offset = 0, },
                                         { .code = htons(bright_codes[3])                , .offset = 0, },
                                       };

    if (code != 0)
    {
        for (uint16_t i = 0, size = sizeof(bright_codes)/2; i < size; i++)
        {
             if (code == bright_codes[i])
                 return createSendMEPFrame(type,0xFFFE,sizeof (tAESYS_MEP_GET_CMD),trans_id,MEP_GET,(uint8_t *)&bright_info[i+1]);
        }

        return NULL;
    }

    return createSendMEPFrame(type,0xFFFE,sizeof (bright_info),trans_id,MEP_GET,(uint8_t *) bright_info);
}
//---------------------------------------------------------------------

tAESYS_MEP_BUFFER * AesysMepBuildTrafficLightInfoMsg(uint8_t type, uint16_t trans_id, uint16_t code)
{
    uint16_t traffic_codes[] = {MEP_TRAFFIC_LIGHT_STATUS_1,MEP_TRAFFIC_LIGHT_STATUS_2,MEP_TRAFFIC_LIGHT_STATUS_3,MEP_TRAFFIC_LIGHT_STATUS_4};
    tAESYS_MEP_GET_CMD traffic_info[] = {
                                            { .code = htons(MEP_CUSTOM_TRAFFIC_INFO_DATA), .offset = 0, },
                                            { .code = htons(traffic_codes[0])            , .offset = 0, },
                                            { .code = htons(traffic_codes[1])            , .offset = 0, },
                                            { .code = htons(traffic_codes[2])            , .offset = 0, },
                                            { .code = htons(traffic_codes[3])            , .offset = 0, },
                                        };

    if (code != 0)
    {
        for (uint16_t i = 0, size = sizeof(traffic_codes)/2; i < size; i++)
        {
             if (code == traffic_codes[i])
                 return createSendMEPFrame(type,0xFFFE,sizeof (tAESYS_MEP_GET_CMD),trans_id,MEP_GET,(uint8_t *)&traffic_info[i+1]);
        }

        return NULL;
    }

    return createSendMEPFrame(type,0xFFFE,sizeof (traffic_info),trans_id,MEP_GET,(uint8_t *) traffic_info);
}
//---------------------------------------------------------------------

tAESYS_MEP_BUFFER * AesysMepBuildEnvBrightnessInfoMsg(uint8_t type, uint16_t trans_id, uint16_t code)
{
    uint16_t ebright_codes[]             = { MEP_ENVIRONMENTAL_BRIGHTNESS_1,MEP_ENVIRONMENTAL_BRIGHTNESS_2,
                                             MEP_ENVIRONMENTAL_BRIGHTNESS_3,MEP_ENVIRONMENTAL_BRIGHTNESS_4,
                                             MEP_ENVIRONMENTAL_BRIGHTNESS_5,MEP_ENVIRONMENTAL_BRIGHTNESS_6,
                                             MEP_ENVIRONMENTAL_BRIGHTNESS_7,MEP_ENVIRONMENTAL_BRIGHTNESS_8 };
    tAESYS_MEP_GET_CMD env_bright_info[] = {
                                              { .code = htons(MEP_CUSTOM_EBRIGHTNESS_INFO_DATA), .offset = 0, },
                                              { .code = htons(ebright_codes[0])                , .offset = 0, },
                                              { .code = htons(ebright_codes[1])                , .offset = 0, },
                                              { .code = htons(ebright_codes[2])                , .offset = 0, },
                                              { .code = htons(ebright_codes[3])                , .offset = 0, },
                                              { .code = htons(ebright_codes[4])                , .offset = 0, },
                                              { .code = htons(ebright_codes[5])                , .offset = 0, },
                                              { .code = htons(ebright_codes[6])                , .offset = 0, },
                                              { .code = htons(ebright_codes[7])                , .offset = 0, },
                                           };

    if (code != 0)
    {
        for (uint16_t i = 0, size = sizeof(ebright_codes)/2; i < size; i++)
        {
             if (code == ebright_codes[i])
                 return createSendMEPFrame(type,0xFFFE,sizeof (tAESYS_MEP_GET_CMD),trans_id,MEP_GET,(uint8_t *)&env_bright_info[i+1]);
        }

        return NULL;
    }

    return createSendMEPFrame(type,0xFFFE,sizeof (env_bright_info),trans_id,MEP_GET,(uint8_t *) env_bright_info);
}
//---------------------------------------------------------------------
/**********************************************************************
*****                 Device manipulation section                 *****
**********************************************************************/

tAESYS_MEP_BUFFER * AesysMepBuildClearPublication(uint8_t type, uint16_t trans_id)
{
    uint8_t buffer[33];
    tAESYS_MEP_SET_CMD commands[] = {
                                      { .code = htons(MEP_CUSTOM_CLEAR_PUB), .offset = 0        , .length = 0        , .data = NULL, }, // Id cmd
                                      { .code = htons(MEP_VIS_EXTENSIBLE)  , .offset = 0        , .length = 0        , .data = NULL, }, // Nice-start.
                                      { .code = htons(MEP_VIS_EXTENSIBLE)  , .offset = 0        , .length = htons(1) , .data = NULL, }, // Text Msg data.
                                      { .code = htons(MEP_VIS_EXTENSIBLE)  , .offset = htonl(1) , .length = 0        , .data = NULL, }, // Nice-end.
                                    };

    buffer[24] = 0x00; // Set clear publication byte.
    memcpy(&buffer[0] ,(uint8_t *)&commands[0],8);
    memcpy(&buffer[8] ,(uint8_t *)&commands[1],8);
    memcpy(&buffer[16],(uint8_t *)&commands[2],8);
    memcpy(&buffer[25],(uint8_t *)&commands[3],8);

    return createSendMEPFrame(type,0xFFFE,sizeof(buffer),trans_id,MEP_SET,buffer);
}
//---------------------------------------------------------------------

tAESYS_MEP_BUFFER * AesysMepBuildResetDeviceMsg(uint8_t type, uint16_t trans_id)
{
    tAESYS_MEP_SET_CMD command = { .code = htons(MEP_RESET), .offset = 0x00, .length = 0x00, .data = NULL, };

    return createSendMEPFrame(type,0xFFFE,8,trans_id,MEP_SET,(uint8_t *)&command);
}
//---------------------------------------------------------------------

tAESYS_MEP_BUFFER * AesysMepBuildDelCode(uint8_t type, uint16_t trans_id, uint16_t code)
{
    uint8_t buffer[4] = {0xFD,0xEC,0x00,0x00};
    const tAESYS_MEP_CODE_PROPERTIES *prop = AesysMepGetCodeProperties(code);

    if (prop == NULL || prop->iops == 1)
        return NULL;

    code = htons(code);
    memcpy(&buffer[2],&code,2);

    return createSendMEPFrame(type,0xFFFE,sizeof(buffer),trans_id,MEP_DEL,buffer);
}
//---------------------------------------------------------------------

tAESYS_MEP_BUFFER * AesysMepBuildClockMsg(uint8_t type, uint16_t trans_id, uint8_t *clock)
{
    uint8_t buffer[14];
    uint8_t days_per_month[]   = {31,29,31,30,31,30,31,31,30,31,30,31};
    tAESYS_MEP_SET_CMD command = { .code = htons(MEP_CLOCK), .offset = 0, .length = htons(6), .data = clock, };

    // Validating CLOCK values. Remember format is ymdhms
    if (clock == NULL || clock[1] == 0 || clock[1] > 12 || clock[2] == 0 ||
        clock[2] > days_per_month[clock[1]-1] || clock[3] > 24 || clock[4] > 59 || clock[5] > 59)
        return NULL;

    memcpy(&buffer[0],(uint8_t *)&command,8);
    memcpy(&buffer[8],clock,6);

    return createSendMEPFrame(type,0xFFFE,sizeof(buffer),trans_id,MEP_SET,buffer);
}
//---------------------------------------------------------------------

tAESYS_MEP_BUFFER * AesysMepBuildBrightnessMsg(uint8_t type, uint16_t trans_id, uint16_t values[])
{
    uint16_t offset = 8;
    uint8_t buffer[44] = {0};
    tAESYS_MEP_SET_CMD commands[] = {
                                       { .code = htons(MEP_CUSTOM_SET_BRIGHTNESS), .offset = 0, .length = 0       , .data = NULL, },
                                       { .code = htons(MEP_BRIGHTNESS_1)         , .offset = 0, .length = htons(1), .data = NULL, },
                                       { .code = htons(MEP_BRIGHTNESS_2)         , .offset = 0, .length = htons(1), .data = NULL, },
                                       { .code = htons(MEP_BRIGHTNESS_3)         , .offset = 0, .length = htons(1), .data = NULL, },
                                       { .code = htons(MEP_BRIGHTNESS_4)         , .offset = 0, .length = htons(1), .data = NULL, },
                                    };

    if (values == NULL)
        return NULL;

    memcpy(&buffer[0],(uint8_t *)&commands[0],8);
    for (uint16_t i = 0, j = 1; i < 4; i++, j++)
    {
         if (values[i] <= 255)
         {
             memcpy(&buffer[offset],(uint8_t *)&commands[j],8);
             buffer[offset+8] = values[i];
             offset += 9;
         }
    }

    if (offset == 8)
        return NULL;

    return createSendMEPFrame(type,0xFFFE,offset,trans_id,MEP_SET,buffer);
}
//---------------------------------------------------------------------

tAESYS_MEP_BUFFER * AesysMepBuildLastPublicationStatusMsg(uint8_t type, uint16_t trans_id, uint8_t status)
{
    uint8_t buffer[9];
    tAESYS_MEP_SET_CMD command = { .code = htons(MEP_REMEMBER_LAST_PUBLICATION), .offset = 0, .length = htons(1), .data = NULL, };

    memcpy(&buffer[0],(uint8_t *)&command,8);
    buffer[8] = (status) ? 1 : 0;

    return createSendMEPFrame(type,0xFFFE,sizeof(buffer),trans_id,MEP_SET,buffer);
}
//---------------------------------------------------------------------

tAESYS_MEP_BUFFER * AesysMepBuildDeviceIdMsg(uint8_t type, uint16_t trans_id, uint16_t size, const char *strId)
{
    uint8_t buffer[32] = {0};
    size = (size > 16) ? 16 : size;
    tAESYS_MEP_SET_CMD commands[] = {
                                       { .code = htons(MEP_CUSTOM_SET_DEVID), .offset = 0, .length = 0          , .data = NULL, },
                                       { .code = htons(MEP_DEVICE_ID)       , .offset = 0, .length = htons(size), .data = NULL, }
                                    };

    if (strId == NULL)
        return NULL;

    memcpy(&buffer[0],(uint8_t *)&commands[0],8);
    memcpy(&buffer[8],(uint8_t *)&commands[1],8);
    memcpy(&buffer[16],strId,size);

    return createSendMEPFrame(type,0xFFFE,sizeof(buffer),trans_id,MEP_SET,buffer);
}
//---------------------------------------------------------------------

tAESYS_MEP_BUFFER * AesysMepBuildDeviceDescMsg(uint8_t type, uint16_t trans_id, uint16_t size, const char *desc)
{
    uint8_t buffer[80] = {0};
    size = (size > 64) ? 64 : size;
    tAESYS_MEP_SET_CMD commands[] = {
                                       { .code = htons(MEP_CUSTOM_SET_DEVDESC), .offset = 0, .length = 0          , .data = NULL, },
                                       { .code = htons(MEP_DEVICE_DESCRIPTION), .offset = 0, .length = htons(size), .data = NULL, }
                                    };

    if (desc == NULL)
        return NULL;

    memcpy(&buffer[0],(uint8_t *)&commands[0],8);
    memcpy(&buffer[8],(uint8_t *)&commands[1],8);
    memcpy(&buffer[16],desc,size);

    return createSendMEPFrame(type,0xFFFE,sizeof(buffer),trans_id,MEP_SET,buffer);
}
//---------------------------------------------------------------------

tAESYS_MEP_BUFFER * AesysMepBuildTrafficLightStatusMsg(uint8_t type, uint16_t trans_id, uint16_t code, uint16_t value)
{
    uint8_t byte;
    uint8_t buffer[18];
    tAESYS_MEP_SET_CMD commands[] = {
                                       { .code = htons(MEP_CUSTOM_SET_TRAFFIC), .offset = 0, .length = 0       , .data = NULL },
                                       { .code = htons(code)                  , .offset = 0, .length = htons(2), .data = NULL }
                                    };

    if (code >= 0x814C && code <= 0x814F)
    {
        memcpy(&buffer[0],(uint8_t *)&commands[0],8);
        memcpy(&buffer[8],(uint8_t *)&commands[1],8);

        byte = (value >> 8) & 0x07;
        if (byte != 1 && byte != 2 && byte != 4)
            return NULL;

        buffer[16] = byte;

        byte = value & 0x07;
        if (byte != 1 && byte != 2 && byte != 4)
            return NULL;

        buffer[17] = byte;

        return createSendMEPFrame(type,0xFFFE,sizeof(buffer),trans_id,MEP_SET,buffer);
    }

    return NULL;
}
//---------------------------------------------------------------------

tAESYS_MEP_BUFFER * AesysMepBuildPictogramMsg(uint8_t type, uint16_t trans_id, uint8_t flashing_lamps, uint16_t picto_code)
{
    uint8_t buffer[42];
    tAESYS_MEP_VIS_EXT_PAGE page  = { .duration = 0x05, .params = 0x00, .type = 0x01, .size = htons(0x02), .page_def = 0x00, };
    tAESYS_MEP_SET_CMD commands[] = {
                                      { .code = htons(MEP_CUSTOM_SET_PICTO), .offset = 0        , .length = 0        , .data = NULL, }, // Id cmd
                                      { .code = htons(MEP_VIS_EXTENSIBLE)  , .offset = 0        , .length = 0        , .data = NULL, }, // Nice-start.
                                      { .code = htons(MEP_VIS_EXTENSIBLE)  , .offset = 0        , .length = htons(10), .data = NULL, }, // Text Msg data.
                                      { .code = htons(MEP_VIS_EXTENSIBLE)  , .offset = htonl(10), .length = 0        , .data = NULL, }, // Nice-end.
                                    };

    page.params = (flashing_lamps) ? 1 : 0;
    picto_code  = htons(picto_code);

    // Add the ID cmd, the GET command and VisExt Page
    memcpy(&buffer[0] ,(uint8_t *)&commands[0],8);
    memcpy(&buffer[8] ,(uint8_t *)&commands[1],8);
    memcpy(&buffer[16],(uint8_t *)&commands[2],8);
    memcpy(&buffer[24],"\x01\x00\x01"         ,3);
    memcpy(&buffer[27],(uint8_t *)&page       ,5);
    memcpy(&buffer[32],&picto_code            ,2);
    memcpy(&buffer[34],(uint8_t *)&commands[3],8);

    return createSendMEPFrame(type,0xFFFE,sizeof(buffer),trans_id,MEP_SET,buffer);
}
//---------------------------------------------------------------------

tAESYS_MEP_BUFFER * AesysMepBuildTextMsg(uint8_t type, uint16_t trans_id, uint8_t size, const tAESYS_MEP_MSG_DATA *msg, const tAESYS_MEP_PANEL_DATA *panel)
{
    int psize;
    uint16_t offset = 27;
    char vis_h[3] = {0x01,0x00,0x01};
    tAESYS_MEP_VIS_EXT_PAGE *pages = NULL;
    uint8_t buffer[K_MEP_MAX_DATA_SIZE] = {0};
    tAESYS_MEP_SET_CMD commands[]   = {
                                         { .code = htons(MEP_CUSTOM_SET_TEXT), .offset = 0, .length = 0, .data = NULL, }, // Id cmd
                                         { .code = htons(MEP_VIS_EXTENSIBLE) , .offset = 0, .length = 0, .data = NULL, }, // Nice-start.
                                         { .code = htons(MEP_VIS_EXTENSIBLE) , .offset = 0, .length = 0, .data = NULL, }, // Text Msg data.
                                         { .code = htons(MEP_VIS_EXTENSIBLE) , .offset = 0, .length = 0, .data = NULL, }, // Nice-end.
                                      };

    // Check for invalid parameters
    if (msg == NULL || panel == NULL || size == 0 || panel->font_size[0] == 0 || panel->font_size[1] == 0)
        return NULL;

    // Assign memory for every page
    pages = (tAESYS_MEP_VIS_EXT_PAGE *) calloc(size,sizeof(tAESYS_MEP_VIS_EXT_PAGE));
    if (pages == NULL)
        return NULL;

    vis_h[2] = size;
    memcpy(&buffer[0] ,(uint8_t *)&commands[0],8);
    memcpy(&buffer[8] ,(uint8_t *)&commands[1],8);
    memcpy(&buffer[24],vis_h,3);

    for (uint8_t p = 0; p < size; p++)
    {
         offset+= 5;
         if (msg[p].rows == NULL || msg[p].total_rows == 0)
             break;

         pages[p].duration = (msg[p].duration == 0) ? 1 : msg[p].duration;
         pages[p].params   = msg[p].parameters & 3;
         pages[p].type     = 0;

         if ((psize = addTextProperties(buffer,&offset,&msg[p],panel)) == -1)
             break;

         pages[p].size     = htons(psize);
         memcpy(&buffer[offset-psize-5],(uint8_t *)&pages[p],5);
    }

    free(pages);

    if (psize == -1 || offset+8 > K_MEP_MAX_DATA_SIZE)
        return NULL;

    commands[2].length = htons(offset-24);
    commands[3].offset = htonl(offset-24);

    // Add the VISEXT for MSG and nice-end
    memcpy(&buffer[16],(uint8_t *)&commands[2],8);
    memcpy(&buffer[offset] ,(uint8_t *)&commands[3],8);
    offset += 8;

    return createSendMEPFrame(type,0xFFFE,offset,trans_id,MEP_SET,buffer);
}
//---------------------------------------------------------------------
/**********************************************************************
*****                    Free resources section                   *****
**********************************************************************/

void AesysMepFreeBuffer(tAESYS_MEP_BUFFER *buffer)
{
    if (buffer != NULL)
    {
        if (buffer->data != NULL)
            free(buffer->data);

        free(buffer);
    }
}
//---------------------------------------------------------------------

void AesysMepFreeResponse(tAESYS_MEP_RESPONSE *response)
{
    tAESYS_MEP_RESPONSE_DATA *current = NULL;

    if (response != NULL)
    {
        while (response->data != NULL)
        {
            free(response->data->resp_data);
            current = response->data;
            response->data = response->data->next;
            free(current);
        }

        free(response);
    }
}
//---------------------------------------------------------------------
