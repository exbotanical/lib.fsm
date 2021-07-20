#include "libfsm.h"

#include <stdio.h> /* for snprintf */
#include <string.h> /* for strlen */
#include <stdlib.h> /* for malloc */

// local (non-public API) macros
#define FSM_ITERATE_TABLE_BEGIN(ttable, e)                        \
  do {                                                             \
    for (int idx = 0; idx < MAX_TRANSITION_TABLE_SIZE; idx++) {    \
      e = &(ttable->entry[idx]);                                   \
      if (!e->next_state) break;                                   \


#define FSM_ITERATE_TABLE_END(ttable, e)                          \
  }} while(0);                                                     \

/* Utilities */

void __register_input_comparator(fsm_t* fsm, input_matcher comparator) {
	fsm->input_matcher = comparator;
}

tt_entry_t* __get_next_avail_entry(tt_t* ttable) {
	tt_entry_t* entry = NULL;

	FSM_ITERATE_TABLE_BEGIN(ttable, entry) {
		// no-op
	} FSM_ITERATE_TABLE_END(ttable, entry);

	if (!entry->next_state) {
		return entry;
	}

	return NULL;
}

fsm_bool_t __input_comparator(
	char* transition_key,
	unsigned int size,
	char* data,
	unsigned int data_size,
	unsigned int* n_bytes_read
	) {
	if (size <= data_size) {
		if (memcmp(transition_key, data, size)) return FSM_FALSE;
		*n_bytes_read = size;
		return FSM_TRUE;
	}
	*n_bytes_read = 0;
	return FSM_TRUE;
}

fsm_bool_t __entry_comparator(
	fsm_t* fsm,
	tt_entry_t* entry,
	char* inbuf,
	unsigned int inbuf_size,
	unsigned int* n_bytes_read
) {
	unsigned int i = 0;
	fsm_bool_t has_input_matcher = FSM_FALSE;
	fsm_bool_t ret = FSM_FALSE;

	if (!inbuf) return FSM_TRUE;

	has_input_matcher = entry->input_matchers[0]
		? FSM_TRUE
		: FSM_FALSE;

	if (has_input_matcher) {
		for (; i < MAX_ENTRY_CALLBACKS; i++) {
			if (!entry->input_matchers[i]) return FSM_FALSE;

			if ((entry->input_matchers[i])(NULL, 0, inbuf, inbuf_size, n_bytes_read)) {
					return FSM_TRUE;
			}
			*n_bytes_read = 0;
		}
		return FSM_FALSE;
	}

	if ((ret = fsm->input_matcher(
		entry->transition_key,
		entry->transition_key_size,
		inbuf,
		inbuf_size,
		n_bytes_read
	)) == FSM_TRUE) {
		*n_bytes_read = entry->transition_key_size;
	}

	return ret;
}

state_t* __apply_transition(
	fsm_t* fsm,
	state_t* state,
	char* inbuf, // input to parse
	unsigned int size, // remaining length of unparsed input
	unsigned int* n_bytes_read, // will be set to no bytes read
	fsm_outbuf_t* outbuf // output container
	) {
	tt_entry_t* entry = NULL;
	state_t* next_state = NULL;

	FSM_ITERATE_TABLE_BEGIN((&state->transition_table), entry) {
		if ((entry->transition_key_size <= size) &&
			__entry_comparator(fsm, entry, inbuf, size, n_bytes_read)) {

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

			return next_state;
		}

	}FSM_ITERATE_TABLE_END(&state->transition_table, entry);

	return NULL;
}

/* Public API */

/**
 * @brief Register an entry comparator function
 * This will be utilized to compare entries
 *
 * @param entry
 * @param matcher
 */
void fsm_register_entry_comparator(tt_entry_t* entry, input_matcher matcher) {
	for (unsigned int i = 0; i < MAX_ENTRY_CALLBACKS; i++) {
		if (entry->input_matchers[i]) continue;
		entry->input_matchers[i] = matcher;
		return;
	}
}

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

	__register_input_comparator(fsm, __input_comparator);

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
bool fsm_add_entry(
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
	state_t* current_state;

	if (!(current_state = fsm->initial_state)) {
		return FSM_E_INVALID_INPUTS;
	}

	state_t* next_state = NULL;

	fsm->inbuf_cursor = 0;
	unsigned int n_bytes_read = 0;
	unsigned int inbuf_size = 0;
	char* parsebuf;

	if (fsm_result) *fsm_result = FSM_FALSE;

	if (inbuf && size) {
		parsebuf = inbuf;
		inbuf_size = size;
	} else {
		parsebuf = fsm->inbuf;
		inbuf_size = fsm->inbuf_size;
	}

	// if no outbuf supplied, utilize internal FSM outbuf
	if (!outbuf) outbuf = &fsm->outbuf;

	memset(outbuf->outbuf, 0, MAX_OUTBUF_SIZE);
	outbuf->curr_pos = 0;

	while (fsm->inbuf_cursor < MAX_INBUF_SIZE) {
		n_bytes_read = 0;

		if (!(next_state = __apply_transition(
			fsm,
			current_state,
			parsebuf + fsm->inbuf_cursor,
			(inbuf_size - fsm->inbuf_cursor),
			&n_bytes_read,
			outbuf
		))) return FSM_E_FAILED_TRANSITION;

		if (n_bytes_read) {
			fsm->inbuf_cursor += n_bytes_read;
			current_state = next_state;

			if (fsm->inbuf_cursor == inbuf_size) break;

			continue;
		}

		break;
	}

	if (fsm_result) *fsm_result = current_state->is_final;

#if 0
	if (outbuf->curr_pos < MAX_OUTBUF_SIZE) outbuf->outbuf[outbuf->curr_pos] = '\0';
#endif

	return FSM_SUCCESS;
}
