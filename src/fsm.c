/**
* @file fsm.c
* @brief Library to create Finite State Machines using composition.
*
* This library is expected to be used using composition
* @author Teachers from the Departamento de Ingeniería Electrónica. Original authors: José M. Moya and Pedro J. Malagón. Latest contributor: Román Cárdenas.
* @date 2023-09-20
*/
 
/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdlib.h>
 
/* Other includes */
#include "fsm.h"
 
 
 
//GCOVR_EXCL_START
void* __attribute__((weak)) fsm_malloc(size_t s)
{
    return malloc(s);
}
 
void __attribute__((weak)) fsm_free(void* p)
{
    free(p);
}
//GCOVR_EXCL_STOP
 
fsm_t *fsm_new(fsm_trans_t *p_tt)
{
    if (p_tt == NULL || p_tt ->orig_state == -1 || p_tt->dest_state == -1)
    {
        return NULL;
    }
 
    //Se cuentan las transiciones válidas
    int num_transitions = 0;
    for (fsm_trans_t *p = p_tt; p->orig_state != -1; ++p)
    {
        num_transitions++;
        if (num_transitions > FSM_MAX_TRANSITIONS)
        {
            return NULL;
        }
    }
 
    //Si todo bien, reservamos memoria
    fsm_t *p_fsm = (fsm_t *)fsm_malloc(sizeof(fsm_t));
    if (p_fsm != NULL)
    {
        if (fsm_init(p_fsm, p_tt) == 0)
        {
            //Si init falla, liberamos y devolvemos NULL
            fsm_free(p_fsm);
            return NULL;
        }
    }
    return p_fsm;
}
 
void fsm_destroy(fsm_t *p_fsm)
{
    fsm_free(p_fsm);
}
 
int fsm_init(fsm_t *p_fsm, fsm_trans_t *p_tt)
{
    if (p_tt == NULL || p_fsm == NULL)
    {
        return 0;
    }
 
    int num_transitions = 0;
    for (fsm_trans_t *p = p_tt; p->orig_state != -1; ++p)
    {
        num_transitions++;
        if (num_transitions > FSM_MAX_TRANSITIONS)
        {
            return 0;
        }
    }
 
    p_fsm->p_tt = p_tt;
    p_fsm->current_state = p_tt->orig_state;
    return num_transitions;
}
 
int fsm_get_state(fsm_t *p_fsm)
{
    return p_fsm->current_state;
}
 
void fsm_set_state(fsm_t *p_fsm, int state)
{
    p_fsm->current_state = state;
}
 
int fsm_fire(fsm_t *p_fsm)
{
    if (p_fsm == NULL || p_fsm->p_tt == NULL)
    {
        return -1;
    }
 
    int has_transitions = 0;
    for (fsm_trans_t *p_t = p_fsm->p_tt; p_t->orig_state != -1; ++p_t)
    {
        if (p_fsm->current_state == p_t->orig_state)
        {
            has_transitions = 1;
            bool condition = true;
            if (p_t->in != NULL)
            {
                condition = p_t->in(p_fsm);
            }
            // Si función de guarda es NULL → siempre true
 
            if (condition)
            {
                p_fsm->current_state = p_t->dest_state;
                if (p_t->out)
                {
                    p_t->out(p_fsm);
                }
                return 1;  // Se cumplió y transicionó
            }
        }
    }
    if (has_transitions)
    {
        return 0;  // Había transiciones pero ninguna válida
    }
    return -1;     // No había transiciones para este estado
}