/*
 *  util.c
 *
 */
 
#include "util.h"
#include "../drivers/fat16/fat16.h"
#include "../protocols/uplink/uplink.h"

/**
 * Time
 */
static uint32_t soft_time = 0;

bool time_set (uint32_t time)
{
  soft_time = time;
  return true;
}

uint32_t time_get ()
{
  return soft_time;
}

/**
 * Log Access
 */
bool log_write (uint8_t* mac, uint8_t id, uint32_t time, uint16_t value)
{  
  debug_pgm (PSTR ("LOG: WRITE"));
  
  char filename[30];
  struct fat16_file file;
  snprintf(filename, 30, "/logs/%02x:%02x:%02x:%02x:%02x:%02x-%02x", *(mac), *(mac+1), *(mac+2), *(mac+3), *(mac+4), *(mac+5), id);
  
  if (!fat16_open_file_by_name(&file, &filename)) 
  {
      debug ("Could not open file, creating new one...");
      
      struct fat16_dir_entry dir_dir_entry;
      if(!fat16_get_dir_entry_of_path("/logs", &dir_dir_entry))
        debug("Could not open DirEntry of /logs");
    
      struct fat16_dir dir;
      if(!fat16_open_dir (&dir, &dir_dir_entry))
        debug("Could not open Dir /logs!");
         
      if(!fat16_create_file(&dir, &filename[6], &file.dir_entry))
        debug("Could not create file!");
        
      file.pos = 0;
      // Write start pos...
      fat16_write_file (&file, &time, 6);
  } 
  
  uint16_t new_pos;

  fat16_read_file (&file, new_pos, 2);
  // Pos...
  file.pos = new_pos;
  // Evil hack!!!
  fat16_write_file (&file, &time, 6);
  file.pos = 0;
  new_pos += 6;

  return true;
}

bool log_read (uint8_t* mac, uint8_t id, uint8_t page, uint8_t* buffer)
{
  debug_pgm (PSTR ("LOG: READ"));
  
  char filename[30];
  struct fat16_file file;
  snprintf(filename, 30, "/logs/%02x:%02x:%02x:%02x:%02x:%02x-%02x", *(mac), *(mac+1), *(mac+2), *(mac+3), *(mac+4), *(mac+5), id);
  
  // Clean buffer...
  for (uint8_t i = 0; i < UPLINK_PAYLOAD_LENGTH - 7; i++)
    {
      *(buffer + i) = 0x00;    
    }
  
  if(!fat16_open_file_by_name(&file, &filename)) 
    {
      debug ("Could not open file!");
      return false;
    } 
  
  file.pos = 2 + 6 * 9 * page;

  fat16_read_file (&file, buffer, UPLINK_PAYLOAD_LENGTH - 7);

  return true;
}
