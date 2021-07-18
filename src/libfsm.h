#ifndef LIB_FSM_H
#define LIB_FSM_H

#define MAX_STATE_NAME_SIZE 32
#define MAX_FSM_NAME_SIZE 32
#define MAX_INBUF_SIZE 128
#define MAX_TRANSITION_TABLE_SIZE 128
#define MAX_TRANSITION_KEY_SIZE 64

/**
 * @brief Wrapper for boolean state flag
 */
typedef enum {
	FSM_FALSE,
	FSM_TRUE
} fsm_bool_t;

typedef struct tt_entry {
	char transition_key[MAX_TRANSITION_KEY_SIZE];
	unsigned int transition_key_size;
} tt_entry_t;

/**
 * @brief Table for tracking state transitions
 */
typedef struct tt {
	tt_entry_t entry[MAX_TRANSITION_TABLE_SIZE];
} tt_t;

/**
 * @brief Represents the state of a Finite State Machine
 */
typedef struct state {
	/* The name of the state instance */
	char name[MAX_STATE_NAME_SIZE];

	/* Transition Table for the state instance */
	tt_t* transition_tan;

	/*Flag indicating whether the current state is final */
	fsm_bool_t is_final;
} state_t;

/**
 * @brief Represents a Finite State Machine
 */
typedef struct fsm {
	/* Initial state of the FSM */
	state_t* initial_state;

	/* The name of the FSM instance */
	char name[MAX_FSM_NAME_SIZE];

	/* Application-provided input */
	char input_buffer[MAX_INBUF_SIZE];

	/* Current size of input_buffer */
	unsigned int inbuf_size;

	/* N bytes of input_buffer that has been processed */
	unsigned int inbuf_cursor;
} fsm_t;


#endif /* LIB_FSM_H */
