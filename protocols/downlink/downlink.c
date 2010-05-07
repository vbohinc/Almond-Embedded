#include <stdint.h>
#include <stdbool.h>
#include "downlink.h"
#include "packet_types.h"
#include "../classes.h"

#ifdef SQUIRREL

void downlink_discover () {
	return;
}

static bool ret_package (struct downlink_packet *p) {

	// I think an unbounded array is a better idea, as we don't need to store these huge bunch of pointers.
	
	typedef struct nut_info_list {
		nut_info_entry *head;
		nut_info_entry *end;
	}
	typedef struct nut_info_entry {
		uint32_t MAC;
		uint8_t class_id;
		nut_info_sensor_list_entry *class_list_head;
		nut_info_entry *next;
	}
	typedef struct nut_info_sensor_list_entry {
		uint8_t sensor_id;
		uint8_t class;
		nut_info_sensor_list_entry *next;
	}
	typedef struct sensor_class_descriptor {
		uint8_t sensor_id;
		uint8_t[16] description;
		sensor_class_descriptor *next; 
	}
	typedef struct sensor_class_descriptor_list {
		sensor_class_descriptor_entry *head;
		sensor_class_descriptor_entry *end;
	}
	typedef struct sensor_class_descriptor_list_entry {
		uint8_t sensor_class;
		sensor_class_descriptor *head;
		sensor_class_descriptor_list_entry *next;
	}
	switch (p->opcode & 0x0F) {
		case INFO_NUT:
		if (!find_nut_by_MAC(p)) {
			struct nut_info_entry n;
			n->MAC = bluetooth_get_MAC();
			n->class_id = p->value;
			n->nut_info_sensor_list_entry = NULL;
			nut_info_list->end->next = n;
			nut_info_list->end = n;
			n->next = head;
		}
		case INFO_ID:
		struct nut_info_entry n = find_nut_by_MAC(p);
		if (n->class_list_head == NULL){
			struct nut_info_sensor_list_entry e;
			n->class_list_head = e;
		} else {
			struct nut_info_sensor_list_entry e = n->class_list_head entry;
			while (e->next != NULL) {
				e = e-> next;
			}
		}
		e->sensor_id = p->id;
		e->class = p->value;
		
		
	}
}

#endif

#ifdef NUT
static inline bool downlink_handle_get_package (struct downlink_packet *p) {

	switch (p->opcode & 0x0F) {
		case STANDARD: // Clarify numbers etc...? Maybe adjust proto
			if (p->id < 128 && class_id_sensors[p->id] != INVALID) {
				p->opcode = RET;
				p->value = get_value (p->id);
				return true;
			} else {
				return false;
			}
		case INFO_NUT:
			p->opcode = RET;			
			p->id = 0;
			p->value = class_id_nut;
			return true;
		case INFO_ID:
			p->opcode = RET;			
			p->id = 0;
			p->value = class_id_nut;
			return true;
		default:
			return false;
	}
}

static inline bool downlink_handle_set_package (struct downlink_packet *p) {
	return true;
}


bool downlink_handle_package (struct downlink_packet *p) {
	switch (p->opcode & 0xF0) {
		case GET:
			return downlink_handle_get_package (p);
		case SET:
			return downlink_handle_set_package (p);
		case ECHO:
			return true;
		default:
			return false;
	}
}
#endif



