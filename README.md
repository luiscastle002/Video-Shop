# Video Shop — Gestión de una Tienda con Patrones de Software

Proyecto final para el curso de **Patrones de Software** (Código 74362) — Periodo 2026-2.
**Institución:** Corporación de Estudios Tecnológicos del Norte del Valle — Cotecnova
**Docente:** Felipe Gutiérrez Isaza

---

## 1. Descripción del Proyecto

**Video Shop** es una aplicación de consola desarrollada en **C++17** (sin dependencias externas ni motor de base de datos) que gestiona el catálogo de productos, inventario y ventas de una tienda especializada en videojuegos, consolas y accesorios de entretenimiento digital:

- **Juegos Físicos:** Gravados con IVA del 19 %.
- **Juegos Digitales / DLCs:** Sujetos a cargo de servicio de plataforma del 5 %.
- **Consolas / Hardware:** Gravados con IVA del 19 % más póliza de garantía extendida ($5.000 COP por unidad).
- **Accesorios:** Periféricos gravados con IVA del 19 %.

El proyecto sigue un enfoque evolutivo en dos versiones:
- **Versión 0 (`v0/`):** Implementación inicial directa y monolítica sin patrones de diseño, que evidencia los antipatrones y problemas de acoplamiento cuando el sistema crece.
- **Versión 1 (`v1/`):** Rediseño arquitectónico orientado a objetos que conserva todas las funcionalidades originales (F1–F6), corrige los antipatrones diagnosticados e integra tres patrones GoF en el flujo de venta, además de incorporar un menú numérico validado y persistencia local del catálogo en CSV (en la versión modular).

---

## 2. Integrantes y Responsabilidades

| Nombre | Rol | Responsabilidad Principal |
| :--- | :--- | :--- |
| **Luis Cardona** | **Responsable de código** | Coordina la construcción de las versiones 0 y 1, la integración técnica, compilación, pruebas y control de versiones. |
| **Felipe Muñoz Cano** | **Responsable de diseño** | Coordina el diagnóstico de antipatrones, la selección y justificación de los patrones de diseño y la coherencia arquitectural. |
| **Sebastian Orozco** | **Responsable de modelado** | Elabora los diagramas UML (clases, secuencia y comunicación) y mantiene la documentación funcional. |

---

## 3. Funcionalidades Principales

- **F1 — Registro de productos:** Alta de artículos con código único, nombre, categoría, precio positivo y stock no negativo.
- **F2 — Consulta de inventario:** Listado formateado con código, nombre, precio base, stock actual y categoría.
- **F3 — Registro de ventas multi-artículo:** Carrito interactivo con búsqueda por código, validación de existencias acumuladas y cancelación segura sin alterar el stock.
- **F4 — Cálculo de impuestos, cargos y medios de pago:** Liquidación automática por categoría (IVA 19 %, plataforma 5 %, garantía $5.000/unidad) y ajuste según medio de pago (Efectivo, Transferencia -3 %, Tarjeta -3 % + $800 COP).
- **F5 — Control de stock y alertas:** Descuento transaccional al confirmar la venta y alerta automática cuando el saldo queda por debajo de 5 unidades.
- **F6 — Comprobantes y reporte de ventas:** Emisión de factura detallada por transacción y reporte histórico de ventas de la sesión con recaudo acumulado.
- **Persistencia local (V1 modular):** Almacenamiento del catálogo de productos y existencias en `v1/data/productos.csv` con reconstrucción polimórfica al reiniciar.

---

## 4. Patrones de Diseño Aplicados (Versión 1)

| Patrón | Categoría | Propósito dentro del proyecto | Clases principales (`v1/`) |
| :--- | :--- | :--- | :--- |
| **Factory Method** | Creacional | Creación polimórfica de productos según categoría ensamblando su configuración de cálculo de precio sin `switch`. | `FabricaProducto`, `FabricaFisico`, `FabricaDigital`, `FabricaConsola`, `FabricaAccesorio` |
| **Decorator** | Estructural | Composición dinámica de impuestos, cargos de plataforma y garantías sobre el precio base del producto. | `CalculoPrecio`, `PrecioBase`, `DecoradorPrecio`, `ConIVA`, `ConCargoPlataforma`, `ConGarantia` |
| **Strategy** | Comportamiento | Encapsulación de las reglas de descuento, recargo y descripción de cada medio de pago durante la liquidación de la venta. | `MedioPago`, `PagoEfectivo`, `PagoTransferencia`, `PagoTarjeta` |

---

## 5. Requisitos Técnicos, Compilación y Ejecución

- **Estándar requerido:** C++17 (`-std=c++17`), sin bibliotecas externas.
- **Compilador verificado:** GNU C++ (`g++` 16.1.0 en Windows; compatible con entornos GCC estándar en Linux/macOS).

### 5.1. Versión 0 (Sin patrones — Inmutable tras Seguimiento 1)

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic v0/tienda_v0.cpp -o tienda_v0.exe
```
Ejecución en Windows:
```powershell
.\tienda_v0.exe
```

### 5.2. Versión 1 Modular (Implementación Principal)

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic v1/main.cpp v1/Tienda.cpp -o tienda_v1.exe
```
Ejecución en Windows:
```powershell
.\tienda_v1.exe
```

### 5.3. Versión 1 Unificada (Archivo Único para Compiladores Online)

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic v1/tienda_v1_online.cpp -o tienda_v1_online.exe
```
Ejecución en Windows:
```powershell
.\tienda_v1_online.exe
```

#### Ejecución en OnlineGDB
1. Abrir `https://www.onlinegdb.com/`.
2. Seleccionar el lenguaje **C++17**.
3. Copiar y pegar el contenido completo de [`v1/tienda_v1_online.cpp`](v1/tienda_v1_online.cpp) en el editor (no requiere copiar archivos `.h` ni `.cpp` adicionales).
4. Ejecutar el programa y operar el menú interactivo (`1`, `2`, `3`, `4`, `0`).

> **Nota sobre la persistencia de datos:** La versión modular persiste el catálogo de productos y su stock en `v1/data/productos.csv` (protegido contra errores mediante escritura temporal atómica y excluido del control de versiones en `.gitignore`). La versión unificada (`v1/tienda_v1_online.cpp`) inicia por defecto en **modo demostrativo en memoria** con los 5 productos semilla de V0 para garantizar ejecuciones limpias en compiladores web (puede activarse archivo local pasando `--con-persistencia`). En ambas versiones, el historial de ventas (`F6`) corresponde a la sesión en curso en memoria.

---

## 6. Diagramas UML Oficiales y Documentación

### Diagramas UML Oficiales (`docs/uml/`)
- [Diagrama de Clases — Versión 0](docs/uml/clases_v0.png): Estructura monolítica inicial (`TiendaVideojuegos`, `Producto`, `ItemVenta`, `Venta`).
- [Diagrama de Clases — Versión 1](docs/uml/clases_v1.png): Arquitectura rediseñada con los tres patrones GoF diferenciados por color.
- [Diagrama de Secuencia — Registrar una Venta](docs/uml/secuencia_venta.png): Orden temporal de mensajes entre objetos en el flujo de venta.
- [Diagrama de Comunicación — Registrar una Venta](docs/uml/comunicacion_venta.png): Organización estructural de objetos y numeración jerárquica de mensajes.

### Documentación del Proyecto y Evidencia
- [Ficha del Cliente e Historias de Usuario](docs/01_ficha_cliente.md)
- [Informe de Antipatrones y Olores de Código (V0)](docs/02_antipatrones.md)
- [Justificación de los Patrones de Diseño (V1)](docs/03_justificacion_patrones.md)
- [Salida Esperada de Ejecución (V1)](v1/salida_esperada.txt)

---

## 7. Estructura del Repositorio

```text
Video-Shop/
├── README.md
├── .gitignore
├── docs/
│   ├── 01_ficha_cliente.md
│   ├── 02_antipatrones.md
│   ├── 03_justificacion_patrones.md
│   ├── INSTRUCCIONES_MODELADO.md     ← Documento auxiliar histórico
│   └── uml/
│       ├── clases_v0.png
│       ├── clases_v1.png
│       ├── secuencia_venta.png
│       └── comunicacion_venta.png
├── Diagrama de clases v0.drawio.pdf  ← Documento auxiliar histórico
├── v0/
│   └── tienda_v0.cpp                 ← Versión 0 sin patrones
└── v1/
    ├── CalculoPrecio.h
    ├── FabricaProducto.h
    ├── Inventario.h
    ├── MedioPago.h
    ├── Producto.h
    ├── Tienda.cpp
    ├── Venta.h
    ├── main.cpp
    ├── tienda_v1_online.cpp          ← Versión unificada autocontenida
    └── salida_esperada.txt
```

---

## 8. Etiquetas de Entrega en Git

- **Seguimiento 3:** `entrega-s3`
- **Entrega Final:** `entrega-final`
