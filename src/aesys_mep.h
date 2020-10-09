#ifndef AESYS_MEP_H
#define AESYS_MEP_H
//---------------------------------------------------------------------

/** @file aesys_mep.h
 *  @author Juan Carlos García Vázquez (gavajc)
 *  @date 2020-07-15
 *  @brief Function prototypes for manipulate Aesys MEP frames, create
 *         MEP messages and parse MEP responses.
 *
 *  The MEP Library was developed to facilitate interaction with Aesys
 *  devices that use the Modular Extensible Protocol (MEP).
 *
 *  The library has 3 types of functions:
 *
 *      - << DECODE >>
 *        Prototypes for manipulate Aesys MEP Frames and parse MEP commands.
 *        It's recommended to call one of the following functions before using
 *        any other defined in this library.
 *
 *              AesysMepDecodeUPTBFrame
 *              AesysMepCncopyPPTPFrame
 *
 *        These functions validate and create a copy of the MEP frame to be used
 *        with the defined structures in this library.
 *
 *      - << INFORMATION >>
 *        Prototypes for retrive device information like:
 *
 *              Temperature
 *              Humidity
 *              Brightness
 *              Hardware Info
 *              ...
 *
 *      - << MANIPULATION >>
 *        Prototypes for:
 *
 *              Show text.
 *              Show pictograms.
 *              Reset device
 *              Set Brightness
 *              ...
 */

#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
    #include <winsock2.h>
#else
    #include <arpa/inet.h>
#endif
//---------------------------------------------------------------------
/**********************************************************************
*****                     Definitions section                     *****
**********************************************************************/

#if defined(WIN32) || defined(_WIN32)
    #ifdef AESYS_MEP_EXPORTS
        #define AESYS_MEP_API __declspec(dllexport)
    #elif  AESYS_NO_SHARED
        #define AESYS_MEP_API
    #else
        #define AESYS_MEP_API __declspec(dllimport)
    #endif
    #define AESYS_MEP_CONV   __cdecl
#else
    #ifdef AESYS_MEP_EXPORTS
        #if __GNUC__ < 3
            #define AESYS_MEP_API
        #else
            #define AESYS_MEP_API __attribute__ ((visibility("default")))
        #endif
    #else
        #define AESYS_MEP_API
    #endif
    #define AESYS_MEP_CONV
#endif

#define K_MEP_MAX_FRAME_SIZE     0x4000
#define K_MEP_MAX_DATA_SIZE      0x1FF7
#define K_MEP_TEXT_COLORS_SIZE   0x0005
#define K_MEP_PANEL_ELEMENTS     0x0002
#define K_MEP_MIN_SIZE_UPTB      0x000D
#define K_MEP_MIN_SIZE_PPTB      0x0007
#define K_MEP_STX                0x0002
#define K_MEP_ETX                0x0003
#define K_MEP_DLE                0x0010

//---------------------------------------------------------------------
/**********************************************************************
*****                     Enumerations Section                    *****
**********************************************************************/

/// Represents the MEP frames types.
enum AESYS_MEP_FRAME_TYPES
{
    MEP_PPTP    = 0x00,   ///< Normal MEP PPTP Frame.
    MEP_UPTB          ,   ///< Normal MEP UoPTB Frame.
    MEP_UPTBNTX       ,   ///< UoPTB Frame but without STX/ETX bytes.


};

/// Represents the available MEP commands.
/// At this moment the gzip versions are not supported.
enum AESYS_MEP_COMMANDS {
    MEP_SET = 0x80,
    MEP_GET       ,
    MEP_DEL       ,
    MEP_DAT       ,
};

/// Represents the codes supported by a MEP command.
/// This enumeration only has the MEP codes supported by this library.
enum AESYS_MEP_CODES
{
    MEP_STATUS                     = 0x0000,
    MEP_HARDWARE_MODEL             = 0x0001,
    MEP_FIRMWARE_MODEL             = 0x0002,
    MEP_FIRMWARE_VERSION           = 0x0003,
    MEP_FIRMWARE_RELEASE           = 0x0004,
    MEP_FIRMWARE_DEVICE_TYPE       = 0x0005,
    MEP_DEVICE_ID                  = 0x000A,
    MEP_DEVICE_DESCRIPTION         = 0x000B,
    MEP_RESET                      = 0x0066,
    MEP_VIS_EXTENSIBLE             = 0x03EE,
    MEP_TEMP_1                     = 0x2711,
    MEP_TEMP_2                     = 0x2712,
    MEP_TEMP_3                     = 0x2713,
    MEP_TEMP_4                     = 0x2714,
    MEP_TEMP_5                     = 0x2715,
    MEP_TEMP_6                     = 0x2716,
    MEP_TEMP_7                     = 0x2717,
    MEP_TEMP_8                     = 0x2718,
    MEP_HUMIDITY_1                 = 0x2775,
    MEP_HUMIDITY_2                 = 0x2776,
    MEP_HUMIDITY_3                 = 0x2777,
    MEP_HUMIDITY_4                 = 0x2778,
    MEP_ENVIRONMENTAL_BRIGHTNESS_1 = 0x27D9,
    MEP_ENVIRONMENTAL_BRIGHTNESS_2 = 0x27DA,
    MEP_ENVIRONMENTAL_BRIGHTNESS_3 = 0x27DB,
    MEP_ENVIRONMENTAL_BRIGHTNESS_4 = 0x27DC,
    MEP_ENVIRONMENTAL_BRIGHTNESS_5 = 0x27DD,
    MEP_ENVIRONMENTAL_BRIGHTNESS_6 = 0x27DE,
    MEP_ENVIRONMENTAL_BRIGHTNESS_7 = 0x27DF,
    MEP_ENVIRONMENTAL_BRIGHTNESS_8 = 0x27E0,
    MEP_LED_BRIGHTNESS_OUTPUT      = 0x4E84,
    MEP_LED_OUTPUT_PERCENTAGE      = 0x4E85,
    MEP_DEVICE_RESTARTED           = 0x5209,
    MEP_DOORS_OPEN                 = 0x520A,
    MEP_INTERNAL_ERROR_CODE        = 0x5217,
    MEP_POWER_SAVING_STATUS        = 0x521C,
    MEP_BATTERY_LEVEL              = 0x521D,
    MEP_FANS_ACTIVE                = 0x55F1,
    MEP_HEATING_ACTIVE             = 0x55F2,
    MEP_SIREN_ACTIVE               = 0x55F3,
    MEP_BROKEN_FANS_NUMBER         = 0x59D9,
    MEP_BROKEN_LEDS_NUMBER         = 0x59DB,
    MEP_BROKEN_BACKLIGHTS_NUMBER   = 0x59DD,
    MEP_NUM_BROKEN_LED_BOARDS      = 0x59DF,
    MEP_CLOCK                      = 0x7531,
    MEP_COLORS_CALIBRATION         = 0x7562,
    MEP_REMEMBER_LAST_PUBLICATION  = 0x811A,
    MEP_BRIGHTNESS_1               = 0x7D01,
    MEP_BRIGHTNESS_2               = 0x7D02,
    MEP_BRIGHTNESS_3               = 0x7D03,
    MEP_BRIGHTNESS_4               = 0x7D04,
    MEP_TRAFFIC_LIGHT_STATUS_1     = 0x814C,
    MEP_TRAFFIC_LIGHT_STATUS_2     = 0x814D,
    MEP_TRAFFIC_LIGHT_STATUS_3     = 0x814E,
    MEP_TRAFFIC_LIGHT_STATUS_4     = 0x814F,
};

/// Represents custom response codes that group a set of MEP codes.
/// This codes are used when parse a response with function AesysMepParseResponse.
enum AESYS_MEP_CUSTOM_CODES
{
    MEP_CUSTOM_SET_TEXT              = 0xFDE8, ///< Returned in a response when detect is part of message created with AesysMepBuildTextMsg.
    MEP_CUSTOM_SET_PICTO             = 0xFDE9, ///< Returned in a response when detect is part of message created with AesysMepBuildPictogramMsg.
    MEP_CUSTOM_SET_DEVID             = 0xFDEA, ///< Returned in a response when detect is part of message created with AesysMepBuildDeviceIdMsg.
    MEP_CUSTOM_SET_TRAFFIC           = 0xFDEB, ///< Returned in a response when detect is part of message created with AesysMepBuildTrafficMsg.
    MEP_CUSTOM_SET_DEVDESC           = 0xFDEC, ///< Returned in a response when detect is part of message created with AesysMepBuildDeviceDescMsg.
    MEP_CUSTOM_SET_BRIGHTNESS        = 0xFDED, ///< Returned in a response when detect is part of message created with AesysMepBuildBrightnessMsg and set all codes.
    MEP_CUSTOM_CLEAR_PUB             = 0xFDEE, ///< Returned in a response when detect is part of message created with AesysMepBuildClearPublication.
    MEP_CUSTOM_DELETE_CODE           = 0xFDEF, ///< Returned in a response when detect is part of message created with AesysMepBuildDeleteCode.
    MEP_CUSTOM_STATUS_INFO_DATA      = 0xFDF0, ///< Returned in a response when detect is part of message created with AesysMepBuildDeviceInfoMsg.
    MEP_CUSTOM_DEVICE_INFO_DATA      = 0xFDF1, ///< Returned in a response when detect is part of message created with AesysMepBuildDevStatusInfoMsg.
    MEP_CUSTOM_TRAFFIC_INFO_DATA     = 0xFDF2, ///< Returned in a response when detect is part of message created with AesysMepBuildTrafficLightInfoMsg and retrieve all codes.
    MEP_CUSTOM_HUMIDITY_INFO_DATA    = 0xFDF3, ///< Returned in a response when detect is part of message created with AesysMepBuildHumidityInfoMsg and retrieve all codes.
    MEP_CUSTOM_BRIGHTNESS_INFO_DATA  = 0xFDF4, ///< Returned in a response when detect is part of message created with AesysMepBuildBrightnessInfoMsg and retrieve all codes.
    MEP_CUSTOM_DIAGNOSTIC_INFO_DATA  = 0xFDF5, ///< Returned in a response when detect is part of message created with AesysMepBuildDiagnosticInfoMsg.
    MEP_CUSTOM_TEMPERATURE_INFO_DATA = 0xFDF6, ///< Returned in a response when detect is part of message created with AesysMepBuildTempInfoMsg and retrieve all codes.
    MEP_CUSTOM_EBRIGHTNESS_INFO_DATA = 0xFDF7, ///< Returned in a response when detect is part of message created with AesysMepBuildEnvBrightnessInfoMsg and retrieve all codes.
};

/// Represents the available types in MEP protocol.
enum AESYS_MEP_TYPES
{
    MEP_VOID = 0x00,
    MEP_BOOL,
    MEP_INT8,
    MEP_INT16,
    MEP_INT32,
    MEP_UINT8,
    MEP_UINT16,
    MEP_UINT32,
    MEP_BINARY,
    MEP_STRING,
    MEP_USTRING,
};

/// Represents the available operations that a code can have.
enum AESYS_MEP_CODE_IO
{
    MEP_RD = 0x01,
    MEP_WR,
    MEP_RW,
};
//---------------------------------------------------------------------
/**********************************************************************
*****                      Structures Section                     *****
**********************************************************************/

/**
 *
 * @struct tAESYS_MEP_CODE_PROPERTIES
 * @brief  Represents the MEP code properties.
 */
typedef struct
{
    uint16_t code;      ///< The desired code to obtain information.
    uint8_t  nbne;      ///< Nice-begin, Nice-end support: 0: NOT SUPPORTED; 1: SUPPORTED
    uint8_t  iops:4,    ///< I/O opertion. i.e. R/W (Read/Write). 1: READ; 2: WRITE; 3: R/W
             type:4;    ///< MEP data type.
}tAESYS_MEP_CODE_PROPERTIES;

/**
 *
 * @struct tAESYS_MEP_PANEL_DATA
 * @brief  This structure must be filled with the correct panel information.
 *         The number of leds or pixels that the panel have horizontal and vertical.
 *         The number of leds or pixels used by the default font. Horizontal and vertical.
 *         The number of fans, back lights and led boards have.
 *         The operating temperature in °C.
 *
 *         Example:
 *                  -15 to 45 --> in ops_temp[0] = -15 ops_temp = 45
 *
 *         The maximum percentage of operating humidity. Range 0 to 100%
 */
typedef  struct
{
    uint8_t  fans;                             ///< The number of fans that device have.
    uint8_t  bck_lights;                       ///< The number of back lights that device have.
    uint16_t led_boards;                       ///< The number of led boards that device have.
    uint16_t ops_humidity;                     ///< The maximum percentage of operating humidity
    char ops_temp[K_MEP_PANEL_ELEMENTS];       ///< Is the operating temperature. First is for negative temperatures second for positives.
    uint16_t font_size[K_MEP_PANEL_ELEMENTS];  ///< Leds used by the font. The index 0 is the total leds used horizontal and the index 1 for vertical data.
    uint16_t panel_size[K_MEP_PANEL_ELEMENTS]; ///< Total Leds have the panel. The index 0 is the total leds horizontal and the index 1 for vertical data.
}tAESYS_MEP_PANEL_DATA;

/**
 *
 * @struct tAESYS_MEP_PPTP_FRAME
 * @brief  When use AesysMepCncopyPPTPFrame the return uint8_t pointer can be casted
 *         to this structure directly for represents all the frame data in a structured
 *         way. The uint8_t returned pointer was prepared as a PPTP format.
 */
typedef struct
{
    uint16_t dlen;     ///< The size of the payload
    uint16_t tran;     ///< The id transaction.
    uint8_t  cmd;      ///< The MEP command.
    uint8_t payload;   ///< MEP payload.
}tAESYS_MEP_PPTP_FRAME;

/**
 *
 * @struct tAESYS_MEP_UPTB_FRAME
 * @brief  When use AesysMepDecodeUPTBFrame the return uint8_t pointer can be casted
 *         to this structure directly for represents all the frame data in a structured
 *         way. The uint8_t returned pointer was prepared as a UoPTB format.
 */
typedef struct
{
    uint16_t crc;               ///< Cyclic redundancy check that validates the integrity of the MEP frame.
    uint16_t addr;              ///< The logic address to use. 0xFFFF to broadcast to all connected devices.
    tAESYS_MEP_PPTP_FRAME pptp; ///< The PPTP frame encapsulated into UoPTB frame.
}tAESYS_MEP_UPTB_FRAME;

/**
 *
 * @struct tAESYS_MEP_BUFFER
 * @brief  Used for prepare a MEP message that will be send to device.
 *         This structure must be freeing using the AesysMepFreeBuffer function
 */
typedef struct
{
    uint16_t size;     ///< The size of data field.
    uint8_t *data;     ///< Buffer that have a MEP msg to send.
}tAESYS_MEP_BUFFER;

/**
 *
 * @struct tAESYS_MEP_RESPONSE_DATA
 * @brief  Used when parse a response. This structure stores the data in a response.
 *         Some MEP codes not have data in a response. For check if all is correct
 *         check the "flag" memeber. When "flag" is 0 then the result is ok. The other
 *         possible values for memeber "flag" are:
 *
 *                   2 = offset does not exist
 *                   3 = cannot read/write
 *                   4 = wrong data length
 *                   5 = wrong data
 *                   6 = operation not expected
 *                   15 = generic error
 *
 *         Remember that "type" member in tAESYS_MEP_RESPONSE_DATA it's some MEP code
 *         defined in AESYS_MEP_CODES enumeration.
 *         This structure must be freeing when free tAESYS_MEP_RESPONSE using the AesysMepFreeResponse function.
 */
typedef struct resp_data
{
    uint8_t  type;          ///< Represents the MEP type for resp_data. See AESYS_MEP_TYPES enumeration.
    uint8_t  flag;          ///< If the code in a response have errors then will be distinct 0.
    uint16_t code;          ///< The MEP code. For more information see AESYS_MEP_CODES enumeration.
    uint16_t size;          ///< The size for the resp_data member.
    void *resp_data;        ///< The data in a response as a void pointer. Cast to correct type using "type" member.
    struct resp_data *next; ///< The next data structure in a response. NULL if there isn't more data.
}tAESYS_MEP_RESPONSE_DATA;

/**
 *
 * @struct tAESYS_MEP_RESPONSE
 * @brief  Used when parse a response. When have a set of MEP codes in a response
 *         and the codes were generated using a function defined in this library, then
 *         the "type" member will have some value defined in the AESYS_MEP_CUSTOM_CODES
 *         enumeration according to the corresponding type of value.
 *
 *         If response only have 1 code then the "type" member will have the same
 *         value as the memeber "type" defined in the struct tAESYS_MEP_RESPONSE_DATA.
 *
 *         If response have a set of codes not generated using this library, the "type"
 *         member will have the first code in the response.
 *
 *         Codes not supported by the device are ignored in the response. If only have
 *         1 response and the response is a code not supported then data will be NULL.
 *
 *         This structure must be freeing with using the AesysMepFreeResponse function.
 */
typedef struct
{
    uint16_t tran;                   ///< Represents the transaction id that response have.
    uint16_t type;                   ///< Indicates if the response is a set of codes. See enum AESYS_MEP_CUSTOM_CODES
    tAESYS_MEP_RESPONSE_DATA *data;  ///< The MEP response data. Cast to correct type acording to "type" member.
}tAESYS_MEP_RESPONSE;

#pragma pack(1)

/**
 *
 * @struct tAESYS_MEP_SET_CMD
 * @brief  For retrieve the SET command information from MEP payload.
 */
typedef struct
{
    uint16_t code;     ///< The MEP code to set
    uint32_t offset;   ///< The offset to use.
    uint16_t length;   ///< The length or size of the data to set.
    uint8_t  *data;    ///< The data or payload to set.
}tAESYS_MEP_SET_CMD;

/**
 *
 * @struct tAESYS_MEP_GET_CMD
 * @brief  For retrieve the GET command information from MEP payload.
 */
typedef struct
{
    uint16_t code;     ///< The MEP code to retrieve information
    uint32_t offset;   ///< The offset to use.
}tAESYS_MEP_GET_CMD;

/**
 *
 * @struct tAESYS_MEP_DAT_CMD
 * @brief  For retrieve the DAT command information from MEP frame payload.
 *         The status byte always is the first byte in MEP frame payload and is not
 *         stored in this struct. The status byte indicates if the DAT response
 *         have errors or not. Remember check it before use this structure.
 *
 *         Flags byte has other errors related to each MEP command executed.
 *         The error code uses the first 4 bits of flags. The possible errors are:
 *
 *              0 = no error
 *              1 = code does not exist
 *              2 = offset does not exist
 *              3 = cannot read/write
 *              4 = wrong data length
 *              5 = wrong data
 *              6 = operation not expected
 *              15 = generic error
 *
 *         If the bit 5 is set in flags the meaning is that more data exist beyond the requested index.
 */
typedef struct
{
    uint16_t code;     ///< The MEP code in the response.
    uint32_t offset;   ///< The offset used.
    uint8_t  flags;    ///< The status flag in a response. See the detailed description.
    uint16_t length;   ///< The size of the data.
    uint8_t  *data;    ///< The payload of the response.
}tAESYS_MEP_DAT_CMD;

/**
 *
 * @struct tAESYS_MEP_VIS_EXT_PAGE
 * @brief  Use for retrieve the VisExtensible page information from a VisExtensible payload.
 *         The params member can be configurated as:
 *
 *              bit 0: enable flashing lamps
 *              bit 1: set time unit for the page (0 seconds, 1 tenths (1/10) of seconds)
 *              bit 2: the page is part of an “extended publication”
 */
typedef struct
{
    uint8_t   duration;  ///< Duration time in a Vis Extensible Page
    uint8_t   params;    ///< Parameters in the page. See detailed description
    uint8_t   type;      ///< The type of the page. 0: By buffer 1: By code.
    uint16_t  size;      ///< The size of the pageDef. If use the function AesysMepReadNextVisExtData this field is in Network Order Byte.
    uint8_t   page_def;  ///< Payload. It's necessary cast it to uint8_t pointer for retrieve all data.
}tAESYS_MEP_VIS_EXT_PAGE;

/**
 *
 * @struct tAESYS_MEP_VIS_EXT_DATA
 * @brief  For retrieve the VisExtensible payload information from a MEP command.
 */
typedef struct
{
    uint8_t id;                      ///< VisExtensible page id.
    tAESYS_MEP_VIS_EXT_PAGE *page;   ///< Payload for the VisExtensible page.
}tAESYS_MEP_VIS_EXT_DATA;

#pragma pack(0)

/**
 *
 * @struct tAESYS_MEP_MSG_ROW
 * @brief  It is used to set the text properties it should have for each row in a MEP text message.
 *         The first byte is used to set only 1 color or to indicate not set the property.
 *
 *              0 = not set this property
 *              1 = red
 *              2 = green
 *              3 = blue
 *              4 = yellow
 *              5 = white
 *              6 = use RBG color
 *
 *         If the first byte is 6 and if the panel support RGB colors then use the next
 *         4 bytes for set and RGB color. The RGB color is represented as follow:
 *
 *              byte 1: RED
 *              byte 2: GREEN
 *              byte 3: BLUE
 *              byte 4: YELLOW
 *
 *         The range for each RGBY byte is: '0'-'F' as char; For example: 600F0 produces text in blue.
 */
typedef struct
{
    uint8_t *msg;             ///< Pointer to a valid buffer that contains a msg.
    uint16_t msg_size;        ///< Size of the msg.
    uint8_t col_spacing;      ///< Col spacing. The range is: 0-9
    uint8_t h_alignment;      ///< Horizontal alignment: 0=left; 1=center; 2=right;
    uint8_t compact_font;     ///< 0: not compact; 1: compact font; When is 1 the horizontal truncate and horizontal alignment are disabled.
    uint8_t scrolling_speed;  ///< 0: Not scrolling; 1: slow 9: fast.
    uint8_t custom_colors[K_MEP_TEXT_COLORS_SIZE]; ///< Only on supported panels. See description for more information.
}tAESYS_MEP_MSG_ROW;

/**
 *
 * @struct tAESYS_MEP_MSG_DATA
 * @brief  Represent the properties of the text in a MEP text message.
 */
typedef struct
{
    uint8_t  truncate;        ///< Require a valid PanelData struct info. If truncate is 1 & msg > maxcols or maxrows then truncate msg. See compactFont.
    uint8_t  duration;        ///< If set multi page, is the time to change between pages. By default it's in seconds. For change time unit to microseconds set params
    uint8_t  parameters;      ///< Bit 0 is for enable flashing lamps. Bit 1 to change the unit time for the page duration. 0 = seconds, 1 = tenths (1/10) of seconds
    uint8_t  blinking_text;   ///< 0: Not blinking. 1: blinking;
    uint8_t  antialiasing;    ///< Only on supported panel: 0 not antialiasing 9 max
    uint8_t  v_alignment;     ///< Vertical alignment: 0=top; 1=center; 2=bottom;
    uint8_t  row_spacing;     ///< Row spacing: 0-9
    uint16_t total_rows;      ///< The size of the structs array rows. i.e. Total rows.
    tAESYS_MEP_MSG_ROW *rows; ///< Pointer to struct that have all rows information.
}tAESYS_MEP_MSG_DATA;

//---------------------------------------------------------------------
/**********************************************************************
*****                  Decode functions section                   *****
**********************************************************************/

#ifdef __cplusplus
extern "C"{
#endif

/** @brief Return the properties of a valid MEP code.
 *
 * If code is a supported MEP code, then return a const structure of
 * type tAESYS_MEP_CODE_PROPERTIES with the code properties. If the code
 * is invalid or unsupported by this library then return NULL.
 *
 * @param  code The MEP code to get the properties.
 * @return Const pointer to tAESYS_MEP_CODE_PROPERTIES structure that have de code properties.
 */
AESYS_MEP_API const tAESYS_MEP_CODE_PROPERTIES * AESYS_MEP_CONV AesysMepGetCodeProperties(uint16_t code);

/** @brief Decode and validate UPTB frame and fill the corresponding
 *         structure.
 *
 * If the frame param is an invalid pointer (NULL) the function return NULL.
 * frame_size is the size of the frame and must be a valid size. If frame_size
 * is not valid then the result is undefined behavior.
 *
 * The function validate the correct structure of the MEP UoPTB frame,
 * i.e checking minimum size, validate the data size, cmd, crc and if have
 * the STX and ETX bytes. If all is correct then return a new buffer that
 * must be released by the developer.
 *
 * ATENTION:
 *
 * This function not support frames UoPTBNTX generated by the meessage functions
 * like AesysMepBuildDeviceInfoMsg with type 2. The UoPTBNTX frames not have
 * STX/ETX (Begin transmission/End transmission) bytes.
 *
 * The return value is unsigned char * pointer but can be casted
 * as pointer to (tAESYS_MEP_UPTB_FRAME *) structure which represents all the
 * frame data in a structured way. All data over 1 byte converted
 * from Network Order Byte to Host Order Byte.
 *
 * @param  frame     The frame to decode and validate.
 * @param  frame_size The size of the frame.
 * @return Pointer to new decoded frame. Must be released by the developer.
 */
AESYS_MEP_API uint8_t * AESYS_MEP_CONV AesysMepDecodeUPTBFrame(const uint8_t *frame, uint16_t frame_size);

/** @brief Validate and creates a new copy of PPTP frame and fill the
 *         corresponding structure.
 *
 * If the frame param is an invalid pointer (NULL) the function return NULL.
 * frame_size is the size of the frame and must be a valid size. If frame_size
 * is not valid then the result is undefined behavior.
 *
 * The function validate the correct structure of MEP PPTB frame,
 * i.e checking minimum size, validate data size, cmd. If all is correct
 * then return a new buffer that must be released by the developer.
 *
 * The return value is as unsigned char * pointer but can be casted
 * as pointer to (tAESYS_MEP_PPTP_FRAME *) structure which represents all the
 * frame data in a structured way. All data over 1 byte converted
 * from Network Order Byte to Host Order Byte.
 *
 * @param  frame     The frame to validate.
 * @param  frame_size The size of the frame.
 * @return Pointer to new frame. Must be released by the developer.
 */
AESYS_MEP_API uint8_t * AESYS_MEP_CONV AesysMepCncopyPPTPFrame(const uint8_t *frame, uint16_t frame_size);

/** @brief Validate and retrieve a DEL command from MEP Frame.
 *
 * Payload, offset and code must be valid pointers. If not are valid then
 * return -1. p_size is the size of the payload and must be a valid size.
 * If p_size is not valid the result is undefined behavior.
 *
 * The offset is used for save the current position in the payload. The first
 * time call this function the value must be 0. Internally it is modified
 * according to each reading of each DEL command. If offset becomes > p_size
 * then return -1.
 *
 * code it's a pointer to a statically assigned uint16_t for save each
 * DEL command in the payload. If the payload was created using the function
 * AesysMepDecodeUPTBFrame or AesysMepCncopyPPTPFrame, then the bytes
 * are in Host Order Byte.
 *
 * This function only retrieve the bytes used by DEL cmd not validates the codes
 * inside the DEL payload. If the payload isn't a DEL payload then the result is
 * undefined behavior.
 *
 * @param  payload Pointer to a valid payload that have one or more DEL commands.
 * @param  p_size   Is the size of the payload param.
 * @param  offset  Used for save the position in the payload. Must be start in 0.
 * @param  cmd     Pointer to a valid uint16_t var for save the DEL cmd properties.
 * @return -1 if an error occurred. 0 if no more data. 1 if have more DEL commands.
 */
AESYS_MEP_API char AESYS_MEP_CONV AesysMepReadNextDelCMD(uint8_t *payload, uint16_t p_size, uint16_t *offset, uint16_t *code);

/** @brief Validate and retrieve a SET command from MEP Frame.
 *
 * The cms structure param is a pointer to the payload param. If payload
 * becomes invalid then cmd will be invalid too.
 *
 * Payload, offset and cmd must be valid pointers. If not are valid then
 * return -1. p_size is the size of the payload and must be a valid size.
 * If p_size is not valid the result is undefined behavior.
 *
 * The offset is used for save the current position in the payload. The first
 * time call this function the value must be 0. Internally it is modified
 * according to each reading of each SET command. If offset becomes > p_size
 * then return -1.
 *
 * cmd it's a pointer to a statically assigned structure for save each
 * SET command in the payload. If the payload was created using the function
 * AesysMepDecodeUPTBFrame or AesysMepCncopyPPTPFrame, then the bytes
 * are in Host Order Byte.
 *
 * This function only retrieve the bytes used by SET cmd and validates the data size not overflow
 * but not validates the codes, lenght or data inside the tAESYS_MEP_SET_CMD structure. If the
 * payload isn't a SET payload then the result is undefined behavior.
 *
 * @param  payload Pointer to a valid payload that have one or more SET commands.
 * @param  p_size   Is the size of the payload param.
 * @param  offset  Used for save the position in the payload. Must be start in 0.
 * @param  cmd     Pointer to a valid struct for save the SET cmd properties.
 * @return -1 if an error occurred. 0 if no more data. 1 if have more SET commands.
 */
AESYS_MEP_API char AESYS_MEP_CONV AesysMepReadNextSetCMD(uint8_t *payload, uint16_t p_size, uint16_t *offset, tAESYS_MEP_SET_CMD *cmd);

/** @brief Validate and retrieve a GET command from MEP Frame.
 *
 * The cmd structure param is a pointer to the payload param. If payload
 * becomes invalid then cmd will be invalid too.
 *
 * Payload, offset and cmd must be valid pointers. If not are valid then
 * return -1. p_size is the size of the payload and must be a valid size.
 * If p_size is not valid the result is undefined behavior.
 *
 * The offset is used for save the current position in the payload. The first
 * time call this function the value must be 0. Internally it is modified
 * according to each reading of each GET command. If offset becomes > p_size
 * then return -1.
 *
 * cmd it's a pointer to a statically assigned structure for save each
 * GET command in the payload. If the payload was created using the function
 * AesysMepDecodeUPTBFrame or AesysMepCncopyPPTPFrame, then the bytes
 * are in Host Order Byte.
 *
 * This function only retrieve the bytes used by GET cmd and not validates the
 * codes, offset inside the tAESYS_MEP_GET_CMD structure. If the
 * payload isn't a GET payload then the result is undefined behavior.
 *
 * @param  payload Pointer to a valid payload that have one or more GET commands.
 * @param  p_size   Is the size of the payload param.
 * @param  offset  Used for save the position in the payload. Must be start in 0.
 * @param  cmd     Pointer to a valid struct for save the GET cmd properties.
 * @return -1 if an error occurred. 0 if no more data. 1 if have more GET commands.
 */
AESYS_MEP_API char AESYS_MEP_CONV AesysMepReadNextGetCMD(uint8_t *payload, uint16_t p_size, uint16_t *offset, tAESYS_MEP_GET_CMD *cmd);

/** @brief Validate and retrieve a DAT command from MEP Frame. This function
 *         not check the status byte in payload param.
 *
 * The cmd structure param is a pointer to the payload param. If payload
 * becomes invalid then cmd will be invalid too.
 *
 * Payload, offset and cmd must be valid pointers. If not are valid then
 * return -1. p_size is the size of the payload and must be a valid size.
 * If p_size is not valid the result is undefined behavior.
 *
 * The offset is used for save the current position in the payload. The first
 * time call this function the value must be 1. Internally it is modified
 * according to each reading of each DAT command. If offset becomes > p_size
 * then return -1.
 *
 * cmd it's a pointer to a statically assigned structure for save each
 * DAT command in the payload. If the payload was created using the function
 * AesysMepDecodeUPTBFrame or AesysMepCncopyPPTPFrame, then the bytes
 * are in Host Order Byte.
 *
 * This function only retrieve the bytes used by DAT cmd and not validates the
 * codes, offset, flags, lenght or data inside the tAESYS_MEP_DAT_CMD structure.
 * If the payload isn't a DAT payload then the result is undefined behavior.
 *
 * Atention: The first byte in the payload is the status byte. If the status byte
 *           is not 0, then represents an error in DAT command. This function doesn't
 *           verify the status byte. The offset must start at 1 to avoid the status byte.
 *
 * @param  payload Pointer to a valid payload that have one or more DAT commands.
 * @param  p_size  Is the size of the payload param.
 * @param  offset  Used for save the position in the payload. Must be start in 1.
 * @param  cmd     Pointer to a valid struct for save the DAT cmd properties.
 * @return -1 if an error occurred. 0 if no more data. 1 if have more DAT commands.
 */
AESYS_MEP_API char AESYS_MEP_CONV AesysMepReadNextDatCMD(uint8_t *payload, uint16_t p_size, uint16_t *offset, tAESYS_MEP_DAT_CMD *cmd);

/** @brief Checks and parse MEP frame for get the results of a previous MEP command(s).
 *
 * If the mep_frame param is an invalid pointer (NULL) the function return NULL.
 * frame_size is the size of the frame and must be a valid size. If frame_size is
 * not valid then the result is undefined behavior.
 *
 * The mep_frame must be as it is received from the device. Because it's validated
 * and decoded internally by this function using one of the following functions:
 *
 *      AesysMepDecodeUPTBFrame for UoPTB frames or
 *      AesysMepCncopyPPTPFrame for  PPTP frames.
 *
 * ATENTION:
 *
 * This function not support frames UoPTBNTX generated by the meessage functions
 * like AesysMepBuildDeviceInfoMsg with type 2. The UoPTBNTX frames not have
 * STX/ETX (Begin transmission/End transmission) bytes.
 *
 * Acording to the frame_type param. If all is correct then return a new
 * tAESYS_MEP_RESPONSE structure that must be freeing by the developer using
 * the function AesysMepFreeResponse.
 *
 * Each response data must be cast according to the type set in the "type" member
 * of the tAESYS_MEP_RESPONSE_DATA structured. The response types are defined
 * in AESYS_MEP_RESPONSE_CODES enumeration. Some devices not support all codes, when
 * a response have a not supported code, the code will be ignored. For more information
 * see the tAESYS_MEP_RESPONSE and tAESYS_MEP_RESPONSE_DATA descriptions.
 *
 * If the code in the response is not supported by this library the "type" member
 * in the struct tAESYS_MEP_RESPONSE_DATA will be set acording at member "size" in the
 * same struct.
 *
 * For example:
 *
 *      VISUALIZATION_TIMEOUT_SECS_1 is a MEP code with size 2 but isn't
 *      implemented in this library therefore it's unsupported code so the
 *      "type" member of the structure tAESYS_MEP_RESPONSE_DATA will be set
 *      according to its size. In this case MUINT16.
 *
 *      SIZES:     0          1           2         3 | > 4         4
 *      TYPES: MEP_VOID | MEP_UINT8 | MEP_UINT16 | MEP_BINARY | MEP_UINT32 |
 *
 * @param  mep_frame      A complete MEP frame that have a response. Must be as it is received by RX buffer.
 * @param  mep_frame_size The size of the mep_frame.
 * @param  mep_frame_type The type of mep_frame. 0: PPTP or 1: UPTB. See AESYS_MEP_FRAME_TYPES enum.
 * @return NULL on error an errno is set with specified error or pointer to a tAESYS_MEP_RESPONSE structure.
 */
AESYS_MEP_API tAESYS_MEP_RESPONSE * AESYS_MEP_CONV AesysMepParseResponse(uint8_t *mep_frame, uint16_t mep_frame_size, uint8_t mep_frame_type);

/** @brief Validate and retrieve a VisExt code data from MEP frame.
 *
 * The p_data structure param is a pointer to the payload param. If payload
 * becomes invalid then pData will be invalid too.
 *
 * This function is normally used with a SET command. Where the data in SET
 * command represents the payload.
 *
 * Payload, offset, elements and p_data must be valid pointers. If not are valid
 * then return -1. p_size is the size of the payload and must be a valid size.
 * If p_size is not valid the result is undefined behavior.
 *
 * The offset is used for save the current position in the payload. The first
 * time call this function the value must be 1. Internally it's modified
 * according to each reading of each VisExtData page. If offset becomes > p_size
 * then return -1.
 *
 * p_data it's a pointer to a statically assigned VisExtData structure for save
 * each VisExt page in the payload. VisExtData have a pointer to a VisExtPage
 * page structure. The VisExtPage have a uint16_t size. This structure field
 * is in Network Order Byte when use this function.
 *
 * If have an invalid structure or values then return -1.
 *
 * @param  payload  Pointer to a valid payload that have one or more VisExt pages.
 * @param  p_size   Is the size of the payload param.
 * @param  offset   Used for save the position in the payload. Must be start in 1.
 * @param  elements Pointer to a valid uint16_t var. Is used internally. Must be start in 0.
 * @param  p_data   Pointer to a valid struct for save the VisExtData properties.
 * @return -1 if an error occurred. 0 if no more data. 1 if have more DEL commands.
 */
AESYS_MEP_API char AESYS_MEP_CONV AesysMepReadNextVisExtData(uint8_t *payload, uint16_t p_size, uint16_t *offset, uint16_t *elements, tAESYS_MEP_VIS_EXT_DATA *p_data);

/**********************************************************************
*****               Information functions section                 *****
**********************************************************************/

/** @brief Build a MEP message for retrieve clock information.
 *
 * The available types are:
 *                         - 0: PPTP     frame
 *                         - 1: UoPTB    frame with STX and ETX bytes
 *                         - 2: UoPTBNTX frame without STX/ETX bytes
 *
 * If param type is > 2 or if occurs memory allocation error return NULL.
 * The return tAESYS_MEP_BUFFER must be freeing by developer using the
 * function AesysMepFreeBuffer.
 *
 * When parse a response using AesysMepParseResponse function, the "type" member in
 * tAESYS_MEP_RESPONSE structure will have the MEP code "CLOCK" defined in AESYS_MEP_CODES
 * enumeration. The same code will be present in member "type" in tAESYS_MEP_RESPONSE_DATA
 * structure. If the code is not supported by the device then "data" member in tAESYS_MEP_RESPONSE
 * structure will be NULL.
 *
 * @param  type     The type of tAESYS_MEP_BUFFER to construct.
 * @param  trans_id The transaction id to use. 0 for not set.
 * @return NULL if an error occurred or a pointer to a tAESYS_MEP_BUFFER structure allocated dynamically.
 */
AESYS_MEP_API tAESYS_MEP_BUFFER * AESYS_MEP_CONV AesysMepBuildClockInfoMsg(uint8_t type, uint16_t trans_id);

/** @brief Build a MEP message for retrieve device information.
 *
 * Device information to retrieve:
 *      - HARDWARE_MODEL
 *      - FIRMWARE_MODEL
 *      - FIRMWARE_VERSION
 *      - FIRMWARE_RELEASE
 *      - FIRMWARE_DEVICE_TYPE
 *      - DEVICE_ID
 *      - DEVICE_DESCRIPTION
 *
 * Keep in mind that some devices not support all information codes thus the DAT
 * response will have unknow code response for unsupported codes.
 * When use AesysMepParseResponse the unsupported codes are ignored and cannot be
 * found in the tAESYS_MEP_RESPONSE structure.
 *
 * The available types are:
 *                         - 0: PPTP     frame
 *                         - 1: UoPTB    frame with STX and ETX bytes
 *                         - 2: UoPTBNTX frame without STX/ETX bytes
 *
 * If param type is > 2 or if occurs memory allocation error return NULL.
 * The return tAESYS_MEP_BUFFER must be freeing by developer using the
 * function AesysMepFreeBuffer.
 *
 * When parse a response using AesysMepParseResponse function, the "type" member in
 * tAESYS_MEP_RESPONSE structure will have custom code "DEVICE_INFO_DATA" defined in
 * AESYS_MEP_CUSTOM_CODES enumeration. The member "type" in tAESYS_MEP_RESPONSE_DATA
 * structure will have the corresponding MEP code for each supported code. If some code
 * have an error then the member "flag" in tAESYS_MEP_RESPONSE_DATA structure will be
 * non-zero for that MEP code.
 *
 * @param  type     The type of tAESYS_MEP_BUFFER to construct.
 * @param  trans_id The transaction id to use. 0 for not set.
 * @return NULL if an error occurred or a pointer to a tAESYS_MEP_BUFFER structure allocated dynamically.
 */
AESYS_MEP_API tAESYS_MEP_BUFFER * AESYS_MEP_CONV AesysMepBuildDeviceInfoMsg(uint8_t type, uint16_t trans_id);

/** @brief Build a MEP message for retrieve device status information.
 *
 * The codes that are returned in the response depend on the firmware implementation on the device.
 * For example. Some devices may return:
 *
 *      TEMP_1
 *      TEMP_2
 *      ENV_BRIGHT_1
 *      ENV_BRIGHT_2
 *      ...
 *
 * Because return codes depend on the firmware implementation on the device, some
 * return codes are not implemented in the library but they will still be present
 * in the response structure tAESYS_MEP_RESPONSE.
 *
 * The available types are:
 *                         - 0: PPTP     frame
 *                         - 1: UoPTB    frame with STX and ETX bytes
 *                         - 2: UoPTBNTX frame without STX/ETX bytes
 *
 * When parse a response using AesysMepParseResponse function, the "type" member in
 * tAESYS_MEP_RESPONSE structure will have custom code "STATUS_INFO_DATA" defined in
 * AESYS_MEP_CUSTOM_CODES enumeration. The member "type" in tAESYS_MEP_RESPONSE_DATA
 * structure will have the corresponding MEP code for each supported code. If some code
 * have an error then the member "flag" in tAESYS_MEP_RESPONSE_DATA structure will be
 * non-zero for that MEP code.
 *
 * If param type is > 2 or if occurs memory allocation error return NULL.
 * The return tAESYS_MEP_BUFFER must be freeing by developer using the
 * function AesysMepFreeBuffer.
 *
 * @param  type     The type of tAESYS_MEP_BUFFER to construct.
 * @param  trans_id The transaction id to use. 0 for not set.
 * @return NULL if an error occurred or a pointer to a tAESYS_MEP_BUFFER structure allocated dynamically.
 */
AESYS_MEP_API tAESYS_MEP_BUFFER * AESYS_MEP_CONV AesysMepBuildDevStatusInfoMsg(uint8_t type, uint16_t trans_id);

/** @brief Build a MEP message for retrieve diagnostic information.
 *
 * Diagnostic information to retrieve:
 *
 *   - DOORS_OPEN
 *   - INTERNAL_ERROR_CODE
 *   - POWER_SAVING_STATUS
 *   - BATTERY_LEVEL
 *   - FANS_ACTIVE
 *   - HEATING_ACTIVE
 *   - SIREN_ACTIVE
 *   - BROKEN_FANS_NUMBER
 *   - BROKEN_LEDS_NUMBER
 *   - BROKEN_BACKLIGHTS_NUMBER
 *   - NUM_BROKEN_LED_BOARDS
 *
 * Keep in mind that some devices not support all diagnostic codes thus the DAT
 * response will have unknow code response for unsupported codes.
 *
 * When use AesysMepParseResponse the unsupported codes are ignored and cannot be
 * found in the tAESYS_MEP_RESPONSE struct
 *
 * For POWER_SAVING_STATUS the available options are:
 *
 *          0 = No power saving (AC connected)
 *          1 = No power saving (running on batteries)
 *          2 = Power saving level 1
 *          3 = Power saving level 2
 *          ...
 *
 * The available types are:
 *                         - 0: PPTP     frame
 *                         - 1: UoPTB    frame with STX and ETX bytes
 *                         - 2: UoPTBNTX frame without STX/ETX bytes
 *
 * If param type is > 2 or if occurs memory allocation error return NULL.
 * The return tAESYS_MEP_BUFFER must be freeing by developer using the
 * function AesysMepFreeBuffer.
 *
 * When parse a response using AesysMepParseResponse function, the "type" member in
 * tAESYS_MEP_RESPONSE struct will have custom code "DIAGNOSTIC_INFO_DATA" defined in
 * AESYS_MEP_CUSTOM_CODES enumeration. The member "type" in tAESYS_MEP_RESPONSE_DATA
 * structure will have the corresponding MEP code for each supported code. If some code
 * have an error then the member "flag" in tAESYS_MEP_RESPONSE_DATA structure will be
 * non-zero for that MEP code.
 *
 * @param  type     The type of tAESYS_MEP_BUFFER to construct.
 * @param  trans_id The transaction id to use. 0 for not set.
 * @return NULL if an error occurred or a pointer to a tAESYS_MEP_BUFFER structure allocated dynamically.
 */
AESYS_MEP_API tAESYS_MEP_BUFFER * AESYS_MEP_CONV AesysMepBuildDiagnosticInfoMsg(uint8_t type, uint16_t trans_id);

/** @brief Build a MEP message for retrieve device restarted information.
 *
 * The available types are:
 *                         - 0: PPTP     frame
 *                         - 1: UoPTB    frame with STX and ETX bytes
 *                         - 2: UoPTBNTX frame without STX/ETX bytes
 *
 * If param type is > 2 or if occurs memory allocation error return NULL.
 * The return tAESYS_MEP_BUFFER must be freeing by developer using the
 * function AesysMepFreeBuffer.
 *
 * When parse a response using AesysMepParseResponse function, the "type" member in
 * tAESYS_MEP_RESPONSE struct will have the MEP code "DEVICE_RESTARTED" defined in AESYS_MEP_CODES
 * enumeration. The same code will be present in member "type" in tAESYS_MEP_RESPONSE_DATA
 * structure. If the code is not supported by the device then "data" member in tAESYS_MEP_RESPONSE
 * structure will be NULL.
 *
 * @param  type     The type of tAESYS_MEP_BUFFER to construct.
 * @param  trans_id The transaction id to use. 0 for not set.
 * @return NULL if an error occurred or a pointer to a tAESYS_MEP_BUFFER structure allocated dynamically.
 */
AESYS_MEP_API tAESYS_MEP_BUFFER * AESYS_MEP_CONV AesysMepBuildDevRestartedInfoMsg(uint8_t type, uint16_t trans_id);

/** @brief Build a MEP message for retrieve Last publication status. Some devices
 *         not support this command.
 *
 * The available types are:
 *                         - 0: PPTP     frame
 *                         - 1: UoPTB    frame with STX and ETX bytes
 *                         - 2: UoPTBNTX frame without STX/ETX bytes
 *
 * If param type is > 2 or if occurs memory allocation error return NULL.
 * The return tAESYS_MEP_BUFFER must be freeing by developer using the
 * function AesysMepFreeBuffer.
 *
 * When parse a response using AesysMepParseResponse function, the "type" member in
 * tAESYS_MEP_RESPONSE structure will have the MEP code "REMEMBER_LAST_PUBLICATION" defined in
 * AESYS_MEP_CODES enumeration. The same code will be present in member "type" in
 * tAESYS_MEP_RESPONSE_DATA structre. If the code is not supported by the device then "data"
 * member in tAESYS_MEP_RESPONSE struct will be NULL.
 *
 * @param  type     The type of tAESYS_MEP_BUFFER to construct.
 * @param  trans_id The transaction id to use. 0 for not set.
 * @return NULL if an error occurred or a pointer to a tAESYS_MEP_BUFFER structure allocated dynamically.
 */
AESYS_MEP_API tAESYS_MEP_BUFFER * AESYS_MEP_CONV AesysMepBuildLastPublicationInfoMsg(uint8_t type, uint16_t trans_id);

/** @brief Build a MEP message for retrieve temperature information.
 *
 * The constructed message is for the specified temperature code specified in code param
 * or for all temperature codes specified in the MEP definition if param code
 * is 0. If select all temperature codes when parse response keep in mind that
 * some devices not support all temperature codes thus the DAT response will have
 * unknow code response for unsupported codes. If code param is not a valid TEMP code then
 * return NULL.
 *
 * When use AesysMepParseResponse the unsupported codes are ignored and cannot be
 * found in the tAESYS_MEP_RESPONSE structure.
 *
 * The available types are:
 *                         - 0: PPTP     frame
 *                         - 1: UoPTB    frame with STX and ETX bytes
 *                         - 2: UoPTBNTX frame without STX/ETX bytes
 *
 * If param type is > 2 or if occurs memory allocation error return NULL.
 * The return tAESYS_MEP_BUFFER must be freeing by developer using the
 * function AesysMepFreeBuffer.
 *
 * When parse a response using AesysMepParseResponse function, If all codes were requested
 * then the "type" member in tAESYS_MEP_RESPONSE struct will have custom code
 * "TEMPERATURE_INFO_DATA" defined in AESYS_MEP_CUSTOM_CODES enumeration. If only request 1
 * temperature code then "type" member will have the corresponding MEP code.The member "type"
 * in tAESYS_MEP_RESPONSE_DATA structure will have the corresponding MEP code for each
 * supported code. If some code have an error then the member "flag" in
 * tAESYS_MEP_RESPONSE_DATA structure will be non-zero for that MEP code.
 *
 * @param  type     The type of tAESYS_MEP_BUFFER to construct.
 * @param  trans_id The transaction id to use. 0 for not set.
 * @param  code     The temperature code to construct. If 0 retrieve all suported temperature codes.
 * @return NULL if an error occurred or a pointer to a tAESYS_MEP_BUFFER structure allocated dynamically.
 */
AESYS_MEP_API tAESYS_MEP_BUFFER * AESYS_MEP_CONV AesysMepBuildTempInfoMsg(uint8_t type, uint16_t trans_id, uint16_t code);

/** @brief Build a MEP message for retrieve humidity information.
 *
 * The constructed message is for the specified humidity code specified in code param
 * or for all humidity codes specified in the MEP definition if param code
 * is 0. If select all humidity codes, when parse response keep in mind that
 * some devices not support all humidity codes thus the DAT response will have
 * unknow code response for unsupported codes. If code param is not a valid HUMIDITY
 * code then return NULL.
 *
 * When use AesysMepParseResponse the unsupported codes are ignored and cannot be
 * found in the tAESYS_MEP_RESPONSE structure.
 *
 * The available types are:
 *                         - 0: PPTP     frame
 *                         - 1: UoPTB    frame with STX and ETX bytes
 *                         - 2: UoPTBNTX frame without STX/ETX bytes
 *
 * If param type is > 2 or if occurs memory allocation error return NULL.
 * The return tAESYS_MEP_BUFFER must be freeing by developer using the
 * function AesysMepFreeBuffer.
 *
 * When parse a response using AesysMepParseResponse function, If all codes were requested
 * then the "type" member in tAESYS_MEP_RESPONSE struct will have custom code
 * "HUMIDITY_INFO_DATA" defined in AESYS_MEP_CUSTOM_CODES enumeration. If only request 1
 * humidity code then "type" member will have the corresponding MEP code. The member "type"
 * in tAESYS_MEP_RESPONSE_DATA structure will have the corresponding MEP code for each
 * supported code. If some code have an error then the member "flag" in
 * tAESYS_MEP_RESPONSE_DATA structure will be non-zero for that MEP code.
 *
 *
 * @param  type     The type of tAESYS_MEP_BUFFER to construct.
 * @param  trans_id The transaction id to use. 0 for not set.
 * @param  code     The humidity code to construct. If 0 retrieve all suported humidity codes.
 * @return NULL if an error occurred or a pointer to a tAESYS_MEP_BUFFER structure allocated dynamically.
 */
AESYS_MEP_API tAESYS_MEP_BUFFER * AESYS_MEP_CONV AesysMepBuildHumidityInfoMsg(uint8_t type, uint16_t trans_id, uint16_t code);

/** @brief Build a MEP message for retrieve brightness information.
 *
 * The constructed message is for the specified brightness code specified in
 * code param or for all brightness codes specified in the MEP definition if param
 * code is 0. If select all brightness codes when parse a response keep in mind that
 * some devices not support all brightness codes thus the DAT response will have
 * unknow code response for unsupported codes. If code param is not a valid BRIGHTNESS
 * code then return NULL.
 *
 * When use AesysMepParseResponse the unsupported codes are ignored and cannot be
 * found in the tAESYS_MEP_RESPONSE structure.
 *
 * The available types are:
 *                         - 0: PPTP     frame
 *                         - 1: UoPTB    frame with STX and ETX bytes
 *                         - 2: UoPTBNTX frame without STX/ETX bytes
 *
 * If param type is > 2 or if occurs memory allocation error return NULL.
 * The return tAESYS_MEP_BUFFER must be freeing by developer using the
 * function AesysMepFreeBuffer.
 *
 * When parse a response using AesysMepParseResponse function, If all codes were requested
 * then the "type" member in tAESYS_MEP_RESPONSE struct will have custom code
 * "BRIGHTNESS_INFO_DATA" defined in AESYS_MEP_CUSTOM_CODES enumeration. If only request 1
 * brightness code then "type" member will have the corresponding MEP code. The member "type"
 * in tAESYS_MEP_RESPONSE_DATA structure will have the corresponding MEP code for each
 * supported code. If some code have an error then the member "flag" in
 * tAESYS_MEP_RESPONSE_DATA structure will be non-zero for that MEP code.
 *
 * @param  type     The type of tAESYS_MEP_BUFFER to construct.
 * @param  trans_id The transaction id to use. 0 for not set.
 * @param  code     The brightness code to construct. If 0 retrieve all suported brightness codes.
 * @return NULL if an error occurred or a pointer to a tAESYS_MEP_BUFFER structure allocated dynamically.
 */
AESYS_MEP_API tAESYS_MEP_BUFFER * AESYS_MEP_CONV AesysMepBuildBrightnessInfoMsg(uint8_t type, uint16_t trans_id, uint16_t code);

/** @brief Build a MEP message for retrieve trafficLights information.
 *
 * The constructed message is for the specified trafficLights code specified in code param
 * or for all trafficLights codes specified in the MEP definition if param code
 * is 0. If select all trafficLights codes, when parse response keep in mind that
 * some devices not support all trafficLights codes thus the DAT response will have
 * unknow code response for unsupported codes. If code param is not a valid
 * TRAFFIC LIGHT code then return NULL.
 *
 * When use AesysMepParseResponse the unsupported codes are ignored and cannot be
 * found in the tAESYS_MEP_RESPONSE structure.
 *
 * The available types are:
 *                         - 0: PPTP     frame
 *                         - 1: UoPTB    frame with STX and ETX bytes
 *                         - 2: UoPTBNTX frame without STX/ETX bytes
 *
 * If param type is > 2 or if occurs memory allocation error return NULL.
 * The return tAESYS_MEP_BUFFER must be freeing by developer using the
 * function AesysMepFreeBuffer.
 *
 * When parse a response using AesysMepParseResponse function, If all codes were requested
 * then the "type" member in tAESYS_MEP_RESPONSE struct will have custom code
 * "TRAFFIC_INFO_DATA" defined in AESYS_MEP_CUSTOM_CODES enumeration. If only request 1
 * traffic code then "type" member will have the corresponding MEP code. The member "type"
 * in tAESYS_MEP_RESPONSE_DATA structure will have the corresponding MEP code for each
 * supported code. If some code have an error then the member "flag" in
 * tAESYS_MEP_RESPONSE_DATA structure will be non-zero for that MEP code.
 *
 * @param  type     The type of tAESYS_MEP_BUFFER to construct.
 * @param  trans_id The transaction id to use. 0 for not set.
 * @param  code     The trafficLights code to construct. If 0 retrieve all suported traffic light codes.
 * @return NULL if an error occurred or a pointer to a tAESYS_MEP_BUFFER structure allocated dynamically.
 */
AESYS_MEP_API tAESYS_MEP_BUFFER * AESYS_MEP_CONV AesysMepBuildTrafficLightInfoMsg(uint8_t type, uint16_t trans_id, uint16_t code);

/** @brief Build a MEP message for retrieve EnvBrightness information.
 *
 * The constructed message is for the specified EnvBrightness code specified in code param
 * or for all EnvBrightness codes specified in the MEP definition if param code
 * is 0. If select all EnvBrightness codes, when parse response keep in mind that
 * some devices not support all EnvBrightness codes thus the DAT response will have
 * unknow code response for unsupported codes. If code param is not a valid
 * ENV BRIGHTNESS code then return NULL.
 *
 * When use AesysMepParseResponse the unsupported codes are ignored and cannot be
 * found in the tAESYS_MEP_RESPONSE structure.
 *
 * The available types are:
 *                         - 0: PPTP     frame
 *                         - 1: UoPTB    frame with STX and ETX bytes
 *                         - 2: UoPTBNTX frame without STX/ETX bytes
 *
 * If param type is > 2 or if occurs memory allocation error return NULL.
 * The return tAESYS_MEP_BUFFER must be freeing by developer using the
 * function AesysMepFreeBuffer.
 *
 * When parse a response using AesysMepParseResponse function, If all codes were requested
 * then the "type" member in tAESYS_MEP_RESPONSE struct will have custom code
 * "EBRIGHTNESS_INFO_DATA" defined in AESYS_MEP_CUSTOM_CODES enumeration. If only request 1
 * env brightness code then "type" member will have the corresponding MEP code. The member
 * "type" in tAESYS_MEP_RESPONSE_DATA structure will have the corresponding MEP code for each
 * supported code. If some code have an error then the member "flag" in
 * tAESYS_MEP_RESPONSE_DATA structure will be non-zero for that MEP code.
 *
 * @param  type     The type of tAESYS_MEP_BUFFER to construct.
 * @param  trans_id The transaction id to use. 0 for not set.
 * @param  code     The EnvBrightness code to construct. If 0 retrieve all suported env brightness codes.
 * @return NULL if an error occurred or a pointer to a tAESYS_MEP_BUFFER structure allocated dynamically.
 */
AESYS_MEP_API tAESYS_MEP_BUFFER * AESYS_MEP_CONV AesysMepBuildEnvBrightnessInfoMsg(uint8_t type, uint16_t trans_id, uint16_t code);

/**********************************************************************
*****               Manipulation functions section                *****
**********************************************************************/

/** @brief Build a MEP message for reset device.
 *
 * The available types are:
 *                         - 0: PPTP     frame
 *                         - 1: UoPTB    frame with STX and ETX bytes
 *                         - 2: UoPTBNTX frame without STX/ETX bytes
 *
 * If param type is > 2 or if occurs memory allocation error return NULL.
 * The return tAESYS_MEP_BUFFER must be freeing by developer using the
 * function AesysMepFreeBuffer.
 *
 * When parse a response using AesysMepParseResponse function, the "type" member in
 * tAESYS_MEP_RESPONSE structure will have the MEP code "RESET" defined in AESYS_MEP_CODES
 * enumeration. The same code will be present in member "type" in tAESYS_MEP_RESPONSE_DATA
 * structure. If the code is not supported by the device then "data" member in tAESYS_MEP_RESPONSE
 * structure will be NULL.
 *
 * @param  type     The type of tAESYS_MEP_BUFFER to construct.
 * @param  trans_id The transaction id to use. 0 for not set.
 * @return NULL if an error occurred or a pointer to a tAESYS_MEP_BUFFER structure allocated dynamically.
 */
AESYS_MEP_API tAESYS_MEP_BUFFER * AESYS_MEP_CONV AesysMepBuildResetDeviceMsg(uint8_t type, uint16_t trans_id);

/** @brief Build a MEP message for clear publication.
 *
 * The available types are:
 *                         - 0: PPTP     frame
 *                         - 1: UoPTB    frame with STX and ETX bytes
 *                         - 2: UoPTBNTX frame without STX/ETX bytes
 *
 * If param type is > 2 or occurs memory allocation error then return NULL. The return
 * tAESYS_MEP_BUFFER must be freeing by developer using the function AesysMepFreeBuffer.
 *
 * When parse a response using AesysMepParseResponse function, the "type" member in
 * tAESYS_MEP_RESPONSE structure will have the MEP code "CLEAR_PUB" defined in AESYS_MEP_CUSTOM_CODES
 * enumeration. The VIS_EXTENSIBLE code will be present in member "type" in tAESYS_MEP_RESPONSE_DATA
 * structure. If the code is not supported by the device then "data" member in tAESYS_MEP_RESPONSE
 * structure will be NULL.
 *
 * @param  type     The type of tAESYS_MEP_BUFFER to construct.
 * @param  trans_id The transaction id to use. 0 for not set.
 * @return NULL if an error occurred or a pointer to a tAESYS_MEP_BUFFER structure allocated dynamically.
 */
AESYS_MEP_API tAESYS_MEP_BUFFER * AESYS_MEP_CONV AesysMepBuildClearPublication(uint8_t type, uint16_t trans_id);

/** @brief Build a MEP message for delete code.
 *
 * The available types are:
 *                         - 0: PPTP     frame
 *                         - 1: UoPTB    frame with STX and ETX bytes
 *                         - 2: UoPTBNTX frame without STX/ETX bytes
 *
 * If param type is > 2 or if code not support the i/o WRITE operation or occurs memory allocation
 * error then return NULL. The return tAESYS_MEP_BUFFER must be freeing by developer using the
 * function AesysMepFreeBuffer.
 *
 * When parse a response using AesysMepParseResponse function, the "type" member in
 * tAESYS_MEP_RESPONSE structure will have the MEP code "DELETE_CODE" defined in AESYS_MEP_CUSTOM_CODES
 * enumeration. The deleted code will be present in member "type" in tAESYS_MEP_RESPONSE_DATA
 * structure. If the code is not supported by the device then "data" member in tAESYS_MEP_RESPONSE
 * structure will be NULL.
 *
 * @param  type     The type of tAESYS_MEP_BUFFER to construct.
 * @param  trans_id The transaction id to use. 0 for not set.
 * @param  code     The code to delete. The code must be support the operation WRITE. If not then error.
 * @return NULL if an error occurred or a pointer to a tAESYS_MEP_BUFFER structure allocated dynamically.
 */
AESYS_MEP_API tAESYS_MEP_BUFFER * AESYS_MEP_CONV AesysMepBuildDelCode(uint8_t type, uint16_t trans_id, uint16_t code);

/** @brief Build a MEP message for setting clock.
 *
 * The clock data structure is ymdhms. All elements as integers.
 *
 * WHERE:
 *
 *      y = year
 *      m = month
 *      d = day
 *      h = hour
 *      m = minutes
 *      s = seconds
 *
 * The available types are:
 *                         - 0: PPTP     frame
 *                         - 1: UoPTB    frame with STX and ETX bytes
 *                         - 2: UoPTBNTX frame without STX/ETX bytes
 *
 * If param type is > 2, clock is NULL or if occurs a memory allocation
 * error then return NULL. If length of param clock is < to 6 then the result
 * is undefined behavior.
 *
 * The return tAESYS_MEP_BUFFER must be freeing by developer using the
 * function AesysMepFreeBuffer.
 *
 * When parse a response using AesysMepParseResponse function, the "type" member in
 * tAESYS_MEP_RESPONSE structure will have the MEP code "CLOCK" defined in AESYS_MEP_CODES
 * enumeration. The same code will be present in member "type" in tAESYS_MEP_RESPONSE_DATA
 * structure. If the code is not supported by the device then "data" member in tAESYS_MEP_RESPONSE
 * structure will be NULL.
 *
 * @param   type    The type of tAESYS_MEP_BUFFER to construct.
 * @param  trans_id The transaction id to use. 0 for not set.
 * @param  clock    The clock to set as 6 bytes data with format ymdhms. See function description for more info.
 * @return NULL if an error occurred or a pointer to a tAESYS_MEP_BUFFER structure allocated dynamically.
 */
AESYS_MEP_API tAESYS_MEP_BUFFER * AESYS_MEP_CONV AesysMepBuildClockMsg(uint8_t type, uint16_t trans_id, uint8_t *clock);

/** @brief Build a MEP message for set the brightness in a device.
 *
 * The range for each brightness value is: 0-255
 *
 *      0   is interpeted as automatic brightness.
 *      255 maximun brightness.
 *
 * The size for array values always must be 4.
 *
 *      Starting at index 0 is for BRIGHTNESS_1
 *                  index 1 is for BRIGHTNESS_2
 *                  index 2 is for BRIGHTNESS_3
 *                  index 3 is for BRIGHTNESS_4
 *
 * If the size of array values is < 4 the result is undefined behavior.
 * If some value in values is > 255 then is interpreted as not set the code.
 * For Example:
 *
 *      values[0] = 256; not set  BRIGHTNESS_1 this code will not send to device.
 *      values[1] = 256; not set  BRIGHTNESS_2 this code will not send to device.
 *      values[2] =   0; auto for BRIGHTNESS_3
 *      values[3] = 255; set max  BRIGHTNESS_4
 *
 * Keep in mind that some devices not support all Brightness codes thus the DAT
 * response will have unknow code as response for unsupported codes. If use the
 * function AesysMepParseResponse, the response only returns the codes that were
 * set or error if some value could not be established for the selected supported
 * code or codes. Unsupported codes that are not established are not treated as
 * errors, they are simply ignored.
 *
 * When set 1 or more values the structure returned by AesysMepParseResponse
 * function set the type as SET_BRIGHTNESS. If some code have an error then the
 * member "flag" in tAESYS_MEP_RESPONSE_DATA structure will be non-zero for that
 * MEP code.
 *
 * The available types are:
 *                         - 0: PPTP     frame
 *                         - 1: UoPTB    frame with STX and ETX bytes
 *                         - 2: UoPTBNTX frame without STX/ETX bytes
 *
 * If param type is > 2 or if occurs memory allocation error return NULL.
 * The return tAESYS_MEP_BUFFER must be freeing by developer using the
 * function AesysMepFreeBuffer.
 *
 * @param  type     The type of tAESYS_MEP_BUFFER to construct.
 * @param  trans_id The transaction id to use. 0 for not set.
 * @param  values   Represents the brightness values to set as array of unsigned short. Always must be 4 size.
 * @return NULL if an error occurred or a pointer to a tAESYS_MEP_BUFFER structure allocated dynamically.
 */
AESYS_MEP_API tAESYS_MEP_BUFFER * AESYS_MEP_CONV AesysMepBuildBrightnessMsg(uint8_t type, uint16_t trans_id, uint16_t values[]);

/** @brief Build a MEP message for set the status for the last publication message. Some
 *         devices not support this functionality.
 *
 * The available values to set is 1: true or 0: false
 * If true then when device reboots the device loads the last publication. A publication
 * can be TEXT or a PICTOGRAM. Some devices not support this code. In that case the
 * unsupported code is returned when parse a response.
 *
 * When parse a response using AesysMepParseResponse function, the "type" member in
 * tAESYS_MEP_RESPONSE structure will have the MEP code "REMEMBER_LAST_PUBLICATION" defined
 * in AESYS_MEP_CODES enumeration. The same code will be present in member "type" in
 * tAESYS_MEP_RESPONSE_DATA structure. If the code is not supported by the device then
 * "data" member in tAESYS_MEP_RESPONSE structure will be NULL.
 *
 * The available types are:
 *                         - 0: PPTP     frame
 *                         - 1: UoPTB    frame with STX and ETX bytes
 *                         - 2: UoPTBNTX frame without STX/ETX bytes
 *
 * If param type is > 2 or if occurs memory allocation error return NULL.
 * The return tAESYS_MEP_BUFFER must be freeing by developer using the
 * function AesysMepFreeBuffer.
 *
 * @param  type     The type of tAESYS_MEP_BUFFER to construct.
 * @param  trans_id The transaction id to use. 0 for not set.
 * @param  status   1: Remember last publication when reboots. 0: Not remember.
 * @return NULL if an error occurred or a pointer to a tAESYS_MEP_BUFFER structure allocated dynamically.
 */
AESYS_MEP_API tAESYS_MEP_BUFFER * AESYS_MEP_CONV AesysMepBuildLastPublicationStatusMsg(uint8_t type, uint16_t trans_id, uint8_t status);

/** @brief Build a MEP message for set string id in the device.
 *
 * The available types are:
 *                         - 0: PPTP     frame
 *                         - 1: UoPTB    frame with STX and ETX bytes
 *                         - 2: UoPTBNTX frame without STX/ETX bytes
 *
 * If param type is > 2 or if param strId is NULL or if occurs memory allocation
 * error then return NULL. The return tAESYS_MEP_BUFFER must be freeing by developer using the
 * function AesysMepFreeBuffer.
 *
 * If the size for param id > 16 the the message is truncate to 16 bytes. If the "size" param is >
 * than the real size of param "strId" then the result is undefined behavior.
 *
 * When parse a response using AesysMepParseResponse function, the "type" member in
 * tAESYS_MEP_RESPONSE structure will have the MEP code "SET_DEVID" defined in AESYS_MEP_CODES
 * enumeration. The same code will be present in member "type" in tAESYS_MEP_RESPONSE_DATA
 * structure. If the code is not supported by the device then "data" member in tAESYS_MEP_RESPONSE
 * structure will be NULL.
 *
 * @param  type     The type of tAESYS_MEP_BUFFER to construct.
 * @param  trans_id The transaction id to use. 0 for not set.
 * @param  size     The size of the param strId. The max size supported is 16 bytes. If size is > 16 then truncate.
 * @param  strId    The string id to set in the device. Max lenght is 16 bytes. The end of string is ignored. Use param size for set the lenght.
 * @return NULL if an error occurred or a pointer to a tAESYS_MEP_BUFFER structure allocated dynamically.
 */
AESYS_MEP_API tAESYS_MEP_BUFFER * AESYS_MEP_CONV AesysMepBuildDeviceIdMsg(uint8_t type, uint16_t trans_id, uint16_t size, const char *strId);

/** @brief Build a MEP message for set device description in the device.
 *
 * The available types are:
 *                         - 0: PPTP     frame
 *                         - 1: UoPTB    frame with STX and ETX bytes
 *                         - 2: UoPTBNTX frame without STX/ETX bytes
 *
 * If param type is > 2 or if param description is NULL or if occurs memory allocation
 * error then return NULL. The return tAESYS_MEP_BUFFER must be freeing by developer using the
 * function AesysMepFreeBuffer.
 *
 * If the size for param "desc" > 64 the the message is truncate to 64 bytes. If the "size" param is >
 * than the real size of param "desc" then the result is undefined behavior.
 *
 * When parse a response using AesysMepParseResponse function, the "type" member in
 * tAESYS_MEP_RESPONSE structure will have the MEP code "SET_DEVDESC" defined in AESYS_MEP_CODES
 * enumeration. The same code will be present in member "type" in tAESYS_MEP_RESPONSE_DATA
 * structure. If the code is not supported by the device then "data" member in tAESYS_MEP_RESPONSE
 * structure will be NULL.
 *
 * @param  type     The type of tAESYS_MEP_BUFFER to construct.
 * @param  trans_id The transaction id to use. 0 for not set.
 * @param  size     The size of the param desc. The max size supported is 64 bytes. If size is > 64 then truncate.
 * @param  desc     The string description to set in the device. Max lenght is 64 bytes. The end of string is ignored. Use param size for set the lenght.
 * @return NULL if an error occurred or a pointer to a tAESYS_MEP_BUFFER structure allocated dynamically.
 */
AESYS_MEP_API tAESYS_MEP_BUFFER * AESYS_MEP_CONV AesysMepBuildDeviceDescMsg(uint8_t type, uint16_t trans_id, uint16_t size, const char *desc);

/** @brief Build a MEP message for set the Traffic Light status in a device. Some
 *         devices not support this functionality.
 *
 * The param value is set according to:
 *
 *      byte 0:
 *              bit 0 = red lamp
 *              bit 1 = orange lamp
 *              bit 2 = green lamp
 *      byte 1:
 *              bit 0 = red flashing
 *              bit 1 = orange flashing
 *              bit 2 = green flashing
 *
 * If 2 or more bits are set in each byte then is treated as error and return NULL.
 * The available types are:
 *                         - 0: PPTP     frame
 *                         - 1: UoPTB    frame with STX and ETX bytes
 *                         - 2: UoPTBNTX frame without STX/ETX bytes
 *
 * If param type is > 2 or if occurs memory allocation error return NULL.
 * The return tAESYS_MEP_BUFFER must be freeing by developer using the
 * function AesysMepFreeBuffer.
 *
 * When parse a response using AesysMepParseResponse function, the "type" member in
 * tAESYS_MEP_RESPONSE structure will have the MEP code "TRAFFIC_LIGHT_STATUS_X" defined
 * in AESYS_MEP_CODES enumeration. The X depends on the code sent in "code" param. Can be
 * 1 or 2 or 3 or 4. The same code will be present in member "type" in
 * tAESYS_MEP_RESPONSE_DATA structure. If the code is not supported by the device then
 * "data" member in tAESYS_MEP_RESPONSE structure will be NULL. On Error the member "flag"
 * in tAESYS_MEP_RESPONSE_DATA structure will be non-zero for that MEP code.
 *
 * @param  type     The type of tAESYS_MEP_BUFFER to construct.
 * @param  trans_id The transaction id to use. 0 for not set.
 * @param  code     The traffic light code to set
 * @param  value    Represents the values to set for the traffic light code.
 * @return NULL if an error occurred or a pointer to a tAESYS_MEP_BUFFER structure allocated dynamically.
 */
AESYS_MEP_API tAESYS_MEP_BUFFER * AESYS_MEP_CONV AesysMepBuildTrafficLightStatusMsg(uint8_t type, uint16_t trans_id, uint16_t code, uint16_t value);

/** @brief Build a MEP message for set a pictogram.
 *
 * The available types are:
 *                         - 0: PPTP     frame
 *                         - 1: UoPTB    frame with STX and ETX bytes
 *                         - 2: UoPTBNTX frame without STX/ETX bytes
 *
 * If param type is > 2 or if occurs memory allocation error return NULL.
 * The return tAESYS_MEP_BUFFER must be freeing by developer using the
 * function AesysMepFreeBuffer.
 *
 * When parse a response with AesysMepParseResponse function, the type memeber in the
 * returned struct tAESYS_MEP_RESPONSE will have SET_PICTO.
 *
 * @param  type           The type of tAESYS_MEP_BUFFER to construct.
 * @param  trans_id       The transaction id to use. 0 for not set.
 * @param  flashing_lamps 1 for flashing, 0 for not.
 * @param  picto_code     The id of the pictogram to set.
 * @return NULL if an error occurred or a pointer to a tAESYS_MEP_BUFFER structure allocated dynamically.
 */
AESYS_MEP_API tAESYS_MEP_BUFFER * AESYS_MEP_CONV AesysMepBuildPictogramMsg(uint8_t type, uint16_t trans_id, uint8_t flashing_lamps, uint16_t picto_code);

/** @brief Build a MEP message for set a single text or multi text.
 *
 * The params msg and panel must be valid pointers. If not are valid
 * then return NULL.
 *
 * If any element in msg param is out of range or have an incorrect value then is treated as
 * error and return NULL. See the comments in the tAESYS_MEP_MSG_DATA definition.
 *
 * The panel param have the information about panel. i.e. total elements horizontal
 * and vertical as well as the number of elements used by the font horizontal and
 * vertical. If the information provided is incorrect then the result is
 * undefined behavior. For example: The Aesys Display HG300 60*16
 *
 *      Have 60 Leds Horizontal and 16 Leds Vertical.
 *      The default font uses 5 Leds horizontal and 7 vertical.
 *      Both are Panel properties and must be present in tAESYS_MEP_PANEL_DATA structure.
 *
 * The available types are:
 *                         - 0: PPTP     frame
 *                         - 1: UoPTB    frame with STX and ETX bytes
 *                         - 2: UoPTBNTX frame without STX/ETX bytes
 *
 * If param type is > 2 or if occurs memory allocation error return NULL.
 * The return tAESYS_MEP_BUFFER must be freeing by developer using the
 * function AesysMepFreeBuffer.
 *
 * When parse a response with AesysMepParseResponse function, the type memeber in the
 * returned struct tAESYS_MEP_RESPONSE will have the value SET_TEXT.
 *
 * For set a multi text, then pass an array of tAESYS_MEP_MSG_DATA structures in msg param and
 * set param "size" with the total number of elements in msg.
 *
 * If you want to set only a single text then set size to 1 and "msg" param is a single element.
 * i.e. msg is an array of size 1: msg[1].
 *
 *
 * @param  type     The type of tAESYS_MEP_BUFFER to construct.
 * @param  trans_id The transaction id to use. 0 for not set.
 * @param  size     Is the size of msg structure. i.e. The number of elements that msg array have.
 * @param  msg      Array of structures that contains the text properties to apply.
 * @param  panel    Structure that contains the panel information to use.
 * @return NULL if an error occurred or a pointer to a tAESYS_MEP_BUFFER structure allocated dynamically.
 */
AESYS_MEP_API tAESYS_MEP_BUFFER * AESYS_MEP_CONV AesysMepBuildTextMsg(uint8_t type, uint16_t trans_id, uint8_t size, const tAESYS_MEP_MSG_DATA *msg, const tAESYS_MEP_PANEL_DATA *panel);
/**********************************************************************
*****               Free resources functions section              *****
**********************************************************************/

/** @brief Free resource used by tAESYS_MEP_BUFFER structure generated
 *         when use AesysMepBuildXXXMsg function.
 *
 * If buffer is NULL then do nothing.
 * If buffer is not a valid struct (tAESYS_MEP_BUFFER) the result
 * is undefined behavior.
 *
 * @param  buffer Pointer to tAESYS_MEP_BUFFER structure to free.
 * @return void
 */
AESYS_MEP_API void AESYS_MEP_CONV AesysMepFreeBuffer(tAESYS_MEP_BUFFER *buffer);

/** @brief Free resource used by tAESYS_MEP_RESPONSE structure generated
 *         when use AesysMepParseResponse function.
 *
 * If buffer is NULL then do nothing.
 * If buffer is not a valid struct (tAESYS_MEP_RESPONSE) the result
 * is undefined behavior.
 *
 * @param  buffer Pointer to tAESYS_MEP_RESPONSE structure to free.
 * @return void
 */
AESYS_MEP_API void AESYS_MEP_CONV AesysMepFreeResponse(tAESYS_MEP_RESPONSE *response);

#ifdef __cplusplus
}
#endif
//---------------------------------------------------------------------
#endif
