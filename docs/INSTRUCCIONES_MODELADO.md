# Guia de Modelado y Documentacion para Seguimiento 1

Estimado **Responsable de Modelado**:

La **Version 0** del proyecto se encuentra implementada y disponible en [v0/tienda_v0.cpp](file:///c:/Users/Luis/Documents/COTECNOVA/SEPTIMO%20SEMESTRE/3.Software%20Paterns/Video%20Shop/v0/tienda_v0.cpp).

Para completar el **Seguimiento 1** segun las especificaciones del PDF (Paginas 6, 7 y 8), se deben incorporar en este repositorio los siguientes dos entregables:

---

## 1. Ficha del Cliente e Historias de Usuario (`docs/01_ficha_cliente.md`)
Debe describir el contexto del cliente (Tienda de Videojuegos "Video Shop"), sus usuarios principales (Vendedor/Cajero, Administrador de inventario) y redactar entre **5 y 8 historias de usuario** en formato estandar:
> *Como [rol] quiero [accion] para [beneficio]*

Cubriendo directamente las 6 funciones implementadas:
- **F1:** Registrar productos con codigo, nombre, precio, stock y categoria.
- **F2:** Consultar inventario completo.
- **F3:** Registrar una venta con multiples items y cantidades.
- **F4:** Aplicar impuestos (IVA 19%, digitales 5%) y promociones por medio de pago (Efectivo, Transferencia, Tarjeta).
- **F5:** Controlar stock con aviso automatico si baja de 5 unidades.
- **F6:** Consultar reporte de ventas y dinero acumulado en caja.

---

## 2. Diagrama de Clases de la Version 0 (`docs/uml/clases_v0.png`)
Debe reflejar fielmente las estructuras y clases del codigo de [v0/tienda_v0.cpp](file:///c:/Users/Luis/Documents/COTECNOVA/SEPTIMO%20SEMESTRE/3.Software%20Paterns/Video%20Shop/v0/tienda_v0.cpp):
- `struct Producto`: `codigo: string`, `nombre: string`, `precio: double`, `stock: int`, `tipo: int`.
- `struct ItemVenta`: `codigo: string`, `nombre: string`, `cantidad: int`, `precioUnitarioBase: double`, `precioFinalItem: double`.
- `struct Venta`: `idVenta: int`, `items: vector<ItemVenta>`, `formaPago: int`, `subtotalBase: double`, `totalConImpuestosYDescuentos: double`.
- `class TiendaVideojuegos`:
  - Atributos: `productos: vector<Producto>`, `historialVentas: vector<Venta>`, `totalVentas: double`, `contadorVentas: int`.
  - Metodos: `registrarProducto()`, `consultarInventario()`, `buscarProducto(codigo: string): Producto*`, `registrarVenta()`, `reporteDeVentas()`.

