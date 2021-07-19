#include "libfsm.h"

#include <stdio.h> /* for snprintf */
#include <string.h> /* for strlen */
#include <stdlib.h> /* for malloc */

// local (non-public API) macros
#define FSM_ITERATE_TTABLE_BEGIN(ttable, entry)     \
  do {                                              \
    unsigned int idx = 0;                           \
    for(; idx < MAX_TRANSITION_TABLE_SIZE; idx++) { \
      entry = &(ttable->entry[idx]);                \
      if (!entry->next_state) break;                \


#define FSM_ITERATE_TTABLE_END(ttable, entry)       \
  }} while(0);                                      \

/* Utilities */
tt_entry_t* __get_next_avail_entry(tt_t* ttable) {
	tt_entry_t* entry = NULL;

	FSM_ITERATE_TTABLE_BEGIN(ttable, entry) {
		// no-op
	} FSM_ITERATE_TTABLE_END(ttable, entry);

	if (!entry->next_state) return entry;

	return NULL;
}

fsm_bool_t __default_input_comparator(char* transition_key, unsigned int size, char* data) {
	if (memcmp(transition_key, data, size)) return FSM_FALSE;
	return FSM_TRUE;
}

state_t* __apply_transition(
	fsm_t* fsm,
	state_t* state,
	char* inbuf, // input to parse
	unsigned int size, // remaining length of unparsed input
	unsigned int* len_read, // will be set to no bytes read
	fsm_outbuf_t* outbuf // output container
	) {
	tt_entry_t* entry = NULL;
	state_t* next_state = NULL;

	FSM_ITERATE_TTABLE_BEGIN((&state->transition_table), entry) {
		if ((entry->transition_key_size <= size) &&
			__default_input_comparator(entry->transition_key, entry->transition_key_size, inbuf)) {

			next_state = entry->next_state;

			if (entry->out_handler) {
				entry->out_handler(
					state,
					next_state,
					inbuf,
					entry->transition_key_size,
					outbuf
				);
			}

			*len_read = entry->transition_key_size;

			return next_state;
		}

	} FSM_ITERATE_TTABLE_END((&state->transition_table), entry);

	return NULL;
}

/* Public API */

/**
 * @brief Initialize and allocate memory for a new Finite State Machine
 *
 * Returns NULL if erroneous
 *
 * @param name
 * @return fsm_t*
 */
fsm_t* fsm_init(const char* name) {
	if (strlen(name) > MAX_FSM_NAME_SIZE) return NULL;

	fsm_t* fsm = malloc(sizeof(fsm_t));
	snprintf(fsm->name, sizeof(fsm->name), "%s", name);

	return fsm;
}

/**
 * @brief Initialize and allocate memory for a new FSM state object
 *
 * Returns NULL if erroneous
 *
 * @param name
 * @param is_final
 * @return state_t*
 */
state_t* fsm_state_init(const char* name, fsm_bool_t is_final) {
	// a name is absolutely required
	if (!name) return NULL;

	state_t* state = malloc(sizeof(state_t));
	snprintf(state->name, sizeof(state->name), "%s", name);
	state->is_final = is_final;

	return state;
}

/**
 * @brief Set the initial state of a given FSM. The FSM should not already
 * have an initial state
 *
 * @param fsm
 * @param state
 * @return true
 * @return bool Indicative of whether the state was set successfully
 */
bool fsm_set_initial_state(fsm_t* fsm, state_t* state) {
	if (fsm->initial_state) return false;

	fsm->initial_state = state;
	return true;
}

/**
 * @brief Create and insert a new Transition Table Entry
 *
 * @param t_table
 * @param transition_key
 * @param sizeof_key
 * @param next_state
 * @return true
 * @return false
 */
bool fsm_add_tt_entry(
	tt_t* t_table,
	char* transition_key,
	unsigned int sizeof_key,
	state_t* next_state,
	outhandler output_handler
) {
	if (sizeof_key > MAX_TRANSITION_KEY_SIZE) return false;

	tt_entry_t* entry;

	if (!(entry = __get_next_avail_entry(t_table))) {
		printf("FATAL: Transition Table is full\n");
		return false;
	}

	// TODO replace w/ snprintf or strlcpy
	memcpy(entry->transition_key, transition_key, sizeof_key);
	entry->transition_key[sizeof_key] = '\0';
	entry->transition_key_size = sizeof_key;
	entry->next_state = next_state;
	entry->out_handler = output_handler;

	return true;
}

fsm_error_t fsm_invoke(
	fsm_t* fsm, char*
	inbuf,
	unsigned int size,
	fsm_outbuf_t* outbuf,
	fsm_bool_t* fsm_result
) {
	state_t* initial_state = fsm->initial_state;

	if (!fsm->initial_state) return FSM_E_INVALID_INPUTS;

	state_t* current_state = initial_state;
	state_t* next_state = NULL;

	fsm->inbuf_cursor = 0;
	unsigned int len_read = 0;
	unsigned int inbuf_len = size;

	if (fsm_result) *fsm_result = FSM_FALSE;

	while (fsm->inbuf_cursor < MAX_INBUF_SIZE) {
		len_read = 0;

		next_state = __apply_transition(
			fsm,
			current_state,
			inbuf + fsm->inbuf_cursor,
			(inbuf_len - fsm->inbuf_cursor),
			&len_read,
			outbuf
		);

		if (len_read) {
			fsm->inbuf_cursor += len_read;

			if (!next_state) {
				return FSM_E_FAILED_TRANSITION;
			}

			current_state = next_state;
			continue;
		}

		break;
	}

	if (fsm_result) *fsm_result = current_state->is_final;

	return FSM_SUCCESS;
}
