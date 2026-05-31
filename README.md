# 🚀 SD2 — UART2 + DMA en MCXC444

> **RX por interrupción. TX por DMA. CPU libre para hacer lo que quiera.**

Driver de UART2 para el microcontrolador **MCXC444** que combina recepción por interrupción con transmisión asistida por DMA — sin bloquear al procesador mientras manda datos.

---

## ¿Qué hace exactamente?

El driver inicializa UART2 a **115200 bps (8N1)** y expone dos mecanismos bien diferenciados:

| Canal | Mecanismo | ¿Qué hace el CPU? |
|-------|-----------|-------------------|
| 📥 **RX** | Interrupción por byte | Ejecuta tu callback al instante |
| 📤 **TX** | DMA | **Nada** — el DMA lo maneja solo |

El ejemplo en `main.c` recibe un byte por UART2 y lo imprime por la consola de debug de MCUXpresso con `PRINTF`. Simple, pero muestra el flujo completo.

---

## 🔧 Hardware necesario

- Placa **FRDM-MCXC444**
- **Conversor USB-TTL** (el debugger OpenSDA/LPUART0 no sirve acá — UART2 es independiente)

### Conexiones

```
MCXC444          Conversor USB-TTL
  PTD2  ───────►  RX
  PTD3  ◄───────  TX
  GND   ─────────  GND
```

> ⚠️ Sin el conversor no vas a poder ver nada. No es opcional.

---

## ⚙️ Configuración del terminal serie

Abrí PuTTY, Tera Term, o lo que uses, con estos parámetros:

| Parámetro | Valor |
|-----------|-------|
| Velocidad | **115200 bps** |
| Bits de datos | 8 |
| Paridad | Ninguna |
| Bits de stop | 1 |
| Control de flujo | Ninguno |

---

## ▶️ Cómo correrlo

```
1. Importar el proyecto en MCUXpresso IDE
2. Compilar y flashear en la placa
3. Conectar el conversor USB-TTL a PTD2/PTD3
4. Abrir el terminal serie
5. Mandar cualquier carácter → el micro te lo muestra en la consola de debug
```

---

## 📁 Estructura del proyecto

```
SD2_MCXC444_UART2_DMA/
├── source/      → Driver uart2_drv.c/h + main.c (ejemplo)
├── drivers/     → SDK de NXP: UART, DMA, DMAMUX
├── device/      → Headers del MCXC444
└── board/       → Pines, clocks y consola
```
