# Walkthrough — Shared Memory IPC on `microroseth`

All changes to enable CM4 → CM7 sensor data transfer via SRAM4 shared memory.

---

## 1. [shared_data.h](file:///c:/Polispace_Board/Micro_ros_eth/microroseth/Common/Inc/shared_data.h) — NEW

Shared header defining the inter-core data structure at `0x38000000` (SRAM4, 64KB, D3 domain).

```c
typedef struct {
  volatile uint32_t distance_cm;  // sensor distance [cm]
  volatile uint32_t data_ready;   // 1 = new data available
} SharedData_t;

#define SHARED_DATA    ((SharedData_t *)0x38000000U)
#define HSEM_ID_SENSOR 1U
```

---

## 2. [CM7 main.c](file:///c:/Polispace_Board/Micro_ros_eth/microroseth/CM7/Core/Src/main.c) — MODIFIED

### 2a. Added `#include "shared_data.h"`
```diff
 #include "microros_transports.h"
 #include "microros_sim_network.h"
+#include "shared_data.h"
```

### 2b. New FreeRTOS task: [StartSensorDataTask](file:///c:/Polispace_Board/Micro_ros_eth/microroseth/CM7/Core/Src/main.c#67-86)
Polls shared memory every 100ms. Uses `__DSB()` memory barriers for multi-core coherency.

```diff
+static void StartSensorDataTask(void *argument)
+{
+  for(;;)
+  {
+    __DSB();
+    if(SHARED_DATA->data_ready)
+    {
+      uint32_t dist = SHARED_DATA->distance_cm;
+      SHARED_DATA->data_ready = 0U;
+      __DSB();
+      printf("CM7 ricevuto: distanza = %lu cm\r\n", dist);
+    }
+    osDelay(100);
+  }
+}
```

### 2c. Task created in [MX_FREERTOS_Init](file:///c:/Polispace_Board/Micro_ros_eth/microroseth/CM7/Core/Src/main.c#477-516)
```diff
+  sensorDataTaskHandle = osThreadNew(StartSensorDataTask, NULL,
+                                     &sensor_data_task_attributes);
```
Stack: 1024B, priority: `osPriorityNormal`.

### 2d. APB1 clock divider aligned to CM4
```diff
-  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
+  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV4;
```
**Why:** RCC registers are shared. If CM4 uses DIV4 and CM7 uses DIV2, USART3 baud rate gets corrupted when CM4 boots.

---

## 3. [CM7 Makefile](file:///c:/Polispace_Board/Micro_ros_eth/microroseth/Makefile/CM7/Makefile) — MODIFIED

```diff
 C_INCLUDES = \
 -I../../CM7/Core/Inc \
+-I../../Common/Inc \
 -I../../CM7/LWIP/App \
```

---

## 4. [CM4 Makefile](file:///c:/Polispace_Board/Micro_ros_eth/microroseth/Makefile/CM4/Makefile) — MODIFIED

```diff
 C_INCLUDES = \
 -I../../CM4/Core/Inc \
+-I../../Common/Inc \
 -I../../Drivers/STM32H7xx_HAL_Driver/Inc \
```

---

## Data Flow

```mermaid
graph LR
    A["Sensore JSN-SR04T"] -->|echo| B["CM4: dist_cm"]
    B -->|"write + __DSB()"| C["SRAM4 @ 0x38000000"]
    C -->|"__DSB() + polling"| D["CM7: SensorDataTask"]
    D -->|printf / USART3| E["ST-Link VCP → PC"]
```

## Build & Flash

```bash
cd Makefile/CM7 && make clean && make
cd ../CM4  && make clean && make
```
Flash CM7 at `0x08000000`, then CM4 at `0x08100000` (no full chip erase for second flash).
