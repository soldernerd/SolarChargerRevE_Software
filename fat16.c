
#include <stdint.h>
#include <xc.h>
#include "string.h"
#include "os.h"
#include "fat16.h"
#include "flash.h"
#include "rtcc.h"

static uint8_t _get_mbr(uint16_t idx);
static uint8_t _get_fbr(uint16_t idx);
static uint8_t _get_fat(uint16_t idx);
static uint8_t _get_root(uint16_t idx);
static uint16_t _get_time(void);
static uint16_t _get_date(void);
//static void _get_timestamp(timestamp_t *timestamp);
static uint8_t _get_available_root_entry(void);
static uint16_t _get_available_cluster(void);
static void _write_fat(uint16_t cluster, uint16_t value);
static void _write_root(uint8_t slot, rootEntry_t *data);

uint8_t buffer[512];

static uint16_t _get_time(void)
{
    uint16_t time;
    uint8_t hours = rtcc_get_hours_decimal();
    uint8_t minutes = rtcc_get_minutes_decimal();
    uint8_t seconds = rtcc_get_seconds_decimal();
    time = ((hours&0b11111) << 11);
    time |= ((minutes&0b111111) << 5);
    time |= ((seconds>>1)&0b11111);
    return time;
};

static uint16_t _get_date(void)
{
    uint16_t date;
    uint8_t year = rtcc_get_year_decimal();
    uint8_t month = rtcc_get_month_decimal();
    uint8_t day = rtcc_get_day_decimal();
    date = (((year+20)&0b1111111) << 9);
    date |= ((month&0b1111) << 5);
    date |= (day&0b11111);
    return date;
}

//static void _get_timestamp(timestamp_t *timestamp)
//{
//    (*timestamp).date = _get_date();
//    (*timestamp).time = _get_time();
//}

static uint8_t _get_available_root_entry(void)
{
    uint8_t slot;
    uint16_t sector;
    uint16_t offset;
    uint8_t buffer;
    slot = 0;
    for(sector = ROOT_FIRST_SECTOR; sector <= ROOT_LAST_SECTOR; ++sector)
    {
        for(offset = 0; offset<512; offset += 32)
        {
           flash_partial_read(sector, offset, 1, &buffer);
           if((buffer==0x00) || (buffer==0xE5))
           {
               return slot;
           }
           ++slot;
        }
        ++slot;
    }
    return 0xFF; //Indicating an error, i.e there are no available slots
}

static uint16_t _get_available_cluster(void)
{
    uint16_t cluster;
    uint16_t fat_sector;
    uint16_t offset;
    uint16_t value;
    
    //Start at 2, first two positions are reserved
    for(cluster=2; cluster<(DATA_NUMBER_OF_SECTORS+2); ++cluster)
    {
        fat_sector = cluster>>8; 
        fat_sector += FAT_FIRST_SECTOR;
        offset = cluster & 0xFF;
        offset <<= 1;
        flash_partial_read(fat_sector, offset, 2, &value);
        if(value==0x0000)
        {
            return cluster;
        }
    }
    return 0x0000; //Indicating that there are no free clusters
}

static void _write_fat(uint16_t cluster, uint16_t value)
{
    uint16_t fat_sector;
    uint16_t offset;
    fat_sector = cluster>>8; 
    fat_sector += FAT_FIRST_SECTOR;
    offset = cluster & 0xFF;
    offset <<= 1;
    flash_partial_write(fat_sector, offset, 2, &value);
}

static void _write_root(uint8_t slot, rootEntry_t *data)
{
    uint16_t root_sector;
    uint16_t offset;
    root_sector = (slot >> 4) + ROOT_FIRST_SECTOR;
    offset = (slot & 0b1111);
    offset <<= 5;
    flash_partial_write(root_sector, offset, 32, data);
    //flash_partial_write(root_sector, offset, sizeof(rootEntry_t), data);
}

uint8_t find_file(char *name, char *extension)
{
    uint8_t slot;
    uint16_t sector;
    uint16_t offset;
    uint8_t buffer[11];
    slot = 0;
    for(sector = ROOT_FIRST_SECTOR; sector <= ROOT_LAST_SECTOR; ++sector)
    {
        for(offset = 0; offset<512; offset += 32)
        {
           flash_partial_read(sector, offset, 11, &buffer);
           if(buffer[0]==0x00)
           {
               //There are no valid entries after a 0x00 entry
               //Indicate that there is no such file
               return 0xFF; 
           }
           else if(buffer[0]==0xE5)
           {
               //This file has been deleted
               //But there might be valid entries after this
               continue;
           }
           else
           {
               //This is a valid entry, check if name matches
               if(strncmp(name, &buffer[0], 8) == 0)
               {
                   //Name matches so check if extension matches as well
                   if(strncmp(extension, &buffer[8], 3) == 0)
                   {
                       //Both name and extension match
                       //We have found the file we're looking for
                       return slot;
                   }
               }
           }
           ++slot;
        }
        ++slot;
    }
    return 0xFF; //Indicating an error, i.e there are no available slots    
}

uint8_t fat_create_file(char *name, char *extension, uint32_t size)
{
    uint8_t root_slot;
    rootEntry_t root_entry;
    uint16_t first_cluster;
    
    //Check if a file with this name already exists in root
    //Return an error if the file is found
    if(find_file(name, extension) != 0xFF)
    {
        //We found a file with this name and extension
        //Return an error
        return 0xFF;
    }
    
    //Find an available root slot
    //Return an error if no slot is available
    root_slot = _get_available_root_entry();
    if(root_slot == 0xFF)
    {
        //There is no empty slot
        //Return an error
        return 0xFE;
    }
    
    //Check if there's enough space for the desired file size
    
    //Find first cluster
    first_cluster = _get_available_cluster();
    
    //Prepare root entry
    memcpy(root_entry.fileName, name, 8);
    memcpy(root_entry.fileExtension, extension, 3);
    root_entry.attributes = 0x00;
    root_entry.reserved1 = 0x00;
    root_entry.secondFractions = 0x00;
    root_entry.createdTime = _get_time();
    root_entry.createdDate = _get_date();
    root_entry.lastAccessDate = root_entry.createdDate;
    root_entry.reserved2 = 0x00;
    root_entry.modifiedTime = root_entry.createdTime;
    root_entry.modifiedDate = root_entry.createdDate;
    root_entry.firstCluster = first_cluster;
    root_entry.fileSize = size;
    
    //Write root entry
    _write_root(root_slot, &root_entry);
    
    //Reserve first cluster
    _write_fat(first_cluster, 0xFFFF);
    
    return root_slot;
}

static uint8_t _get_mbr(uint16_t idx)
{
	switch (idx)
	{
//        case 0x1B8:
//            return 0xF5;
//        case 0x1B9:
//            return 0x8B;
//        case 0x1BA:
//            return 0x16;
//        case 0x1BB:
//            return 0xEA;
		case 0x1BE:
			return MRB_PARTITION_STATUS;
		case 0x1BF:
			return MBR_PARTITION_START_SECTOR;
            //return 0x01;
		case 0x1C0:
			return ((MBR_PARTITION_START_CYLINDER & 0b00000011)<<6) | (MBR_PARTITION_START_HEAD & 0b00111111);
            //return 0x01;
		case 0x1C1:
			return MBR_PARTITION_START_CYLINDER >> 2;
            //return 0x00;
		case 0x1C2:
			return MBR_PARTITION_TYPE;
            //return 0x01;
		case 0x1C3:
			return MBR_PARTITION_END_SECTOR;
            //return 0x07;
		case 0x1C4:
			return ((MBR_PARTITION_END_CYLINDER & 0b00000011) << 6) | (MBR_PARTITION_END_HEAD & 0b00111111);
            //return 0xFF;
		case 0x1C5:
			return MBR_PARTITION_END_CYLINDER >> 2;
            //return 0xE6;
		case 0x1C6:
            return LOW_BYTE(LOW_WORD(((uint32_t)MBR_FIRST_PARTITION_SECTOR)));
		case 0x1C7:
			return HIGH_BYTE(LOW_WORD(((uint32_t)MBR_FIRST_PARTITION_SECTOR)));
		case 0x1C8:
			return LOW_BYTE(HIGH_WORD(((uint32_t)MBR_FIRST_PARTITION_SECTOR)));
		case 0x1C9:
            return HIGH_BYTE(HIGH_WORD(((uint32_t)MBR_FIRST_PARTITION_SECTOR)));
		case 0x1CA:
			return LOW_BYTE(LOW_WORD(((uint32_t)MBR_PARTITION_SIZE)));
		case 0x1CB:
			return HIGH_BYTE(LOW_WORD(((uint32_t)MBR_PARTITION_SIZE)));
		case 0x1CC:
			return LOW_BYTE(HIGH_WORD(((uint32_t)MBR_PARTITION_SIZE)));
		case 0x1CD:
			return HIGH_BYTE(HIGH_WORD(((uint32_t)MBR_PARTITION_SIZE)));
		case 0x1FE:
			return MBR_SIGNATURE >> 8;
		case 0x1FF:
			return MBR_SIGNATURE & 0XFF;
		default:
			return 0X00;
	}
}

static uint8_t _get_fbr(uint16_t idx)
{
	switch (idx)
	{
        case 0x00:
            return 0xEB;
        case 0x01:
            return 0x3C;
        case 0x02:
            return 0x90;
		case 0x03:
			return FBR_OEM_IDENTIFIER[0];
		case 0x04:
			return FBR_OEM_IDENTIFIER[1];
		case 0x05:
			return FBR_OEM_IDENTIFIER[2];
		case 0x06:
			return FBR_OEM_IDENTIFIER[3];
		case 0x07:
			return FBR_OEM_IDENTIFIER[4];
		case 0x08:
			return FBR_OEM_IDENTIFIER[5];
		case 0x09:
			return FBR_OEM_IDENTIFIER[6];
		case 0x0A:
			return FBR_OEM_IDENTIFIER[7];
		case 0x0B:
			return LOW_BYTE(((uint16_t)FBR_BYTES_PER_SECTOR));
		case 0x0C:
			return HIGH_BYTE(((uint16_t)FBR_BYTES_PER_SECTOR));
		case 0x0D:
			return FBR_SECTORS_PER_CLUSTER;
		case 0x0E:
            return LOW_BYTE(((uint16_t)FBR_RESERVED_SECTORS));
			//return FBR_RESERVED_SECTORS & 0XFF;
		case 0x0F:
            return HIGH_BYTE(((uint16_t)FBR_RESERVED_SECTORS));
			//return FBR_RESERVED_SECTORS >> 8;
		case 0x10:
			return FBR_NUMBER_OF_FATS;
		case 0x11:
            return LOW_BYTE(((uint16_t)FBR_ROOT_ENTRIES));
			//return FBR_ROOT_ENTRIES & 0XFF;
		case 0x12:
            return HIGH_BYTE(((uint16_t)FBR_ROOT_ENTRIES));
			//return FBR_ROOT_ENTRIES >> 8;
		case 0x13:
            return LOW_BYTE(((uint16_t)FBR_NUMBER_OF_SECTORS));
			//return FBR_NUMBER_OF_SECTORS & 0XFF;
		case 0x14:
            return HIGH_BYTE(((uint16_t)FBR_NUMBER_OF_SECTORS));
			//return FBR_NUMBER_OF_SECTORS >> 8;
		case 0x15:
			return FBR_MEDIA_DESCRIPTOR;
		case 0x16:
            return LOW_BYTE(((uint16_t)FBR_SECTORS_PER_FAT));
			//return FBR_SECTORS_PER_FAT & 0XFF;
		case 0x17:
            return HIGH_BYTE(((uint16_t)FBR_SECTORS_PER_FAT));
			//return FBR_SECTORS_PER_FAT >> 8;
		case 0x18:
            return LOW_BYTE(((uint16_t)FBR_SECTORS_PER_HEAD));
			//return FBR_SECTORS_PER_HEAD & 0XFF;
		case 0x19:
            return HIGH_BYTE(((uint16_t)FBR_SECTORS_PER_HEAD));
			//return FBR_SECTORS_PER_HEAD >> 8;
		case 0x1A:
            return LOW_BYTE(((uint16_t)FBR_HEADS_PER_CYLINDER));
			//return FBR_HEADS_PER_CYLINDER & 0XFF;
		case 0x1B:
            return HIGH_BYTE(((uint16_t)FBR_HEADS_PER_CYLINDER));
			//return FBR_HEADS_PER_CYLINDER >> 8;
		case 0x1C:
            return LOW_BYTE(LOW_WORD(((uint32_t)FBR_HIDDEN_SECTORS)));
			//return FBR_HIDDEN_SECTORS & 0XFF;
		case 0x1D:
            return HIGH_BYTE(LOW_WORD(((uint32_t)FBR_HIDDEN_SECTORS)));
			//return (FBR_HIDDEN_SECTORS >> 8) & 0XFF;
		case 0x1E:
            return LOW_BYTE(HIGH_WORD(((uint32_t)FBR_HIDDEN_SECTORS)));
			//return (FBR_HIDDEN_SECTORS >> 16) & 0XFF;
		case 0x1F:
            return HIGH_BYTE(HIGH_WORD(((uint32_t)FBR_HIDDEN_SECTORS)));
			//return (FBR_HIDDEN_SECTORS >> 24) & 0XFF;
		case 0x28:
            return LOW_BYTE(((uint16_t)FBR_EXT_FLAGS));
			//return FBR_EXT_FLAGS & 0XFF;
		case 0x29:
            return HIGH_BYTE(((uint16_t)FBR_EXT_FLAGS));
			//return FBR_EXT_FLAGS >> 8;
		case 0x2C:
            return LOW_BYTE(LOW_WORD(((uint32_t)FBR_ROOT_DIRECTORY_START)));
			//return FBR_ROOT_DIRECTORY_START & 0XFF;
		case 0x2D:
            return HIGH_BYTE(LOW_WORD(((uint32_t)FBR_ROOT_DIRECTORY_START)));
			//return (FBR_ROOT_DIRECTORY_START >> 8) & 0XFF;
		case 0x2E:
            return LOW_BYTE(HIGH_WORD(((uint32_t)FBR_ROOT_DIRECTORY_START)));
			//return (FBR_ROOT_DIRECTORY_START >> 16) & 0XFF;
		case 0x2F:
            return HIGH_BYTE(HIGH_WORD(((uint32_t)FBR_ROOT_DIRECTORY_START)));
			//return (FBR_ROOT_DIRECTORY_START >> 24) & 0XFF;
        case 0x36:
            return 'F';
        case 0x37:
            return 'A';
        case 0x38:
            return 'T';
        case 0x39:
            return '1';
        case 0x3A:
            return '6';
        case 0x3B:
            return ' ';
        case 0x3C:
            return ' ';
        case 0x3D:
            return ' ';
		case 0x1FE:
            return HIGH_BYTE(((uint16_t)FBR_SIGNATURE));
			//return FBR_SIGNATURE >> 8;
		case 0x1FF:
            return LOW_BYTE(((uint16_t)FBR_SIGNATURE));
			//return FBR_SIGNATURE & 0XFF;
		default:
			return 0X00;
	}
}

static uint8_t _get_fat(uint16_t idx)
{
	switch (idx)
	{
        //Bytes 0-4 are reserved
		case 0x00:
			return 0xF8;
		case 0x01:
			return 0xFF;
		case 0x02:
			return 0xFF;
		case 0x03:
			return 0xFF;
        //Bytes 4 & 5 = 0xFFFF. So there is 1 file occupying a single cluster
		case 0x04:
			return 0xFF;
		case 0x05:
			return 0xFF;
		default:
			return 0X00;
	}
}

static uint8_t _get_root(uint16_t idx)
{
	if (idx < 0x20) // Volume information
	{
		if (idx < 11)
		{
			return ROOT_DRIVE_NAME[idx];
		}
        if(idx==11)
        {
            return 0x08;
        }
	}
	if (idx < 0x40) // Example file
	{
		idx -= 0x20;
		if (idx < 0+8)
		{
			return ROOT_FILE_NAME[idx];
		}
		if (idx < 11)
		{
			return ROOT_FILE_EXTENSION[idx-8];
		}
		switch (idx)
		{
            case 11:
                return 0x20;
            case 0x0D:
                return 0x06;
            case 0x0E:
                return 0x28;
            case 0x0F:
                return 0x78;
            case 0x10:
                return 0xDE;
            case 0x11:
                return 0x38;
            case 0x12:
                return 0x5F;
            case 0x13:
                return 0x4B;
            case 0x14:
                return 0x00;
            case 0x15:
                return 0x00;
            case 0x16:
                return 0x04;
            case 0x17:
                return 0x77;
            case 0x18:
                return 0xDE;
            case 0x19:
                return 0x38;
            case 26:
                return LOW_BYTE(((uint16_t)ROOT_FILE_FIRST_CLUSTER));
            case 27:
                return HIGH_BYTE(((uint16_t)ROOT_FILE_FIRST_CLUSTER));
			case 28:
                return LOW_BYTE(LOW_WORD(((uint32_t)ROOT_FILE_SIZE)));
				//return ROOT_FILE_SIZE & 0XFF;
			case 29:
                return HIGH_BYTE(LOW_WORD(((uint32_t)ROOT_FILE_SIZE)));
				//return (ROOT_FILE_SIZE >> 8) & 0XFF;
			case 30:
                return LOW_BYTE(HIGH_WORD(((uint32_t)ROOT_FILE_SIZE)));
				//return (ROOT_FILE_SIZE >> 16) & 0XFF;
			case 31:
                return HIGH_BYTE(HIGH_WORD(((uint32_t)ROOT_FILE_SIZE)));
				//return (ROOT_FILE_SIZE >> 24) & 0XFF;
			default:
				return 0x00;
		}
	}
	return 0X00;
}

static uint8_t _get_data(uint16_t idx)
{
	if (idx < ROOT_FILE_SIZE) // Dummy file
	{
        return ROOT_FILE_CONTENT[idx];
	}
	return 0X00;
}

void fat_format_flash(void)
{
    uint16_t cntr;
    
    //Write MBR to sector 0
    for(cntr=0; cntr<512; ++cntr)
    {
        buffer[cntr] = _get_mbr(cntr);
    }
    flash_page_write(0, buffer);
    
    //Write FBR to sector 1
    for(cntr=0; cntr<512; ++cntr)
    {
        buffer[cntr] = _get_fbr(cntr);
    }
    flash_page_write(1, buffer);
    
    //Write first FAT page to sector 2
    for(cntr=0; cntr<512; ++cntr)
    {
        buffer[cntr] = _get_fat(cntr);
    }
    flash_page_write(2, buffer);
    
    //Fill remaining FAT pages to sectors 3-17 (all zeros)
    for(cntr=0; cntr<512; ++cntr)
    {
        buffer[cntr] = 0x00;
    }
    for(cntr=3; cntr<=17; ++cntr)
    {
        flash_page_write(cntr, buffer);
    }
    
    //Write beginning of root to sector 18
    for(cntr=0; cntr<512; ++cntr)
    {
        buffer[cntr] = _get_root(cntr);
    }
    flash_page_write(18, buffer);
    
    //Fill remaining ROOT pages to sectors 19-21 (all zeros)
    for(cntr=0; cntr<512; ++cntr)
    {
        buffer[cntr] = 0x00;
    }
    for(cntr=19; cntr<=21; ++cntr)
    {
        flash_page_write(cntr, buffer);
    }
    
    //Write Data to sector 22
    for(cntr=0; cntr<512; ++cntr)
    {
        buffer[cntr] = _get_data(cntr);
    }
    flash_page_write(22, buffer);
    
}
