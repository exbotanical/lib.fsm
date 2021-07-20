#include "libfsm.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// state transition routine
void bitflip_routine(
	state_t* from,
	state_t* to,
	char* inbuf,
	unsigned int inbuf_size,
	fsm_outbuf_t* outbuf
) {
	char out;
	out = (*inbuf == '1') ? '0' : '1';

	outbuf->curr_pos += snprintf(
		outbuf->outbuf + outbuf->curr_pos,
		(MAX_OUTBUF_SIZE - outbuf->curr_pos - 1),
		"%s --> %c|%c --> %s\n",
		from->name,
		*inbuf,
		out,
		to->name
	);
}

void init_outbuf(fsm_outbuf_t* outbuf) {
	memset(outbuf->outbuf, 0, MAX_OUTBUF_SIZE);
	outbuf->curr_pos = 0;
}

int main(int argc, char* argv[]) {
	char transition_key_0 = '0',
			 transition_key_1 = '1';

	unsigned int transition_keysize = 1;

	fsm_t* fsm = fsm_init("bit flipper - alternator");

	state_t* state_0, *state_1, *state_2, *D;

	// create initial state
	state_0 = fsm_state_init("q0", FSM_FALSE);
	fsm_set_initial_state(fsm, state_0);

	// create possible final states
	state_1 = fsm_state_init("q1", FSM_TRUE);
	state_0 = fsm_state_init("q2", FSM_TRUE);

	// dead state
	D = fsm_state_init("D", FSM_FALSE);

	// state transitions

	// q0
	fsm_add_entry(
		&state_0->transition_table,
		&transition_key_1,
		transition_keysize,
		state_1,
		bitflip_routine
	);

	fsm_add_entry(
		&state_0->transition_table,
		&transition_key_0,
		transition_keysize,
		state_2,
		bitflip_routine
	);

	// q1
	fsm_add_entry(
		&state_1->transition_table,
		&transition_key_1,
		transition_keysize,
		D,
		bitflip_routine
	);

	fsm_add_entry(
		&state_1->transition_table,
		&transition_key_0,
		transition_keysize,
		state_2,
		bitflip_routine
	);

	// q2
	fsm_add_entry(
		&state_2->transition_table,
		&transition_key_1,
		transition_keysize,
		state_1,
		bitflip_routine
	);

	fsm_add_entry(
		&state_2->transition_table,
		&transition_key_0,
		transition_keysize,
		D,
		bitflip_routine
	);

	// dead state
	fsm_add_entry(
		&D->transition_table,
		&transition_key_1,
		transition_keysize,
		D,
		bitflip_routine
	);

	fsm_add_entry(
		&D->transition_table,
		&transition_key_0,
		transition_keysize,
		D,
		bitflip_routine
	);

	fsm_bool_t result;
	fsm_error_t error;
	fsm_outbuf_t output;

	init_outbuf(&output);

	char* input = "0101010101\0";

	if ((error = fsm_invoke(
		fsm,
		input,
		strlen(input),
		&output,
		&result
	)) == FSM_SUCCESS) {
		if (result == FSM_TRUE) {
			printf("FSM OUTPUT: %s\n", output.outbuf);
		} else {
			printf("NO OUTPUT\n");
		}
	}	else if (error == FSM_E_FAILED_TRANSITION) printf("error: failed transition\n");

	return EXIT_SUCCESS;
}
