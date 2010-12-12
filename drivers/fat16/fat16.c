/**
 * FAT driver
 * @file fat16.c
 * @author Christian Rupprecht
 */
#include "fat16.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sd/sd.h>

// Globals:
static struct fat16_filesystem g_filesystem;

// TEMP:
char *g_filename = "fat16.img";
typedef uint8_t (*interval_handler)(uint8_t* buffer, uint32_t offset, void* p);
/////////

#define PARTITION_TYPE_NONE         0x00    // table entry is not used.
#define PARTITION_TYPE_FAT12        0x01    // contains a FAT12 file system.
#define PARTITION_TYPE_FAT16_32MB   0x04    // contains a FAT16 file system with 32MB maximum.
#define PARTITION_TYPE_EXTENDED     0x05    // is an extended partition with its own partition table.
#define PARTITION_TYPE_FAT16        0x06    // contains a FAT16 file system.
#define PARTITION_TYPE_FAT32        0x0b    // contains a FAT32 file system.
#define PARTITION_TYPE_FAT32_LBA    0x0c    // contains a FAT32 file system with LBA.
#define PARTITION_TYPE_FAT16_LBA    0x0e    // contains a FAT16 file system with LBA.
#define PARTITION_TYPE_EXTENDED_LBA 0x0f    // is an extended partition with LBA.
#define FAT16_CLUSTER_FREE          0x0000
#define FAT16_CLUSTER_RESERVED_MIN  0xfff0
#define FAT16_CLUSTER_RESERVED_MAX  0xfff6
#define FAT16_CLUSTER_BAD           0xfff7
#define FAT16_CLUSTER_LAST_MIN      0xfff8
#define FAT16_CLUSTER_LAST_MAX      0xffff

#define FAT16_DIRENTRY_DELETED      0xe5
#define FAT16_DIRENTRY_LFNLAST      (1 << 6)
#define FAT16_DIRENTRY_LFNSEQMASK   ((1 << 6) - 1)




struct fat16_partition {
    uint8_t type; // one of PARTITION_TYPE_*
    uint32_t offset; // offset in bytes from beginning of the disk
    uint32_t length; // length of the partition
};

struct fat16_header {
    uint32_t size;
    uint32_t fat_offset;
    uint32_t fat_size;
    uint16_t sector_size;
    uint16_t cluster_size;
    uint32_t root_dir_offset;
    uint32_t cluster_zero_offset;
};

struct fat16_filesystem {
    struct fat16_partition partition;
    struct fat16_header header;
};

struct fat16_read_callback_arg {
    uint16_t entry_cur;
    uint16_t entry_num;
    uint32_t entry_offset;
    uint8_t byte_count;
};



// Internal functions
static uint8_t partition_open(uint8_t index);
static uint8_t fat16_read_header(void);

//static uint8_t fat16_read_root_dir_entry(uint16_t entry_num, struct fat16_dir_entry* dir_entry);
static uint8_t fat16_dir_entry_seek_callback(uint8_t* buffer, uint32_t offset, void* p);
static uint8_t fat16_dir_entry_read_callback(uint8_t* buffer, uint32_t offset, void* p);
static uint8_t fat16_interpret_dir_entry(struct fat16_dir_entry* dir_entry, const uint8_t* raw_entry);

static uint16_t fat16_get_next_cluster(uint16_t cluster_num);
static uint16_t fat16_append_cluster(uint16_t cluster_num);
static uint8_t fat16_free_cluster(uint16_t cluster_num);

static uint8_t storage_read_interval(uint32_t offset, uint8_t* buffer, uint16_t interval, uint16_t length, interval_handler callback, void* p);

static uint8_t storage_read_interval(uint32_t offset, uint8_t* buffer, uint16_t interval, uint16_t length, interval_handler callback, void* p)
{
    while (length >= interval)
    {
        if (sd_read_bytes(offset, buffer, interval) != 1)
            return 0;
        uint8_t i;
        if (callback(buffer, offset, p) != 1)
            break;
        offset += interval;
        length -= interval;
    }
    return 1;
}


static uint8_t partition_open(uint8_t index)
{
    uint8_t buffer[0x10];

    // read specified partition table index
    if (sd_read_bytes(0x01be + index * 0x10, buffer, sizeof(buffer)) != 1)
    {
        debug_pgm(PSTR("Read Bytes Failed!"));
        return 0;
    }
        
    // abort on empty partition entry
    if (buffer[4] == PARTITION_TYPE_NONE)
    {
        debug_pgm(PSTR("Partitiontype is NONE"));
        return 0;
    }

    // fill partition descriptor
    g_filesystem.partition.type = buffer[4];
    g_filesystem.partition.offset = ((uint32_t) buffer[8])
                                    | ((uint32_t) buffer[9] << 8)
                                    | ((uint32_t) buffer[10] << 16)
                                    | ((uint32_t) buffer[11] << 24);

    g_filesystem.partition.length = ((uint32_t) buffer[12])
                                    | ((uint32_t) buffer[13] << 8)
                                    | ((uint32_t) buffer[14] << 16)
                                    | ((uint32_t) buffer[15] << 24);

    return 1;
}

extern uint8_t fat16_init(uint8_t partition_index)
{
	if(partition_index >= 0 && partition_index <= 4)
	{
		if (partition_open(partition_index) != 1)
			return 0;
	}else{
		g_filesystem.partition.type = PARTITION_TYPE_FAT16;
		g_filesystem.partition.offset = 0;
        g_filesystem.partition.length = 0; //never used...
	}

    if (fat16_read_header() != 1)
        return 0;

    return 1;
}

static uint8_t fat16_read_header(void)
{
    uint8_t buffer[25];

    //Look for the right Partition type (FAT16)
    if (g_filesystem.partition.type != PARTITION_TYPE_FAT16 &&
            g_filesystem.partition.type != PARTITION_TYPE_FAT16_LBA &&
            g_filesystem.partition.type != PARTITION_TYPE_FAT16_32MB)
    {
        debug_pgm(PSTR("Wrong Partition type"));
        return 0;
    }

    uint32_t partition_offset = g_filesystem.partition.offset * 512;
    if (!sd_read_bytes(partition_offset + 0x0b, buffer, sizeof(buffer)))
        return 0;

//  memset(header, 0, sizeof(*header));

    uint16_t bytes_per_sector = ((uint16_t) buffer[0x00])
                                | ((uint16_t) buffer[0x01] << 8);
    uint8_t sectors_per_cluster = buffer[0x02];
    uint16_t reserved_sectors = ((uint16_t) buffer[0x03])
                                | ((uint16_t) buffer[0x04] << 8);
    uint8_t fat_copies = buffer[0x05];
    uint16_t max_root_entries = ((uint16_t) buffer[0x06])
                                | ((uint16_t) buffer[0x07] << 8);
    uint16_t sectors_per_fat = ((uint16_t) buffer[0x0b])
                               | ((uint16_t) buffer[0x0c] << 8);
    uint32_t sector_count = ((uint32_t) buffer[0x15])
                            | ((uint32_t) buffer[0x16] << 8) | ((uint32_t) buffer[0x17] << 16)
                            | ( (uint32_t) buffer[0x18] << 24);

    g_filesystem.header.size                = sector_count * bytes_per_sector;
    g_filesystem.header.fat_offset          = partition_offset + (uint32_t)reserved_sectors * bytes_per_sector;
    g_filesystem.header.fat_size            = (uint32_t)sectors_per_fat * bytes_per_sector;
    g_filesystem.header.sector_size         = bytes_per_sector;
    g_filesystem.header.cluster_size        = (uint32_t)bytes_per_sector * sectors_per_cluster;
    g_filesystem.header.root_dir_offset     = g_filesystem.header.fat_offset + (uint32_t)fat_copies * sectors_per_fat * bytes_per_sector;
    g_filesystem.header.cluster_zero_offset = g_filesystem.header.root_dir_offset + (uint32_t)max_root_entries * 32;

    return 1;
}

extern uint8_t fat16_read_root_dir_entry(uint16_t entry_num, struct fat16_dir_entry* dir_entry)
{
    if (!dir_entry)
        return 0;

    //Read root directory entry
    uint8_t buffer[32];

    //look for the file at position entry_num
    struct fat16_read_callback_arg arg;
    memset(&arg, 0, sizeof(arg));
    arg.entry_num = entry_num;

    if (storage_read_interval(
                g_filesystem.header.root_dir_offset,
                buffer,
                sizeof(buffer),
                g_filesystem.header.cluster_zero_offset - g_filesystem.header.root_dir_offset,
                fat16_dir_entry_seek_callback,
                &arg) != 1 || arg.entry_offset == 0)
    {
        return 0;
    }
    
    //Read the file
    memset(dir_entry, 0, sizeof(*dir_entry));
    if (storage_read_interval(arg.entry_offset, buffer, sizeof(buffer),
                               arg.byte_count, fat16_dir_entry_read_callback, dir_entry) != 1)
    {
        return 0;
    }

    if(dir_entry->long_name[0] == '\0')
    {
        return 0;
    }

    return 1;
}

static uint8_t fat16_dir_entry_seek_callback(uint8_t* buffer, uint32_t offset, void* p)
{
    struct fat16_read_callback_arg* arg = p;

    //skip deleted entries
    if (buffer[0] == FAT16_DIRENTRY_DELETED)
        return 1;

    if (arg->entry_cur == arg->entry_num)
    {
        arg->entry_offset = offset;
        arg->byte_count = buffer[11] == 0x0f ? ((buffer[0]
                                                & FAT16_DIRENTRY_LFNSEQMASK) + 1) * 32 : 32;
        return 0;
    }

    // if we read a 8.3 entry, we reached a new directory entry
    if (buffer[11] != 0x0f)
        ++arg->entry_cur;

    return 1;
}

static uint8_t fat16_dir_entry_read_callback(uint8_t* buffer, uint32_t offset, void* p)
{
    struct fat16_dir_entry* dir_entry = p;

    // there should not be any deleted entries */
    if (buffer[0] == FAT16_DIRENTRY_DELETED)
        return 0;

    if (!dir_entry->entry_offset)
        dir_entry->entry_offset = offset;

    switch (fat16_interpret_dir_entry(dir_entry, buffer))
    {
    case 0: // failure */
        return 0;
    case 1: // buffer successfully parsed, continue */
        return 1;
    case 2: // directory entry complete, finish */
        return 0;
    }

    return 0;
}

/*
 * Reads a directory entry of a given parent directory.
 *
 * @param in entry_num The index of the directory entry to read.
 * @param in parent Directory entry descriptor in which to read directory entry.
 * @param out dir_entry Directory entry descriptor which will get filled.
 * @returns 0 on failure, 1 on success
 */
static uint8_t fat16_read_sub_dir_entry(uint16_t entry_num, const struct fat16_dir_entry* parent, struct fat16_dir_entry* dir_entry)
{
    if (!parent || !dir_entry)
        return 0;

    // we are in a parent directory and want to search within its directory entry table
    if (!(parent->attributes & FAT16_ATTRIB_DIR))
        return 0;

    // loop through all clusters of the directory
    uint8_t buffer[32];
    uint32_t cluster_offset;
    uint16_t cluster_size = g_filesystem.header.cluster_size;
    uint16_t cluster_num = parent->cluster;
    struct fat16_read_callback_arg arg;

    while (1)
    {
        // calculate new cluster offset
        cluster_offset = g_filesystem.header.cluster_zero_offset
                         + (uint32_t) (cluster_num - 2) * cluster_size;

        // seek to the n-th entry
        memset(&arg, 0, sizeof(arg));
        arg.entry_num = entry_num;
        if (!storage_read_interval(cluster_offset, buffer, sizeof(buffer), cluster_size, fat16_dir_entry_seek_callback, &arg))
            return 0;

        // check if we found the entry
        if (arg.entry_offset)
            break;

        // get number of next cluster
        if (!(cluster_num = fat16_get_next_cluster(cluster_num)))
            return 0; // directory entry not found
    }

    memset(dir_entry, 0, sizeof(*dir_entry));

    // read entry
    if (!storage_read_interval(arg.entry_offset, buffer, sizeof(buffer),
                               arg.byte_count, fat16_dir_entry_read_callback, dir_entry))
        return 0;

    return dir_entry->long_name[0] != '\0' ? 1 : 0;
}

/* returns:
 * 0 failure
 * 1 long file name entry
 * 2 complete
 */
static uint8_t fat16_interpret_dir_entry(struct fat16_dir_entry* dir_entry,
        const uint8_t* raw_entry)
{
    if (!dir_entry || !raw_entry || !raw_entry[0])
        return 0;

    char* long_name = dir_entry->long_name;
    if (raw_entry[11] == 0x0f)
    {
        uint16_t char_offset = ((raw_entry[0] & 0x3f) - 1) * 13;

        if (char_offset + 12 < sizeof(dir_entry->long_name))
        {
            //No unicode Support! Read every 2nd Byte as ASCII
            long_name[char_offset + 0] = raw_entry[1];
            long_name[char_offset + 1] = raw_entry[3];
            long_name[char_offset + 2] = raw_entry[5];
            long_name[char_offset + 3] = raw_entry[7];
            long_name[char_offset + 4] = raw_entry[9];
            long_name[char_offset + 5] = raw_entry[14];
            long_name[char_offset + 6] = raw_entry[16];
            long_name[char_offset + 7] = raw_entry[18];
            long_name[char_offset + 8] = raw_entry[20];
            long_name[char_offset + 9] = raw_entry[22];
            long_name[char_offset + 10] = raw_entry[24];
            long_name[char_offset + 11] = raw_entry[28];
            long_name[char_offset + 12] = raw_entry[30];
        }

        return 1; //Long File Name entry
    }
    else
    {
        //Check if long filename
        if (long_name[0] == '\0')
        {
            uint8_t i;
            for (i = 0; i < 8; ++i)
            {
                if (raw_entry[i] == ' ')
                    break;
                long_name[i] = raw_entry[i];
            }
            if (raw_entry[8] != ' ')
            {
                long_name[i++] = '.';

                uint8_t j = 8;
                for (; j < 11; ++j)
                {
                    if (raw_entry[j] != ' ')
                    {
                        long_name[i++] = raw_entry[j];
                    }
                    else
                    {
                        break;
                    }
                }
            }

            long_name[i] = '\0';
        }

        dir_entry->attributes = raw_entry[11];
        dir_entry->cluster = ((uint16_t) raw_entry[26])
                             | ((uint16_t) raw_entry[27] << 8);
        dir_entry->file_size = ((uint32_t) raw_entry[28])
                               | ((uint32_t) raw_entry[29] << 8) | ((uint32_t) raw_entry[30]
                                                                    << 16) | ((uint32_t) raw_entry[31] << 24);

        return 2;
    }
}

extern uint8_t fat16_get_dir_entry_of_path(const char* path, struct fat16_dir_entry* dir_entry)
{
    if (!path || path[0] == '\0' || !dir_entry)
        return 0;

    if (path[0] == '/')
        ++path;

    // begin with the root directory
    memset(dir_entry, 0, sizeof(*dir_entry));
    dir_entry->attributes = FAT16_ATTRIB_DIR;
    if (path[0] == '\0')
        return 1;

    while (1)
    {
        struct fat16_dir dd;
        if (fat16_open_dir(&dd, dir_entry) != 1)
        {
            break;
        }
        // extract the next hierarchy we will search for
        const char* sep_pos = strchr(path, '/');
        if (!sep_pos)
            sep_pos = path + strlen(path);
        uint8_t length_to_sep = sep_pos - path;

        // read directory entries
        while (fat16_read_dir(&dd, dir_entry))
        {
            // check if we have found the next hierarchy
            if ((strlen(dir_entry->long_name) != length_to_sep || strncmp(path, dir_entry->long_name, length_to_sep) != 0))
                continue;

            //fat16_close_dir(dd);
            //dd = 0;

            if (path[length_to_sep] == '\0')
                return 1; // we iterated through the whole path and have found the file

            if (dir_entry->attributes & FAT16_ATTRIB_DIR)
            {
                // we found a parent directory of the file we are searching for
                path = sep_pos + 1;
                break;
            }

            // a parent of the file exists, but not the file itself
            return 0;
        }

        //fat16_close_dir(dd);
    }

    return 0;
}

/*
 * Retrieves the next following cluster of a given cluster.
 *
 * Using the filesystem file allocation table, this function returns
 * the number of the cluster containing the data directly following
 * the data within the cluster with the given number.
 *
 * @param in cluster_num The number of the cluster for which to determine its successor.
 * @returns The wanted cluster number, or 0 on error.
 */
static uint16_t fat16_get_next_cluster(uint16_t cluster_num)
{
    if (cluster_num < 2)
        return 0;

    // read appropriate fat entry
    uint8_t fat_entry[2];
    if (!sd_read_bytes(g_filesystem.header.fat_offset + 2 * cluster_num, fat_entry,2))
        return 0;

    // determine next cluster from fat
    cluster_num = ((uint16_t) fat_entry[0]) | ((uint16_t) fat_entry[1] << 8);

    if (cluster_num == FAT16_CLUSTER_FREE
            || cluster_num == FAT16_CLUSTER_BAD
            || (cluster_num >= FAT16_CLUSTER_RESERVED_MIN
                && cluster_num  <= FAT16_CLUSTER_RESERVED_MAX)
            || (cluster_num >= FAT16_CLUSTER_LAST_MIN
                && cluster_num <= FAT16_CLUSTER_LAST_MAX))
        return 0;

    return cluster_num;
}

/*
 * Appends a cluster to an existing cluster chain.
 *
 * Set cluster_num to zero to allocate the first cluster
 * within the chain.
 *
 * @param in cluster_num The cluster to which to append a free one.
 * @returns 0 on failure, the number of the new cluster on success.
 */
static uint16_t fat16_append_cluster(uint16_t cluster_num)
{
    uint32_t fat_offset_from = g_filesystem.header.fat_offset;
    uint32_t fat_offset_to   = fat_offset_from + g_filesystem.header.fat_size;
    uint32_t fat_offset      = fat_offset_from;
    uint16_t cluster_new     = 0;
    uint8_t buffer[2];

    while (1)
    {
        if (!sd_read_bytes(fat_offset, buffer, sizeof(buffer)))
            return 0;

        // check if this is a free cluster
        if (buffer[0] == (FAT16_CLUSTER_FREE & 0xff) &&
                buffer[1] == ((FAT16_CLUSTER_FREE >> 8) & 0xff))
            break;

        ++cluster_new;
        fat_offset += sizeof(buffer);

        // abort if we reached the end of the fat
        if (fat_offset >= fat_offset_to)
            return 0;
    }

    buffer[0] = FAT16_CLUSTER_LAST_MAX & 0xff;
    buffer[1] = (FAT16_CLUSTER_LAST_MAX >> 8) & 0xff;
    if (!sd_write_bytes(fat_offset_from + 2 * cluster_new, buffer, sizeof(buffer)))
        return 0;

    if (cluster_num >= 2)
    {
        buffer[0] = cluster_new & 0xff;
        buffer[1] = (cluster_new >> 8) & 0xff;
        sd_write_bytes(fat_offset_from + 2 * cluster_num, buffer, sizeof(buffer));
    }

    return cluster_new;
}

/*
 * Marks the specified cluster as free. It may then again be
 * used for future file allocations.
 *
 * @param in cluster_num The cluster which to free.
 * @returns 0 on failure, 1 on success.
 */
static uint8_t fat16_free_cluster(uint16_t cluster_num)
{
    if (cluster_num < 2)
        return 0;

    uint8_t buffer[2];

    buffer[0] = FAT16_CLUSTER_FREE & 0xff;
    buffer[1] = (FAT16_CLUSTER_FREE >> 8) & 0xff;

    return sd_write_bytes(g_filesystem.header.fat_offset + 2*cluster_num, buffer, 2);
}


extern uint8_t fat16_open_file(struct fat16_file* file, const struct fat16_dir_entry* dir_entry)
{
    if (!dir_entry || (dir_entry->attributes & FAT16_ATTRIB_DIR) || !file)
        return 0;

    memcpy(&file->dir_entry, dir_entry, sizeof(*dir_entry));
    file->pos = 0;

    return 1;
}

extern uint8_t fat16_open_file_by_name(struct fat16_file* file, const char* filename)
{
    if (!file || !filename || !filename[0])
        return 0;

    if (fat16_get_dir_entry_of_path(filename, &file->dir_entry) == 0)
    {
        return 0;
    }
    
    if (file->dir_entry.attributes & FAT16_ATTRIB_DIR)
        return 0;

    file->pos = 0;

    return 1;
}

extern int16_t fat16_read_file(struct fat16_file* fd, uint8_t* buffer, uint16_t buffer_len)
{
    // check arguments
    if (!fd || !buffer || buffer_len < 1)
        return -1;

    // determine number of bytes to read
    if (fd->pos + buffer_len > fd->dir_entry.file_size)
        buffer_len = fd->dir_entry.file_size - fd->pos;

    if (buffer_len == 0)
        return 0;

    uint16_t cluster_size = g_filesystem.header.cluster_size;
    uint16_t cluster_num = fd->dir_entry.cluster;
    uint16_t buffer_left = buffer_len;
    uint16_t first_cluster_offset;

    // find cluster in which to start reading
    if (cluster_num)
    {
        uint32_t pos = fd->pos;
        while (pos >= cluster_size)
        {
            pos -= cluster_size;
            cluster_num = fat16_get_next_cluster(cluster_num);
            if (!cluster_num)
                return -1;
        }

        first_cluster_offset = pos;
    }
    else
    {
        return -1;
    }

    // read data
    while (1)
    {
        // calculate data size to copy from cluster
        uint32_t cluster_offset = g_filesystem.header.cluster_zero_offset
                                  + (uint32_t) (cluster_num - 2) * cluster_size
                                  + first_cluster_offset;
        uint16_t copy_length = cluster_size - first_cluster_offset;
        if (copy_length > buffer_left)
            copy_length = buffer_left;

        // read data
        if (!sd_read_bytes(cluster_offset, buffer, copy_length))
            return buffer_len - buffer_left;

        // calculate new file position
        buffer += copy_length;
        buffer_left -= copy_length;
        fd->pos += copy_length;

        // check if we are done
        if (buffer_left == 0)
            break;

        // we are on a cluster boundary, so get the next cluster
        if ((cluster_num = fat16_get_next_cluster(cluster_num)))
            first_cluster_offset = 0;
        else
            return buffer_len - buffer_left;
    }

    return buffer_len;
}

extern int16_t fat16_write_file(struct fat16_file* fd, uint8_t* buffer, uint16_t buffer_len)
{
    // check arguments
    if (!fd || !buffer || buffer_len < 1)
        return -1;
    if (fd->pos > fd->dir_entry.file_size)
        return -1;

    uint16_t cluster_size = g_filesystem.header.cluster_size;
    uint16_t cluster_num = fd->dir_entry.cluster;
    uint16_t buffer_left = buffer_len;
    uint16_t first_cluster_offset = 0;

    // find cluster in which to start writing
    if (cluster_num)
    {
        uint32_t pos = fd->pos;
        while (pos >= cluster_size)
        {
            pos -= cluster_size;
            cluster_num = fat16_get_next_cluster(cluster_num);
            if (!cluster_num && pos == 0)
                // the file exactly ends on a cluster boundary, and we append to it
                cluster_num = fat16_append_cluster(cluster_num);
            if (!cluster_num)
                return -1;
        }

        first_cluster_offset = pos;
    }
    else
    {
        fd->dir_entry.cluster = cluster_num = fat16_append_cluster(0);
        if (!cluster_num)
            return -1;
    }

    // write data
    while (1)
    {
        // calculate data size to write to cluster
        uint32_t cluster_offset = g_filesystem.header.cluster_zero_offset
                                  + (uint32_t) (cluster_num - 2) * cluster_size
                                  + first_cluster_offset;
        uint16_t write_length = cluster_size - first_cluster_offset;
        if (write_length > buffer_left)
            write_length = buffer_left;

        // write data which fits into the current cluster
        if (!sd_write_bytes(cluster_offset, buffer, write_length))
            break;

        // calculate new file position
        buffer += write_length;
        buffer_left -= write_length;
        fd->pos += write_length;

        // check if we are done
        if (buffer_left == 0)
            break;

        // we are on a cluster boundary, so get the next cluster
        uint16_t cluster_num_next = fat16_get_next_cluster(cluster_num);
        if (!cluster_num_next)
        {
            cluster_num_next = fat16_append_cluster(cluster_num);
            if (!cluster_num_next)
                break;
        }
        cluster_num = cluster_num_next;

        first_cluster_offset = 0;
    }

    // update directory entry
    if (fd->pos > fd->dir_entry.file_size)
    {
        uint32_t size_old = fd->dir_entry.file_size;

        // update file size
        fd->dir_entry.file_size = fd->pos;
        // write directory entry
        if (!fat16_write_dir_entry(&fd->dir_entry))
        {
            /* do not return an error here since we actually wrote
             * some data to disk. So we calculate the amount of data
             * we wrote to disk and which lies within the old file size.
             */
            buffer_left = fd->pos - size_old;
            fd->pos = size_old;
        }
    }

    return buffer_len - buffer_left;
}

extern uint8_t fat16_seek_file(struct fat16_file* fd, int32_t* offset, uint8_t whence)
{
    if (!fd || !offset)
        return 0;

    uint32_t new_pos = fd->pos;
    switch (whence)
    {
    case FAT16_SEEK_SET:
        new_pos = *offset;
        break;
    case FAT16_SEEK_CUR:
        new_pos += *offset;
        break;
    case FAT16_SEEK_END:
        new_pos = fd->dir_entry.file_size + *offset;
        break;
    default:
        return 0;
    }

    if (new_pos > fd->dir_entry.file_size && !fat16_resize_file(fd, new_pos))
        return 0;

    fd->pos = new_pos;
    *offset = new_pos;
    return 1;
}

extern uint8_t fat16_resize_file(struct fat16_file* fd, uint32_t size)
{
    if (!fd)
        return 0;

    uint16_t cluster_num = fd->dir_entry.cluster;
    uint16_t cluster_size = g_filesystem.header.cluster_size;
    uint32_t size_new = size;

    if (fd->dir_entry.file_size < size)
    {
        // check if the file owns a cluster
        if (cluster_num == 0)
        {
            // allocate first cluster
            if (!(cluster_num = fat16_append_cluster(cluster_num)))
                return 0;
            fd->dir_entry.cluster = cluster_num;
        }
        else
        {
            // get last cluster of file
            uint16_t cluster_num_next;
            while ((cluster_num_next = fat16_get_next_cluster(cluster_num)))
            {
                cluster_num = cluster_num_next;
                size_new -= cluster_size;
            }
        }

        // append new clusters as needed
        if (size_new > cluster_size)
        {
            while (1)
            {
                if (!(cluster_num = fat16_append_cluster(cluster_num)))
                    return 0;

                if (size_new <= cluster_size)
                    break;

                size_new -= cluster_size;
            }
        }

        // write new directory entry
        fd->dir_entry.file_size = size;
        if (!fat16_write_dir_entry(&fd->dir_entry))
            return 0;
    }
    else if (fd->dir_entry.file_size > size)
    {
        // write new directory entry
        fd->dir_entry.file_size = size;
        if (size == 0)
            fd->dir_entry.cluster = 0;
        if (!fat16_write_dir_entry(&fd->dir_entry))
            return 0;

        // get cluster where the file is cut off
        while (size_new > cluster_size)
        {
            if (!(cluster_num = fat16_get_next_cluster(cluster_num)))
                return 0;

            size_new -= cluster_size;
        }

        // free all clusters no longer needed
        if (size == 0 || (cluster_num = fat16_get_next_cluster(cluster_num)))
        {
            while (cluster_num)
            {
                // get next cluster before freeing the previous one
                uint16_t cluster_num_next = fat16_get_next_cluster(cluster_num);

                // delete cluster */
                fat16_free_cluster(cluster_num);

                cluster_num = cluster_num_next;
            }
        }
    }

    return 1;
}

extern uint8_t fat16_open_dir(struct fat16_dir* dir, const struct fat16_dir_entry* dir_entry)
{
    if (!dir_entry)
    {
        return 0;
    }
    if((dir_entry->attributes & FAT16_ATTRIB_DIR) == 0)
    {
        return 0;
    }
    if(!dir)
    {
        return 0;
    }

    memcpy(&dir->dir_entry, dir_entry, sizeof(*dir_entry));
    dir->entry_next = 0;

    return 1;
}

extern uint8_t fat16_read_dir(struct fat16_dir* dd, struct fat16_dir_entry* dir_entry)
{
    if (!dd || !dir_entry)
        return 0;

    if (dd->dir_entry.cluster == 0)
    {
        // read entry from root directory
        if (fat16_read_root_dir_entry(dd->entry_next, dir_entry))
        {
            ++dd->entry_next;
            return 1;
        }
    }
    else
    {
        // read entry from a subdirectory
        if (fat16_read_sub_dir_entry(dd->entry_next, &dd->dir_entry,
                                     dir_entry))
        {
            ++dd->entry_next;
            return 1;
        }
    }

    // restart reading */
    dd->entry_next = 0;

    return 0;
}

extern uint8_t fat16_reset_dir(struct fat16_dir* dd)
{
    if (!dd)
        return 0;

    dd->entry_next = 0;
    return 1;
}

extern uint8_t fat16_write_dir_entry(const struct fat16_dir_entry* dir_entry)
{
    if (!dir_entry)
        return 0;

    uint32_t offset = dir_entry->entry_offset;
    uint8_t name_len = strlen(dir_entry->long_name);
    uint8_t lfn_entry_count = (name_len + 12) / 13;
    uint8_t buffer[32];

    // write 8.3 entry

    memset(&buffer[0], ' ', 11);
    char* name_ext = strrchr(dir_entry->long_name, '.');
    if (name_ext)
    {
        ++name_ext;

        uint8_t name_ext_len = strlen(name_ext);
        name_len -= name_ext_len + 1;

        if (name_ext_len > 3)
            name_ext_len = 3;

        memcpy(&buffer[8], name_ext, name_ext_len);
    }

    if (name_len <= 8)
    {
        memcpy(buffer, dir_entry->long_name, name_len);
    }
    else
    {
        memcpy(buffer, dir_entry->long_name, 8);

        // Minimize 8.3 name clashes by appending the lower byte of the cluster number.
        uint8_t num = dir_entry->cluster & 0xff;

        buffer[6] = (num < 0xa0) ? ('0' + (num >> 4)) : ('a' + (num >> 4));
        num &= 0x0f;
        buffer[7] = (num < 0x0a) ? ('0' + num) : ('a' + num);
    }

    // fill directory entry buffer
    memset(&buffer[11], 0, sizeof(buffer) - 11);
    buffer[0x0b] = dir_entry->attributes;
    buffer[0x1a] = (dir_entry->cluster >> 0) & 0xff;
    buffer[0x1b] = (dir_entry->cluster >> 8) & 0xff;
    buffer[0x1c] = (dir_entry->file_size >> 0) & 0xff;
    buffer[0x1d] = (dir_entry->file_size >> 8) & 0xff;
    buffer[0x1e] = (dir_entry->file_size >> 16) & 0xff;
    buffer[0x1f] = (dir_entry->file_size >> 24) & 0xff;

    // write to disk
    if (!sd_write_bytes(offset + (uint32_t) lfn_entry_count * 32, buffer, sizeof(buffer)))
        return 0;

    // calculate checksum of 8.3 name
    uint8_t checksum = buffer[0];
    uint8_t i;
    for (i = 1; i < 11; ++i)
        checksum = ((checksum >> 1) | (checksum << 7)) + buffer[i];

    // write lfn entries
    uint8_t lfn_entry;
    for (lfn_entry = lfn_entry_count; lfn_entry > 0; --lfn_entry)
    {
        memset(buffer, 0, sizeof(buffer));
        memset(&buffer[0x01], 0xff, 10);
        memset(&buffer[0x0e], 0xff, 12);
        memset(&buffer[0x1c], 0xff, 4);

        buffer[0x00] = lfn_entry;
        if (lfn_entry == lfn_entry_count)
            buffer[0x00] |= FAT16_DIRENTRY_LFNLAST;

        // set file name
        const char* long_name_curr = dir_entry->long_name + (lfn_entry - 1)*13;
        uint8_t i = 1;
        while (i < 0x1f)
        {
            buffer[i++] = *long_name_curr;
            buffer[i++] = 0;

            switch (i)
            {
            case 0x0b:
                i = 0x0e;
                break;
            case 0x1a:
                i = 0x1c;
                break;
            }

            if (!*long_name_curr++)
                break;
        }

        // mark as lfn entry
        buffer[0x0b] = 0x0f;

        // set checksum
        buffer[0x0d] = checksum;

        // write entry
        sd_write_bytes(offset, buffer, sizeof(buffer));

        offset += sizeof(buffer);
    }

    return 1;
}

extern uint8_t fat16_create_file(struct fat16_dir* parent, const char* file, struct fat16_dir_entry* dir_entry)
{
    if (!parent || !file || !file[0])
        return 0;

    // check if the file already exists
    while (1)
    {
        if (!fat16_read_dir(parent, dir_entry))
            break;

        if (strcmp(file, dir_entry->long_name) == 0)
        {
            fat16_reset_dir(parent);
            return 1;
        }
    }

    memset(dir_entry, 0, sizeof(*dir_entry));

    strncpy(dir_entry->long_name, file, sizeof(dir_entry->long_name));
    dir_entry->long_name[sizeof(dir_entry->long_name)-1] = '\0';

    // search for a place where to write the directory entry to disk
    uint8_t free_dir_entries_needed = strlen(file) / 13 + 1 + 1;
    uint8_t free_dir_entries_found = 0;
    uint16_t cluster_num = parent->dir_entry.cluster;
    uint32_t dir_entry_offset = 0;
    uint32_t offset = 0;
    uint32_t offset_to = 0;

    if (cluster_num == 0)
    {
        // we read/write from the root directory entry
        offset = g_filesystem.header.root_dir_offset;
        offset_to = g_filesystem.header.cluster_zero_offset;
        dir_entry_offset = offset;
    }

    while (1)
    {
        if (offset == offset_to)
        {
            if (cluster_num == 0)
                // We iterated through the whole root directory entry and could not find enough space for the directory entry.
                return 0;

            if (offset)
            {
                // We reached a cluster boundary and have to switch to the next cluster.

                uint16_t cluster_next = fat16_get_next_cluster(cluster_num);
                if (!cluster_next)
                {
                    cluster_next = fat16_append_cluster(cluster_num);
                    if (!cluster_next)
                        return 0;

                    // we appended a new cluster and know it is free
                    dir_entry_offset = g_filesystem.header.cluster_zero_offset + (uint32_t) (cluster_next - 2)*g_filesystem.header.cluster_size;
                    break;
                }
                cluster_num = cluster_next;
            }

            offset = g_filesystem.header.cluster_zero_offset + (uint32_t) (cluster_num-2) * g_filesystem.header.cluster_size;
            offset_to = offset + g_filesystem.header.cluster_size;
            dir_entry_offset = offset;
            free_dir_entries_found = 0;
        }

        // read next lfn or 8.3 entry
        uint8_t first_char;
        if (!sd_read_bytes(offset, &first_char, sizeof(first_char)))
            return 0;

        // check if we found a free directory entry
        if (first_char == FAT16_DIRENTRY_DELETED || !first_char)
        {
            if (!first_char)
            {
                // Mark the directory entries as deleted. This is needed if our new directory entry is too large to fit into the current cluster.
                first_char = FAT16_DIRENTRY_DELETED;
                if (!sd_write_bytes(offset, &first_char, sizeof(first_char)))
                    return 0;
            }

            // check if we have the needed number of available entries
            ++free_dir_entries_found;
            if (free_dir_entries_found >= free_dir_entries_needed)
                break;

            offset += 32;
        }
        else
        {
            offset += 32;
            dir_entry_offset = offset;
            free_dir_entries_found = 0;
        }
    }

    // write directory entry to disk */
    dir_entry->entry_offset = dir_entry_offset;
    if (!fat16_write_dir_entry(dir_entry))
        return 0;

    return 1;
}

extern uint8_t fat16_delete_file(struct fat16_dir_entry* dir_entry)
{
    if (!dir_entry)
        return 0;

    // get offset of the file's directory entry
    uint32_t dir_entry_offset = dir_entry->entry_offset;
    if (!dir_entry_offset)
        return 0;

    uint8_t buffer[12];
    while (1)
    {
        // read directory entry
        if (!sd_read_bytes(dir_entry_offset, buffer, sizeof(buffer)))
            return 0;

        // mark the directory entry as deleted
        buffer[0] = FAT16_DIRENTRY_DELETED;

        // write back entry
        if (!sd_write_bytes(dir_entry_offset, buffer, sizeof(buffer)))
            return 0;

        // check if we deleted the whole entry
        if (buffer[11] != 0x0f)
            break;

        dir_entry_offset += 32;
    }

    // We deleted the directory entry. The next thing to do is marking all occupied clusters as free.
    uint16_t cluster_num = dir_entry->cluster;
    uint16_t cluster_num_next;
    while (cluster_num)
    {
        // get next cluster before freeing the previous one
        cluster_num_next = fat16_get_next_cluster(cluster_num);

        // free cluster
        fat16_free_cluster(cluster_num);

        cluster_num = cluster_num_next;
    }

    return 1;
}
