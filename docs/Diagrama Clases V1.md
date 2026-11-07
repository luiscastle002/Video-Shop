[gemini-code-1794084967753.md](https://github.com/user-attachments/files/34528622/gemini-code-1794084967753.md)
# Diagrama de Clases - Tienda de Vídeos (Versión 1)

```mermaid
classDiagram
    direction TB

    %% Definición de estilos por color
    classDef creacional fill:#d4edda,stroke:#28a745,stroke-width:2px,color:#155724;
    classDef estructural fill:#cce5ff,stroke:#004085,stroke-width:2px,color:#004085;
    classDef comportamiento fill:#fff3cd,stroke:#856404,stroke-width:2px,color:#856404;
    classDef dominio fill:#e2d9f3,stroke:#6f42c1,stroke-width:2px,color:#3d1a6f;

    %% Patrón Estructural: Decorator
    class CalculoPrecio {
        <<interface>>
        +calcular(int cantidad) double
    }
    class PrecioBase {
        -double precioUnitario_
        +PrecioBase(double precioUnitario)
        +calcular(int cantidad) double
    }
    class DecoradorPrecio {
        +DecoradorPrecio(void* envuelto)
        +calcular(int cantidad) double
    }
    class ConIVA {
        +double TASA_IVA
        +ConIVA(void* envuelto)
        +calcular(int cantidad) double
    }
    class ConCargoPlataforma {
        +double TASA_PLATAFORMA
        +ConCargoPlataforma(void* envuelto)
        +calcular(int cantidad) double
    }
    class ConGarantia {
        +double COSTO_GARANTIA_UNITARIA
        +ConGarantia(void* envuelto)
        +calcular(int cantidad) double
    }

    CalculoPrecio <|.. PrecioBase
    CalculoPrecio <|.. DecoradorPrecio
    DecoradorPrecio <|-- ConIVA
    DecoradorPrecio <|-- ConCargoPlataforma
    DecoradorPrecio <|-- ConGarantia
    DecoradorPrecio o-- CalculoPrecio : envuelve

    %% Entidad de Dominio: Producto
    class Producto {
        -string codigo_
        -string nombre_
        -double precioBase_
        -int stock_
        -int idTipo_
        -string categoria_
        +int UMBRAL_STOCK_CRITICO
        +codigo() string
        +nombre() string
        +precioBase() double
        +stock() int
        +idTipo() int
        +categoria() string
        +calcularPrecioLinea(int cantidad) double
        +descontarStock(int cantidad) bool
        +tieneStockCritico() bool
    }

    Producto *-- CalculoPrecio : usa calculo

    %% Patrón Creacional: Factory Method
    class FabricaProducto {
        <<interface>>
        +crearProducto(string cod, string nom, double p, int s) Producto
        +idTipo() int
        +nombreCategoria() string
    }
    class FabricaFisico {
        +crearProducto(string cod, string nom, double p, int s) Producto
        +idTipo() int
        +nombreCategoria() string
    }
    class FabricaDigital {
        +crearProducto(string cod, string nom, double p, int s) Producto
        +idTipo() int
        +nombreCategoria() string
    }
    class FabricaConsola {
        +crearProducto(string cod, string nom, double p, int s) Producto
        +idTipo() int
        +nombreCategoria() string
    }
    class FabricaAccesorio {
        +crearProducto(string cod, string nom, double p, int s) Producto
        +idTipo() int
        +nombreCategoria() string
    }

    FabricaProducto <|.. FabricaFisico
    FabricaProducto <|.. FabricaDigital
    FabricaProducto <|.. FabricaConsola
    FabricaProducto <|.. FabricaAccesorio
    FabricaProducto ..> Producto : crea

    %% Patrón de Comportamiento: Strategy
    class MedioPago {
        <<interface>>
        +ajustar(double subtotal) double
        +nombre() string
        +descripcionMenu() string
        +etiquetaComprobante() string
        +tieneAjuste() bool
    }
    class PagoEfectivo {
        +ajustar(double subtotal) double
        +nombre() string
        +descripcionMenu() string
        +etiquetaComprobante() string
        +tieneAjuste() bool
    }
    class PagoTransferencia {
        +double TASA_DESCUENTO
        +ajustar(double subtotal) double
        +nombre() string
        +descripcionMenu() string
        +etiquetaComprobante() string
        +tieneAjuste() bool
    }
    class PagoTarjeta {
        +double TASA_DESCUENTO
        +double RECARGO_PASARELA
        +ajustar(double subtotal) double
        +nombre() string
        +descripcionMenu() string
        +etiquetaComprobante() string
        +tieneAjuste() bool
    }

    MedioPago <|.. PagoEfectivo
    MedioPago <|.. PagoTransferencia
    MedioPago <|.. PagoTarjeta

    %% Clases de Soporte y Dominio Adicionales
    class ItemVenta {
        +string codigo
        +string nombre
        +int cantidad
        +double precioUnitarioBase
        +double precioFinalItem
    }
    class Venta {
        -int idVenta_
        -double subtotalConImpuestos_
        -double totalFinal_
        -string nombreMedioPago_
        +Venta(int idVenta)
        +agregarItem(Producto prod, int cantidad)
        +cantidadEnCarrito(string codigo) int
        +estaVacia() bool
        +liquidarConMedioPago(MedioPago medioPago)
        +idVenta() int
        +subtotalConImpuestos() double
        +totalFinal() double
        +totalArticulos() int
    }
    class Inventario {
        -double totalRecaudado_
        -int consecutivoVentas_
        +Inventario()
        +existeCodigo(string codigo) bool
        +agregarProducto(Producto prod) bool
        +buscarProducto(string codigo) Producto
        +siguienteIdVenta() int
        +registrarVentaConfirmada(Venta venta)
        +cargarSemillaInicial()
        +cargarArchivo() bool
        +guardarArchivo() bool
    }
    class Comprobante {
        +mostrarInventario(Inventario inv)
        +mostrarAlertaStock(Producto prod)
        +mostrarFactura(Venta venta)
        +mostrarReporteVentas(Inventario inv)
    }

    Venta "1" *-- "*" ItemVenta : contiene
    Inventario "1" --> "*" Producto : almacena
    Inventario "1" --> "*" Venta : registra
    Venta ..> MedioPago : se liquida usando

    %% Asignación de colores por patrón (compatible con GitHub)
    class CalculoPrecio estructural
    class PrecioBase estructural
    class DecoradorPrecio estructural
    class ConIVA estructural
    class ConCargoPlataforma estructural
    class ConGarantia estructural

    class FabricaProducto creacional
    class FabricaFisico creacional
    class FabricaDigital creacional
    class FabricaConsola creacional
    class FabricaAccesorio creacional

    class MedioPago comportamiento
    class PagoEfectivo comportamiento
    class PagoTransferencia comportamiento
    class PagoTarjeta comportamiento

    class Producto dominio
    class ItemVenta dominio
    class Venta dominio
    class Inventario dominio
    class Comprobante dominio
