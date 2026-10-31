# Video Shop — Gestión de una Tienda con Patrones de Software

Proyecto final para el curso de **Patrones de Software** (Código 74362) — Periodo 2026-2.  
**Institución:** Corporación de Estudios Tecnológicos del Norte del Valle — Cotecnova  
**Docente:** Felipe Gutiérrez Isaza  

---

## 1. Integrantes y Roles

| Nombre | Rol | Responsabilidad Principal |
| :--- | :--- | :--- |
| **Luis Cardona** | **Responsable de código** | Coordina la construcción de las versiones 0 y 1, el control de versiones y su integración técnica. |
| *(Por definir)* | **Responsable de diseño** | Coordina la selección de patrones, diagnóstico de antipatrones y coherencia arquitectural. |
| **Sebastian Orozco** | **Responsable de modelado** | Elabora los diagramas UML (clases, secuencia, comunicación) y mantiene la documentación. |

---

## 2. Dominio del Proyecto: Tienda de Videojuegos (Video Shop)

La aplicación modela la operación de una tienda especializada en videojuegos, consolas y accesorios de entretenimiento digital:
- **Juegos Físicos:** Medios físicos en disco/cartucho sujetos a impuestos convencionales (IVA 19%).
- **Juegos Digitales / DLCs:** Licencias y códigos digitales sujetos a tarifas de servicios digitales (5%).
- **Consolas / Hardware:** Equipos sujetos a IVA y garantía extendida obligatoria.
- **Accesorios / Coleccionables:** Controles, auriculares y periféricos.

---

## 3. Estructura del Repositorio

Siguiendo estrictamente los lineamientos del proyecto:

```text
Video-Shop/
├── README.md                     ← Integrantes, roles, tipo de tienda, cómo compilar
├── .gitignore                    ← Filtro para binarios y temporales
├── docs/
│   ├── 01_ficha_cliente.md           (Seguimiento 1)
│   ├── 02_antipatrones.md            (Seguimiento 2)
│   ├── 03_justificacion_patrones.md  (Seguimiento 2)
│   └── uml/
│       ├── clases_v0.png             (Seguimiento 1)
│       ├── clases_v1.png             (Seguimiento 2)
│       ├── secuencia_venta.png       (Seguimiento 3)
│       └── comunicacion_venta.png    (Seguimiento 3)
├── v0/
│   └── tienda_v0.cpp             ← Versión 0 sin patrones (inmutable tras Seguimiento 1)
└── v1/
    ├── main.cpp                  ← Versión 1 con patrones de diseño
    ├── (archivos .h y .cpp)
    └── salida_esperada.txt       ← Registro de ejecución del flujo de venta
```

---

## 4. Instrucciones de Compilación y Ejecución

La aplicación está desarrollada en C++ estándar y no requiere librerías externas ni bases de datos (datos en memoria).

### Compilación de la Versión 0 (v0)

Utilizando el compilador GNU C++ (`g++`):
```bash
g++ -std=c++17 v0/tienda_v0.cpp -o v0/tienda_v0.exe
```

Ejecución en consola (Windows):
```bash
.\v0\tienda_v0.exe
```

O en Linux / macOS:
```bash
g++ -std=c++17 v0/tienda_v0.cpp -o v0/tienda_v0
./v0/tienda_v0
```

---

## 5. Funcionalidades Mínimas Implementadas (F1 a F6)

1. **F1 — Registrar productos:** Registro con código, nombre, precio unitario, stock disponible y tipo de producto.
2. **F2 — Consultar el inventario:** Listado completo y formateado del inventario actual.
3. **F3 — Registrar una venta:** Carrito de compras que admite múltiples productos y diferentes cantidades en una sola transacción.
4. **F4 — Aplicar descuentos o impuestos:** Cálculo de IVA según tipo de producto y descuentos/recargos según método de pago (Efectivo, Transferencia, Tarjeta).
5. **F5 — Controlar el stock:** Descuento automático de existencias con alertas automáticas cuando el stock es inferior al mínimo permitido.
6. **F6 — Reporte de ventas:** Historial detallado de recibos emitidos y acumulador financiero total de la caja.

---

## 6. Etiquetas de Entrega en Git

- **Seguimiento 1:** `entrega-s1`
- **Seguimiento 2:** `entrega-s2`
- **Seguimiento 3:** `entrega-s3`
- **Entrega Final:** `entrega-final`

