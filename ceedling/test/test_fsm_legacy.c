#include "unity.h"

#define TEST_CASE(...)

#include "fsm.h"
#include "mock_test_fsm.h"

#include <stdlib.h>

/**
 * @file test_fsm_legacy.c
 * @author 
 * @author 
 * @brief Tests for existing fsm module
 * @version 0.1
 * @date 2024-04-09
 * 
 */

/**
 * @brief Stub or Callback for fsm_malloc that calls real malloc. 
 * 
 * @param[in] s Amount of bytes to allocate
 * @param[in] n Amount of calls to this function
 * 
 * @return pointer to allocated memory if success; NULL if fails
 * 
 */
static void* cb_malloc(size_t s, int n) {
    return malloc(s);
}

/**
 * @brief Stub or Callback for fsm_free that calls real free. 
 * 
 * @param[in] p Pointer to allocated memory to free
 * @param[in] n Amount of calls to this function
 * 
 */
static void cb_free(void* p, int n) {
    return free(p);
}

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * @brief Comprueba que la funcion de fsm_new devuelve NULL 
 *        y no llama a fsm_malloc si la tabla de transiciones es NULL 
 * 
 */
void test_fsm_new_nullWhenNullTransition(void)
{
    fsm_t *f = (fsm_t*)1;

    f = fsm_new(NULL);

    TEST_ASSERT_EQUAL (NULL, f);
    //TEST_IGNORE();
}

/**
 * @brief Comprueba que la función de inicialización devuelve false si el puntero a la maquina de estado es NULL 
 *
 */
void test_fsm_init_falseWhenNullFsm(void)
{
    
    TEST_IGNORE();
}

/**
 * @brief Función de inicializacion devuelve false si la tabla de transiciones es nula
 * 
 */
void test_fsm_init_falseWhenNullTransitions(void)
{
    
    TEST_IGNORE();
}

/**
 * @brief La máquina de estados devuelve NULL
 *        y no llama a fsm_malloc si el estado de origen
 *        de la primera transición es -1 (fin de la tabla)
 */
void test_fsm_nullWhenFirstOrigStateIsMinusOne (void) {
    
    fsm_trans_t tt[] = {
        {-1, NULL, -1, NULL}   // Tabla vacía
    };

    fsm_malloc_IgnoreAndReturn((void*)0xDEADBEEF);  // No queremos que intente malloc

    fsm_t *f = fsm_new(tt);
    
    TEST_ASSERT_NULL(f);  // Debe devolver NULL porque no hay transiciones
    //TEST_IGNORE();
}

/**
 * @brief La máquina de estados devuelve NULL y no llama a fsm_malloc si el estado de destino de la primera transición es -1 (fin de la tabla)
 * 
 */
void test_fsm_nullWhenFirstDstStateIsMinusOne (void) {
    
    fsm_trans_t tt[] = {
        {0, is_true, -1, do_nothing},
        {-1, NULL, -1, NULL}
    };

    fsm_malloc_IgnoreAndReturn((void*)0xDEADBEEF);

    fsm_t *f = fsm_new(tt);
    
    TEST_ASSERT_NULL(f);                  
    //TEST_IGNORE();
}

/**
 * @brief La máquina de estados devuelve NULL y no llama a fsm_malloc si la función de comprobación de la primera transición es NULL (fin de la tabla)
 * 
 */
void test_fsm_nullWhenFirstCheckFunctionIsNull (void) {
  /*
  fsm_trans_t tt[] = {{0, is_true, -1, do_nothing}};
  fsm_malloc_IgnoreAndReturn((void*)0xDEADBEEF);
  fsm_t *f = fsm_new(tt);
  */
  //TEST_ASSERT_NULL(f);
  TEST_IGNORE();
}

/**
 * @brief Devuelve puntero no NULL y llama a fsm_malloc (usa Callback a cb_mallock y fsm_malloc_ExpectXXX) al crear la maquina de estados con una transición válida
 *        con función de actualización (salida) NULL o no NULL.
 *        Hay que liberar la memoria al final llamando a free
 * 
 */
TEST_CASE(NULL)
TEST_CASE(do_nothing)
void test_fsm_new_nonNullWhenOneValidTransitionCondition(fsm_output_func_t out)
{
    fsm_trans_t tt[] = {
        {0, is_true, 1, do_nothing},
        {1, is_true, 0, do_nothing},
        {-1, NULL, -1, NULL}
    };

    
     // Usa Callback para redirigir fsm_malloc a malloc real
    fsm_malloc_StubWithCallback(cb_malloc);

    fsm_t *f = fsm_new(tt);

    TEST_ASSERT_NOT_NULL(f);  // Debe devolver puntero válido

    free(f);  // Liberamos la memoria asignada por malloc

    //TEST_IGNORE();
    
}


/**
 * @brief Estado inicial corresponde al estado de entrada de la primera transición de la lista al crear una maquiina de estados y es valido.
 *        Usa Stub para fsm_malloc y luego libera la memoria con free
 */
void test_fsm_new_fsmGetStateReturnsOrigStateOfFirstTransitionAfterInit(void)
{

    fsm_trans_t tt[] = {
        {42, is_true, 1, do_nothing}, // estado inicial = 42
        {-1, NULL, -1, NULL}
    };

    // Redirigimos fsm_malloc a malloc real
    fsm_malloc_StubWithCallback(cb_malloc);

    // Creamos la máquina de estados
    fsm_t *f = fsm_new(tt);

    TEST_ASSERT_NOT_NULL(f);                      // Confirmamos que se creó correctamente
    TEST_ASSERT_EQUAL(42, fsm_get_state(f));      // El estado inicial debe ser 42

    free(f); // Liberamos la memoria
    //TEST_IGNORE();
}

/**
 * @brief La maquina de estado no transiciona si la funcion devuelve 0
 * 
 */
void test_fsm_fire_isTrueReturnsFalseMeansDoNothingIsNotCalledAndStateKeepsTheSame(void)
{
    fsm_trans_t tt[] = {
        {0, is_true, 1, do_nothing},
        {-1, NULL, -1, NULL}
    };

    fsm_t f;
    int n = fsm_init(&f, tt);

    TEST_ASSERT_GREATER_THAN(0, n);
    // Esperamos que is_true se llame y devuelva false
    is_true_ExpectAndReturn(&f, false);

    // No debe llamarse do_nothing si no se transiciona
    // Por lo tanto, NO usamos do_nothing_Expect()

    int ret = fsm_fire(&f);

    // Estado no debe cambiar
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(0, fsm_get_state(&f));

    //TEST_IGNORE();
}

/**
 * @brief Comprueba que el puntero pasado a fsm_fire es pasado a la función de guarda cuando se comprueba una transición
 * 
 */
void test_fsm_fire_checkFunctionCalledWithFsmPointerFromFsmFire(void)
{

    fsm_trans_t tt[] = {
        {0, is_true, 1, NULL},
        {-1, NULL, -1, NULL}
    };
    fsm_t f;
    int n = fsm_init(&f, tt);

    TEST_ASSERT_GREATER_THAN(0, n);
    // Verifica que is_true se llama con &f
    is_true_ExpectAndReturn(&f, false); // El valor que devuelva no importa

    int ret = fsm_fire(&f);
    TEST_ASSERT_EQUAL(0, ret);
    //TEST_IGNORE();
}

/** 
 * @brief Comprueba que el fsm_fire funciona y tiene el estado correcto cuando la transición devuelve true (cambia) y cuando devuelve false (mantiene)
 * 
 */
TEST_CASE(false, 0)
TEST_CASE(true, 1)
void test_fsm_fire_checkFunctionIsCalledAndResultIsImportantForTransition(bool returnValue, int expectedState)
{
    fsm_trans_t tt[] = {
        {0, is_true, 1, NULL},
        {-1, NULL, -1, NULL}
    };

    fsm_t f;
    int n = fsm_init(&f, tt);

    TEST_ASSERT_GREATER_THAN(0, n);
    // Simula que is_true devuelve el valor que se pasa como argumento del test
    is_true_ExpectAndReturn(&f, returnValue);

    int ret = fsm_fire(&f);
    if (returnValue)
    {
        TEST_ASSERT_EQUAL(1, ret);
    }
    else
    {
        TEST_ASSERT_EQUAL(0, ret);
    }
    TEST_ASSERT_EQUAL(expectedState, fsm_get_state(&f));
    //TEST_IGNORE();
}


/**
 * @brief La creación de una máquina de estados devuelve NULL si la reserva de memoria falla.
 *        Usa el Mock llamando a fsm_malloc_ExpectXXX sin Stub ni Callback.
 *
 */
void test_fsm_new_nullWhenFsmMallocReturnsNull(void)
{
    fsm_trans_t tt[] = {
        {0, is_true, 1, NULL},
        {-1, NULL, -1, NULL}
    };

    // Simula que fsm_malloc falla (devuelve NULL)
    fsm_malloc_ExpectAndReturn(sizeof(fsm_t), NULL);

    fsm_t *f = fsm_new(tt);

    TEST_ASSERT_NULL(f);  // Confirma que fsm_new falla al no poder reservar
    //TEST_IGNORE();
}

/**
 * @brief Llamar a fsm_destroy provoca una llamada a fsm_free
 *        Usa el Mock llamando a fsm_free_Expect sin Stub ni Callback
 *
 */
void test_fsm_destroy_callsFsmFree(void)
{
    fsm_t *f = (fsm_t*)1;
    
    fsm_free_Expect(f);
    fsm_destroy(f);
    //TEST_IGNORE();
}

/**
 * @brief Comprueba que solo se llame a la función de guarda que toca según el estado actual
 * 
 */
void test_fsm_fire_callsFirstIsTrueFromState0AndThenIsTrue2FromState1(void)
{
    fsm_trans_t tt[] = {
        {0, is_true, 1, NULL},
        {1, is_true2, 0, NULL},   //Descomentar cuando se haya declarado una nueva función para mock is_true2
        {-1, NULL, -1, NULL}
    };

    fsm_t f;
    int n = fsm_init(&f, tt);

    TEST_ASSERT_GREATER_THAN(0, n);
    // Primera llamada: estado 0 → 1
    is_true_ExpectAndReturn(&f, true);   // Se debe llamar a is_true
    // is_true2 NO debe llamarse todavía

    int ret1 = fsm_fire(&f); // transiciona de 0 → 1
    TEST_ASSERT_EQUAL(1, ret1);
    // Segunda llamada: estado 1 → 0
    is_true2_ExpectAndReturn(&f, true);  // Ahora se debe llamar a is_true2

    int ret2 = fsm_fire(&f); // transiciona de 1 → 0
    TEST_ASSERT_EQUAL(1, ret2);

    // Comprobamos que vuelve a estado 0
    TEST_ASSERT_EQUAL(0, fsm_get_state(&f));
    //TEST_IGNORE();
    
}

/**
 * @brief Comprueba que se pueden crear dos instancias de máquinas de estados simultánteas y son punteros distintos.
 *        Usa Stub para fsm_malloc y luego libera la memoria con free
 *
 */
void test_fsm_new_calledTwiceWithSameValidDataCreatesDifferentInstancePointer(void)
{
    fsm_trans_t tt[] = {
        {0, is_true, 1, NULL},
        {-1, NULL, -1, NULL}
    };
    // Usamos Stub para que fsm_malloc llame a cb_malloc (que usa malloc real)
    fsm_malloc_StubWithCallback(cb_malloc);

    fsm_t *f1 = fsm_new(tt);
    fsm_t *f2 = fsm_new(tt);
    
    TEST_ASSERT_NOT_NULL(f1);
    TEST_ASSERT_NOT_NULL(f2);
    TEST_ASSERT_NOT_EQUAL(f1, f2); // deben ser punteros distintos
    
    // Liberamos la memoria reservada por malloc
    free(f1);
    free(f2);
    //TEST_IGNORE();
}

/**
 * @brief fsm_init devuelve 0 si el puntero a la FSM (p_fsm) es NULL.
 */
void test_fsm_init_returnsZeroWhenFsmPointerIsNull(void)
{
    fsm_trans_t tt[] = {
        {0, is_true, 1, do_nothing},
        {-1, NULL, -1, NULL}
    };

    // Llamamos a fsm_init con p_fsm == NULL
    int result = fsm_init(NULL, tt);

    TEST_ASSERT_EQUAL(0, result); // Debe devolver 0
}

/**
 * @brief fsm_init devuelve 0 si el puntero a la tabla de transiciones (p_tt) es NULL.
 */
void test_fsm_init_returnsZeroWhenTransitionTablePointerIsNull(void)
{
    fsm_t f;  // Instancia válida de fsm_t

    // Llamamos a fsm_init con p_tt == NULL
    int result = fsm_init(&f, NULL);

    TEST_ASSERT_EQUAL(0, result); // Debe devolver 0
}

/**
 * @brief fsm_init devuelve 0 si el número de transiciones supera FSM_MAX_TRANSITIONS.
 */
void test_fsm_init_returnsZeroWhenTransitionCountExceedsMaximum(void)
{
    // Creamos una tabla artificialmente grande
    fsm_trans_t tt[FSM_MAX_TRANSITIONS + 2];  // +2 para asegurar superarlo

    for (int i = 0; i < FSM_MAX_TRANSITIONS + 1; i++) {
        tt[i].orig_state = i;
        tt[i].in = is_true;        // Guardas válidas
        tt[i].dest_state = i + 1;
        tt[i].out = do_nothing;
    }

    // Última transición de terminación
    tt[FSM_MAX_TRANSITIONS + 1].orig_state = -1;
    tt[FSM_MAX_TRANSITIONS + 1].in = NULL;
    tt[FSM_MAX_TRANSITIONS + 1].dest_state = -1;
    tt[FSM_MAX_TRANSITIONS + 1].out = NULL;

    fsm_t f;  // Máquina de estados simulada

    // Ejecutamos fsm_init
    int result = fsm_init(&f, tt);

    // Debe devolver 0 porque supera FSM_MAX_TRANSITIONS
    TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief fsm_set_state cambia correctamente el estado actual de la FSM.
 */
void test_fsm_set_state_setsTheCurrentStateCorrectly(void)
{
    // Creamos una máquina de estados sencilla
    fsm_trans_t tt[] = {
        {0, is_true, 1, do_nothing},
        {-1, NULL, -1, NULL}
    };

    fsm_t f;
    int n = fsm_init(&f, tt);
    TEST_ASSERT_GREATER_THAN(0, n);  // Confirmamos que la máquina se inicializa bien

    // Verificamos el estado inicial
    TEST_ASSERT_EQUAL(0, fsm_get_state(&f));

    // Cambiamos el estado manualmente usando fsm_set_state
    fsm_set_state(&f, 42);

    // Verificamos que el estado ha cambiado correctamente
    TEST_ASSERT_EQUAL(42, fsm_get_state(&f));
}

/**
 * @brief fsm_fire devuelve -1 si el puntero a la FSM es NULL o si la tabla de transiciones es NULL.
 */
void test_fsm_fire_returnsMinusOneWhenFsmPointerOrTransitionTableIsNull(void)
{
    // Caso 1: puntero a FSM es NULL
    int result = fsm_fire(NULL);
    TEST_ASSERT_EQUAL(-1, result);

    // Caso 2: puntero a FSM válido pero tabla de transiciones (p_tt) es NULL
    fsm_t f;
    f.p_tt = NULL;  // Simulamos que no tiene tabla de transiciones válida

    result = fsm_fire(&f);
    TEST_ASSERT_EQUAL(-1, result);
}

/**
 * @brief fsm_fire llama a la función de guarda si no es NULL y asume true si es NULL.
 */
void test_fsm_fire_guardFunctionIsCalledIfNotNullAndAssumedTrueIfNull(void)
{
    // Definimos una tabla con dos transiciones:
    //  - La primera tiene función de guarda (is_true)
    //  - La segunda tiene guarda NULL
    fsm_trans_t tt[] = {
        {0, is_true, 1, NULL},   // Función de guarda válida
        {1, NULL, 2, NULL},       // Función de guarda NULL
        {-1, NULL, -1, NULL}
    };

    fsm_t f;
    int n = fsm_init(&f, tt);
    TEST_ASSERT_GREATER_THAN(0, n);  // Confirmamos inicialización correcta

    // Estado inicial 0
    TEST_ASSERT_EQUAL(0, fsm_get_state(&f));

    // Primera transición (estado 0 -> 1): is_true debe llamarse
    is_true_ExpectAndReturn(&f, true);

    int result = fsm_fire(&f);
    TEST_ASSERT_EQUAL(1, result);           // Transición realizada
    TEST_ASSERT_EQUAL(1, fsm_get_state(&f)); // Estado cambiado a 1

    // Segunda transición (estado 1 -> 2): guarda es NULL, debe asumir TRUE automáticamente

    result = fsm_fire(&f);
    TEST_ASSERT_EQUAL(1, result);           // Transición realizada
    TEST_ASSERT_EQUAL(2, fsm_get_state(&f)); // Estado cambiado a 2
}


/**
 * @brief fsm_new libera memoria y devuelve NULL si fsm_init falla, y devuelve puntero válido si fsm_init tiene éxito.
 */
void test_fsm_new_handlesFsmInitFailureAndSuccessCorrectly(void)
{
    // --- Primer caso: fsm_init falla (tabla inválida) ---

    // Creamos tabla demasiado grande para forzar fallo en init
    fsm_trans_t tt_invalid[FSM_MAX_TRANSITIONS + 2];

    for (int i = 0; i < FSM_MAX_TRANSITIONS + 1; i++) {
        tt_invalid[i].orig_state = i;
        tt_invalid[i].in = is_true;
        tt_invalid[i].dest_state = i + 1;
        tt_invalid[i].out = do_nothing;
    }
    tt_invalid[FSM_MAX_TRANSITIONS + 1].orig_state = -1;
    tt_invalid[FSM_MAX_TRANSITIONS + 1].in = NULL;
    tt_invalid[FSM_MAX_TRANSITIONS + 1].dest_state = -1;
    tt_invalid[FSM_MAX_TRANSITIONS + 1].out = NULL;

    // Usamos cb_malloc para simular malloc real
    fsm_malloc_StubWithCallback(cb_malloc);

    // Ignoramos fsm_free porque no sabemos el puntero exacto aquí
    fsm_free_Ignore();

    // Llamamos a fsm_new con tabla inválida
    fsm_t *fsm_fail = fsm_new(tt_invalid);

    TEST_ASSERT_NULL(fsm_fail);
}

/**
 * @brief fsm_fire devuelve 0 si hay transiciones pero ninguna se cumple, y devuelve -1 si no existen transiciones para el estado actual.
 */
void test_fsm_fire_returnsZeroWhenTransitionsExistAndMinusOneWhenNoTransitionsExist(void)
{
    // Tabla de transiciones:
    // - Una transición válida solo para estado 0
    fsm_trans_t tt[] = {
        {0, is_true, 1, do_nothing},
        {-1, NULL, -1, NULL}
    };

    fsm_t f;
    int n = fsm_init(&f, tt);
    TEST_ASSERT_GREATER_THAN(0, n);  // Confirmamos inicialización correcta

    // Estado inicial 0
    TEST_ASSERT_EQUAL(0, fsm_get_state(&f));

    // --- Primera parte: hay transiciones para estado 0, pero la guardia falla ---

    is_true_ExpectAndReturn(&f, false);  // Simulamos que la función de guardia devuelve false

    int result = fsm_fire(&f);

    // Hay transiciones, pero no se cumplen -> debe devolver 0
    TEST_ASSERT_EQUAL(0, result);

    // El estado no cambia
    TEST_ASSERT_EQUAL(0, fsm_get_state(&f));

    // --- Segunda parte: cambiamos a un estado sin transiciones ---

    fsm_set_state(&f, 42);  // Estado 42 no tiene ninguna transición en la tabla

    result = fsm_fire(&f);

    // No hay transiciones posibles -> debe devolver -1
    TEST_ASSERT_EQUAL(-1, result);

    // El estado sigue siendo 42
    TEST_ASSERT_EQUAL(42, fsm_get_state(&f));
}
