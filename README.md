# SD2_MCXC444_UART2_DMA

Ejemplo de manejo de UART2 en el microcontrolador MCXC444, con **recepción por interrupción** y **transmisión por DMA**.

## Descripción

El driver inicializa UART2 a 115200 bps (8N1) y expone dos mecanismos:

- **RX:** cada byte recibido dispara una interrupción que llama al callback registrado por el usuario.
- **TX:** los datos se envían mediante DMA, liberando al CPU durante la transmisión.

El ejemplo en `main.c` recibe un byte por UART2 y lo muestra por la consola de debug de MCUXpresso usando `PRINTF`.

## Hardware requerido

- Placa FRDM-MCXC444
- **Conversor USB-TTL** conectado a:
  - **PTD2** → RX del conversor (recepción del micro)
  - **PTD3** → TX del conversor (transmisión del micro)
  - **GND** → GND común

> Sin el conversor USB-TTL no es posible verificar el funcionamiento del driver, ya que UART2 es independiente del puerto serie del debugger (OpenSDA/LPUART0).

## Configuración del terminal serie

Abrir un terminal serie (PuTTY, Tera Term, etc.) con los siguientes parámetros:

| Parámetro | Valor |
|-----------|-------|
| Velocidad | 115200 bps |
| Bits de datos | 8 |
| Paridad | Ninguna |
| Bits de stop | 1 |
| Control de flujo | Ninguno |

## Uso

1. Importar el proyecto en **MCUXpresso IDE**
2. Compilar y flashear en la placa
3. Conectar el conversor USB-TTL a PTD2/PTD3
4. Abrir el terminal serie y enviar cualquier carácter
5. El micro responde mostrando el byte recibido en la consola de debug

## Estructura del proyecto

| Carpeta | Contenido |
|---------|-----------|
| `source/` | Código fuente del driver (`uart2_drv.c/h`) y ejemplo (`main.c`) |
| `drivers/` | Drivers del SDK de NXP (UART, DMA, DMAMUX) |
| `device/` | Headers del dispositivo MCXC444 |
| `board/` | Configuración de pines, clocks y consola |
