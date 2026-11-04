# Ficha del Cliente e Historias de Usuario - Video Shop

**Proyecto:** Video Shop — Gestión de una Tienda con Patrones de Software
**Curso:** Patrones de Software (Código 74362) — Cotecnova 2026-2
**Docente:** Felipe Gutiérrez Isaza
**Elaborado por:** Responsable de modelado — Sebastián Orozco
**Entrega:** Seguimiento 1

---

## 1. Identificación del Negocio

- **Nombre comercial:** Video Shop.
- **Tipo de tienda:** Tienda especializada en videojuegos, consolas y entretenimiento digital.
- **Productos comercializados:**
  1. **Videojuegos físicos (`tipo 1`):** Juegos en formato disco o cartucho, gravados con IVA estándar del 19 %.
  2. **Videojuegos digitales y DLCs (`tipo 2`):** Códigos de descarga y expansiones digitales, sujetos a un cargo de servicio de plataforma del 5 %.
  3. **Consolas y hardware (`tipo 3`):** Equipos de juego que aplican IVA del 19 % más una póliza obligatoria de garantía extendida de $5.000 COP por unidad.
  4. **Accesorios (`tipo 4`):** Controles inalámbricos, periféricos y complementos sujetos a IVA del 19 %.
- **Contexto y propósito del negocio:** Video Shop es un establecimiento comercial minorista dedicado a la venta directa en mostrador de artículos de videojuegos. Su propósito operativo es mantener un control ordenado de sus existencias por categoría, agilizar el cobro en caja calculando correctamente los cargos de cada producto y las condiciones del medio de pago, y llevar el registro de los ingresos generados durante la jornada.

---

## 2. Problema y Necesidades del Cliente

Antes de contar con el sistema, las operaciones cotidianas de mostrador e inventario presentan las siguientes necesidades operativas que justifican el desarrollo de la aplicación:

1. **Registro estructurado de productos e inventario (F1):** Se requiere incorporar nuevos artículos al catálogo asignándoles un código único, nombre, precio unitario base, stock inicial y categoría (físico, digital, consola o accesorio), evitando la duplicidad de códigos.
2. **Consulta rápida de existencias y búsqueda de artículos (F2):** El personal necesita visualizar en cualquier momento el listado completo del inventario con sus precios base, cantidades disponibles y tipo de producto, así como localizar un artículo específico por su código durante el proceso de venta.
3. **Registro de ventas con múltiples productos (F3):** Un cliente de la tienda suele adquirir varios artículos y distintas cantidades en una misma compra (por ejemplo, una consola junto con controles o videojuegos), por lo que el sistema debe permitir conformar un carrito de varios renglones dentro de una sola transacción.
4. **Cálculo exacto de impuestos, cargos y ajustes por forma de pago (F4):** Cada categoría de producto tiene un tratamiento tributario o cargo distinto (IVA del 19 %, servicio digital del 5 % o póliza de garantía de $5.000 COP), y el valor total varía según la forma de pago elegida por el comprador:
   - **Efectivo (`opción 1`):** Sin descuentos ni recargos adicionales.
   - **Transferencia bancaria (`opción 2`):** Descuento comercial del 3 % sobre el subtotal.
   - **Tarjeta de crédito (`opción 3`):** Descuento del 3 % más un recargo fijo de pasarela de $800 COP.
5. **Control automático de stock y alertas de reabastecimiento (F5):** Al concretar la salida de unidades en una venta, el sistema debe validar que exista stock suficiente, descontar inmediatamente las unidades vendidas y emitir una alerta visible cuando el inventario restante de un artículo sea inferior a 5 unidades.
6. **Historial de ventas y consolidado de caja (F6):** Se requiere consultar un reporte con todas las ventas efectuadas (número de venta, cantidad total de artículos, forma de pago y valor final cobrado) junto con el recaudo total acumulado en caja.

---

## 3. Usuarios del Sistema e Interesados

El sistema es una aplicación de consola operada directamente en el punto de venta por dos perfiles de usuario internos:

| Usuario / Rol | Responsabilidades en el negocio | Interacción con la aplicación |
| :--- | :--- | :--- |
| **Administrador de la tienda** | Gestionar el catálogo de artículos, supervisar las existencias en bodega, atender las alertas de stock crítico y revisar el cierre de ingresos de las ventas. | Utiliza las funciones de registro de nuevos productos (**F1**), consulta general de inventario (**F2**), verificación de alertas de stock bajo (**F5**) y generación del reporte histórico de ventas (**F6**). |
| **Cajero / Vendedor** | Atender al comprador en el mostrador, verificar disponibilidad y precios de los productos solicitados, registrar las ventas aplicando el medio de pago y entregar el comprobante. | Utiliza las funciones de consulta de inventario y búsqueda por código (**F2**), registro de ventas multi-producto (**F3**), liquidación de impuestos y formas de pago (**F4**) y visualización del comprobante de venta. |

> **Nota sobre los compradores:** Los clientes finales de la tienda reciben la atención en mostrador, pero no interactúan directamente con la aplicación de consola ni poseen cuentas de usuario registradas en el sistema.

---

## 4. Historias de Usuario

A continuación se definen **7 historias de usuario** bajo el formato estándar exigido, alineadas con las funciones mínimas **F1 a F6** implementadas en la Versión 0 (`v0/tienda_v0.cpp`):

| ID | Función | Historia de Usuario | Criterio de aceptación en la aplicación |
| :--- | :---: | :--- | :--- |
| **HU-01** | **F1** | **Como** administrador de la tienda, **quiero** registrar nuevos productos indicando su código, nombre, precio unitario, stock inicial y tipo de producto, **para** mantener actualizado el catálogo de artículos disponibles para la venta. | El sistema valida que el código no exista previamente y que el tipo esté entre 1 y 4 antes de guardar el producto en memoria. |
| **HU-02** | **F2** | **Como** vendedor, **quiero** consultar el listado completo del inventario con el código, nombre, precio base, stock actual y categoría de cada artículo, **para** informar con precisión a los compradores sobre los productos disponibles. | El sistema despliega una tabla formateada por consola con los productos precargados y los registrados durante la sesión. |
| **HU-03** | **F2 / F3** | **Como** cajero, **quiero** buscar un producto por su código al momento de armar una venta y verificar sus unidades disponibles, **para** asegurar que el artículo existe y cuenta con stock suficiente antes de agregarlo al carrito. | Si el código no existe o la cantidad solicitada supera el stock actual, el sistema informa el error y no agrega el renglón. |
| **HU-04** | **F3** | **Como** cajero, **quiero** registrar una venta que permita incluir varios productos y diferentes cantidades en una misma transacción, **para** facturar el pedido completo de un comprador en un solo comprobante. | El sistema permite agregar múltiples ítems de forma iterativa hasta que el cajero indique que no desea añadir más productos. |
| **HU-05** | **F4** | **Como** cajero, **quiero** que el sistema calcule automáticamente los impuestos o cargos por tipo de producto (IVA del 19 %, cargo digital del 5 % o garantía de $5.000 COP) y aplique el descuento o recargo según la forma de pago elegida, **para** cobrar el valor exacto sin errores de cálculo manual. | El comprobante muestra el precio final de cada línea con sus cargos, el subtotal, el ajuste por efectivo, transferencia (-3 %) o tarjeta (-3 % + $800 COP) y el total pagado. |
| **HU-06** | **F5** | **Como** administrador de la tienda, **quiero** que el sistema descuente automáticamente del inventario las unidades vendidas y muestre una alerta en pantalla cuando el stock de un producto quede por debajo de 5 unidades, **para** identificar oportunamente qué artículos requieren reabastecimiento. | Al registrar cada ítem vendido, el stock disminuye en la cantidad comprada y, si el saldo es menor a 5, se imprime `[ALERTA DE STOCK (F5)]`. |
| **HU-07** | **F6** | **Como** administrador de la tienda, **quiero** consultar un reporte histórico con todas las ventas realizadas y el recaudo total acumulado en caja, **para** conocer el desempeño de las ventas y el dinero total ingresado durante la operación. | El reporte lista el número de venta, total de artículos, forma de pago, valor total de cada transacción y la suma global recaudada. |

---

## 5. Alcance de la Aplicación y Supuestos

De acuerdo con los lineamientos del curso y la implementación real en `v0/tienda_v0.cpp`, se establece claramente la distinción entre lo incluido en el sistema y lo que está fuera del alcance:

### 5.1 Funcionalidades implementadas en la Versión 0
- **Ejecución en consola:** Interfaz interactiva por menú de texto en C++ estándar.
- **Almacenamiento en memoria:** Gestión de productos e historial de ventas mediante estructuras en memoria (`std::vector`), iniciando con 5 productos semilla representativos de las 4 categorías.
- **Ciclo completo F1–F6:** Registro de productos con validación de código único, consulta de inventario, venta multi-artículo, cálculo de impuestos por tipo y ajustes por medio de pago, control de stock con alerta en umbral menor a 5 unidades y reporte general de ventas.

### 5.2 Aspectos fuera del alcance (No implementados)
- **Sin persistencia en base de datos ni archivos:** Los datos residen únicamente en memoria RAM durante la ejecución del programa.
- **Sin interfaz gráfica de usuario (GUI):** Toda la interacción se realiza por entrada y salida estándar de consola.
- **Sin gestión de cuentas de clientes ni pasarelas en línea:** No existe registro de datos personales de compradores ni conexión real con entidades bancarias; la forma de pago representa la regla de liquidación aplicada en caja.
