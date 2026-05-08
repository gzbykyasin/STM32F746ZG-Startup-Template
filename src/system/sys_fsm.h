/**
 * @file    sys_fsm.h
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Generic Finite State Machine engine.
 * @date    5 May 2026
 * @version 1.0.0
 */

#ifndef SYSTEM_INC_SYS_FSM_H_
#define SYSTEM_INC_SYS_FSM_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/
/**
 * @brief Matches any source state; use for global transitions (e.g. fault).
 */
#define SYS_FSM_ANY_STATE (0xFFU)

/*******************************************************************************
 * Typedef, Enum & Structs
 ******************************************************************************/

/**
 * @brief State identifier type.
 */
typedef uint8_t sys_fsm_state_t;

/**
 * @brief Event identifier type.
 */
typedef uint8_t sys_fsm_event_t;

/**
 * @brief Action callback function type.
 */
typedef void (*sys_fsm_callback_t)(void);

/**
 * @brief State handler structure containing lifecycle callbacks.
 */
typedef struct
{
    sys_fsm_callback_t enter;   /**< Called when entering the state. */
    sys_fsm_callback_t exit;    /**< Called when exiting the state. */
    sys_fsm_callback_t process; /**< Called periodically while in the state. */
} sys_fsm_state_handler_t;

/**
 * @brief Transition rule structure.
 */
typedef struct
{
    sys_fsm_state_t from_state; /**< Source state, or SYS_FSM_ANY_STATE. */
    sys_fsm_event_t event;      /**< Triggering event.                   */
    sys_fsm_state_t to_state;   /**< Destination state.                  */
} sys_fsm_transition_t;

/**
 * @brief FSM instance structure.
 */
typedef struct
{
    sys_fsm_state_t current_state;                      /**< Current state of the machine. */
    const sys_fsm_transition_t *p_transitions;          /**< Pointer to the transition table. */
    uint8_t transitions_len;                            /**< Number of entries in the transition table. */
    const sys_fsm_state_handler_t *p_state_handlers;    /**< Pointer to state handlers array. */
    uint8_t states_count;                               /**< Total number of states. */
} sys_fsm_t;

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initializes the FSM instance with a transition table and initial state.
 *
 * @param[out] p_fsm      FSM instance.
 * @param[in]  p_table    Transition table array.
 * @param[in]  table_len  Number of rows in p_table.
 * @param[in]  init_state Starting state.
 */
void sys_fsm_init(sys_fsm_t *p_fsm, 
                  const sys_fsm_transition_t *p_transitions, 
                  uint8_t transitions_len,
                  const sys_fsm_state_handler_t *p_state_handlers, 
                  uint8_t states_count,
                  sys_fsm_state_t init_state);

/**
 * @brief Dispatches an event; executes the first matching transition found.
 *
 * @param[in,out] p_fsm  FSM instance.
 * @param[in]     event  Event to dispatch.
 * @return true if a matching transition was found, false otherwise.
 */
bool sys_fsm_dispatch(sys_fsm_t *p_fsm, sys_fsm_event_t event);

/**
 * @brief Retrieves the current state from the FSM instance.
 *
 * @param[in] p_fsm  FSM instance.
 * @return Current state value.
 */
sys_fsm_state_t sys_fsm_get_state(const sys_fsm_t *p_fsm);

/**
 * @brief Processes the current state's periodic handler.
 *
 * @param[in] p_fsm  FSM instance.
 */
void sys_fsm_process(sys_fsm_t *p_fsm);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_INC_SYS_FSM_H_ */
