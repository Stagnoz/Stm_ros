/**
 ******************************************************************************
 * @file    shared_data.h
 * @brief   Struttura dati condivisa tra CM7 e CM4 via SRAM4 (0x38000000)
 *
 * Questo header va incluso da ENTRAMBI i core (CM4 e CM7).
 * I linker scripts di entrambi i core mappano la sezione .shared_data
 * nella regione SHARED_RAM a 0x38000000.
 ******************************************************************************
 */

#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <stdint.h>

/* ── HSEM IDs ─────────────────────────────────────────────────────────────── */
/* HSEM_ID 0 è usato per la sincronizzazione al boot (vedi CM7 main.c).       */
/* Usiamo un ID diverso per la notifica dati sensore.                          */
#define HSEM_ID_SENSOR  1U

/* ── Struttura dati condivisa ──────────────────────────────────────────────── */
typedef struct
{
    volatile uint32_t distance_cm;   /**< Distanza misurata dal sensore (cm)  */
    volatile uint32_t data_ready;    /**< 1 = dato nuovo pronto, 0 = già letto */
} SharedData_t;

/* Puntatore di accesso alla struttura condivisa.                              *
 * L'indirizzo è il primo byte della SRAM4, identico in entrambi i linker.    */
#define SHARED_DATA  ((SharedData_t *)0x38000000U)

#endif /* SHARED_DATA_H */
