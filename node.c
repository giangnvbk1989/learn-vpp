/*
 * Copyright (c) 2015 Cisco and/or its affiliates.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <vlib/vlib.h>
#include <vnet/vnet.h>
#include <vnet/pg/pg.h>
#include <vnet/ethernet/ethernet.h>
#include <vppinfra/error.h>
#include <learn-vpp/learn.h>

typedef struct
{
  u32 next_index;
  u32 sw_if_index;
} learn_trace_t;

/* packet trace format function */
static u8 *
format_learn_trace (u8 * s, va_list * args)
{
  CLIB_UNUSED (vlib_main_t * vm) = va_arg (*args, vlib_main_t *);
  CLIB_UNUSED (vlib_node_t * node) = va_arg (*args, vlib_node_t *);
  learn_trace_t *t = va_arg (*args, learn_trace_t *);

  s = format (s, "LEARN: sw_if_index %d, next index %d\n",
	      t->sw_if_index, t->next_index);

  return s;
}

extern vlib_node_registration_t learn_node_internal;
extern vlib_node_registration_t learn_node_input;

#define foreach_learn_error \
_(PROCESSED, "Packets processed")

typedef enum
{
#define _(sym,str) LEARN_ERROR_##sym,
  foreach_learn_error
#undef _
    LEARN_N_ERROR,
} learn_error_t;

static char *learn_error_strings[] = {
#define _(sym,string) string,
  foreach_learn_error
#undef _
};

typedef enum
{
  LEARN_NEXT_INTERFACE_OUTPUT,
  LEARN_N_NEXT,
} learn_next_t;

VLIB_NODE_FN (learn_node_internal) (vlib_main_t * vm,
                                    vlib_node_runtime_t * node,
			            vlib_frame_t * frame)
{
  u32 n_left_from, *from;
  u32 pkts_processed = 0;
  vlib_buffer_t *bufs[VLIB_FRAME_SIZE], **b;
  u16 nexts[VLIB_FRAME_SIZE], *next;

  from = vlib_frame_vector_args (frame);
  n_left_from = frame->n_vectors;

  vlib_get_buffers (vm, from, bufs, n_left_from);
  b = bufs;
  next = nexts;

  while (n_left_from > 0)
    {
      /*
      ip4_header_t *ip0 = 0;
      ip0 = vlib_buffer_get_current (b[0]);
      */

      next[0] = LEARN_NEXT_INTERFACE_OUTPUT;

      b += 1;
      next += 1;
      n_left_from -= 1;
      pkts_processed += 1;
    }
  vlib_buffer_enqueue_to_next (vm, node, from, (u16 *) nexts,
			       frame->n_vectors);

  vlib_node_increment_counter (vm, learn_node_internal.index,
			       LEARN_ERROR_PROCESSED, pkts_processed);

  if (PREDICT_FALSE ((node->flags & VLIB_NODE_FLAG_TRACE)))
    {
      int i;
      b = bufs;

      for (i = 0; i < frame->n_vectors; i++)
	{
	  if (b[0]->flags & VLIB_BUFFER_IS_TRACED)
	    {
	      learn_trace_t *t =
		vlib_add_trace (vm, node, b[0], sizeof (*t));
	      t->sw_if_index = vnet_buffer (b[0])->sw_if_index[VLIB_TX];
	      t->next_index = LEARN_NEXT_INTERFACE_OUTPUT;
	      b++;
	    }
	  else
	    break;
	}
    }
  return frame->n_vectors;
}

/* *INDENT-OFF* */
VLIB_REGISTER_NODE (learn_node_internal) =
{
  .name = "learn-vpp-internal",
  .vector_size = sizeof (u32),
  .format_trace = format_learn_trace,
  .type = VLIB_NODE_TYPE_INTERNAL,

  .n_errors = ARRAY_LEN(learn_error_strings),
  .error_strings = learn_error_strings,

  .n_next_nodes = LEARN_N_NEXT,

  /* edit / add dispositions here */
  .next_nodes = {
    [LEARN_NEXT_INTERFACE_OUTPUT] = "interface-output",
  },
};
/* *INDENT-ON* */

VLIB_NODE_FN (learn_vpp_input) (vlib_main_t *vm,
		                vlib_node_runtime_t *rt,
                                vlib_frame_t *f)
{
  vlib_node_increment_counter (vm, learn_node_input.index,
			       LEARN_ERROR_PROCESSED, 1);
  return 0;
}

/* *INDENT-OFF* */
VLIB_REGISTER_NODE (learn_vpp_input) = {
  .name = "learn-vpp-input",
  .type = VLIB_NODE_TYPE_INPUT,
  .state = VLIB_NODE_STATE_POLLING,
  //.state = VLIB_NODE_STATE_INTERRUPT,
};
/* *INDENT-ON* */

/*
 * fd.io coding-style-patch-verification: ON
 *
 * Local Variables:
 * eval: (c-set-style "gnu")
 * End:
 */
