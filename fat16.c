
#include <stdint.h>
#include <xc.h>
#include "string.h"
#include "os.h"
#include "fat16.h"
#include "flash.h"

#ifdef REAL_TIME_CLOCK_AVAILABLE   
#include "rtcc.h"
#endif

/*****************************************************************************
 * Module variables                                                          *
 *****************************************************************************/

uint8_t buffer[BYTES_PER_SECTOR];


/*****************************************************************************
 * Static functions prototypes                                               *
 *****************************************************************************/

static uint16_t _sector_from_file_number(uint8_t file_number);
static uint16_t _offset_from_file_number(uint8_t file_number);
static uint16_t _data_sector_from_cluster(uint16_t cluster);
static uint16_t _fat_offset_from_cluster(uint16_t cluster);
static uint16_t _cluster_from_sector_and_offset(uint16_t sector, uint16_t offset);
static uint8_t _get_mbr(uint16_t idx);
static uint8_t _get_fbr(uint16_t idx);
static uint8_t _get_fat(uint16_t idx);
static uint8_t _get_root(uint16_t idx);
static uint16_t _get_time(void);
static uint16_t _get_date(void);
static uint8_t _get_available_root_entry(void);
static uint16_t _read_fat(uint16_t cluster);
static void _write_root(uint8_t slot, rootEntry_t *data);
static void _delete_root(uint8_t slot);
static uint16_t _get_first_cluster(uint8_t slot);
static uint16_t _get_available_offset_from_buffer(uint16_t minimum_offset, uint8_t *buffer);
static uint16_t _read_value_from_offset(uint16_t offset, uint8_t *buffer);
static void _write_value_to_offset(uint16_t offset, uint8_t *buffer, uint16_t value);
static uint16_t _get_available_cluster(uint16_t first_sector, uint16_t skip_sector);
static uint16_t _find_nth_cluster(uint16_t start_cluster, uint16_t n);
static uint16_t _make_cluster_chain(uint16_t first_cluster, uint16_t number_of_clusters);


/*****************************************************************************
 * Static functions                                                          *
 *****************************************************************************/

static uint16_t _sector_from_file_number(uint8_t file_number)
{
    uint16_t sector;
    sector = (uint16_t) file_number;
    sector >>= 4;
    sector += ROOT_FIRST_SECTOR;
    return sector;
}

static uint16_t _offset_from_file_number(uint8_t file_number)
{
    uint16_t offset;
    offset = file_number;
    offset &= 0b1111;
    offset <<= 5;
    return offset;
}

static uint16_t _data_sector_from_cluster(uint16_t fat_cluster)
{
    return fat_cluster + DATA_FIRST_SECTOR - 2;
}

static uint16_t _fat_sector_from_cluster(uint16_t cluster)
{
    cluster >>= 8;
    cluster += FAT_FIRST_SECTOR;
    return cluster;
}

static uint16_t _fat_offset_from_cluster(uint16_t cluster)
{
    cluster &= 0xFF;
    cluster <<= 1;
    return cluster;
}

static uint16_t _cluster_from_sector_and_offset(uint16_t sector, uint16_t offset)
{
    offset >>= 1;
    sector -= FAT_FIRST_SECTOR;
    sector <<= 8;
    sector |= offset;
    return sector;
}

static uint16_t _get_available_offset_from_buffer(uint16_t minimum_offset, uint8_t *buffer)
{
    uint16_t offset;

    for(offset = minimum_offset; offset<BYTES_PER_SECTOR; offset += 2)
    {
        if((buffer[offset] == 0x00) && (buffer[offset + 1] == 0x00))
        {
            return offset;
        }
    }

    //No offset available
    return 0xFFFF;
}

static uint16_t _read_value_from_offset(uint16_t offset, uint8_t *buffer)
{
    uint16_t value;
    value = buffer[offset+1];
    value <<= 8;
    value |= buffer[offset];
    return value;
}

static void _write_value_to_offset(uint16_t offset, uint8_t *buffer, uint16_t value)
{
    buffer[offset] = (uint8_t) (value & 0xFF);
    value >>= 8;
    buffer[offset+1] = (uint8_t) (value & 0xFF);
}

static uint16_t _get_available_cluster(uint16_t first_sector, uint16_t skip_sector)
{
    uint16_t cluster;
    uint16_t sector;
    uint16_t offset;

    //Loop through FAT sectors
    for (sector=first_sector; sector<=FAT_LAST_SECTOR; ++sector)
    {
        //Skip a specific sector
        if(sector == skip_sector)
        {
            continue;
        }

        //Read entire sector into buffer
        flash_sector_read(sector, buffer);

        //Try to find an available cluster within this sector
        if(sector == FAT_FIRST_SECTOR)
        {
            //First 2 positions are reserved
            offset = _get_available_offset_from_buffer(4, buffer);
        }
        else
        {
            offset = _get_available_offset_from_buffer(0, buffer);
        }

        //If we found an available cluster, we're done
        if(offset != 0xFFFF)
        {
            //Calculate cluster
            cluster = _cluster_from_sector_and_offset(sector, offset);
            //Make sure cluster is within allowable range
            if(cluster > DATA_LAST_SECTOR)
            {
                //We've exceeded the allowable range, hence there are no available clusters
                return 0x0000;
            }
            else
            {
                //We have a valid cluster, return it
                return cluster;
            }
        }
    }

    //If we get to here, there are no available clusters
    return 0x0000;
}

static uint16_t _find_nth_cluster(uint16_t cluster, uint16_t n)
{
    uint16_t sector;
    uint16_t offset;
    uint16_t sector_in_buffer = 0;
    
    //Call a more efficient function in case n=1
    if(n==1)
    {
        return _read_fat(cluster);
    }

    while (n > 0)
    {
        //Find sector and offset of next cluster
        sector = _fat_sector_from_cluster(cluster);
        offset = _fat_offset_from_cluster(cluster);

        //Read new sector into buffer if necessary
        if(sector != sector_in_buffer)
        {
            flash_sector_read(sector, buffer);
        }

        //Read next cluster
        cluster = _read_value_from_offset(offset, buffer);
    }

    return cluster;
}

static uint16_t _make_cluster_chain(uint16_t first_cluster, uint16_t number_of_clusters)
{
    uint16_t remaining_clusters;
    uint16_t minimum_sector;
    uint16_t sector;
    uint16_t offset;
    uint16_t next_offset;
    uint16_t sector_in_buffer;
    uint16_t next_cluster;
    uint16_t different_sector_cluster;
    uint8_t data_changed;  

    //Initialize variables
    sector_in_buffer = 0;
    remaining_clusters = number_of_clusters;
    minimum_sector = FAT_FIRST_SECTOR;
    data_changed = 0;

    //Obtain first cluster if necessary
    if((first_cluster==0) && (number_of_clusters>0))
    {
        first_cluster = _get_available_cluster(minimum_sector, 0);
        //Update minimum sector
        minimum_sector = _fat_sector_from_cluster(first_cluster);
        if(first_cluster == 0)
        {
            return 0xFFFF; //Indicating no cluster available
        }
    }
    next_cluster = first_cluster;

    //Continue while we need more clusters or as long as an existing cluster chain continues
    while ((remaining_clusters > 0) || ((next_cluster>=FAT_MINIMUM_VALUE) && (next_cluster<=FAT_MAXIMUM_VALUE)))
    {
        
//        if(remaining_clusters==0)
//        {
//            ++cntr;
//            //flash_sector_write(sector_in_buffer, buffer);
//            if(cntr>100)
//            {
//                return first_cluster;
//            }
//            
//        }
        
        sector = _fat_sector_from_cluster(next_cluster);
        offset = _fat_offset_from_cluster(next_cluster);

        //Read entire sector into buffer if we haven't done so already
        if(sector != sector_in_buffer)
        {
            //Write data
            if(data_changed)
            {
                flash_sector_write(sector_in_buffer, buffer);
            }

            //Obtain a cluster located in a different sector
            different_sector_cluster = _get_available_cluster(minimum_sector, sector);

            //Update minimum sector
            minimum_sector = _fat_sector_from_cluster(different_sector_cluster);

            //Read new sector
            flash_sector_read(sector, buffer);
            sector_in_buffer = sector;
            data_changed = 0;
        }

        //We still need more clusters to follow
        if(remaining_clusters > 1)
        {
            //Get the next cluster
            next_cluster = _read_value_from_offset(offset, buffer);

            //Is the current buffer already pointing to the next one?
            if((next_cluster < FAT_MINIMUM_VALUE) || (next_cluster > FAT_MAXIMUM_VALUE))
            {
                //Find a new cluster
                next_offset = _get_available_offset_from_buffer(offset+2, buffer);
                
                if(next_offset != 0xFFFF) //We found a cluster on this FAT sector
                {
                    //Calculate cluster value from offset
                    next_cluster = sector_in_buffer - FAT_FIRST_SECTOR;
                    next_cluster <<= 8;
                    next_cluster |= (next_offset>>1);
                }
                else //We need to use a cluster located on a different FAT sector
                {
                    if(different_sector_cluster == 0x0000)
                    {
                        //We are out of clusters. Return an error
                        return 0xFFFF;
                    }
                    else
                    {
                        //There is a cluster on a different sector. Use that one
                        next_cluster = different_sector_cluster;
                    }
                }
                //Write that cluster to the buffer
                _write_value_to_offset(offset, buffer, next_cluster);
                //Indicate that data has changed
                data_changed = 1;
            }

            //Decrement clusters left to process
            --remaining_clusters;
        }

        //This is the last cluster
        else if(remaining_clusters == 1)
        {
            //Get the next cluster
            next_cluster = _read_value_from_offset(offset, buffer);

            if(next_cluster != 0xFFFF)
            {
                //Write 0xFFFF to the buffer, indicating that this is the last cluster
                _write_value_to_offset(offset, buffer, 0xFFFF);
                //Indicate that data has changed
                data_changed = 1;
            }

            //Decrement clusters left to process
            --remaining_clusters;
        }

        //There are more clusters than needed. Free them
        else
        {
            //Get the next cluster
            next_cluster = _read_value_from_offset(offset, buffer);

            //Write 0x0000 to the buffer, marking the cluster as free
            _write_value_to_offset(offset, buffer, 0x0000);
            //Indicate that data has changed
            data_changed = 1;
        }

    } //while ((number_of_clusters > 0) || next_cluster < 0xFFF0)

    //Write data if necessary
    if(data_changed)
    {
        flash_sector_write(sector_in_buffer, buffer);
    }

    //Return first cluster if any
    if(number_of_clusters>0)
    {
        return first_cluster;
    }
    else
    {
        return 0x0000;
    }
    
}


static uint16_t _get_time(void)
{
    #ifdef REAL_TIME_CLOCK_AVAILABLE   
        uint16_t time;
        uint8_t hours = rtcc_get_hours_decimal();
        uint8_t minutes = rtcc_get_minutes_decimal();
        uint8_t seconds = rtcc_get_seconds_decimal();
        time = ((hours&0b11111) << 11);
        time |= ((minutes&0b111111) << 5);
        time |= ((seconds>>1)&0b11111);
        return time;
    #else
        return 0x0000;
    #endif
};

static uint16_t _get_date(void)
{
    #ifdef REAL_TIME_CLOCK_AVAILABLE  
        uint16_t date;
        uint8_t year = rtcc_get_year_decimal();
        uint8_t month = rtcc_get_month_decimal();
        uint8_t day = rtcc_get_day_decimal();
        date = (((year+20)&0b1111111) << 9);
        date |= ((month&0b1111) << 5);
        date |= (day&0b11111);
        return date;
    #else
        return 0x0000;
    #endif
}

static uint8_t _root_is_available(uint8_t file_number)
{
    uint16_t root_sector;
    uint16_t offset;
    uint8_t first_byte;
    
    //Sanity check
    //Make sure we have a valid file number
    if(file_number>=FBR_ROOT_ENTRIES)
    {
        //Indicate that root entry is not available
        return 0;
    }
    
    //Find position of root entry
    root_sector = _sector_from_file_number(file_number);
    offset = _offset_from_file_number(file_number);
    
    //Read the first character of the root entry
    flash_partial_read(root_sector, offset, 1, &first_byte);   
    
    //Check the value of the first byte and return accordingly
    if((first_byte==0x00) || (first_byte==0xE5))
    {
        //Root entry is available
        return 1;
    }
    else
    {
        //Root entry is not available
        return 0;
    }
}

static uint8_t _get_available_root_entry(void)
{
    uint8_t file_number;
    
    for(file_number=0; file_number<FBR_ROOT_ENTRIES; ++file_number)
    {
        if(_root_is_available(file_number))
        {
            return file_number;
        }
    }
    return 0xFF; //Indicating an error, i.e there are no available slots
}

static uint16_t _read_fat(uint16_t cluster)
{
    uint16_t sector;
    uint16_t offset;
    uint16_t value;
    
    //Find FAT location
    sector = _fat_sector_from_cluster(cluster);
    offset = _fat_offset_from_cluster(cluster);
    
    //Read value from flash
    flash_partial_read(sector, offset, 2, &value);
    
    //Return result
    return value;
}

static void _write_root(uint8_t file_number, rootEntry_t *data)
{
    uint16_t sector;
    uint16_t offset;
    
    //Find root location
    sector = _sector_from_file_number(file_number);
    offset = _offset_from_file_number(file_number);

    //Write root entry
    flash_partial_write(sector, offset, 32, data);
}

static void _delete_root(uint8_t file_number)
{
    uint16_t sector;
    uint16_t offset;
    uint8_t deleted_value;
    
    //Find root location
    sector = _sector_from_file_number(file_number);
    offset = _offset_from_file_number(file_number);
    
    //Mark entry as reusable / deleted
    deleted_value = 0xE5;
    flash_partial_write(sector, offset, 1, &deleted_value);
}

static uint16_t _get_first_cluster(uint8_t file_number)
{
    uint16_t first_cluster;
    uint16_t sector;
    uint16_t offset;
    
    //Get position
    sector = _sector_from_file_number(file_number);
    offset = _offset_from_file_number(file_number);
    offset += 26;
    
    //Read data and return first cluster
    flash_partial_read(sector, offset, 2, &first_cluster);
    return first_cluster;
}

uint8_t fat_find_file(char *name, char *extension)
{
    uint8_t file_number = 0;
    uint16_t sector;
    uint16_t offset;

    for(sector = ROOT_FIRST_SECTOR; sector <= ROOT_LAST_SECTOR; ++sector)
    {
        //Read entire sector into buffer
        flash_sector_read(sector, buffer);
        
        for(offset = 0; offset<512; offset += 32)
        {
           if(buffer[offset]==0x00)
           {
               //There are no valid entries after a 0x00 entry
               //Indicate that there is no such file
               return 0xFF; 
           }
           else if(buffer[offset]==0xE5)
           {
               //This file has been deleted
               //But there might be valid entries after this
               //Just do nothing
           }
           else
           {
               //This is a valid entry, check if name matches
               if(strncmp(name, &buffer[offset], 8) == 0)
               {
                   //Name matches so check if extension matches as well
                   if(strncmp(extension, &buffer[offset+8], 3) == 0)
                   {
                       //Both name and extension match
                       //We have found the file we're looking for
                       return file_number;
                   }
               }
           }
           ++file_number;
        }
    }
    return 0xFF; //Indicating an error, i.e file not found  
}

uint32_t fat_get_file_size(uint8_t file_number)
{
    uint32_t file_size;
    uint16_t sector;
    uint16_t offset;
    
    //Check if file number is valid, i.e. point to a valid file
    if(_root_is_available(file_number))
    {
        //Indicate an error
        return 0xFFFFFFFF;
    }
    
    //Find position of root entry
    sector = _sector_from_file_number(file_number);
    offset = _offset_from_file_number(file_number);

    //Read file information
    flash_partial_read(sector, offset+28, 4, &file_size);
    
    //Return result
    return file_size;
}

uint8_t fat_create_file(char *name, char *extension, uint32_t size)
{
    rootEntry_t root_entry;
    uint8_t file_number;
    uint16_t number_of_clusters;
    uint16_t first_cluster;
    
    //Check if a file with this name already exists in root
    //Return an error if the file is found
    if(fat_find_file(name, extension) != 0xFF)
    {
        //We found a file with this name and extension
        //Return an error
        return 0xFF;
    }
    
    //Find an available file number
    //Return an error if no slot is available
    file_number = _get_available_root_entry();
    if(file_number == 0xFF)
    {
        //There is no empty slot
        //Return an error
        return 0xFE;
    }
    
    //Calculate number of clusters
    number_of_clusters = (size + BYTES_PER_SECTOR - 1) >> 9;
    
    //Create cluster chain in FAT
    first_cluster = _make_cluster_chain(0x0000, number_of_clusters);
    
    //Check if we were successful
    if(first_cluster == 0xFFFF)
    {
        //There is not enough space on the drive
        //Return an error
        return 0xFD;
    }

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
    _write_root(file_number, &root_entry);
    
    return file_number;
}

uint8_t fat_delete_file(uint8_t file_number)
{
    uint16_t first_cluster;
    
    //Make sure we have a valid file number
    if(file_number>=FBR_ROOT_ENTRIES)
    {
        //Nothing to be done
        return 0xFF;
    }
    
    //Check if file is in use
    if(_root_is_available(file_number))
    {
        //Nothing to be done
        return 0xFE;
    }
    
    //Get number of first cluster, i.e where to start
    first_cluster = _get_first_cluster(file_number);
    
    //Free clusters
    _make_cluster_chain(first_cluster, 0);
    
    //Now all the clusters are marked as re-usable
    //But the file entry in the root still exists
    _delete_root(file_number);
    
    return 0x00;
}

uint8_t fat_append_to_file(uint8_t file_number, uint16_t number_of_bytes, uint8_t *data)
{
    uint32_t old_file_size;
    uint32_t new_file_size;
    uint8_t return_code;
    
    //Make sure we have a valid file number
    if(file_number>=FBR_ROOT_ENTRIES)
    {
        //Return an error
        return 0xFF;
    }
    
    //Check if file is in use
    if(_root_is_available(file_number))
    {
        //Return an error
        return 0xFE;
    }
    
    //Get (old) file size
    old_file_size = fat_get_file_size(file_number);
    
    //Calculate new file size
    new_file_size = old_file_size + number_of_bytes;
    
    //Resize file
    return_code = fat_resize_file(file_number, new_file_size);
    if(return_code!=0x00)
    {
        //Return an error. Subtract something to make them distinguishable from error codes above
        return return_code - 0x0F;
    }
    
    //Write data to newly added part of the file
    fat_modify_file(file_number, old_file_size, number_of_bytes, data);

    //Report success
    return 0x00;
}

uint8_t fat_rename_file(uint8_t file_number, char *name, char *extension)
{
    rootEntry_t root;
    uint8_t cntr;
    uint8_t return_code;
    
    //Obtain a copy of root entry
    return_code = fat_get_file_information(file_number, &root);
    if(return_code!=0x00)
    {
        //No valid file, return
        return return_code;
    }
    
    //Change file name
    for(cntr=0; cntr<8; ++cntr)
    {
        root.fileName[cntr] = name[cntr];
    }
    
    //Change extension
    for(cntr=0; cntr<3; ++cntr)
    {
        root.fileExtension[cntr] = extension[cntr];
    }
    
    //Write modified root entry to flash
    _write_root(file_number, &root);
    
    return 0x00;
}

uint8_t fat_read_from_file(uint8_t file_number, uint32_t start_byte, uint32_t length, uint8_t *data)
{
    rootEntry_t root;
    uint16_t cluster;
    uint16_t cluster_number;
    uint8_t return_code;
    
    //Read root entry
    return_code = fat_get_file_information(file_number, &root);
    if(return_code!=0x00)
    {
        //No valid file, return error code
        return return_code;
    }
    
    //Make sure we do not read past file end
    if(start_byte > root.fileSize)
    {
        //User wants to read beyond end of file
        return 0xFF;
    }
    
    if(start_byte+length > root.fileSize)
    {
        //User wants to read past end of file. Only read until end of file.
        length = root.fileSize - start_byte;
    }
    
    //Prepare variables for call to fat_read_from_file_fast
    cluster = root.firstCluster;
    cluster_number = 0;
    
    //Now we know where to start, call fast function
    return fat_read_from_file_fast(start_byte, length, data, &cluster, &cluster_number);
}

uint8_t fat_read_from_file_fast(uint32_t start_byte, uint32_t length, uint8_t *data, uint16_t *cluster, uint16_t *cluster_number)
{
    uint32_t position;
    uint16_t offset;
    uint16_t sector;
    uint16_t read_length;
    uint16_t working_cluster;
    uint16_t needed_cluster;
    
    //Calculate number of needed cluster
    needed_cluster = (uint16_t) (start_byte>>9);
    
    //Return an error if the provided cluster is already past what we need
    if((*cluster_number) > needed_cluster)
    {
        return 0xFF;
    }
    
    //Find needed cluster
    (*cluster) = _find_nth_cluster((*cluster), (needed_cluster-(*cluster_number)));
    
    //Update cluster number
    (*cluster_number) = needed_cluster;

    //Only use working cluster from now on
    //Do no longer update pointers *cluster and *cluster_number
    working_cluster = (*cluster);
    
    //Calculate offset
    position = (needed_cluster << 9);    
    offset = start_byte - position;

    //Now we know where to start reading data
    position = 0;
    while(position < length)
    {
        //Do we need another cluster first?
        if(offset==512)
        {
            //Need to get the next cluster cluster
            working_cluster  = _read_fat(working_cluster);
            offset = 0;
        }
        
        //Get physical flash sector from logical cluster address
        sector = _data_sector_from_cluster(working_cluster);
        
        //How much data can we/should we write to the current cluster?
        read_length = 512 - offset; //Maximum we can read from this cluster
        if(read_length > (length-position))
        {
            //Just read all remaining bytes
            read_length = length - position;
        }
        
        //Read that data
        flash_partial_read(sector, offset, read_length, &data[position]);
        
        //Update position and offset
        position += read_length;
        offset += read_length;
    }
    
    //Indicate success
    return 0x00;
}

uint8_t fat_copy_file(uint8_t file_number, char *name, char *extension)
{
    uint32_t file_size;
    uint8_t new_file_number;
    uint16_t number_of_clusters;
    uint16_t sector;
    uint8_t return_value;
    
    //Make sure we have a valid file number
    if(file_number>=FBR_ROOT_ENTRIES)
    {
        //Return an error
        return 0xFF;
    }
    
    //Check if file is in use
    if(_root_is_available(file_number))
    {
        //Return an error
        return 0xFE;
    }
    
    //Get file size
    file_size = fat_get_file_size(file_number);
    
    //Try to create a file
    new_file_number = fat_create_file(name, extension, file_size);
    
    //Make sure we have a valid new file number
    if(new_file_number>=FBR_ROOT_ENTRIES)
    {
        //Return an error
        return 0xFD;
    }
    
    //Calculate number of clusters
    number_of_clusters = (file_size + BYTES_PER_SECTOR - 1) >> 9;
    
    //Copy sectors, one by one
    for(sector=0; sector<number_of_clusters; ++sector)
    {
        return_value = fat_copy_sector_to_buffer(file_number, sector);
        if(return_value!=0x00)
        {
            return 0xFC;
        }
        return_value = fat_write_sector_from_buffer(new_file_number, sector);
        if(return_value!=0x00)
        {
            return 0xFB;
        }
    }
    
    return 0x00;
}

uint8_t fat_resize_file(uint8_t file_number, uint32_t new_file_size)
{
    rootEntry_t root;
    uint16_t old_number_of_clusters;
    uint16_t new_number_of_clusters;
    uint16_t first_cluster;
    
    //Make sure we have a valid file number
    if(file_number>=FBR_ROOT_ENTRIES)
    {
        //Return an error
        return 0xFF;
    }
    
    //Check if file is in use
    if(_root_is_available(file_number))
    {
        //Return an error
        return 0xFE;
    }
    
    //Collect data from the root entry
    fat_get_file_information(file_number, &root);
    
    //Calculate number of clusters necessary
    old_number_of_clusters = (root.fileSize+BYTES_PER_SECTOR-1)>>9;
    new_number_of_clusters = (new_file_size+BYTES_PER_SECTOR-1)>>9;
    
    //Free or reserve clusters if necessary
    if(old_number_of_clusters!=new_number_of_clusters)
    {
        first_cluster = _make_cluster_chain(root.firstCluster, new_number_of_clusters);
        if(first_cluster==0xFFFF)
        {
            //Return an error
            return 0xFD;
        }
        
        //Update root
        root.firstCluster = first_cluster;
    }
    
    //Update root entry with new file size
    root.fileSize = new_file_size;
    
    //Write root
    _write_root(file_number, &root);
    
    //Indicate success
    return 0x00;
}

uint8_t fat_modify_file(uint8_t file_number, uint32_t start_byte, uint16_t length, uint8_t *data)
{
    rootEntry_t root;
    uint16_t cluster;
    uint32_t position;
    uint16_t offset;
    uint16_t sector;
    uint16_t number_of_bytes;
    uint8_t return_code;
    
    //Read root entry
    return_code = fat_get_file_information(file_number, &root);
    if(return_code!=0x00)
    {
        //No valid file, return
        return return_code;
    }
    
    //Make sure we do not write past file end
    if(start_byte>root.fileSize)
    {
        //User wants to write at a position beyond end of file
        return 0xF0;
    }
    
    if((start_byte+length) > root.fileSize)
    {
        //User wants to write past end of file. Only write until end of file.
        length = root.fileSize - start_byte;
    }
    
    //Find first relevant cluster
    cluster = _find_nth_cluster(root.firstCluster, (start_byte>>9));
    
    //Calculate position. Just blank out the last 9 bits
    position = start_byte & 0xFFFFFE00;
    
    //Calculate offset
    offset = (uint16_t) (start_byte-position);
    
    //Now we know where to start writing data
    position = 0;
    while(position < length)
    {
        //Do we need another cluster first?
        if(offset==512)
        {
            //Find the next cluster
            cluster = _read_fat(cluster);
            //Reset offset
            offset = 0;
        }
        
        //Get physical flash sector from logical cluster address
        sector = _data_sector_from_cluster(cluster);
        
        //How much data can we/should we write to the current cluster?
        number_of_bytes = 512 - offset;
        if(number_of_bytes > (length-position))
        {
            number_of_bytes = length - position;
        }
        
        //Write that data
        flash_partial_write(sector, offset, number_of_bytes, &data[position]);
        
        //Update position and offset
        position += number_of_bytes;
        offset += number_of_bytes;
    }
    
    return 0x00;
}


static uint8_t _get_mbr(uint16_t idx)
{
	switch (idx)
	{
		case 0x1BE:
			return MRB_PARTITION_STATUS;
		case 0x1BF:
            return MBR_PARTITION_START_HEAD;
		case 0x1C0:
            return MBR_PARTITION_START_SECTOR;
		case 0x1C1:
            return MBR_PARTITION_START_CYLINDER;
		case 0x1C2:
			return MBR_PARTITION_TYPE;
		case 0x1C3:
            return MBR_PARTITION_END_HEAD;
		case 0x1C4:
            return MBR_PARTITION_END_SECTOR;
		case 0x1C5:
			return MBR_PARTITION_END_CYLINDER;
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
			return HIGH_BYTE((uint16_t)MBR_SIGNATURE);
		case 0x1FF:
			return LOW_BYTE((uint16_t)MBR_SIGNATURE);
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
		case 0x0F:
            return HIGH_BYTE(((uint16_t)FBR_RESERVED_SECTORS));
		case 0x10:
			return FBR_NUMBER_OF_FATS;
		case 0x11:
            return LOW_BYTE(((uint16_t)FBR_ROOT_ENTRIES));
		case 0x12:
            return HIGH_BYTE(((uint16_t)FBR_ROOT_ENTRIES));
		case 0x13:
            return LOW_BYTE(((uint16_t)FBR_NUMBER_OF_SECTORS));
		case 0x14:
            return HIGH_BYTE(((uint16_t)FBR_NUMBER_OF_SECTORS));
		case 0x15:
			return FBR_MEDIA_DESCRIPTOR;
		case 0x16:
            return LOW_BYTE(((uint16_t)FBR_SECTORS_PER_FAT));
		case 0x17:
            return HIGH_BYTE(((uint16_t)FBR_SECTORS_PER_FAT));
		case 0x18:
            return LOW_BYTE(((uint16_t)FBR_SECTORS_PER_HEAD));
		case 0x19:
            return HIGH_BYTE(((uint16_t)FBR_SECTORS_PER_HEAD));
		case 0x1A:
            return LOW_BYTE(((uint16_t)FBR_HEADS_PER_CYLINDER));
		case 0x1B:
            return HIGH_BYTE(((uint16_t)FBR_HEADS_PER_CYLINDER));
		case 0x1C:
            return LOW_BYTE(LOW_WORD(((uint32_t)FBR_HIDDEN_SECTORS)));
		case 0x1D:
            return HIGH_BYTE(LOW_WORD(((uint32_t)FBR_HIDDEN_SECTORS)));
		case 0x1E:
            return LOW_BYTE(HIGH_WORD(((uint32_t)FBR_HIDDEN_SECTORS)));
		case 0x1F:
            return HIGH_BYTE(HIGH_WORD(((uint32_t)FBR_HIDDEN_SECTORS)));
		case 0x28:
            return LOW_BYTE(((uint16_t)FBR_EXT_FLAGS));
		case 0x29:
            return HIGH_BYTE(((uint16_t)FBR_EXT_FLAGS));
		case 0x2C:
            return LOW_BYTE(LOW_WORD(((uint32_t)FBR_ROOT_DIRECTORY_START)));
		case 0x2D:
            return HIGH_BYTE(LOW_WORD(((uint32_t)FBR_ROOT_DIRECTORY_START)));
		case 0x2E:
            LOW_BYTE(HIGH_WORD(((uint32_t)FBR_ROOT_DIRECTORY_START)));
		case 0x2F:
            return HIGH_BYTE(HIGH_WORD(((uint32_t)FBR_ROOT_DIRECTORY_START)));
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
		case 0x1FF:
            return LOW_BYTE(((uint16_t)FBR_SIGNATURE));
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
	if(idx < 0x20) // Volume information
	{
		if(idx < 11)
		{
			return ROOT_DRIVE_NAME[idx];
		}
        if(idx==11)
        {
            return 0x08;
        }
	}
	if(idx < 0x40) // Example file
	{
		idx -= 0x20;
		if(idx < 0+8)
		{
			return ROOT_FILE_NAME[idx];
		}
		if(idx < 11)
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
			case 29:
                return HIGH_BYTE(LOW_WORD(((uint32_t)ROOT_FILE_SIZE)));
			case 30:
                return LOW_BYTE(HIGH_WORD(((uint32_t)ROOT_FILE_SIZE)));
			case 31:
                return HIGH_BYTE(HIGH_WORD(((uint32_t)ROOT_FILE_SIZE)));
			default:
				return 0x00;
		}
	}
	return 0X00;
}

static uint8_t _get_data(uint16_t idx)
{
	if(idx < ROOT_FILE_SIZE) // Dummy file
	{
        return ROOT_FILE_CONTENT[idx];
	}
	return 0X00;
}

formatStatus_t fat_get_format_status(void)
{
    uint16_t cntr;
    
    //Check Master Boot Record (sector 0)
    flash_sector_read(MBR_SECTOR, buffer);
    for(cntr=0; cntr<BYTES_PER_SECTOR; ++cntr)
    {
        if(buffer[cntr] != _get_mbr(cntr))
        {
            return DRIVE_NOT_FORMATED;
        }
    }
    
    //Check First Boot Record (sector 1)
    flash_sector_read(FBR_SECTOR, buffer);
    for(cntr=0; cntr<BYTES_PER_SECTOR; ++cntr)
    {
        if(buffer[cntr] != _get_fbr(cntr))
        {
            return DRIVE_NOT_FORMATED;
        }
    }
    
    //If we get here, the drive is formated
    return DRIVE_FORMATED;
}

uint8_t fat_format(void)
{
    uint16_t cntr;
    
    //Write MBR
    for(cntr=0; cntr<BYTES_PER_SECTOR; ++cntr)
    {
        buffer[cntr] = _get_mbr(cntr);
    }
    flash_sector_write(MBR_SECTOR, buffer);
    
    //Write FBR
    for(cntr=0; cntr<BYTES_PER_SECTOR; ++cntr)
    {
        buffer[cntr] = _get_fbr(cntr);
    }
    flash_sector_write(FBR_SECTOR, buffer);
    
    //Write first FAT sector
    for(cntr=0; cntr<BYTES_PER_SECTOR; ++cntr)
    {
        buffer[cntr] = _get_fat(cntr);
    }
    flash_sector_write(FAT_FIRST_SECTOR, buffer);
    
    //Fill remaining FAT sectors (all zeros)
    for(cntr=0; cntr<BYTES_PER_SECTOR; ++cntr)
    {
        buffer[cntr] = 0x00;
    }
    for(cntr=FAT_FIRST_SECTOR+1; cntr<=FAT_LAST_SECTOR; ++cntr)
    {
        flash_sector_write(cntr, buffer);
    }
    
    //Write first root sector
    for(cntr=0; cntr<BYTES_PER_SECTOR; ++cntr)
    {
        buffer[cntr] = _get_root(cntr);
    }
    flash_sector_write(ROOT_FIRST_SECTOR, buffer);
    
    //Fill remaining root sectors (all zeros)
    for(cntr=0; cntr<BYTES_PER_SECTOR; ++cntr)
    {
        buffer[cntr] = 0x00;
    }
    for(cntr=ROOT_FIRST_SECTOR+1; cntr<=ROOT_LAST_SECTOR; ++cntr)
    {
        flash_sector_write(cntr, buffer);
    }
    
    //Write Data of hello world file
    for(cntr=0; cntr<BYTES_PER_SECTOR; ++cntr)
    {
        buffer[cntr] = _get_data(cntr);
    }
    flash_sector_write(DATA_FIRST_SECTOR, buffer);
    
    return 0x00;
}

void fat_init(void)
{
    //Format flash if necessary
    if(fat_get_format_status()==DRIVE_NOT_FORMATED)
    {
        fat_format();
    }
}

uint8_t fat_get_file_information(uint8_t file_number, rootEntry_t *data)
{
    uint16_t root_sector;
    uint16_t offset;
    
    if(file_number>=FBR_ROOT_ENTRIES)
    {
        //Error: invalid file_number
        return 0x01;
    }
    
    //Find position of root entry
    root_sector = _sector_from_file_number(file_number);
    offset = _offset_from_file_number(file_number);

    //Read file information
    flash_partial_read(root_sector, offset, 32, (uint8_t*) data);
    
    //Check if this is a valid file (e.g. not deleted)
    if((data->fileName[0]==0x00) || (data->fileName[0]==0xE5))
    {
        //Error: there is no file in that location
        return 0x02;
    }
    
    //Indicate success
    return 0x00;
}

uint8_t fat_copy_sector_to_buffer(uint8_t file_number, uint16_t sector)
{
    uint32_t file_size;
    uint16_t number_of_clusters;
    uint16_t cluster;
    uint16_t physical_sector;

    //Check if we have a valid file
    if(_root_is_available(file_number))
    {
        //Return an error
        return 0xFF;
    }
    
    //Get file size
    file_size = fat_get_file_size(file_number);
    
    //Check if sector is valid
    number_of_clusters = (uint16_t) ((file_size + BYTES_PER_SECTOR - 1) >> 9);
    if(sector >= number_of_clusters)
    {
        //Return an error
        return 0xFE;
    }
    
    //Get the right cluster
    cluster = _get_first_cluster(file_number);
    cluster = _find_nth_cluster(cluster, sector);
    
    //Find physical sector
    physical_sector = _data_sector_from_cluster(cluster);
    
    //Copy that sector to buffer 2
    flash_copy_page_to_buffer(physical_sector);
    
    //Return success
    return 0x00;
}

uint8_t fat_write_sector_from_buffer(uint8_t file_number, uint16_t sector)
{
    uint32_t file_size;
    uint16_t number_of_clusters;
    uint16_t cluster;
    uint16_t physical_sector;
    
    //Check if we have a valid file
    if(_root_is_available(file_number))
    {
        //Return an error
        return 0xFF;
    }
    
    //Get file size
    file_size = fat_get_file_size(file_number);
    
    //Check if sector is valid
    number_of_clusters = (file_size + BYTES_PER_SECTOR - 1) >> 9;
    if(sector >= number_of_clusters)
    {
        //Return an error
        return 0xFE;
    }
    
    //Get the right cluster
    cluster = _get_first_cluster(file_number);
    //cluster = _find_nth_cluster(cluster, sector);
    
    //Find physical sector
    //physical_sector = _data_sector_from_cluster(cluster);
    
    //Write buffer to that sector
    //flash_write_page_from_buffer(physical_sector);
    
    //Return success
    return 0x00;
}

void fat_read_from_buffer(uint16_t start, uint16_t length, uint8_t *data)
{
    flash_read_from_buffer(start, length, data);
}

void fat_write_to_buffer(uint16_t start, uint16_t length, uint8_t *data)
{
    flash_write_to_buffer(start, length, data);
}