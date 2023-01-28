
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#include "libfsm.h"

typedef struct bit_counter {
  unsigned int count_1;
  unsigned int count_0;
} bit_counter_t;

void outfn(state_t* from, state_t* to, char* input_buff,
           unsigned int input_buff_size, fsm_outbuf_t* outbuf) {
  char out;

  if (outbuf->curr_pos == 0) {
    outbuf->curr_pos = sizeof(bit_counter_t);
  }

  bit_counter_t* stat = (bit_counter_t*)outbuf->outbuf;

  if (*input_buff == '1') {
    out = '0';
    stat->count_1++;
  } else {
    out = '1';
    stat->count_0++;
  }

  outbuf->curr_pos +=
      snprintf(outbuf->outbuf + outbuf->curr_pos,
               (MAX_OUTBUF_SIZE - outbuf->curr_pos - 1), "%s-->%c | %c-->%s \n",
               from->name, *input_buff, out, to->name);
}

int main(int argc, char** argv) {
  /* Initialize a new FSM */
  fsm_t* fsm = fsm_init("Bit Flipper");

  /* Initialize FSM State */
  state_t* s0 = fsm_state_init("S0", FSM_TRUE);

  /*Set FSM initial state*/
  fsm_set_initial_state(fsm, s0);

  /* insert transitions into state's transition table */
  char bit = '0';
  fsm_add_entry(&s0->transition_table, &bit, 1, s0, outfn);

  bit = '1';
  fsm_add_entry(&s0->transition_table, &bit, 1, s0, outfn);

  /* FSM is now complete; let us see the outputs */

  fsm_bool_t ret;
  fsm_error_t err;

  err = fsm_invoke(fsm,
                   "0000000\0",  // input to process
                   strlen("0000000\0"),
                   0,    // output will default to using the internal FSM buffer
                   &ret  // did the FSM end in a designated 'final state'
  );

  if (err == FSM_SUCCESS) {
    printf("FSM result: %s\n", ret == FSM_TRUE ? "FSM_TRUE" : "FSM_FALSE");
    printf("FSM output: %s\n", fsm->outbuf.outbuf + sizeof(bit_counter_t));
    printf("FSM Stats: \t1: %u\t0: %u\n",
           ((bit_counter_t*)fsm->outbuf.outbuf)->count_1,
           ((bit_counter_t*)fsm->outbuf.outbuf)->count_0);
  }

  fsm_outbuf_t outbuf;

  memset((&outbuf)->outbuf, 0, MAX_OUTBUF_SIZE);
  (&outbuf)->curr_pos = 0;

  err = fsm_invoke(fsm, "1111111\0", strlen("1111111\0"), &outbuf, &ret);

  if (err == FSM_SUCCESS) {
    printf("FSM result: %s\n", ret == FSM_TRUE ? "FSM_TRUE" : "FSM_FALSE");
    printf("FSM output: %s\n", outbuf.outbuf + sizeof(bit_counter_t));
    printf("FSM Stats: \t1: %u\t0: %u\n",
           ((bit_counter_t*)outbuf.outbuf)->count_1,
           ((bit_counter_t*)outbuf.outbuf)->count_0);
  }

  snprintf(fsm->inbuf, sizeof(fsm->inbuf), "%s", "0101010101010\0");

  size_t size = strlen("0101010101010\0");
  fsm->inbuf[size] = '\0';
  fsm->inbuf_size = strlen("0101010101010\0");

  err = fsm_invoke(fsm, 0, 0, &outbuf, &ret);

  if (err == FSM_SUCCESS) {
    printf("FSM result: %s\n", ret == FSM_TRUE ? "FSM_TRUE" : "FSM_FALSE");
    printf("FSM output: %s\n", outbuf.outbuf + sizeof(bit_counter_t));
    printf("FSM Stats: \t1: %u\t0: %u\n",
           ((bit_counter_t*)outbuf.outbuf)->count_1,
           ((bit_counter_t*)outbuf.outbuf)->count_0);
  }

  return EXIT_SUCCESS;
}
