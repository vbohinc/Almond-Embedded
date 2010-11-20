/**
*\brief fifo.h - a simple Fifo Part of the ALMOND Project
* @file fifo.h
* @author Pascal Schnurr
*/

#include <common.h>
#include <avr/pgmspace.h>

#ifndef _FIFO_H_
#define _FIFO_H_

/**
*fifo data structure all vital fifo information
*/
typedef struct
{
    uint16_t count; /**< current number of elements */
    uint16_t head; /**< position of the head element */ 
    uint16_t size; /**< size equals max number of entrys*/
    uint8_t* buffer; /**< pointer to memory area where the fifo is to be saved */
} fifo_t;


/** \brief initialize of a fifo
 * sets all the information in your given fifo structure
 * @param fifo pointer to an allocated fifo_t structure
 * @param buffer pointer to an a allocated memory space for the fifo of size = sizeof(uint8_t) * size
 * @param size max number of entrys the fifo will hold
 */
void fifo_init (fifo_t *fifo, uint8_t *buffer, uint16_t size);

/** \brief checks if fifo is empty
 * @param fifo pointer to your initialized fifo_t structure
 * @return true if empty otherwise false
 */
bool fifo_is_empty (const fifo_t *fifo);

/** \brief checks if fifo is full
 * @param fifo pointer to your initialized fifo_t structure
 * @return true if full otherwise false
 */
bool fifo_is_full (const fifo_t *fifo);

/** \brief clears the fifo
 * resets your fifo structure to 0 elements
 * @param fifo pointer to your initialized fifo_t structure
 * @return always true (never fails)
 */
bool fifo_clear (fifo_t *fifo);

/** \brief reads head of fifo
 * reads the first element and removes it
 * @param fifo pointer to your initialized fifo_t structure
 * @return false if fifo is empty false otherwise
 */
bool fifo_read (fifo_t *fifo, char *data);

/** \brief inserts a char into the fifo
 * adds a char to the end of the fifo
 * @param fifo pointer to your initialized fifo_t structure
 * @param data the char data to be inserted
 * @return false if fifo is full true otherwise
 */
bool fifo_write (fifo_t *fifo, const char data);

/** \brief compares first elements with prog_char string
 * if pgm equals the first elements of the fifo these elements are removed
 * @param fifo pointer to your initialized fifo_t structure
 * @param pgm a prog_char string for comparison
 * @return true if pgm is equal to the first entrys in the fifo, false otherwise
 */
bool fifo_cmp_pgm (fifo_t* fifo, const prog_char* pgm);

/** \brief searches a string in the whole fifo
 * starts at the beginning and searches for the pgm string in the fifo,
 * if they are found previous entrys and the string are removed from the fifo
 * @param fifo pointer to your initialized fifo_t structure
 * @param pgm a prog_char with the search string
 * @return true if found, false otherwise
 */
bool fifo_strstr_pgm (fifo_t *fifo, const prog_char *pgm);

#endif /* _FIFO_H_ */
