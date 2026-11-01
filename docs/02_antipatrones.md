# Informe de Antipatrones y Olores de Codigo - Version 0

**Proyecto:** Video Shop - Tienda de Videojuegos
**Curso:** Patrones de Software (Codigo 74362) - Cotecnova 2026-2
**Archivo analizado:** `v0/tienda_v0.cpp` (etiqueta `entrega-s1`, no modificado)
**Elaborado por:** Responsable de diseno
**Entrega:** Seguimiento 2

> La version 0 no se corrige. Todos los problemas de este informe son el punto de partida
> para justificar los patrones de la version 1 (ver `03_justificacion_patrones.md`).

---

## 1. Resumen de hallazgos

| ID | Antipatron / olor | Ubicacion (`v0/tienda_v0.cpp`) | Gravedad |
|---|---|---|---|
| AP-01 | Clase Dios (God Class) | L51-58 (comentario), clase completa L58-339; metodo `registrarVenta()` L154-300 | Alta |
| AP-02 | `switch` por tipo de producto | L185-201 (impuestos), L126-132 (presentacion) | Alta |
| AP-03 | Condicionales de forma de pago repetidos | L251-255 (calculo), L288-294 (impresion), L323-326 (reporte) | Alta |
| AP-04 | Numeros magicos | L187, L190, L193, L196, L216, L252, L254, L290 | Media |
| AP-05 | Codigo duplicado | L187 y L196; L252 y L254; L251-255, L288-294 y L323-326 | Media |
| AP-06 | Notificacion acoplada a la consola | L216-219 | Media |
| AP-07 | Datos publicos y sin encapsulacion | L59-63, L25-49, L213 | Baja |

---

## 2. Detalle de cada hallazgo

### AP-01 - Clase Dios (God Class)
- **Donde:** `class TiendaVideojuegos`, L58-339. El propio codigo lo declara en L51-57.
- **Evidencia:** una sola clase administra el inventario (L75-151), el carrito y la venta (L154-300), el
  calculo de impuestos (L185-201), los descuentos (L251-255), el control de stock (L213), las alertas
  (L216-219), la entrada y salida por consola en todos los metodos, el comprobante (L269-299) y el reporte
  (L303-338). `registrarVenta()` por si solo mezcla unas 147 lineas de responsabilidades distintas.
- **Problema que causa:** viola el principio de responsabilidad unica. Cualquier cambio (un impuesto nuevo,
  otro formato de comprobante, otra regla de stock) obliga a editar la misma clase y el mismo metodo.
  No se puede probar la logica de precios sin pasar por `cin` y `cout`.

### AP-02 - `switch` por tipo de producto
- **Donde:**
  - L185-201: calculo del precio con impuesto segun `prod->tipo`.
  - L126-132: traduccion de `tipo` a texto para mostrar el inventario.
- **Evidencia:** el comportamiento de cada tipo (1 fisico, 2 digital, 3 consola, 4 accesorio) vive en
  condicionales, y el tipo es un `int` sin significado en el compilador (L30).
- **Problema que causa:** viola el principio abierto/cerrado. Agregar un tipo nuevo (por ejemplo una
  suscripcion) obliga a tocar como minimo L99-105, L126-132 y L185-201. Un `tipo` mal escrito cae en
  `default` y se cobra sin impuesto, sin avisar.

### AP-03 - Condicionales de forma de pago repetidos
- **Donde:** L251-255 (total), L288-294 (lineas de descuento del comprobante) y L323-326 (texto del reporte).
  El menu de L236-238 repite ademas la descripcion de las reglas.
- **Evidencia:** la forma de pago es un `int` (L46, L240) y cada decision se toma con `if/else` por valor.
- **Problema que causa:** una nueva forma de pago o una nueva promocion exige modificar al menos tres
  lugares. Si se olvida uno, el comprobante o el reporte quedan inconsistentes con el total. Un valor fuera
  de 1 a 3 no se valida (L240-241): se cobra como efectivo y el reporte lo muestra como "Otro" (L326).

### AP-04 - Numeros magicos
- **Donde y valor:**

| Linea | Literal | Significado real |
|---|---|---|
| L187, L193, L196 | `1.19` | IVA 19% |
| L190 | `1.05` | Cargo de plataforma digital 5% |
| L193 | `5000.0` | Poliza de garantia por unidad |
| L216 | `5` | Umbral de stock critico |
| L252, L254 | `0.97` | Descuento 3% |
| L254 | `800.0` | Recargo de pasarela |
| L290 | `0.03` | Descuento 3% (otra forma de escribir lo mismo) |

- **Problema que causa:** los valores no tienen nombre ni un unico lugar donde cambiarlos. Si el IVA cambia,
  hay que buscar y editar tres lineas distintas. El descuento del 3% aparece como `0.97` y como `0.03`, y
  pueden quedar desfasados.

### AP-05 - Codigo duplicado
- **Donde:**
  - L187 y L196: la formula `precioBaseLinea * 1.19` esta copiada para fisicos y accesorios.
  - L252 y L254: el descuento del 3% se aplica en dos ramas.
  - L251-255, L288-294 y L323-326: la logica de forma de pago se repite en calculo, impresion y reporte.
  - L113-115 y L269-271, L304-306: los bloques de bordes y titulos de tablas se repiten.
- **Problema que causa:** correccion en un sitio y olvido en otro. Es la principal fuente de errores al
  cambiar una regla de negocio.

### AP-06 - Notificacion acoplada a la consola
- **Donde:** L215-219.
- **Evidencia:** la regla de negocio "stock menor a 5" y el mecanismo de aviso (`cout`) estan en el mismo
  bloque, dentro del flujo de venta.
- **Problema que causa:** el aviso solo puede ir a la pantalla y solo para quien esta vendiendo. No se puede
  avisar al administrador o al proveedor, registrar la alerta en un archivo ni agregar un destinatario
  nuevo sin editar `registrarVenta()`. Es el caso exacto que el enunciado anticipa como "nuevo destinatario
  para la alerta de stock".

### AP-07 - Datos publicos y sin encapsulacion
- **Donde:** L59-63 (atributos `public` de la tienda), L25-49 (structs con campos abiertos), L213
  (`prod->stock -= cant` modificado directamente desde el flujo de venta).
- **Problema que causa:** cualquier parte del programa puede dejar el stock negativo o cambiar el total sin
  pasar por una regla. No hay un punto unico que proteja los invariantes del inventario.

---

## 3. Otras observaciones (no cuentan como antipatron, se registran por transparencia)

- **F1 sin validacion numerica:** precio y stock negativos se aceptan (L93-97), y `cin.ignore()` sin
  argumento (L90) es fragil con entradas inesperadas.
- **Entrada invalida de cantidad o forma de pago:** si `cin` falla en L173 o L241, el programa queda en
  estado de error sin recuperarse.
- **Stock descontado antes de elegir la forma de pago** (L213 frente a L241): el stock ya cambio aunque el
  proceso se interrumpa despues.

Estas observaciones no se corrigen en la v0 (esta fijada con `entrega-s1`). Se atienden en la v1.

---

## 4. Relacion entre antipatrones y patrones propuestos

| Antipatron | Patron que lo corrige | Tipo |
|---|---|---|
| AP-02, AP-04, AP-05 (impuestos por tipo, L185-201) | **Factory Method** + **Decorator** | Creacional + Estructural |
| AP-03, AP-04, AP-05 (formas de pago, L251-294, L323-326) | **Strategy** | Comportamiento |
| AP-01 | Efecto conjunto de los tres patrones al repartir responsabilidades | - |
| AP-06 | Fuera de la combinacion principal; queda como mejora (Observer) | - |
| AP-07 | Se resuelve con clases con atributos privados en la v1 | - |

El razonamiento completo esta en `03_justificacion_patrones.md`.
