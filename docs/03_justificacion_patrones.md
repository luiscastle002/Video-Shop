# Justificacion de los Patrones - Version 1

**Proyecto:** Video Shop - Tienda de Videojuegos
**Curso:** Patrones de Software (Codigo 74362) - Cotecnova 2026-2
**Elaborado por:** Responsable de diseno
**Entrega:** Seguimiento 2
**Documento relacionado:** `02_antipatrones.md` (los IDs AP-xx y las lineas citadas vienen de alli)

---

## 1. Combinacion elegida

| Grupo | Patron | Problema de la v0 que corrige |
|---|---|---|
| Creacional | **Factory Method** | AP-02: la decision "que tipo de producto es y como se cobra" esta repartida en `switch` (L99-105, L126-132, L185-201). |
| Estructural | **Decorator** | AP-02, AP-04, AP-05: impuestos y cargos por tipo con numeros magicos y formulas duplicadas (L185-201). |
| Comportamiento | **Strategy** | AP-03, AP-04, AP-05: forma de pago resuelta con `if/else` repetidos en tres lugares (L251-255, L288-294, L323-326). |

> **Registro de combinacion (regla 3.1 del enunciado).** La combinacion
> **Factory Method + Decorator + Strategy** debe registrarse ante el docente antes del cierre del
> Seguimiento 2 (domingo 8 de noviembre, 11:59 p. m.), porque ningun equipo puede repetirla. Si otro
> equipo ya la tomo, las alternativas estan en la seccion 6.

---

## 2. Por que estos tres y no otros

El enunciado exige que cada patron corrija un problema concreto de la v0. Los tres elegidos atacan los
hallazgos de mayor gravedad y los que mas se repiten en el flujo de venta:

- **El calculo del precio por tipo** (L185-201) es el bloque con mas olores juntos: `switch`, numeros
  magicos y codigo duplicado.
- **La forma de pago** se decide en tres sitios distintos y es la regla que mas probablemente cambie
  (nuevas promociones, nuevas pasarelas).
- Ambos viven dentro de `registrarVenta()`, el flujo principal que el enunciado pide que los patrones
  atraviesen.

---

## 3. Diseno propuesto

### 3.1 Factory Method (creacional)
```
FabricaProducto            (creador abstracto)
  + crearProducto(codigo, nombre, precioBase, stock) : Producto   <- metodo fabrica
    |-- FabricaFisico       -> arma el calculo de precio con IVA 19%
    |-- FabricaDigital      -> arma el calculo de precio con cargo 5%
    |-- FabricaConsola      -> arma el calculo con IVA 19% + garantia
    |-- FabricaAccesorio    -> arma el calculo con IVA 19%
```
Cada fabrica entrega un `Producto` que ya trae su cadena de calculo de precio correcta. El resto del
programa deja de preguntar "de que tipo es".

### 3.2 Decorator (estructural)
```
CalculoPrecio              (componente: calcular(cantidad) : double)
  |-- PrecioBase           (componente concreto: precioUnitario * cantidad)
  |-- DecoradorPrecio      (decorador abstracto, envuelve a otro CalculoPrecio)
        |-- ConIVA              (+19%, constante nombrada)
        |-- ConCargoPlataforma  (+5%)
        |-- ConGarantia         (+5.000 COP por unidad)
```
Cada regla es una clase con una sola responsabilidad y se combina envolviendo. Una consola se arma como
`ConGarantia(ConIVA(PrecioBase))`. La clase `Producto` no se modifica para agregar un impuesto.

### 3.3 Strategy (comportamiento)
```
MedioPago                  (interfaz: ajustar(subtotal) : double, descripcion() : string)
  |-- PagoEfectivo         (sin ajuste)
  |-- PagoTransferencia    (descuento 3%)
  |-- PagoTarjeta          (descuento 3% + recargo 800 COP)
```
La venta recibe un `MedioPago` y le delega el ajuste y el texto del comprobante y del reporte. Los tres
`if` de AP-03 desaparecen porque cada decision queda dentro de su clase.

### 3.4 Papel de cada clase restante
| Clase | Responsabilidad | Atiende |
|---|---|---|
| `Inventario` | Guardar productos y controlar el stock (atributos privados). | AP-01, AP-07 |
| `Venta` | Acumular items, sumar el resultado de los decoradores y aplicar el `MedioPago`. | AP-01 |
| `Comprobante` / presentacion | Dar formato a la salida, separado de las reglas. | AP-01 |
| `main` | Menu y entrada de datos. | AP-01 |

---

## 4. Por que los tres se integran en el flujo de venta

Flujo de `registrarVenta()` en la v1:

```
1. El cajero elige un producto del Inventario.
   El Producto fue creado por una FabricaX (Factory Method) y ya trae su cadena de precio.
2. Venta pide al producto el precio de la linea.
   La cadena de decoradores calcula el valor con impuestos (Decorator).
3. Venta suma las lineas y entrega el subtotal al MedioPago elegido (Strategy).
   El MedioPago devuelve el total final y su texto para el comprobante.
```

Se cumplen las dos condiciones del enunciado:

**a) Comparten abstracciones o se pasan objetos entre si.**
- El objeto que produce **Factory Method** (un `Producto` con su `CalculoPrecio`) es el objeto que usa
  **Decorator** al calcular cada linea.
- El valor que produce **Decorator** (el subtotal) es la entrada de **Strategy**.
- Los tres trabajan sobre la misma cadena `Producto -> linea -> subtotal -> total`.

**b) Al retirar cualquiera, vuelven los condicionales o el codigo duplicado.**

| Si se retira... | Vuelve a aparecer... | Lineas de la v0 |
|---|---|---|
| Factory Method | Un `switch (tipo)` para decidir que cadena de precio armar. | L99-105, L185-201 |
| Decorator | Las formulas con `1.19`, `1.05` y `5000.0` repetidas por tipo. | L187-196 |
| Strategy | Los `if (formaPago == ...)` en calculo, comprobante y reporte. | L251-255, L288-294, L323-326 |

---

## 5. Prueba del cambio en vivo (sustentacion)

El enunciado espera que un cambio pequeno se resuelva **agregando una clase nueva, sin modificar las
existentes**. Con este diseno:

| Cambio que podrian pedir | Que se agrega |
|---|---|
| Nuevo tipo de producto (por ejemplo, suscripcion) | Una `FabricaSuscripcion` y, si lleva un cargo propio, un decorador nuevo. |
| Nueva forma de pago (por ejemplo, billetera digital) | Una clase `PagoBilletera : MedioPago`. |
| Nueva promocion o impuesto (por ejemplo, 2x1 o impuesto saludable) | Un decorador nuevo. |

Nota para el codigo: el menu necesita un punto de registro (por ejemplo un mapa de tipo a fabrica y de
opcion a medio de pago) para no volver a poner un `switch`. Debe quedar documentado en el README.

---

## 6. Alternativas y lo que queda fuera

**Antipatron que esta combinacion no corrige.** AP-06 (alerta de stock acoplada a `cout`, L215-219) queda
para el patron Observer, que no entra en esta combinacion. Se documenta como mejora posible. Si sobra
tiempo despues de cumplir F1 a F6 con los tres patrones, puede implementarse como extra, sin sustituir
ninguno de los tres.

**Combinaciones alternativas** por si la principal ya fue tomada por otro equipo:

| Opcion | Combinacion | Lo que corrige | Lo que deja sin corregir |
|---|---|---|---|
| B | Factory Method + Decorator + **Observer** | AP-02, AP-04, AP-05 (precio), AP-06 | AP-03 (forma de pago) |
| C | **Builder** + Decorator + Strategy | AP-02 (precio), AP-03, AP-05 y el armado paso a paso de `registrarVenta()` | AP-06 |
| D | Factory Method + **Facade** + Strategy | AP-01 (via Caja), AP-03 | AP-02 parcial (el `switch` de precios sigue) |

La opcion B es la mas facil de cambiar, porque conserva dos de los tres patrones y las clases ya
disenadas. Quien registre la combinacion debe confirmar con el docente antes de empezar a codificar.

---

## 7. Pendientes para el Seguimiento 2

- [ ] Registrar la combinacion ante el docente (antes del 8 nov, 11:59 p. m.).
- [ ] `docs/uml/clases_v1.png`: diagrama de clases de la v1 con cada patron en un color distinto
      (propuesta: un color por patron; Factory Method, Decorator, Strategy y las clases sin patron).
- [ ] Verificar con el Responsable de modelado que el diagrama muestre las relaciones de la seccion 3.
- [ ] Etiqueta de Git: `git tag entrega-s2` y `git push origin entrega-s2`.
