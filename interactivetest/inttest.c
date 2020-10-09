#include <string.h>
#include <stdio.h>

#include "aesys_mep.h"
#include "socknet.h"
//---------------------------------------------------------------------

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
    #define CLEARSCREEN "cls"
    #define SLEEP(SEC) (Sleep(SEC*1000))
#else
    #define CLEARSCREEN "clear"
    #define SLEEP(SEC) (sleep(SEC))
#endif
//---------------------------------------------------------------------

/**
 *
 * @struct tCODES_DESC
 * @brief  Represents the MEP code string description for each code and custom codes.
 */
typedef struct
{
    uint16_t code;
    char desc[30];
}
tCODES_DESC;
//---------------------------------------------------------------------

tCODES_DESC codes_names[] =
{
    { .code = MEP_STATUS                      ,  .desc = "STATUS"                     },
    { .code = MEP_HARDWARE_MODEL              ,  .desc = "HARDWARE_MODEL"             },
    { .code = MEP_FIRMWARE_MODEL              ,  .desc = "FIRMWARE_MODEL"             },
    { .code = MEP_FIRMWARE_VERSION            ,  .desc = "FIRMWARE_VERSION"           },
    { .code = MEP_FIRMWARE_RELEASE            ,  .desc = "FIRMWARE_RELEASE"           },
    { .code = MEP_FIRMWARE_DEVICE_TYPE        ,  .desc = "FIRMWARE_DEVICE_TYPE"       },
    { .code = MEP_DEVICE_ID                   ,  .desc = "DEVICE_ID"                  },
    { .code = MEP_DEVICE_DESCRIPTION          ,  .desc = "DEVICE_DESCRIPTION"         },
    { .code = MEP_RESET                       ,  .desc = "RESET"                      },
    { .code = MEP_VIS_EXTENSIBLE              ,  .desc = "VIS_EXTENSIBLE"             },
    { .code = MEP_TEMP_1                      ,  .desc = "TEMP_1"                     },
    { .code = MEP_TEMP_2                      ,  .desc = "TEMP_2"                     },
    { .code = MEP_TEMP_3                      ,  .desc = "TEMP_3"                     },
    { .code = MEP_TEMP_4                      ,  .desc = "TEMP_4"                     },
    { .code = MEP_TEMP_5                      ,  .desc = "TEMP_5"                     },
    { .code = MEP_TEMP_6                      ,  .desc = "TEMP_6"                     },
    { .code = MEP_TEMP_7                      ,  .desc = "TEMP_7"                     },
    { .code = MEP_TEMP_8                      ,  .desc = "TEMP_8"                     },
    { .code = MEP_HUMIDITY_1                  ,  .desc = "HUMIDITY_1"                 },
    { .code = MEP_HUMIDITY_2                  ,  .desc = "HUMIDITY_2"                 },
    { .code = MEP_HUMIDITY_3                  ,  .desc = "HUMIDITY_3"                 },
    { .code = MEP_HUMIDITY_4                  ,  .desc = "HUMIDITY_4"                 },
    { .code = MEP_ENVIRONMENTAL_BRIGHTNESS_1  ,  .desc = "ENVIRONMENTAL_BRIGHTNESS_1" },
    { .code = MEP_ENVIRONMENTAL_BRIGHTNESS_2  ,  .desc = "ENVIRONMENTAL_BRIGHTNESS_2" },
    { .code = MEP_ENVIRONMENTAL_BRIGHTNESS_3  ,  .desc = "ENVIRONMENTAL_BRIGHTNESS_3" },
    { .code = MEP_ENVIRONMENTAL_BRIGHTNESS_4  ,  .desc = "ENVIRONMENTAL_BRIGHTNESS_4" },
    { .code = MEP_ENVIRONMENTAL_BRIGHTNESS_5  ,  .desc = "ENVIRONMENTAL_BRIGHTNESS_5" },
    { .code = MEP_ENVIRONMENTAL_BRIGHTNESS_6  ,  .desc = "ENVIRONMENTAL_BRIGHTNESS_6" },
    { .code = MEP_ENVIRONMENTAL_BRIGHTNESS_7  ,  .desc = "ENVIRONMENTAL_BRIGHTNESS_7" },
    { .code = MEP_ENVIRONMENTAL_BRIGHTNESS_8  ,  .desc = "ENVIRONMENTAL_BRIGHTNESS_8" },
    { .code = MEP_LED_BRIGHTNESS_OUTPUT       ,  .desc = "LED_BRIGHTNESS_OUTPUT"      },
    { .code = MEP_LED_OUTPUT_PERCENTAGE       ,  .desc = "LED_OUTPUT_PERCENTAGE"      },
    { .code = MEP_DEVICE_RESTARTED            ,  .desc = "DEVICE_RESTARTED"           },
    { .code = MEP_DOORS_OPEN                  ,  .desc = "DOORS_OPEN"                 },
    { .code = MEP_INTERNAL_ERROR_CODE         ,  .desc = "INTERNAL_ERROR_CODE"        },
    { .code = MEP_POWER_SAVING_STATUS         ,  .desc = "POWER_SAVING_STATUS"        },
    { .code = MEP_BATTERY_LEVEL               ,  .desc = "BATTERY_LEVEL"              },
    { .code = MEP_FANS_ACTIVE                 ,  .desc = "FANS_ACTIVE"                },
    { .code = MEP_HEATING_ACTIVE              ,  .desc = "HEATING_ACTIVE"             },
    { .code = MEP_SIREN_ACTIVE                ,  .desc = "SIREN_ACTIVE"               },
    { .code = MEP_BROKEN_FANS_NUMBER          ,  .desc = "BROKEN_FANS_NUMBER"         },
    { .code = MEP_BROKEN_LEDS_NUMBER          ,  .desc = "BROKEN_LEDS_NUMBER"         },
    { .code = MEP_BROKEN_BACKLIGHTS_NUMBER    ,  .desc = "BROKEN_BACKLIGHTS_NUMBER"   },
    { .code = MEP_NUM_BROKEN_LED_BOARDS       ,  .desc = "NUM_BROKEN_LED_BOARDS"      },
    { .code = MEP_CLOCK                       ,  .desc = "CLOCK"                      },
    { .code = MEP_COLORS_CALIBRATION          ,  .desc = "COLORS_CALIBRATION"         },
    { .code = MEP_REMEMBER_LAST_PUBLICATION   ,  .desc = "REMEMBER_LAST_PUBLICATION"  },
    { .code = MEP_BRIGHTNESS_1                ,  .desc = "BRIGHTNESS_1"               },
    { .code = MEP_BRIGHTNESS_2                ,  .desc = "BRIGHTNESS_2"               },
    { .code = MEP_BRIGHTNESS_3                ,  .desc = "BRIGHTNESS_3"               },
    { .code = MEP_BRIGHTNESS_4                ,  .desc = "BRIGHTNESS_4"               },
    { .code = MEP_TRAFFIC_LIGHT_STATUS_1      ,  .desc = "TRAFFIC_LIGHT_STATUS_1"     },
    { .code = MEP_TRAFFIC_LIGHT_STATUS_2      ,  .desc = "TRAFFIC_LIGHT_STATUS_2"     },
    { .code = MEP_TRAFFIC_LIGHT_STATUS_3      ,  .desc = "TRAFFIC_LIGHT_STATUS_3"     },
    { .code = MEP_TRAFFIC_LIGHT_STATUS_4      ,  .desc = "TRAFFIC_LIGHT_STATUS_4"     },
    { .code = MEP_CUSTOM_SET_TEXT             ,  .desc = "SET_TEXT"                   },
    { .code = MEP_CUSTOM_SET_PICTO            ,  .desc = "SET_PICTO"                  },
    { .code = MEP_CUSTOM_SET_DEVID            ,  .desc = "SET_DEVID"                  },
    { .code = MEP_CUSTOM_SET_TRAFFIC          ,  .desc = "SET_TRAFFIC"                },
    { .code = MEP_CUSTOM_SET_DEVDESC          ,  .desc = "SET_DEVDESC"                },
    { .code = MEP_CUSTOM_SET_BRIGHTNESS       ,  .desc = "SET_BRIGHTNESS"             },
    { .code = MEP_CUSTOM_DELETE_CODE          ,  .desc = "DELETE_CODE"                },
    { .code = MEP_CUSTOM_CLEAR_PUB            ,  .desc = "CLEAR_PUB"                  },
    { .code = MEP_CUSTOM_STATUS_INFO_DATA     ,  .desc = "STATUS_INFO_DATA"           },
    { .code = MEP_CUSTOM_DEVICE_INFO_DATA     ,  .desc = "DEVICE_INFO_DATA"           },
    { .code = MEP_CUSTOM_TRAFFIC_INFO_DATA    ,  .desc = "TRAFFIC_INFO_DATA"          },
    { .code = MEP_CUSTOM_HUMIDITY_INFO_DATA   ,  .desc = "HUMIDITY_INFO_DATA"         },
    { .code = MEP_CUSTOM_BRIGHTNESS_INFO_DATA ,  .desc = "BRIGHTNESS_INFO_DATA"       },
    { .code = MEP_CUSTOM_DIAGNOSTIC_INFO_DATA ,  .desc = "DIAGNOSTIC_INFO_DATA"       },
    { .code = MEP_CUSTOM_TEMPERATURE_INFO_DATA,  .desc = "TEMPERATURE_INFO_DATA"      },
    { .code = MEP_CUSTOM_EBRIGHTNESS_INFO_DATA,  .desc = "EBRIGHTNESS_INFO_DATA"      },
};
//--------------------------------------------------------------------

/** @brief Return the code description of a valid MEP code.
 *
 * @param  code The MEP code to get the string description.
 * @return const string with code description.
 */
const char * find_code_desc(uint16_t code)
{
    uint16_t size = sizeof(codes_names)/sizeof(tCODES_DESC);

    for (uint16_t i = 0; i < size; i++)
         if (code == codes_names[i].code)
             return codes_names[i].desc;

    return NULL;
}
//---------------------------------------------------------------------

/** @brief Prepare and build simple or multitext message.
 *
 * @param  str Is the message introduced by the user and converted to a simple message.
 * @return The build text msg or NULL on error.
 */
tAESYS_MEP_BUFFER * PrepareText(uint8_t type, uint8_t *str)
{
    uint8_t pages = (str != NULL) ? 1 : 4;
    char *texts[] = { "Hi", "Kapsch", "Friends", "Greetings", "From", "Kapsch", "Mexico"};
    tAESYS_MEP_PANEL_DATA panel = { .font_size = {0x5,0x07}, .panel_size = {0x3C,0x10}, };
    tAESYS_MEP_MSG_ROW rows[7] =
    {
       { .msg = (uint8_t *)texts[0], .msg_size = strlen(texts[0]), .h_alignment = 1, .col_spacing = 1, .compact_font = 0, .custom_colors[0] = 0, .scrolling_speed = 0 },
       { .msg = (uint8_t *)texts[1], .msg_size = strlen(texts[1]), .h_alignment = 1, .col_spacing = 1, .compact_font = 0, .custom_colors[0] = 0, .scrolling_speed = 0 },
       { .msg = (uint8_t *)texts[2], .msg_size = strlen(texts[2]), .h_alignment = 1, .col_spacing = 1, .compact_font = 0, .custom_colors[0] = 0, .scrolling_speed = 0 },
       { .msg = (uint8_t *)texts[3], .msg_size = strlen(texts[3]), .h_alignment = 1, .col_spacing = 1, .compact_font = 0, .custom_colors[0] = 0, .scrolling_speed = 0 },
       { .msg = (uint8_t *)texts[4], .msg_size = strlen(texts[4]), .h_alignment = 1, .col_spacing = 1, .compact_font = 0, .custom_colors[0] = 0, .scrolling_speed = 0 },
       { .msg = (uint8_t *)texts[5], .msg_size = strlen(texts[5]), .h_alignment = 1, .col_spacing = 1, .compact_font = 0, .custom_colors[0] = 0, .scrolling_speed = 0 },
       { .msg = (uint8_t *)texts[6], .msg_size = strlen(texts[6]), .h_alignment = 1, .col_spacing = 1, .compact_font = 0, .custom_colors[0] = 0, .scrolling_speed = 0 },
    };
    tAESYS_MEP_MSG_DATA msg[4] =
    {
       { .total_rows = 1, .rows = &rows[0], .row_spacing = 1, .v_alignment = 1, .truncate = 1, .parameters = 0, .duration = 1, .antialiasing = 0, .blinking_text = 0 },
       { .total_rows = 2, .rows = &rows[1], .row_spacing = 1, .v_alignment = 1, .truncate = 1, .parameters = 0, .duration = 1, .antialiasing = 0, .blinking_text = 0 },
       { .total_rows = 2, .rows = &rows[3], .row_spacing = 1, .v_alignment = 1, .truncate = 1, .parameters = 0, .duration = 1, .antialiasing = 0, .blinking_text = 0 },
       { .total_rows = 2, .rows = &rows[5], .row_spacing = 1, .v_alignment = 1, .truncate = 1, .parameters = 0, .duration = 1, .antialiasing = 0, .blinking_text = 0 },
    };

    if (str != NULL)
    {
        rows[0].msg = str;
        rows[0].msg_size = strlen((char *)str);
    }

    return AesysMepBuildTextMsg(type, 19, pages, msg, &panel);
}
//---------------------------------------------------------------------

/** @brief Print the response recieved by Aesys device.
 *
 * @param  response The mep resposne recieved from Aesys device to print.
 * @return void
 */
void PrintResponse(tAESYS_MEP_RESPONSE *response)
{
    if (response)
    {
        tAESYS_MEP_RESPONSE_DATA *current = response->data;
        const char *desc = find_code_desc(response->type);

        printf("Response. Transaction ID: 0x%.2X Response Type: ",response->tran);

        if (desc != NULL)
            printf("%s\n",desc);
        else
            printf("0x%.2X\n",response->type);

        if (current == NULL)
            printf("Operation is not supported by device.\n");

        while (current != NULL)
        {
            desc = find_code_desc(current->code);
            if (desc != NULL)
                printf("\nMEP CODE: %s\n",desc);
            else
                printf("\nMEP CODE: 0x%.2X\n",current->code);

            if (current->flag)
                printf("Error retrieving value data. FLAG CODE: %d\n",current->flag);
            else
            {
                if ((response->type >= 0xFDE8 && response->type <= 0xFDEF) || (response->type == MEP_RESET) || ((
                     response->type == MEP_CLOCK || response->type == MEP_REMEMBER_LAST_PUBLICATION) && !current->size))
                    printf("RESULT: Code established correctly\n");
                else
                {
                    if (!current->size)
                        printf("VALUE: NULL\n");
                    else
                    {
                        switch(current->type)
                        {
                            case MEP_VOID:
                                    printf("VALUE: NULL\n");
                            break;
                            case MEP_BOOL:
                                    printf("VALUE: %d\n",(*(uint8_t *)current->resp_data) ? 1 : 0);
                            break;
                            case MEP_INT8:
                                    printf("VALUE: %d\n",(*(char *)current->resp_data));
                            break;
                            case MEP_INT16:
                                    printf("VALUE: %d\n",(*(short *)current->resp_data));
                            break;
                            case MEP_INT32:
                                    printf("VALUE: %d\n",(*(int *)current->resp_data));
                            break;
                            case MEP_UINT8:
                                    if (response->type == MEP_CUSTOM_BRIGHTNESS_INFO_DATA && ((*(uint8_t *)current->resp_data)) == 0)
                                        printf("VALUE: AUTO\n");
                                    else
                                        printf("VALUE: %d\n",(*(uint8_t *)current->resp_data));
                            break;
                            case MEP_UINT16:
                                    printf("VALUE: %d\n",(*(uint16_t *)current->resp_data));
                            break;
                            case MEP_UINT32:
                                    printf("VALUE: %d\n",(*(uint32_t *)current->resp_data));
                            break;
                            case MEP_BINARY:
                            {
                                    uint8_t *ptr = (uint8_t *)current->resp_data;

                                    if (current->code == MEP_CLOCK)
                                    {
                                        printf("VALUE: In format(ymdhms) -->  ");
                                        for (unsigned i = 0; i < current->size; i++)
                                        {
                                             printf("%d",ptr[i]);

                                             if (i < 2)                printf("/");
                                             else if (i > 1 && i < 5)  printf(":");
                                        }
                                        printf("\n");
                                    }
                                    else
                                    {
                                        printf("VALUE: ");
                                        for (unsigned i = 0; i < current->size; i++)
                                             printf("0x%.2X ",ptr[i]);
                                        printf("\n");
                                    }
                            }
                            break;
                            case MEP_STRING:
                                    printf("VALUE: %.*s\n",current->size,(char *)current->resp_data);
                            break;
                            case MEP_USTRING:
                                    printf("VALUE: %ls\n",(wchar_t *)current->resp_data);
                            break;
                        }
                    }
                }
            }

            current = current->next;
        }
    }
}
//---------------------------------------------------------------------

/** @brief Sends a MEP message to Aesys device and wait for response.
 *
 * @param  socket     The socket Id to use for send a message.
 * @param  msg_buffer The MEP message to send.
 * @return void.
 */
void SendAndParseResponse(int socket, uint8_t type, tAESYS_MEP_BUFFER *msg_buffer)
{
    int bytes;
    tAESYS_MEP_RESPONSE *response = NULL;
    uint8_t rx_buffer[K_MEP_MAX_FRAME_SIZE+2];

    if (msg_buffer != NULL)
    {
        if (send(socket, (const char *)msg_buffer->data, msg_buffer->size, 0) < 0)
            printf("Error sending command to device\n");
        else
        {
            bytes = recv(socket, (char *) rx_buffer, sizeof (rx_buffer), 0);

            if (bytes > 0)
            {
                response = AesysMepParseResponse(rx_buffer,bytes,type);
                if (response != NULL)
                    PrintResponse(response);
                else
                    printf("#### Error decoding response. %s ####\n",strerror(errno));

                AesysMepFreeResponse(response);
            }
            else if (bytes < 0)  printf("Error recieving data\n");
            else if (bytes == 0) printf("Data response lenght is 0\n");
        }
    }
    else
        printf("Error generating message. Invalid values.\n");
}
//---------------------------------------------------------------------

int main(int argc, char **argv)
{
    int  socket          = 0;
    int  option          = 0;
    char msg_type        = (argc > 1) ? atoi(argv[1]) : 1;
    char exec_all        = 0;
    char pub_status      = 0;
    char input[256]      = {0};
    uint16_t t_code      = MEP_TRAFFIC_LIGHT_STATUS_1;
    uint16_t traffic     = 0x0201;
    uint16_t del_code    = 10;
    uint16_t picto_id    = 0;
    uint16_t bright[4]   = {0,255,0,0};
    tAESYS_MEP_BUFFER *msg_buffer = NULL;
    uint8_t clock_dat[6] = {0x14,0x07,0x16,0x17,0x15,0x32};
    const char *address  = (argc > 2) ? ((const char *)argv[2]) : "148.198.229.177";

    printf("Configuring library for messages of type %d. Connecting with Aesys device: %s\n",msg_type,address);
    socket = netIPV4Connect(address,SOCK_STREAM,5600);

    if (socket == -1) {
        printf("Error connecting with the device. Aborting ....\n");
        return -1;
    }

    do
    {
        printf("Options availables:\n"
               "                         0.-  Exit\n"
               "                         1.-  Get device clock information\n"
               "                         2.-  Get device information\n"
               "                         3.-  Get device status information\n"
               "                         4.-  Get device diagnostic information\n"
               "                         5.-  Get device restarted status\n"
               "                         6.-  Get device last publication status\n"
               "                         7.-  Get device temperature information\n"
               "                         8.-  Get device humidity information\n"
               "                         9.-  Get device brightness information\n"
               "                         10.- Get device traffic lights information\n"
               "                         11.- Get device environment brightness information\n"
               "                         12.- Set device string ID\n"
               "                         13.- Set device description\n"
               "                         14.- Set device clock data\n"
               "                         15.- Set brightness information\n"
               "                         16.- Set last publication status\n"
               "                         17.- Set traffic light data\n"
               "                         18.- Set pictogram\n"
               "                         19.- Set text\n"
               "                         20.- Del code\n"
               "                         21.- Clear publication\n"
               "                         22.- Reset device\n"
               "                         23.- Execute All\n\n"
               "                         Please select an option: "
              );

        fgets(input,255,stdin);
        printf("\n");
        option    = atoi(input);
        exec_all  = 0;

        if (option == 23) {
            option   = 1;
            exec_all = 1;
        }

        switch (option)
        {
            case 0:  printf("#### Terminating application ... ####\n"); break;
            default: printf("#### Invalid option %d ####\n",option);     break;
            case 1: // Get device clock information

                msg_buffer = AesysMepBuildClockInfoMsg(msg_type,option);
                SendAndParseResponse(socket,msg_type,msg_buffer);
                AesysMepFreeBuffer(msg_buffer);

                if (!exec_all)
                    break;

                printf("\n#### Preparing next test. Please wait... ####\n");
                SLEEP(3);

                /* fall through */
            case 2: // Get device information

                msg_buffer = AesysMepBuildDeviceInfoMsg(msg_type,option);
                SendAndParseResponse(socket,msg_type,msg_buffer);
                AesysMepFreeBuffer(msg_buffer);

                if (!exec_all)
                    break;

                printf("\n#### Preparing next test. Please wait... ####\n");
                SLEEP(3);

                /* fall through */
            case 3: // Get device status information

                msg_buffer = AesysMepBuildDevStatusInfoMsg(msg_type,option);
                SendAndParseResponse(socket,msg_type,msg_buffer);
                AesysMepFreeBuffer(msg_buffer);

                if (!exec_all)
                    break;

                printf("\n#### Preparing next test. Please wait... ####\n");
                SLEEP(3);

                /* fall through */
            case 4: // Get device diagnostic information

                msg_buffer = AesysMepBuildDiagnosticInfoMsg(msg_type,option);
                SendAndParseResponse(socket,msg_type,msg_buffer);
                AesysMepFreeBuffer(msg_buffer);

                if (!exec_all)
                    break;

                printf("\n#### Preparing next test. Please wait... ####\n");
                SLEEP(3);

                /* fall through */
            case 5: // Get device restarted status

                msg_buffer = AesysMepBuildDevRestartedInfoMsg(msg_type,option);
                SendAndParseResponse(socket,msg_type,msg_buffer);
                AesysMepFreeBuffer(msg_buffer);

                if (!exec_all)
                    break;

                printf("\n#### Preparing next test. Please wait... ####\n");
                SLEEP(3);

                /* fall through */
            case 6: // Get device last publication status

                msg_buffer = AesysMepBuildLastPublicationInfoMsg(msg_type,option);
                SendAndParseResponse(socket,msg_type,msg_buffer);
                AesysMepFreeBuffer(msg_buffer);

                if (!exec_all)
                    break;

                printf("\n#### Preparing next test. Please wait... ####\n");
                SLEEP(3);

                /* fall through */
            case 7: // Get device temperature information

                msg_buffer = AesysMepBuildTempInfoMsg(msg_type,option,0);
                SendAndParseResponse(socket,msg_type,msg_buffer);
                AesysMepFreeBuffer(msg_buffer);

                if (!exec_all)
                    break;

                printf("\n#### Preparing next test. Please wait... ####\n");
                SLEEP(3);

                /* fall through */
            case 8: // Get device humidity information

                msg_buffer = AesysMepBuildHumidityInfoMsg(msg_type,option,0);
                SendAndParseResponse(socket,msg_type,msg_buffer);
                AesysMepFreeBuffer(msg_buffer);

                if (!exec_all)
                    break;

                printf("\n#### Preparing next test. Please wait... ####\n");
                SLEEP(3);

                /* fall through */
            case 9: // Get device brightness information

                msg_buffer = AesysMepBuildBrightnessInfoMsg(msg_type,option,0);
                SendAndParseResponse(socket,msg_type,msg_buffer);
                AesysMepFreeBuffer(msg_buffer);

                if (!exec_all)
                    break;

                printf("\n#### Preparing next test. Please wait... ####\n");
                SLEEP(3);

                /* fall through */
            case 10: // Get device traffic lights information

                msg_buffer = AesysMepBuildTrafficLightInfoMsg(msg_type,option,0);
                SendAndParseResponse(socket,msg_type,msg_buffer);
                AesysMepFreeBuffer(msg_buffer);

                if (!exec_all)
                    break;

                printf("\n#### Preparing next test. Please wait... ####\n");
                SLEEP(3);

                /* fall through */
            case 11: // Get device environment brightness information

                msg_buffer = AesysMepBuildEnvBrightnessInfoMsg(msg_type,option,0);
                SendAndParseResponse(socket,msg_type,msg_buffer);
                AesysMepFreeBuffer(msg_buffer);

                if (!exec_all)
                    break;

                printf("\n#### Preparing next test. Please wait... ####\n");
                SLEEP(3);

                /* fall through */
            case 12: // Set device string id

                if (!exec_all)
                {
                    printf("Please enter the device string id: ");
                    fgets(input,255,stdin);
                }
                else
                    sprintf(input,"AE-HG300-01-CH");

                msg_buffer = AesysMepBuildDeviceIdMsg(msg_type,option,strlen(input),input);
                SendAndParseResponse(socket,msg_type,msg_buffer);
                AesysMepFreeBuffer(msg_buffer);

                if (!exec_all)
                    break;

                printf("\n#### Preparing next test. Please wait... ####\n");
                SLEEP(3);

                /* fall through */
            case 13: // Set device description

                if (!exec_all)
                {
                    printf("Please enter the device description: ");
                    fgets(input,255,stdin);
                }
                else
                    sprintf(input,"HG300 60X16 Aesys Chile");

                msg_buffer = AesysMepBuildDeviceDescMsg(msg_type,option,strlen(input),input);
                SendAndParseResponse(socket,msg_type,msg_buffer);
                AesysMepFreeBuffer(msg_buffer);

                if (!exec_all)
                    break;

                printf("\n#### Preparing next test. Please wait... ####\n");
                SLEEP(3);

                /* fall through */
            case 14: // Set device clock data

                if (!exec_all)
                {
                    printf("Please enter clock data to set as (y-m-d-h-m-s): ");
                    fgets(input,255,stdin);
                    for (uint16_t i = 0, j = 0, k = 0, s = strlen(input); i <= s; i++)
                    {
                         if (input[i] == '-' || input[i] == '\0')
                         {
                             input[i]     = 0;
                             clock_dat[k] = atoi(&input[j]);
                             j = i+1;
                             k++;
                         }
                    }
                }

                printf("\n #### SETTING CLOCK DATA: ");
                for (uint16_t i = 0; i < 6; i++)
                {
                     printf("%d",clock_dat[i]);
                     if (i < 5)
                         printf("-");
                }

                printf(" ####\n");
                msg_buffer = AesysMepBuildClockMsg(msg_type,option,clock_dat);
                SendAndParseResponse(socket,msg_type,msg_buffer);
                AesysMepFreeBuffer(msg_buffer);
                memcpy(clock_dat,"\x14\x07\x16\x17\x15\x32",6);

                if (!exec_all)
                    break;

                printf("\n#### Preparing next test. Please wait... ####\n");
                SLEEP(3);

                /* fall through */
            case 15: // Set brightness information

                if (!exec_all)
                {
                    printf("Please enter brightness data to set as (0,255,0,256): ");
                    fgets(input,255,stdin);
                    for (uint16_t i = 0, j = 0, k = 0, s = strlen(input); i <= s; i++)
                    {
                         if (input[i] == ',' || input[i] == '\0')
                         {
                             input[i]     = 0;
                             bright[k] = atoi(&input[j]);
                             j = i+1;
                             k++;
                         }
                    }
                }

                printf("\n #### SETTING BRIGHTNESS DATA: ");
                for (uint16_t i = 0; i < 4; i++)
                {
                     printf("%d",bright[i]);
                     if (i < 3)
                         printf(",");
                }

                printf(" ####\n");
                msg_buffer = AesysMepBuildBrightnessMsg(msg_type,option,bright);
                SendAndParseResponse(socket,msg_type,msg_buffer);
                AesysMepFreeBuffer(msg_buffer);

                if (!exec_all)
                    break;

                printf("\n#### Preparing next test. Please wait... ####\n");
                SLEEP(3);

                /* fall through */
            case 16: // Set last publication status

                if (!exec_all)
                {
                    printf("Please enter status to set for last publication (0 or 1): ");
                    fgets(input,255,stdin);
                    pub_status = (input[0] == '0') ? 0 : 1;
                }

                printf("\n #### SETTING LAST PUBLICATION STATUS: %d ####\n",pub_status);
                msg_buffer = AesysMepBuildLastPublicationStatusMsg(msg_type,option,pub_status);
                SendAndParseResponse(socket,msg_type,msg_buffer);
                AesysMepFreeBuffer(msg_buffer);

                if (!exec_all)
                    break;

                printf("\n#### Preparing next test. Please wait... ####\n");
                SLEEP(3);

                /* fall through */
            case 17: // Set traffic light data

                if (!exec_all)
                {
                    printf("Please enter code to set data traffic (code,value): ");
                    fgets(input,255,stdin);
                    for (uint16_t i = 0; input[i] != 0; i++)
                    {
                         if (input[i] == ',')
                         {
                             input[i] = 0;
                             t_code  = atoi(&input[0]);
                             if (AesysMepGetCodeProperties(t_code) == NULL)
                                 t_code = MEP_TRAFFIC_LIGHT_STATUS_1;
                             else
                                 if (t_code != MEP_TRAFFIC_LIGHT_STATUS_1 && t_code != MEP_TRAFFIC_LIGHT_STATUS_2 &&
                                     t_code != MEP_TRAFFIC_LIGHT_STATUS_3 && t_code != MEP_TRAFFIC_LIGHT_STATUS_4)
                                     t_code  = MEP_TRAFFIC_LIGHT_STATUS_1;
                             i++;
                             if (input[i] != 0)
                                 traffic = atoi(&input[i]);

                             break;
                         }
                    }
                }

                printf("\n #### SETTING TRAFFIC LIGHT DATA FRO CODE: 0x%.2X  WITH VALUE: 0x%.2X ####\n",t_code,traffic);
                msg_buffer = AesysMepBuildTrafficLightStatusMsg(msg_type,option,t_code,traffic);
                SendAndParseResponse(socket,msg_type,msg_buffer);
                AesysMepFreeBuffer(msg_buffer);

                t_code  = MEP_TRAFFIC_LIGHT_STATUS_1;
                traffic = 0x0201;

                if (!exec_all)
                    break;

                printf("\n#### Preparing next test. Please wait... ####\n");
                SLEEP(3);

                /* fall through */
            case 18: // Set pictogram

                if (!exec_all)
                {
                    printf("Please enter the pictogram id to set: ");
                    fgets(input,255,stdin);
                    picto_id = atoi(input);
                }

                printf("\n #### SETTING PICTOGRAM WITH ID: %d ####\n",picto_id);
                msg_buffer = AesysMepBuildPictogramMsg(msg_type,option,0,picto_id);
                SendAndParseResponse(socket,msg_type,msg_buffer);
                AesysMepFreeBuffer(msg_buffer);

                picto_id   = 0;

                if (!exec_all)
                    break;

                printf("\n#### Preparing next test. Please wait... ####\n");
                SLEEP(3);

                /* fall through */
            case 19: // Set text

                if (!exec_all)
                {
                    printf("Please enter the text to set: ");
                    fgets(input,255,stdin);
                    msg_buffer = PrepareText(msg_type,(uint8_t*)input);
                    printf("\n #### SETTING TEXT: %s ####\n",input);
                }
                else {
                    msg_buffer = PrepareText(msg_type,NULL);
                    printf("\n #### SETTING TEXT: TAG INVALIDO ####\n");
                }

                SendAndParseResponse(socket,msg_type,msg_buffer);
                AesysMepFreeBuffer(msg_buffer);

                if (!exec_all)
                    break;

                printf("\n#### Preparing next test. Please wait... ####\n");
                SLEEP(3);

                /* fall through */
            case 20: // Delete code

                if (!exec_all)
                {
                    printf("Please enter the code to delete (remember code must support write I/O): ");
                    fgets(input,255,stdin);
                    del_code = atoi(input);
                }

                msg_buffer = AesysMepBuildDelCode(msg_type,option,del_code);
                SendAndParseResponse(socket,msg_type,msg_buffer);
                AesysMepFreeBuffer(msg_buffer);
                del_code = 10;

                if (!exec_all)
                    break;

                printf("\n#### Preparing next test. Please wait... ####\n");
                SLEEP(3);

                /* fall through */
            case 21: // Clear Publication

                msg_buffer = AesysMepBuildClearPublication(msg_type,option);
                SendAndParseResponse(socket,msg_type,msg_buffer);
                AesysMepFreeBuffer(msg_buffer);

                if (!exec_all)
                    break;

                printf("\n#### Preparing next test. Please wait... ####\n");
                SLEEP(3);

                /* fall through */
            case 22: // Reset device

                msg_buffer = AesysMepBuildResetDeviceMsg(msg_type,option);
                SendAndParseResponse(socket,msg_type,msg_buffer);
                AesysMepFreeBuffer(msg_buffer);
                option = 0;

            break;
        }

        printf("\nPess any key to continue ...\n");
        getchar();
        system(CLEARSCREEN);
    }
    while(option != 0);

    close(socket);

    return 0;
}
//---------------------------------------------------------------------
