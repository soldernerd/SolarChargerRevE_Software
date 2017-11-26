/* 
 * File:   fat16.h
 * Author: Luke
 *
 * Created on 30. October 2017, 21:59
 */

#ifndef FAT16_H
#define	FAT16_H

#define FBR_OEM_IDENTIFIER "MSDOS5.0"
#define FBR_BYTES_PER_SECTOR 512
#define FBR_SECTORS_PER_CLUSTER 1
#define FBR_RESERVED_SECTORS 1
#define FBR_NUMBER_OF_FATS 1
#define FBR_ROOT_ENTRIES 64
#define FBR_NUMBER_OF_SECTORS 4114
#define FBR_MEDIA_DESCRIPTOR 0XF8
#define FBR_SECTORS_PER_FAT 16
#define FBR_SECTORS_PER_HEAD 63
#define FBR_HEADS_PER_CYLINDER 16
#define FBR_HIDDEN_SECTORS 0 /* OR 1? */
#define FBR_EXT_FLAGS 0b00000001
#define FBR_ROOT_DIRECTORY_START ((FBR_SECTORS_PER_FAT*FBR_NUMBER_OF_FATS)+1)
#define FBR_SIGNATURE 0X55AA

#define MRB_PARTITION_STATUS 0x80
#define MBR_PARTITION_TYPE 0x04
#define MBR_FIRST_PARTITION_SECTOR 1
#define MBR_PARTITION_SIZE 5000
#define MBR_PARTITION_START_CYLINDER (MBR_FIRST_PARTITION_SECTOR/(FBR_HEADS_PER_CYLINDER*FBR_SECTORS_PER_HEAD))
#define MBR_PARTITION_START_HEAD ((MBR_FIRST_PARTITION_SECTOR/FBR_SECTORS_PER_HEAD)%FBR_HEADS_PER_CYLINDER)
#define MBR_PARTITION_START_SECTOR ((MBR_FIRST_PARTITION_SECTOR%FBR_SECTORS_PER_HEAD)+1)
#define MBR_LAST_PARTITION_SECTOR (MBR_FIRST_PARTITION_SECTOR+MBR_PARTITION_SIZE-1)
#define MBR_PARTITION_END_CYLINDER (MBR_LAST_PARTITION_SECTOR/(FBR_HEADS_PER_CYLINDER*FBR_SECTORS_PER_HEAD))
#define MBR_PARTITION_END_HEAD ((MBR_LAST_PARTITION_SECTOR/FBR_SECTORS_PER_HEAD)%FBR_HEADS_PER_CYLINDER)
#define MBR_PARTITION_END_SECTOR ((MBR_LAST_PARTITION_SECTOR%FBR_SECTORS_PER_HEAD)+1)
#define MBR_SIGNATURE 0X55AA

#define ROOT_DRIVE_NAME "SolarChargr"
#define ROOT_FILE_NAME "FILE    "
#define ROOT_FILE_EXTENSION "TXT"
#define ROOT_FILE_CONTENT "Hello World!"
#define ROOT_FILE_SIZE 12
#define ROOT_FILE_FIRST_CLUSTER 2

#define MBR_SECTOR 0
#define FBR_SECTOR 1
#define FAT_FIRST_SECTOR 2
#define FAT_LAST_SECTOR 17
#define ROOT_FIRST_SECTOR 18
#define ROOT_LAST_SECTOR 21
#define DATA_FIRST_SECTOR 22
#define DATA_LAST_SECTOR 4114
#define DATA_NUMBER_OF_SECTORS 4074

//typedef struct
//{
//    uint16_t time;
//    uint16_t date;
//} timestamp_t;

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


void fat_format(void);
uint8_t fat_find_file(char *name, char *extension);
uint32_t fat_get_file_size(uint8_t file_number);
uint8_t fat_create_file(char *name, char *extension, uint32_t size);
void fat_delete_file(uint8_t file_number);
uint16_t fat_get_empty_clusters(uint16_t maximum);
uint8_t fat_append_to_file(uint8_t file_number, uint16_t number_of_bytes, uint8_t *data);
void fat_rename_file(uint8_t file_number, char *name, char *extension);
uint8_t fat_read_from_file(uint8_t file_number, uint32_t start_byte, uint32_t length, uint8_t *data);

#endif	/* FAT16_H */

