
# Diagrama de Clases - Tienda de Vídeos (Versión 1)

```mermaid
classDiagram
    direction TB

    %% Definición de estilos globales por color
    classDef creacional fill:#d4edda,stroke:#28a745,stroke-width:2px,color:#155724;
    classDef estructural fill:#cce5ff,stroke:#004085,stroke-width:2px,color:#004085;
    classDef comportamiento fill:#fff3cd,stroke:#856404,stroke-width:2px,color:#856404;
    classDef dominio fill:#e2d9f3,stroke:#6f42c1,stroke-width:2px,color:#3d1a6f;

    %% Patrón Estructural: Decorator
    class CalculoPrecio:::estructural {
        <<interface>>
        +calcular(int cantidad) double
    }
    class PrecioBase:::estructural {
        -double precioUnitario_
        +PrecioBase(double precioUnitario)
        +calcular(int cantidad) double
    }
    class DecoradorPrecio:::estructural {
        +DecoradorPrecio(void* envuelto)
        +calcular(int cantidad) double
    }
    class ConIVA:::estructural {
        +double TASA_IVA
        +ConIVA(void* envuelto)
        +calcular(int cantidad) double
    }
    class ConCargoPlataforma:::estructural {
        +double TASA_PLATAFORMA
        +ConCargoPlataforma(void* envuelto)
        +calcular(int cantidad) double
    }
    class ConGarantia:::estructural {
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
    class Producto:::dominio {
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
    class FabricaProducto:::creacional {
        <<interface>>
        +crearProducto(string cod, string nom, double p, int s) Producto
        +idTipo() int
        +nombreCategoria() string
    }
    class FabricaFisico:::creacional {
        +crearProducto(string cod, string nom, double p, int s) Producto
        +idTipo() int
        +nombreCategoria() string
    }
    class FabricaDigital:::creacional {
        +crearProducto(string cod, string nom, double p, int s) Producto
        +idTipo() int
        +nombreCategoria() string
    }
    class FabricaConsola:::creacional {
        +crearProducto(string cod, string nom, double p, int s) Producto
        +idTipo() int
        +nombreCategoria() string
    }
    class FabricaAccesorio:::creacional {
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
    class MedioPago:::comportamiento {
        <<interface>>
        +ajustar(double subtotal) double
        +nombre() string
        +descripcionMenu() string
        +etiquetaComprobante() string
        +tieneAjuste() bool
    }
    class PagoEfectivo:::comportamiento {
        +ajustar(double subtotal) double
        +nombre() string
        +descripcionMenu() string
        +etiquetaComprobante() string
        +tieneAjuste() bool
    }
    class PagoTransferencia:::comportamiento {
        +double TASA_DESCUENTO
        +ajustar(double subtotal) double
        +nombre() string
        +descripcionMenu() string
        +etiquetaComprobante() string
        +tieneAjuste() bool
    }
    class PagoTarjeta:::comportamiento {
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
    class ItemVenta:::dominio {
        +string codigo
        +string nombre
        +int cantidad
        +double precioUnitarioBase
        +double precioFinalItem
    }
    class Venta:::dominio {
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
    class Inventario:::dominio {
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
    class Comprobante:::dominio {
        +mostrarInventario(Inventario inv)
        +mostrarAlertaStock(Producto prod)
        +mostrarFactura(Venta venta)
        +mostrarReporteVentas(Inventario inv)
    }

    Venta "1" *-- "*" ItemVenta : contiene
    Inventario "1" --> "*" Producto : almacena
    Inventario "1" --> "*" Venta : registra
    Venta ..> MedioPago : se liquida usando
