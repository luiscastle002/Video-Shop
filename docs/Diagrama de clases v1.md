# Diagrama de Clases - Video Shop (Versión 1)
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
        +~CalculoPrecio()
        +calcular(int cantidad) double*
    }
    class PrecioBase {
        -double precioUnitario_
        +PrecioBase(double precioUnitario)
        +calcular(int cantidad) double
    }
    class DecoradorPrecio {
        #unique_ptr~CalculoPrecio~ envuelto_
        +DecoradorPrecio(unique_ptr~CalculoPrecio~ envuelto)
        +calcular(int cantidad) double
    }
    class ConIVA {
        +static double TASA_IVA
        +ConIVA(unique_ptr~CalculoPrecio~ envuelto)
        +calcular(int cantidad) double
    }
    class ConCargoPlataforma {
        +static double TASA_PLATAFORMA
        +ConCargoPlataforma(unique_ptr~CalculoPrecio~ envuelto)
        +calcular(int cantidad) double
    }
    class ConGarantia {
        +static double COSTO_GARANTIA_UNITARIA
        +ConGarantia(unique_ptr~CalculoPrecio~ envuelto)
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
        -unique_ptr~CalculoPrecio~ calculoPrecio_
        +static int UMBRAL_STOCK_CRITICO
        +Producto(...)
        +codigo() const string&
        +nombre() const string&
        +precioBase() double
        +stock() int
        +idTipo() int
        +categoria() const string&
        +calcularPrecioLinea(int cantidad) double
        +descontarStock(int cantidad) bool
        +tieneStockCritico() bool
    }

    Producto *-- CalculoPrecio : usa estrategia de cálculo

    %% Patrón Creacional: Factory Method
    class FabricaProducto {
        <<interface>>
        +~FabricaProducto()
        +crearProducto(...) Producto*
        +idTipo() int*
        +nombreCategoria() string*
    }
    class FabricaFisico {
        +crearProducto(...) Producto
        +idTipo() int
        +nombreCategoria() string
    }
    class FabricaDigital {
        +crearProducto(...) Producto
        +idTipo() int
        +nombreCategoria() string
    }
    class FabricaConsola {
        +crearProducto(...) Producto
        +idTipo() int
        +nombreCategoria() string
    }
    class FabricaAccesorio {
        +crearProducto(...) Producto
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
        +~MedioPago()
        +ajustar(double subtotalConImpuestos) double*
        +nombre() string*
        +descripcionMenu() string*
        +etiquetaComprobante() string*
        +tieneAjuste() bool*
    }
    class PagoEfectivo {
        +ajustar(double subtotalConImpuestos) double
        +nombre() string
        +descripcionMenu() string
        +etiquetaComprobante() string
        +tieneAjuste() bool
    }
    class PagoTransferencia {
        +static double TASA_DESCUENTO
        +ajustar(double subtotalConImpuestos) double
        +nombre() string
        +descripcionMenu() string
        +etiquetaComprobante() string
        +tieneAjuste() bool
    }
    class PagoTarjeta {
        +static double TASA_DESCUENTO
        +static double RECARGO_PASARELA
        +ajustar(double subtotalConImpuestos) double
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
        -vector~ItemVenta~ items_
        -double subtotalConImpuestos_
        -double totalFinal_
        -string nombreMedioPago_
        -string etiquetaAjuste_
        -bool tieneAjustePago_
        +Venta(int idVenta)
        +agregarItem(const Producto& prod, int cantidad)
        +cantidadEnCarrito(const string& codigo) int
        +estaVacia() bool
        +liquidarConMedioPago(const MedioPago& medioPago)
        +idVenta() int
        +items() const vector~ItemVenta~&
        +subtotalConImpuestos() double
        +totalFinal() double
        +totalArticulos() int
    }
    class Inventario {
        -vector~Producto~ productos_
        -vector~Venta~ historialVentas_
        -double totalRecaudado_
        -int consecutivoVentas_
        +Inventario()
        +existeCodigo(const string& codigo) bool
        +agregarProducto(Producto&& prod) bool
        +buscarProducto(const string& codigo) Producto*
        +siguienteIdVenta() int
        +registrarVentaConfirmada(const Venta& venta)
        +cargarSemillaInicial(...)
        +cargarDesdeArchivo(...) bool
        +guardarEnArchivo(...) bool
    }
    class Comprobante {
        +static mostrarInventario(const Inventario& inv)
        +static mostrarAlertaStock(const Producto& prod)
        +static mostrarFactura(const Venta& venta)
        +static mostrarReporteVentas(const Inventario& inv)
    }

    Venta "1" *-- "*" ItemVenta : contiene
    Inventario "1" --> "*" Producto : almacena
    Inventario "1" --> "*" Venta : registra historial
    Venta ..> MedioPago : se liquida usando

    %% Asignación individual de estilos (Compatible con GitHub)
    class CalculoPrecio estructural;
    class PrecioBase estructural;
    class DecoradorPrecio estructural;
    class ConIVA estructural;
    class ConCargoPlataforma estructural;
    class ConGarantia estructural;

    class FabricaProducto creacional;
    class FabricaFisico creacional;
    class FabricaDigital creacional;
    class FabricaConsola creacional;
    class FabricaAccesorio creacional;

    class MedioPago comportamiento;
    class PagoEfectivo comportamiento;
    class PagoTransferencia comportamiento;
    class PagoTarjeta comportamiento;

    class Producto dominio;
    class ItemVenta dominio;
    class Venta dominio;
    class Inventario dominio;
    class Comprobante dominio;
