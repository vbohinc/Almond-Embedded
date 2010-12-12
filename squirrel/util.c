/**
 * utils for Squirrel
 * @file util.c
 */

#include "util.h"
#include <common.h>
#include <string.h>
#include <fat16/fat16.h>
#include <protocols/uplink/uplink.h>

/**
 * Log Access
 */

void lookup (const char *mac, const uint8_t id, char *dst)
{
  char mac_null[13];
        
  memcpy (mac_null, mac, 12);
  mac_null[12] = '\0';

  snprintf (dst, 30, "/logs/%s-%02x", mac_null, id);
}

bool log_write (const char *mac, const uint8_t id, const uint32_t time, const uint16_t value)
{
    struct fat16_file file;
    char filename[30];

    lookup (mac, id, filename);

    uint16_t new_pos;

    if (!fat16_open_file_by_name (&file, filename))
    {
        struct fat16_dir_entry dir_dir_entry;        
        struct fat16_dir dir;

        debug_pgm (PSTR ("File not found, creating new one..."));

        if (!fat16_get_dir_entry_of_path ("/logs", &dir_dir_entry))
            debug_pgm (PSTR ("Could not open DirEntry of /logs"));

        if (!fat16_open_dir (&dir, &dir_dir_entry))
            debug_pgm (PSTR ("Could not open Dir /logs"));

        if (!fat16_create_file (&dir, &filename[6], &file.dir_entry))
            debug_pgm (PSTR ("Could not create file!"));

        file.pos = 0;

        // Write start pos...

        new_pos = 2;
        fat16_write_file (&file, (uint8_t*)&new_pos, 2);
    }

    fat16_read_file (&file, (uint8_t*)&new_pos, 2);

    // Pos...
    file.pos = new_pos;
    fat16_write_file (&file, (uint8_t*)&time, 4);
    fat16_write_file (&file, (uint8_t*)&value, 2);
    file.pos = 0;
    new_pos += 6;
    fat16_write_file (&file, (uint8_t*)&new_pos, 2);
    
    return true;
}

bool log_read (const char *mac, const uint8_t id, const uint8_t page, uint8_t* buffer)
{
    struct fat16_file file;
    char filename[30];

    lookup (mac, id, filename);

    if (!fat16_open_file_by_name (&file, filename))
      {
        debug_pgm (PSTR ("Could not open file!"));
        return false;
      }

    file.pos = 2 + 6 * 8 * page; //buffer: Ein Eintrag ist 6 Byte groß, also passen davon 8 rein (48 byte);

    for (uint8_t i = 0; i < sizeof (log_entry) * 8; i++)
        buffer[i] = 0;

    fat16_read_file (&file, buffer, sizeof (log_entry) * 8);

    return true;
}
