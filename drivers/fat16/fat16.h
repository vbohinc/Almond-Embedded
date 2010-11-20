/**
 * FAT driver
 * @file fat16.h
 * @addtogroup fat16 FAT16 Driver
 * @author Christian Rupprecht
 */

/**@{*/
#ifndef FAT16_H_
#define FAT16_H_


#include <stdint.h>

#define FAT16_SEEK_SET          0           // The given offset is relative to the beginning of the file.
#define FAT16_SEEK_CUR          1           // The given offset is relative to the current read/write position.
#define FAT16_SEEK_END          2           // The given offset is relative to the end of the file.

#define FAT16_ATTRIB_READONLY   (1 << 0)    // The file is read-only.
#define FAT16_ATTRIB_HIDDEN     (1 << 1)    // The file is hidden.
#define FAT16_ATTRIB_SYSTEM     (1 << 2)    // The file is a system file.
#define FAT16_ATTRIB_VOLUME     (1 << 3)    // The file is empty and has the volume label as its name.
#define FAT16_ATTRIB_DIR        (1 << 4)    // The file is a directory.
#define FAT16_ATTRIB_ARCHIVE    (1 << 5)    // The file has to be archived.

/** fat16 entry structure */
struct fat16_dir_entry {
    char long_name[32];     ///<file name, 31 characters.
    uint8_t attributes;     ///<file attributes. one of FAT16_ATTRIB_*
    uint16_t cluster;       ///<cluster in which the file starts
    uint32_t file_size;     ///<file size
    uint32_t entry_offset;  ///<total offset of this entry
};

/** fat16 file structure */
struct fat16_file {
    struct fat16_dir_entry dir_entry; ///< entry structure for the file
    uint32_t pos; ///< current fd position
};

/** fat16 firectory structure */
struct fat16_dir {
    struct fat16_dir_entry dir_entry; ///< entry struct for the directory
    uint16_t entry_next; ///< next entry in directory number
};

/**
 * initializes the fat16 filesystem.
 *
 * @param in partition_index Id of the partition to open. First ist 0
 * @returns 1 on success, 0 on failure.
 */
extern uint8_t fat16_init(uint8_t partition_index);

/**
 * Retrieves the directory entry of a path.
 *
 * @param in path The path of which to read the directory entry.
 * @param out dir_entry The directory entry to fill.
 * @returns 0 on failure, 1 on success.
 */
extern uint8_t fat16_get_dir_entry_of_path(const char* path, struct fat16_dir_entry* dir_entry);

/**
 * Opens a file on a FAT16 filesystem.
 *
 * @param in out the file handle
 * @param in dir_entry The directory entry of the file to open.
 * @returns 1 on success, or 0 on failure.
 */
extern uint8_t fat16_open_file(struct fat16_file* file, const struct fat16_dir_entry* dir_entry);

/**
 * Open a file by an absolute path
 *
 * @param in out file the file handle
 * @param in filename the filename of the file
 * @returns 1 on success, 0 failure
 */
extern uint8_t fat16_open_file_by_name(struct fat16_file* file, const char* filename);

/**
 * Reads data from a file
 *
 * @param in fd The file handle
 * @param out buffer The buffer into which to write.
 * @param in buffer_len The amount of data to read.
 * @returns The number of bytes read, 0 on end of file, or -1 on failure.
 */
extern int16_t fat16_read_file(struct fat16_file* fd, uint8_t* buffer,  uint16_t buffer_len);

/**
 * Writes data to the current file location.
 *
 * @param in fd The file handle
 * @param in buffer The buffer from which to read the data to be written.
 * @param in buffer_len The amount of data to write.
 * @returns The number of bytes written, 0 on disk full, or -1 on failure.
 */
extern int16_t fat16_write_file(struct fat16_file* fd, uint8_t* buffer, uint16_t buffer_len);

/**
 * Changes the file offset
 *
 * If the new offset is beyond the end of the file, fat16_resize_file()
 * is implicitly called, i.e. the file is expanded.
 *
 * - FAT16_SEEK_SET:  *offset is relative to the beginning of the file.
 * - FAT16_SEEK_CUR:  *offset is relative to the current file position.
 * - FAT16_SEEK_END:  *offset is relative to the end of the file.
 *
 * The resulting absolute offset is written to the location the  offset
 * parameter points to.
 *
 * @param in fd The file decriptor
 * @param in,out offset A pointer to the new offset, as affected by the whence
 *                   parameter. The function writes the new absolute offset
 *                   to this location before it returns.
 * @param in whence Affects the way offset is interpreted, see above.
 * @returns 0 on failure, 1 on success.
 */
extern uint8_t fat16_seek_file(struct fat16_file* fd, int32_t* offset, uint8_t whence);

/**
 * Enlarges or shrinks the file pointed to by the file descriptor to have
 * exactly the specified size.
 *
 * If the file is truncated, all bytes having an equal or larger offset
 * than the given size are lost. If the file is expanded, the additional
 * bytes are allocated, but they keep their values.
 *
 * @param in fd The file decriptor
 * @param in size
 * @returns 0 on failure, 1 on success.
 */
extern uint8_t fat16_resize_file(struct fat16_file* fd, uint32_t size);

/**
 * Creates a file and obtains the directory entry of the
 * new file. If the file to create already exists, the
 * directory entry of the existing file will be returned
 * within the dir_entry parameter.
 *
 * The file name is not checked for invalid characters.
 *
 * @param in parent The handle of the directory in which to create the file
 * @param in file The filename
 * @param out dir_entry The directory entry for the new file
 * @returns 0 on failure, 1 on success.
 */
extern uint8_t fat16_create_file(struct fat16_dir* parent, const char* file, struct fat16_dir_entry* dir_entry);

/**
 * Deletes a file or directory.
 *
 * It is not checked if the file to delete is a directory.
 * If a directory is deleted without first deleting its
 * subdirectories and files, disk space occupied by these
 * files will get wasted as there is no chance to release
 * it and mark it as free!!!
 *
 * @param in dir_entry The directory entry of the file to delete.
 * @returns 0 on failure, 1 on success.
 */
extern uint8_t fat16_delete_file(struct fat16_dir_entry* dir_entry);

/*
 * Opens a directory.
 *
 * @param in out dir the directory handle
 * @param in dir_entry The directory entry which stands for the directory to open.
 * @returns directory descriptor on success, 0 on failure.
 */
extern uint8_t fat16_open_dir(struct fat16_dir* dir, const struct fat16_dir_entry* dir_entry);

/**
 * Reads the next directory entry contained within a parent directory.
 *
 * @param in dd The descriptor of the parent directory
 * @param out dir_entry Pointer to a buffer into which to write the directory entry information.
 * @returns 0 on failure, 1 on success.
 */
extern uint8_t fat16_read_dir(struct fat16_dir* dd, struct fat16_dir_entry* dir_entry);

/*
 * Resets the directory handle such that reading restarts
 * with the first directory entry.
 *
 * @param in dd The directory handle
 * @returns 0 on failure, 1 on success.
 */
extern uint8_t fat16_reset_dir(struct fat16_dir* dd);

/**
 * Writes a directory entry to disk.
 *
 * The file name is not checked for invalid characters.
 *
 * @param in dir_entry
 * @returns 0 on failure, 1 on success.
 */
extern uint8_t fat16_write_dir_entry(const struct fat16_dir_entry* dir_entry);

#endif /* FAT16_H_ */

