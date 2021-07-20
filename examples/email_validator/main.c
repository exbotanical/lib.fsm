
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "libfsm.h"

/* Utilities and Matcher Functions */

fsm_bool_t match_alpha_lower(
	char* d, // transition entry key, which will be an empty buffer
	unsigned int size,
	char* d2,
	unsigned int d_size,
	unsigned int* n_bytes_read
) {

	if (d_size == 0){
		*n_bytes_read = 0;
		return FSM_FALSE;
	}

	if (*d2 >= 97 && *d2 <= 122) {
		*n_bytes_read = 1;
		return FSM_TRUE;
	}

	return FSM_FALSE;
}

fsm_bool_t match_alpha_upper(
	char* d,
	unsigned int size,
	char* d2,
	unsigned int d_size,
	unsigned int* n_bytes_read
) {
	if (d_size == 0){
		*n_bytes_read = 0;
		return FSM_FALSE;
	}

	if (*d2 >= 65 && *d2 <= 90){
		*n_bytes_read = 1;
		return FSM_TRUE;
	}

  return FSM_FALSE;
}

fsm_bool_t match_digit(
	char* d1,
	unsigned int size,
	char* d2,
	unsigned int d_size,
	unsigned int* n_bytes_read
) {
	if (d_size == 0){
		*n_bytes_read = 0;
		return FSM_FALSE;
	}

	if (*d2 >= 48 && *d2 <= 57){
		*n_bytes_read = 1;
		return FSM_TRUE;
	}

  return FSM_FALSE;
}

fsm_bool_t match_char(
	char* d1,
	unsigned int size,
	char* d2,
	unsigned int d_size,
	unsigned int* n_bytes_read
) {
  if (d_size == 0){
		*n_bytes_read = 0;
		return FSM_FALSE;
	}

	*n_bytes_read = 1;
	return FSM_TRUE;
}

int main(int argc, char* argv[]) {

  /* Initialize a new FSM */
  fsm_t* fsm = fsm_init("Email Validator");

  /* Initialize FSM State */
  state_t* s0 = fsm_state_init("S0", FSM_TRUE);

	tt_entry_t* tt_entry = NULL;
	char transition_key[MAX_TRANSITION_KEY_SIZE];
	unsigned int transition_keysize = 0;

	state_t* dead_state = fsm_state_init("D", FSM_FALSE);
	fsm_add_wildcard_entry(
		dead_state,
		dead_state,
		0,
		match_char
	);

  state_t* state_F = fsm_state_init("F", FSM_TRUE);
	fsm_add_wildcard_entry(
		state_F,
		dead_state,
		0,
		match_char
	);

	state_t* state_q6 = fsm_state_init("q6", FSM_FALSE);

	transition_keysize = strlen("gmail.com");
	strncpy(transition_key, "gmail.com", transition_keysize);

	fsm_add_entry(
		&state_q6->transition_table,
		transition_key,
		transition_keysize,
		state_F,
		0
	);

	transition_keysize = strlen("protonmail.com");
	strncpy(transition_key, "protonmail.com", transition_keysize);

	fsm_add_entry(
		&state_q6->transition_table,
		transition_key,
		transition_keysize,
		state_F,
		0
	);

	fsm_add_wildcard_entry(
		state_q6,
		dead_state,
		0,
		match_char
	);

	state_t* state_q5 = fsm_state_init("q5", FSM_FALSE);
	tt_entry = fsm_add_entry(
		&state_q5->transition_table,
		0,
		0,
		state_q5,
		0
	);

	fsm_register_entry_comparator(
		tt_entry,
		match_digit
	);

	fsm_register_entry_comparator(
		tt_entry,
		match_alpha_lower
	);

	fsm_register_entry_comparator(
		tt_entry,
		match_alpha_upper
	);


	transition_keysize = 1;
	transition_key[0] = '@';

	fsm_add_entry(
		&state_q5->transition_table,
		transition_key,
		transition_keysize,
		state_q6,
		0
	);

	fsm_add_wildcard_entry(
		state_q5,
		dead_state,
		0,
		match_char
	);

	state_t* state_q4 = fsm_state_init("q4", FSM_FALSE);
	tt_entry = fsm_add_entry(
		&state_q4->transition_table,
		0,
		0,
		state_q5,
		0
	);

	fsm_register_entry_comparator(
		tt_entry,
		match_digit
	);

	fsm_register_entry_comparator(
		tt_entry,
		match_alpha_lower
	);

	fsm_register_entry_comparator(
		tt_entry,
		match_alpha_upper
	);

	fsm_add_wildcard_entry(
		state_q4,
		dead_state,
		0,
		match_char
	);

	state_t* state_q3 = fsm_state_init("q3", FSM_FALSE);
	tt_entry = fsm_add_entry(
		&state_q3->transition_table,
		0,
		0,
		state_q4,
		0
	);

	fsm_register_entry_comparator(
		tt_entry,
		match_digit
	);

	fsm_register_entry_comparator(
		tt_entry,
		match_alpha_lower
	);

	fsm_register_entry_comparator(
		tt_entry,
		match_alpha_upper
	);

	fsm_add_wildcard_entry(
		state_q3,
		dead_state,
		0,
		match_char
	);

	state_t* state_q2 = fsm_state_init("q2", FSM_FALSE);
	tt_entry = fsm_add_entry(
		&state_q2->transition_table,
		0,
		0,
		state_q3,
		0
	);

	fsm_register_entry_comparator(
		tt_entry,
		match_digit
	);

	fsm_register_entry_comparator(
		tt_entry,
		match_alpha_lower
	);

	fsm_register_entry_comparator(
		tt_entry,
		match_alpha_upper
	);

	fsm_add_wildcard_entry(
		state_q2,
		dead_state,
		0,
		match_char
	);

	state_t* state_q1 = fsm_state_init("q1", FSM_FALSE);
	tt_entry = fsm_add_entry(
		&state_q1->transition_table,
		0,
		0,
		state_q2,
		0
	);

	fsm_register_entry_comparator(
		tt_entry,
		match_digit
	);

	fsm_register_entry_comparator(
		tt_entry,
		match_alpha_lower
	);

	fsm_register_entry_comparator(
		tt_entry,
		match_alpha_upper
	);

  fsm_add_wildcard_entry(
		state_q1,
		dead_state,
		0,
		match_char
	);

	state_t* state_q0 = fsm_state_init("q0", FSM_FALSE);
	fsm_set_initial_state(fsm, state_q0);

	tt_entry = fsm_add_entry(
		&state_q0->transition_table,
		0,
		0,
		state_q1,
		0
	);

	fsm_register_entry_comparator(
		tt_entry,
		match_digit
	);

	fsm_register_entry_comparator(
		tt_entry,
		match_alpha_lower
	);

	fsm_register_entry_comparator(
		tt_entry,
		match_alpha_upper
	);

	fsm_add_wildcard_entry(
		state_q0,
		dead_state,
		0,
		match_char
	);

	fsm_bool_t ret;
	fsm_error_t err;

  if ((err = fsm_invoke(
		fsm,
		"email@gmail.com\0",
		strlen("email@gmail.com\0"),
		0,
		&ret
	))) {
		if (ret == FSM_TRUE) {
			printf("Valid email addr\n");
		} else {
			printf("Invalid email addr\n");
		}
	} else {
    printf("FSM failure\n");
  }
}
