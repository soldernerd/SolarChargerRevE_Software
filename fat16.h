/* 
 * File:   fat16.h
 * Author: Luke
 *
 * Created on 30. October 2017, 21:59
 */

#ifndef FAT16_H
#define	FAT16_H

/******************************************************************************
 * The drive is organized as follows:                                         *
 *   - There are 8192 sectors of 512 bytes each, numbered from 0 to 8191      *
 *   - There is one partition, occupying the entire space except the MBR      *
 *   - Sector 0:        (1 sector)      Master boot record (MBR)              *
 *   - Sector 1:        (1 sector)      First boot record (FBR)               *
 *   - Sectors 2-33:    (32 sectors)    File allocation table (FAT)           *
 *   - Sectors 34-37:   (4 sectors)     Root directory                        *
 *   - Sectors 38-8191: (8154 sectors)  Data
 ******************************************************************************/

//General drive layout
#define MBR_SECTOR 0
#define FBR_SECTOR 1
#define FAT_FIRST_SECTOR 2
#define FAT_LAST_SECTOR 33
#define FAT_MINIMUM_VALUE 2
#define FAT_MAXIMUM_VALUE 8153
#define ROOT_FIRST_SECTOR 34
#define ROOT_LAST_SECTOR 37
#define DATA_FIRST_SECTOR 38
#define DATA_LAST_SECTOR 8191
#define DATA_NUMBER_OF_SECTORS 8154
#define BYTES_PER_SECTOR 512
#define CLUSTERS_PER_FAT_SECTOR 256

//MBR specifics
#define MRB_PARTITION_STATUS 0x80
#define MBR_PARTITION_TYPE 0x04
#define MBR_PARTITION_SIZE 8191
#define MBR_FIRST_PARTITION_SECTOR 1
#define MBR_PARTITION_START_CYLINDER 0
#define MBR_PARTITION_START_HEAD 0
#define MBR_PARTITION_START_SECTOR 2
#define MBR_LAST_PARTITION_SECTOR 8191
#define MBR_PARTITION_END_CYLINDER 8
#define MBR_PARTITION_END_HEAD 2
#define MBR_PARTITION_END_SECTOR 3
#define MBR_SIGNATURE 0X55AA

//FBR specifics
#define FBR_OEM_IDENTIFIER "MSDOS5.0"
#define FBR_BYTES_PER_SECTOR 512
#define FBR_SECTORS_PER_CLUSTER 1
#define FBR_RESERVED_SECTORS 1
#define FBR_NUMBER_OF_FATS 1
#define FBR_ROOT_ENTRIES 64
#define FBR_NUMBER_OF_SECTORS 8191
#define FBR_MEDIA_DESCRIPTOR 0XF8
#define FBR_SECTORS_PER_FAT 32
#define FBR_SECTORS_PER_HEAD 63
#define FBR_HEADS_PER_CYLINDER 16
#define FBR_HIDDEN_SECTORS 0
#define FBR_EXT_FLAGS 0b00000001
#define FBR_ROOT_DIRECTORY_START 34
#define FBR_SIGNATURE 0X55AA

//A hello world file
#define ROOT_DRIVE_NAME "SolarChargr"
#define ROOT_FILE_NAME "FILE    "
#define ROOT_FILE_EXTENSION "TXT"
#define ROOT_FILE_CONTENT "hello world!"
#define ROOT_FILE_SIZE 12
#define ROOT_FILE_FIRST_CLUSTER 2

typedef struct
{
    char fileName[8];
    char fileExtension[3];
    uint8_t attributes;
    uint8_t reserved1;
    uint8_t secondFractions;
    uint16_t createdTime;
    uint16_t createdDate;
    uint16_t lastAccessDate;
    uint16_t reserved2;
    uint16_t modifiedTime;
    uint16_t modifiedDate;
    uint16_t firstCluster;
    uint32_t fileSize;
} rootEntry_t;

typedef enum 
{ 
    DRIVE_NOT_FORMATED = 0x00,
    DRIVE_FORMATED = 0x01       
} formatStatus_t;

void fat_init(void);
formatStatus_t fat_get_format_status(void);
uint8_t fat_format(void);
uint8_t fat_find_file(char *name, char *extension);
uint8_t fat_get_file_information(uint8_t file_number, rootEntry_t *data);
uint32_t fat_get_file_size(uint8_t file_number);
uint8_t fat_create_file(char *name, char *extension, uint32_t size);
uint8_t fat_delete_file(uint8_t file_number);
uint8_t fat_resize_file(uint8_t file_number, uint32_t new_file_size);
uint8_t fat_modify_file(uint8_t file_number, uint32_t start_byte, uint16_t length, uint8_t *data);
uint8_t fat_append_to_file(uint8_t file_number, uint16_t number_of_bytes, uint8_t *data);
uint8_t fat_rename_file(uint8_t file_number, char *name, char *extension);
uint8_t fat_read_from_file(uint8_t file_number, uint32_t start_byte, uint32_t length, uint8_t *data);
uint8_t fat_read_from_file_fast(uint32_t start_byte, uint32_t length, uint8_t *data, uint16_t *cluster, uint16_t *cluster_number);
uint8_t fat_copy_file(uint8_t file_number, char *name, char *extension);

//Read or write access via FLASH_BUFFER_2
uint8_t fat_copy_sector_to_buffer(uint8_t file_number, uint16_t sector);
uint8_t fat_write_sector_from_buffer(uint8_t file_number, uint16_t sector);
void fat_read_from_buffer(uint16_t start, uint16_t length, uint8_t *data);
void fat_write_to_buffer(uint16_t start, uint16_t length, uint8_t *data);

#endif	/* FAT16_H */

