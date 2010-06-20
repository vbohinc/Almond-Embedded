#include <stdlib.h>
#include <stdint.h>

#define PARTITION_TYPE_NONE         0x00	// table entry is not used.
#define PARTITION_TYPE_FAT12        0x01	// contains a FAT12 file system.
#define PARTITION_TYPE_FAT16_32MB   0x04	// contains a FAT16 file system with 32MB maximum.
#define PARTITION_TYPE_EXTENDED     0x05	// is an extended partition with its own partition table.
#define PARTITION_TYPE_FAT16        0x06	// contains a FAT16 file system.
#define PARTITION_TYPE_FAT32        0x0b	// contains a FAT32 file system.
#define PARTITION_TYPE_FAT32_LBA    0x0c	// contains a FAT32 file system with LBA.
#define PARTITION_TYPE_FAT16_LBA    0x0e	// contains a FAT16 file system with LBA.
#define PARTITION_TYPE_EXTENDED_LBA 0x0f	// is an extended partition with LBA.
#define FAT16_CLUSTER_FREE 			0x0000
#define FAT16_CLUSTER_RESERVED_MIN 	0xfff0
#define FAT16_CLUSTER_RESERVED_MAX 	0xfff6
#define FAT16_CLUSTER_BAD 			0xfff7
#define FAT16_CLUSTER_LAST_MIN 		0xfff8
#define FAT16_CLUSTER_LAST_MAX 		0xffff

#define FAT16_DIRENTRY_DELETED 		0xe5
#define FAT16_DIRENTRY_LFNLAST 		(1 << 6)
#define FAT16_DIRENTRY_LFNSEQMASK 	((1 << 6) - 1)

#define FAT16_SEEK_SET 				0	// The given offset is relative to the beginning of the file.
#define FAT16_SEEK_CUR 				1	// The given offset is relative to the current read/write position.
#define FAT16_SEEK_END 				2	// The given offset is relative to the end of the file.
#define FAT16_ATTRIB_READONLY 		(1 << 0) 	// The file is read-only.
#define FAT16_ATTRIB_HIDDEN 		(1 << 1)	// The file is hidden.
#define FAT16_ATTRIB_SYSTEM 		(1 << 2)	// The file is a system file.
#define FAT16_ATTRIB_VOLUME 		(1 << 3)	// The file is empty and has the volume label as its name.
#define FAT16_ATTRIB_DIR 			(1 << 4)	// The file is a directory.
#define FAT16_ATTRIB_ARCHIVE 		(1 << 5)	// The file has to be archived.
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
	struct fat16_partition* partition;
	struct fat16_header header;
};

struct fat16_read_callback_arg {
	uint16_t entry_cur;
	uint16_t entry_num;
	uint32_t entry_offset;
	uint8_t byte_count;
};

struct fat16_dir_entry {
	char long_name[32]; //file name, 31 characters.
	uint8_t attributes; //file attributes. one of FAT16_ATTRIB_*
	uint16_t cluster; //cluster in which the file starts
	uint32_t file_size; //file size
	uint32_t entry_offset; //total offset of this entry
};

struct fat16_file {
	struct fat16_filesystem* fs;
	struct fat16_dir_entry dir_entry;
	uint32_t pos;
};

struct fat16_dir {
	struct fat16_filesystem* fs;
	struct fat16_dir_entry dir_entry;
	uint16_t entry_next;
};

struct fat16_partition* partition_open(uint8_t index);
uint8_t partition_close(struct fat16_partition* partition);

struct fat16_filesystem *fat16_open(struct fat16_partition *partition);
void fat16_close(struct fat16_filesystem *fs);
uint8_t fat16_read_header(struct fat16_filesystem* fs);
uint8_t fat16_read_root_dir_entry(const struct fat16_filesystem* fs,
		uint16_t entry_num, struct fat16_dir_entry* dir_entry);
uint8_t
		fat16_dir_entry_seek_callback(uint8_t* buffer, uint32_t offset, void* p);
uint8_t
		fat16_dir_entry_read_callback(uint8_t* buffer, uint32_t offset, void* p);
uint8_t fat16_interpret_dir_entry(struct fat16_dir_entry* dir_entry,
		const uint8_t* raw_entry);
uint8_t fat16_get_dir_entry_of_path(struct fat16_filesystem* fs,
		const char* path, struct fat16_dir_entry* dir_entry);
uint16_t fat16_get_next_cluster(const struct fat16_filesystem* fs,
		uint16_t cluster_num);
uint16_t fat16_append_cluster(const struct fat16_filesystem* fs,
		uint16_t cluster_num);
uint8_t fat16_free_cluster(struct fat16_filesystem* fs, uint16_t cluster_num);
struct fat16_file* fat16_open_file(struct fat16_filesystem* fs,
		const struct fat16_dir_entry* dir_entry);
void fat16_close_file(struct fat16_file* fd);
int16_t fat16_read_file(struct fat16_file* fd, uint8_t* buffer,
		uint16_t buffer_len);
int16_t fat16_write_file(struct fat16_file* fd, const uint8_t* buffer,
		uint16_t buffer_len);
uint8_t fat16_seek_file(struct fat16_file* fd, int32_t* offset, uint8_t whence);
uint8_t fat16_resize_file(struct fat16_file* fd, uint32_t size);
struct fat16_dir* fat16_open_dir(struct fat16_filesystem* fs,
		const struct fat16_dir_entry* dir_entry);
void fat16_close_dir(struct fat16_dir* dd);
uint8_t fat16_read_dir(struct fat16_dir* dd, struct fat16_dir_entry* dir_entry);
uint8_t fat16_reset_dir(struct fat16_dir* dd);
uint8_t fat16_write_dir_entry(struct fat16_filesystem* fs,
		const struct fat16_dir_entry* dir_entry);
uint8_t fat16_create_file(struct fat16_dir* parent, const char* file,
		struct fat16_dir_entry* dir_entry);
uint8_t fat16_delete_file(struct fat16_filesystem* fs,
		struct fat16_dir_entry* dir_entry);
