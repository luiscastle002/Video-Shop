/**
 * ============================================================================
 * PROYECTO: Video Shop - Tienda de Videojuegos (Version 1 Autocontenida Online)
 * CURSO: Patrones de Software (Codigo 74362) - Cotecnova 2026-2
 * ARCHIVO: v1/tienda_v1_online.cpp
 * DESCRIPCION:
 *   Version unificada (single-file amalgamation) de la Version 1 modular,
 *   disenada especificamente para compilar y ejecutarse directamente en
 *   compiladores en linea (como OnlineGDB) sin requerir archivos .h externos.
 *
 * PATRONES GOF IMPLEMENTADOS E INTEGRADOS:
 *   - Creacional: Factory Method (FabricaProducto y 4 fabricas concretas)
 *   - Estructural: Decorator (CalculoPrecio, PrecioBase, DecoradorPrecio y 3 decoradores)
 *   - Comportamiento: Strategy (MedioPago y 3 estrategias concretas)
 *
 * COMANDO DE COMPILACION:
 *   g++ -std=c++17 -Wall -Wextra -pedantic v1/tienda_v1_online.cpp -o tienda_v1_online
 * ============================================================================
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <limits>
#include <cmath>
#include <fstream>
#include <sstream>
#include <filesystem>

using namespace std;

// ============================================================================
// 1. PATRON ESTRUCTURAL: DECORATOR (Original: v1/CalculoPrecio.h)
// ============================================================================

// ESTRUCTURAL: Decorator
class CalculoPrecio {
public:
    virtual ~CalculoPrecio() = default;
    virtual double calcular(int cantidad) const = 0;
};

class PrecioBase : public CalculoPrecio {
private:
    double precioUnitario_;

public:
    explicit PrecioBase(double precioUnitario)
        : precioUnitario_(precioUnitario) {}

    double calcular(int cantidad) const override {
        return precioUnitario_ * static_cast<double>(cantidad);
    }
};

class DecoradorPrecio : public CalculoPrecio {
protected:
    std::unique_ptr<CalculoPrecio> envuelto_;

public:
    explicit DecoradorPrecio(std::unique_ptr<CalculoPrecio> envuelto)
        : envuelto_(std::move(envuelto)) {}

    double calcular(int cantidad) const override {
        return envuelto_->calcular(cantidad);
    }
};

class ConIVA : public DecoradorPrecio {
public:
    static constexpr double TASA_IVA = 0.19;

    explicit ConIVA(std::unique_ptr<CalculoPrecio> envuelto)
        : DecoradorPrecio(std::move(envuelto)) {}

    double calcular(int cantidad) const override {
        return envuelto_->calcular(cantidad) * (1.0 + TASA_IVA);
    }
};

class ConCargoPlataforma : public DecoradorPrecio {
public:
    static constexpr double TASA_PLATAFORMA = 0.05;

    explicit ConCargoPlataforma(std::unique_ptr<CalculoPrecio> envuelto)
        : DecoradorPrecio(std::move(envuelto)) {}

    double calcular(int cantidad) const override {
        return envuelto_->calcular(cantidad) * (1.0 + TASA_PLATAFORMA);
    }
};

class ConGarantia : public DecoradorPrecio {
public:
    static constexpr double COSTO_GARANTIA_UNITARIA = 5000.0;

    explicit ConGarantia(std::unique_ptr<CalculoPrecio> envuelto)
        : DecoradorPrecio(std::move(envuelto)) {}

    double calcular(int cantidad) const override {
        return envuelto_->calcular(cantidad) + (COSTO_GARANTIA_UNITARIA * static_cast<double>(cantidad));
    }
};

// ============================================================================
// 2. ENTIDAD DE DOMINIO: PRODUCTO (Original: v1/Producto.h)
// ============================================================================

class Producto {
private:
    std::string codigo_;
    std::string nombre_;
    double precioBase_;
    int stock_;
    int idTipo_;
    std::string categoria_;
    std::unique_ptr<CalculoPrecio> calculoPrecio_;

public:
    static constexpr int UMBRAL_STOCK_CRITICO = 5;

    Producto(const std::string& codigo,
             const std::string& nombre,
             double precioBase,
             int stock,
             int idTipo,
             const std::string& categoria,
             std::unique_ptr<CalculoPrecio> calculoPrecio)
        : codigo_(codigo),
          nombre_(nombre),
          precioBase_(precioBase),
          stock_(stock),
          idTipo_(idTipo),
          categoria_(categoria),
          calculoPrecio_(std::move(calculoPrecio)) {}

    Producto(Producto&&) noexcept = default;
    Producto& operator=(Producto&&) noexcept = default;
    Producto(const Producto&) = delete;
    Producto& operator=(const Producto&) = delete;

    const std::string& codigo() const { return codigo_; }
    const std::string& nombre() const { return nombre_; }
    double precioBase() const { return precioBase_; }
    int stock() const { return stock_; }
    int idTipo() const { return idTipo_; }
    const std::string& categoria() const { return categoria_; }

    double calcularPrecioLinea(int cantidad) const {
        return calculoPrecio_->calcular(cantidad);
    }

    bool descontarStock(int cantidad) {
        if (cantidad <= 0 || cantidad > stock_) {
            return false;
        }
        stock_ -= cantidad;
        return true;
    }

    bool tieneStockCritico() const {
        return stock_ < UMBRAL_STOCK_CRITICO;
    }
};

// ============================================================================
// 3. PATRON CREACIONAL: FACTORY METHOD (Original: v1/FabricaProducto.h)
// ============================================================================

// CREACIONAL: Factory Method
class FabricaProducto {
public:
    virtual ~FabricaProducto() = default;
    virtual Producto crearProducto(const std::string& codigo,
                                   const std::string& nombre,
                                   double precioBase,
                                   int stock) const = 0;
    virtual int idTipo() const = 0;
    virtual std::string nombreCategoria() const = 0;
};

class FabricaFisico : public FabricaProducto {
public:
    Producto crearProducto(const std::string& codigo,
                           const std::string& nombre,
                           double precioBase,
                           int stock) const override {
        std::unique_ptr<CalculoPrecio> calculo = std::make_unique<PrecioBase>(precioBase);
        calculo = std::make_unique<ConIVA>(std::move(calculo));
        return Producto(codigo, nombre, precioBase, stock, idTipo(), nombreCategoria(), std::move(calculo));
    }

    int idTipo() const override { return 1; }
    std::string nombreCategoria() const override { return "Juego Fisico"; }
};

class FabricaDigital : public FabricaProducto {
public:
    Producto crearProducto(const std::string& codigo,
                           const std::string& nombre,
                           double precioBase,
                           int stock) const override {
        std::unique_ptr<CalculoPrecio> calculo = std::make_unique<PrecioBase>(precioBase);
        calculo = std::make_unique<ConCargoPlataforma>(std::move(calculo));
        return Producto(codigo, nombre, precioBase, stock, idTipo(), nombreCategoria(), std::move(calculo));
    }

    int idTipo() const override { return 2; }
    std::string nombreCategoria() const override { return "Juego Digital"; }
};

class FabricaConsola : public FabricaProducto {
public:
    Producto crearProducto(const std::string& codigo,
                           const std::string& nombre,
                           double precioBase,
                           int stock) const override {
        std::unique_ptr<CalculoPrecio> calculo = std::make_unique<PrecioBase>(precioBase);
        calculo = std::make_unique<ConIVA>(std::move(calculo));
        calculo = std::make_unique<ConGarantia>(std::move(calculo));
        return Producto(codigo, nombre, precioBase, stock, idTipo(), nombreCategoria(), std::move(calculo));
    }

    int idTipo() const override { return 3; }
    std::string nombreCategoria() const override { return "Consola/Hardware"; }
};

class FabricaAccesorio : public FabricaProducto {
public:
    Producto crearProducto(const std::string& codigo,
                           const std::string& nombre,
                           double precioBase,
                           int stock) const override {
        std::unique_ptr<CalculoPrecio> calculo = std::make_unique<PrecioBase>(precioBase);
        calculo = std::make_unique<ConIVA>(std::move(calculo));
        return Producto(codigo, nombre, precioBase, stock, idTipo(), nombreCategoria(), std::move(calculo));
    }

    int idTipo() const override { return 4; }
    std::string nombreCategoria() const override { return "Accesorio"; }
};

// ============================================================================
// 4. PATRON DE COMPORTAMIENTO: STRATEGY (Original: v1/MedioPago.h)
// ============================================================================

// COMPORTAMIENTO: Strategy
class MedioPago {
public:
    virtual ~MedioPago() = default;
    virtual double ajustar(double subtotalConImpuestos) const = 0;
    virtual std::string nombre() const = 0;
    virtual std::string descripcionMenu() const = 0;
    virtual std::string etiquetaComprobante() const = 0;
    virtual bool tieneAjuste() const = 0;
};

class PagoEfectivo : public MedioPago {
public:
    double ajustar(double subtotalConImpuestos) const override {
        return subtotalConImpuestos;
    }

    std::string nombre() const override {
        return "Efectivo";
    }

    std::string descripcionMenu() const override {
        return "Efectivo (Sin recargos ni descuentos)";
    }

    std::string etiquetaComprobante() const override {
        return "";
    }

    bool tieneAjuste() const override {
        return false;
    }
};

class PagoTransferencia : public MedioPago {
public:
    static constexpr double TASA_DESCUENTO = 0.03;

    double ajustar(double subtotalConImpuestos) const override {
        return subtotalConImpuestos * (1.0 - TASA_DESCUENTO);
    }

    std::string nombre() const override {
        return "Transferencia (-3%)";
    }

    std::string descripcionMenu() const override {
        return "Transferencia Bancaria (Descuento del 3%)";
    }

    std::string etiquetaComprobante() const override {
        return "DESCUENTO TRANSFERENCIA (3%):";
    }

    bool tieneAjuste() const override {
        return true;
    }
};

class PagoTarjeta : public MedioPago {
public:
    static constexpr double TASA_DESCUENTO = 0.03;
    static constexpr double RECARGO_PASARELA = 800.0;

    double ajustar(double subtotalConImpuestos) const override {
        return (subtotalConImpuestos * (1.0 - TASA_DESCUENTO)) + RECARGO_PASARELA;
    }

    std::string nombre() const override {
        return "Tarjeta de Credito";
    }

    std::string descripcionMenu() const override {
        return "Tarjeta de Credito (Descuento especial 3% pero recargo de pasarela $800 COP)";
    }

    std::string etiquetaComprobante() const override {
        return "AJUSTE TARJETA (3% desc + $800 pasarela):";
    }

    bool tieneAjuste() const override {
        return true;
    }
};

// ============================================================================
// 5. CLASE VENTA E ITEMVENTA (Original: v1/Venta.h + v1/Tienda.cpp)
// ============================================================================

struct ItemVenta {
    std::string codigo;
    std::string nombre;
    int cantidad;
    double precioUnitarioBase;
    double precioFinalItem;
};

class Venta {
private:
    int idVenta_;
    std::vector<ItemVenta> items_;
    double subtotalConImpuestos_;
    double totalFinal_;
    std::string nombreMedioPago_;
    std::string etiquetaAjuste_;
    bool tieneAjustePago_;

public:
    explicit Venta(int idVenta)
        : idVenta_(idVenta),
          subtotalConImpuestos_(0.0),
          totalFinal_(0.0),
          tieneAjustePago_(false) {}

    void agregarItem(const Producto& prod, int cantidad) {
        // Integracion con Decorator: el producto delega en su cadena CalculoPrecio
        double precioLinea = prod.calcularPrecioLinea(cantidad);
        items_.push_back({prod.codigo(), prod.nombre(), cantidad, prod.precioBase(), precioLinea});
        subtotalConImpuestos_ += precioLinea;
    }

    int cantidadEnCarrito(const std::string& codigo) const {
        int acumulado = 0;
        for (const auto& item : items_) {
            if (item.codigo == codigo) {
                acumulado += item.cantidad;
            }
        }
        return acumulado;
    }

    bool estaVacia() const { return items_.empty(); }

    void liquidarConMedioPago(const MedioPago& medioPago) {
        // Integracion con Strategy: delega el ajuste y descripcion al medio de pago
        totalFinal_ = medioPago.ajustar(subtotalConImpuestos_);
        nombreMedioPago_ = medioPago.nombre();
        etiquetaAjuste_ = medioPago.etiquetaComprobante();
        tieneAjustePago_ = medioPago.tieneAjuste();
    }

    int idVenta() const { return idVenta_; }
    const std::vector<ItemVenta>& items() const { return items_; }
    double subtotalConImpuestos() const { return subtotalConImpuestos_; }
    double totalFinal() const { return totalFinal_; }
    const std::string& nombreMedioPago() const { return nombreMedioPago_; }
    const std::string& etiquetaAjuste() const { return etiquetaAjuste_; }
    bool tieneAjustePago() const { return tieneAjustePago_; }

    int totalArticulos() const {
        int total = 0;
        for (const auto& item : items_) {
            total += item.cantidad;
        }
        return total;
    }
};

// ============================================================================
// 6. INVENTARIO Y COMPROBANTE (Original: v1/Inventario.h + v1/Tienda.cpp)
// ============================================================================

static const FabricaProducto* buscarFabricaPorTipo(
    const vector<unique_ptr<FabricaProducto>>& fabricas, int idTipo) {
    for (const auto& f : fabricas) {
        if (f->idTipo() == idTipo) {
            return f.get();
        }
    }
    return nullptr;
}

class Inventario {
private:
    std::vector<Producto> productos_;
    std::vector<Venta> historialVentas_;
    double totalRecaudado_;
    int consecutivoVentas_;

public:
    Inventario()
        : totalRecaudado_(0.0),
          consecutivoVentas_(1) {}

    bool existeCodigo(const std::string& codigo) const {
        return buscarProducto(codigo) != nullptr;
    }

    bool agregarProducto(Producto&& prod) {
        if (prod.codigo().empty() || prod.nombre().empty()) {
            return false;
        }
        if (!isfinite(prod.precioBase()) || prod.precioBase() <= 0.0 || prod.stock() < 0) {
            return false;
        }
        if (existeCodigo(prod.codigo())) {
            return false;
        }
        productos_.push_back(std::move(prod));
        return true;
    }

    Producto* buscarProducto(const std::string& codigo) {
        for (auto& p : productos_) {
            if (p.codigo() == codigo) {
                return &p;
            }
        }
        return nullptr;
    }

    const Producto* buscarProducto(const std::string& codigo) const {
        for (const auto& p : productos_) {
            if (p.codigo() == codigo) {
                return &p;
            }
        }
        return nullptr;
    }

    int siguienteIdVenta() const { return consecutivoVentas_; }

    void registrarVentaConfirmada(const Venta& venta) {
        historialVentas_.push_back(venta);
        totalRecaudado_ += venta.totalFinal();
        consecutivoVentas_++;
    }

    const std::vector<Producto>& productos() const { return productos_; }
    const std::vector<Venta>& historialVentas() const { return historialVentas_; }
    double totalRecaudado() const { return totalRecaudado_; }

    void cargarSemillaInicial(const std::vector<std::unique_ptr<FabricaProducto>>& fabricas) {
        productos_.clear();
        struct Semilla {
            string codigo;
            string nombre;
            double precio;
            int stock;
            int tipo;
        };

        const vector<Semilla> iniciales = {
            {"J01", "The Legend of Zelda: Tears", 280000.0, 12, 1},
            {"J02", "Elden Ring Shadow DLC (Code)", 150000.0, 25, 2},
            {"C01", "Consola PlayStation 5 Slim", 2600000.0, 4, 3},
            {"C02", "Consola Nintendo Switch OLED", 1600000.0, 6, 3},
            {"A01", "Control Inalambrico Xbox", 290000.0, 8, 4}
        };

        for (const auto& s : iniciales) {
            const FabricaProducto* fab = buscarFabricaPorTipo(fabricas, s.tipo);
            if (fab != nullptr) {
                agregarProducto(fab->crearProducto(s.codigo, s.nombre, s.precio, s.stock));
            }
        }
    }

    bool cargarDesdeArchivo(const std::string& rutaArchivo,
                            const std::vector<std::unique_ptr<FabricaProducto>>& fabricas,
                            bool& archivoExistia) {
        archivoExistia = filesystem::exists(rutaArchivo);
        if (!archivoExistia) {
            return false;
        }

        ifstream in(rutaArchivo);
        if (!in.is_open()) {
            return false;
        }

        vector<Producto> cargados;
        string linea;
        int numLinea = 0;

        while (getline(in, linea)) {
            numLinea++;
            if (linea.empty() || linea == "\r") {
                continue;
            }
            if (numLinea == 1 && linea.rfind("codigo;", 0) == 0) {
                continue;
            }

            stringstream ss(linea);
            string codigo, nombre, strTipo, strPrecio, strStock;

            if (!getline(ss, codigo, ';') ||
                !getline(ss, nombre, ';') ||
                !getline(ss, strTipo, ';') ||
                !getline(ss, strPrecio, ';') ||
                !getline(ss, strStock, ';')) {
                return false;
            }

            if (!strStock.empty() && strStock.back() == '\r') {
                strStock.pop_back();
            }

            if (codigo.empty() || nombre.empty()) {
                return false;
            }

            int idTipo = 0;
            double precio = 0.0;
            int stock = -1;

            try {
                size_t pos1 = 0, pos2 = 0, pos3 = 0;
                idTipo = stoi(strTipo, &pos1);
                precio = stod(strPrecio, &pos2);
                stock = stoi(strStock, &pos3);
                if (pos1 != strTipo.size() || pos2 != strPrecio.size() || pos3 != strStock.size()) {
                    return false;
                }
            } catch (...) {
                return false;
            }

            if (!isfinite(precio) || precio <= 0.0 || stock < 0) {
                return false;
            }

            for (const auto& existente : cargados) {
                if (existente.codigo() == codigo) {
                    return false;
                }
            }

            const FabricaProducto* fab = buscarFabricaPorTipo(fabricas, idTipo);
            if (fab == nullptr) {
                return false;
            }

            cargados.push_back(fab->crearProducto(codigo, nombre, precio, stock));
        }

        if (cargados.empty()) {
            return false;
        }

        productos_ = std::move(cargados);
        return true;
    }

    bool guardarEnArchivo(const std::string& rutaArchivo) const {
        try {
            filesystem::path p(rutaArchivo);
            if (p.has_parent_path()) {
                filesystem::create_directories(p.parent_path());
            }

            string rutaTemp = rutaArchivo + ".tmp";
            {
                ofstream out(rutaTemp, ios::trunc);
                if (!out.is_open()) {
                    return false;
                }

                out << "codigo;nombre;id_tipo;precio_base;stock\n";
                out << fixed << setprecision(2);
                for (const auto& prod : productos_) {
                    out << prod.codigo() << ";"
                        << prod.nombre() << ";"
                        << prod.idTipo() << ";"
                        << prod.precioBase() << ";"
                        << prod.stock() << "\n";
                }
                out.flush();
                if (!out.good()) {
                    return false;
                }
            }

            error_code ec;
            filesystem::rename(rutaTemp, rutaArchivo, ec);
            if (ec) {
                filesystem::remove(rutaArchivo, ec);
                ec.clear();
                filesystem::rename(rutaTemp, rutaArchivo, ec);
                if (ec) {
                    return false;
                }
            }
            return true;
        } catch (...) {
            return false;
        }
    }
};

class Comprobante {
public:
    static void mostrarInventario(const Inventario& inv) {
        cout << "\n========================================================================================\n";
        cout << "                           INVENTARIO ACTUAL - VIDEO SHOP                               \n";
        cout << "========================================================================================\n";
        cout << left << setw(8) << "CODIGO"
             << setw(35) << "NOMBRE"
             << setw(16) << "PRECIO BASE"
             << setw(10) << "STOCK"
             << setw(18) << "CATEGORIA" << "\n";
        cout << "----------------------------------------------------------------------------------------\n";

        for (const auto& p : inv.productos()) {
            cout << left << setw(8) << p.codigo()
                 << setw(35) << p.nombre()
                 << "$" << right << setw(13) << fixed << setprecision(2) << p.precioBase() << "  "
                 << right << setw(6) << p.stock() << "    "
                 << left << setw(18) << p.categoria() << "\n";
        }
        cout << "========================================================================================\n";
    }

    static void mostrarAlertaStock(const Producto& prod) {
        cout << ">>> [ALERTA DE STOCK] Stock critico para '" << prod.nombre()
             << "'! Quedan solo " << prod.stock() << " unidades en bodega.\n";
    }

    static void mostrarFactura(const Venta& venta) {
        cout << "\n========================================================================================\n";
        cout << "                          COMPROBANTE DE VENTA #" << venta.idVenta() << " - VIDEO SHOP                       \n";
        cout << "========================================================================================\n";
        cout << left << setw(10) << "CODIGO"
             << setw(35) << "DESCRIPCION"
             << setw(10) << "CANTIDAD"
             << setw(18) << "PRECIO LINEA" << "\n";
        cout << "----------------------------------------------------------------------------------------\n";
        for (const auto& it : venta.items()) {
            cout << left << setw(10) << it.codigo
                 << setw(35) << it.nombre
                 << right << setw(6) << it.cantidad << "    "
                 << "$" << right << setw(15) << fixed << setprecision(2) << it.precioFinalItem << "\n";
        }
        cout << "----------------------------------------------------------------------------------------\n";
        cout << left << setw(51) << "SUBTOTAL (CON IMPUESTOS POR TIPO):"
             << "$" << right << setw(15) << fixed << setprecision(2) << venta.subtotalConImpuestos() << "\n";

        if (venta.tieneAjustePago()) {
            double diferencia = venta.totalFinal() - venta.subtotalConImpuestos();
            if (diferencia < 0.0) {
                cout << left << setw(51) << venta.etiquetaAjuste()
                     << "-$" << right << setw(14) << fixed << setprecision(2) << (-diferencia) << "\n";
            } else {
                cout << left << setw(51) << venta.etiquetaAjuste()
                     << "$" << right << setw(15) << fixed << setprecision(2) << diferencia << "\n";
            }
        }

        cout << left << setw(51) << "TOTAL PAGADO:"
             << "$" << right << setw(15) << fixed << setprecision(2) << venta.totalFinal() << "\n";
        cout << "========================================================================================\n";
        cout << ">> Venta registrada con exito.\n";
    }

    static void mostrarReporteVentas(const Inventario& inv) {
        cout << "\n========================================================================================\n";
        cout << "                         REPORTE HISTORICO DE VENTAS                                    \n";
        cout << "========================================================================================\n";
        if (inv.historialVentas().empty()) {
            cout << "No se han registrado ventas en esta sesion.\n";
        } else {
            cout << left << setw(10) << "VENTA #"
                 << setw(15) << "TOTAL ITEMS"
                 << setw(25) << "FORMA DE PAGO"
                 << setw(20) << "VALOR TOTAL" << "\n";
            cout << "----------------------------------------------------------------------------------------\n";

            for (const auto& v : inv.historialVentas()) {
                cout << left << setw(10) << v.idVenta()
                     << right << setw(7) << v.totalArticulos() << "        "
                     << left << setw(25) << v.nombreMedioPago()
                     << "$" << right << setw(16) << fixed << setprecision(2) << v.totalFinal() << "\n";
            }
        }
        cout << "----------------------------------------------------------------------------------------\n";
        cout << left << setw(50) << "RECAUDO TOTAL HISTORICO EN CAJA:"
             << "$" << right << setw(18) << fixed << setprecision(2) << inv.totalRecaudado() << "\n";
        cout << "========================================================================================\n";
    }
};

// ============================================================================
// 7. CONTROLADOR DE CONSOLA Y FUNCION PRINCIPAL (Original: v1/main.cpp)
// ============================================================================

static string recortarEspacios(const string& str) {
    size_t inicio = str.find_first_not_of(" \t\r\n");
    if (inicio == string::npos) return "";
    size_t fin = str.find_last_not_of(" \t\r\n");
    return str.substr(inicio, fin - inicio + 1);
}

static bool leerEnteroSeguro(int& valor) {
    string entrada;
    if (!(cin >> entrada)) {
        if (cin.eof()) return false;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    try {
        size_t pos = 0;
        valor = stoi(entrada, &pos);
        if (pos != entrada.size()) {
            return false;
        }
        return true;
    } catch (...) {
        return false;
    }
}

static bool leerDoubleSeguro(double& valor) {
    string entrada;
    if (!(cin >> entrada)) {
        if (cin.eof()) return false;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    try {
        size_t pos = 0;
        valor = stod(entrada, &pos);
        if (pos != entrada.size() || !isfinite(valor)) {
            return false;
        }
        return true;
    } catch (...) {
        return false;
    }
}

// F1: Registrar producto usando Factory Method + Decorator
static void ejecutarRegistroProducto(Inventario& inventario,
                                     const vector<unique_ptr<FabricaProducto>>& fabricas,
                                     const string& rutaDatos,
                                     bool persistenciaActiva) {
    cout << "\n--- REGISTRAR NUEVO PRODUCTO ---\n";
    cout << "Ingrese codigo del producto (ej. J03): ";
    string codigo;
    if (!(cin >> codigo)) return;
    codigo = recortarEspacios(codigo);

    if (codigo.empty() || codigo.find(';') != string::npos) {
        cout << "[ERROR] Codigo invalido.\n";
        return;
    }

    if (inventario.existeCodigo(codigo)) {
        cout << "[ERROR] Ya existe un producto con el codigo " << codigo << ".\n";
        return;
    }

    cout << "Ingrese nombre del producto: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string nombre;
    getline(cin, nombre);
    nombre = recortarEspacios(nombre);

    if (nombre.empty() || nombre.find(';') != string::npos) {
        cout << "[ERROR] El nombre del producto no puede estar vacio ni contener ';'.\n";
        return;
    }

    cout << "Ingrese precio unitario (COP): ";
    double precio = 0.0;
    if (!leerDoubleSeguro(precio) || precio <= 0.0) {
        cout << "[ERROR] El precio debe ser un numero positivo mayor a 0.\n";
        return;
    }

    cout << "Ingrese stock inicial: ";
    int stock = -1;
    if (!leerEnteroSeguro(stock) || stock < 0) {
        cout << "[ERROR] El stock inicial debe ser un entero mayor o igual a 0.\n";
        return;
    }

    cout << "Seleccione categoria (";
    for (size_t i = 0; i < fabricas.size(); ++i) {
        cout << (i + 1) << ": " << fabricas[i]->nombreCategoria();
        if (i + 1 < fabricas.size()) cout << ", ";
    }
    cout << "): ";
    int opcionTipo = 0;
    if (!leerEnteroSeguro(opcionTipo) || opcionTipo < 1 ||
        static_cast<size_t>(opcionTipo) > fabricas.size()) {
        cout << "[ERROR] Tipo de producto invalido.\n";
        return;
    }

    const FabricaProducto& fabricaElegida = *fabricas[static_cast<size_t>(opcionTipo - 1)];
    Producto nuevo = fabricaElegida.crearProducto(codigo, nombre, precio, stock);
    string nombreCreado = nuevo.nombre();

    if (!inventario.agregarProducto(std::move(nuevo))) {
        cout << "[ERROR] No se pudo registrar el producto.\n";
        return;
    }

    if (persistenciaActiva && !rutaDatos.empty()) {
        if (!inventario.guardarEnArchivo(rutaDatos)) {
            cout << "[ADVERTENCIA] El producto se registro en memoria, pero fallo el guardado en disco.\n";
        }
    }

    cout << ">> Producto '" << nombreCreado << "' registrado exitosamente en inventario.\n";
}

// F3, F4, F5: Registrar venta integrando Factory Method + Decorator + Strategy
static void ejecutarRegistroVenta(Inventario& inventario,
                                  const vector<unique_ptr<MedioPago>>& mediosPago,
                                  const string& rutaDatos,
                                  bool persistenciaActiva) {
    cout << "\n--- REGISTRAR NUEVA VENTA ---\n";
    Venta ventaActual(inventario.siguienteIdVenta());
    char agregarMas = 's';

    while (agregarMas == 's' || agregarMas == 'S') {
        Comprobante::mostrarInventario(inventario);
        cout << "\nIngrese codigo del producto a comprar (o 0 para cancelar): ";
        string cod;
        if (!(cin >> cod)) return;

        if (cod == "0") {
            cout << "[AVISO] Operacion cancelada por el usuario. Stock sin modificaciones.\n";
            return;
        }

        const Producto* prod = inventario.buscarProducto(cod);
        if (prod == nullptr) {
            cout << "[ERROR] Producto no encontrado.\n";
        } else {
            int reservadoEnCarrito = ventaActual.cantidadEnCarrito(cod);
            int disponibleReal = prod->stock() - reservadoEnCarrito;

            cout << "Producto seleccionado: " << prod->nombre()
                 << " (Stock disponible: " << disponibleReal << ")\n";
            cout << "Ingrese cantidad a comprar: ";
            int cant = 0;

            if (!leerEnteroSeguro(cant) || cant <= 0) {
                cout << "[ERROR] La cantidad debe ser un entero mayor a 0.\n";
            } else if (cant > disponibleReal) {
                cout << "[ERROR] Stock insuficiente. Solo hay " << disponibleReal
                     << " unidades disponibles.\n";
            } else {
                ventaActual.agregarItem(*prod, cant);
                int saldoProyectado = disponibleReal - cant;
                if (saldoProyectado < Producto::UMBRAL_STOCK_CRITICO) {
                    cout << ">>> [ALERTA DE STOCK (F5)] Stock critico para '" << prod->nombre()
                         << "'! Quedan solo " << saldoProyectado << " unidades en bodega.\n";
                }
                cout << ">> Item agregado al carrito de venta.\n";
            }
        }

        cout << "\nDesea agregar otro producto a esta misma venta? (s/n, c para cancelar venta): ";
        if (!(cin >> agregarMas)) return;
        if (agregarMas == 'c' || agregarMas == 'C') {
            cout << "[AVISO] Venta cancelada. El stock permanece intacto.\n";
            return;
        }
    }

    if (ventaActual.estaVacia()) {
        cout << "[AVISO] No se seleccionaron productos. Venta cancelada.\n";
        return;
    }

    int opcionPago = 0;
    while (true) {
        cout << "\nSeleccione la forma de pago:\n";
        for (size_t i = 0; i < mediosPago.size(); ++i) {
            cout << (i + 1) << ". " << mediosPago[i]->descripcionMenu() << "\n";
        }
        cout << "0. Cancelar venta\n";
        cout << "Opcion: ";

        if (!leerEnteroSeguro(opcionPago)) {
            cout << "[ERROR] Entrada no numerica. Intente de nuevo.\n";
            continue;
        }
        if (opcionPago == 0) {
            cout << "[AVISO] Venta cancelada antes del pago. El stock permanece intacto.\n";
            return;
        }
        if (opcionPago >= 1 && static_cast<size_t>(opcionPago) <= mediosPago.size()) {
            break;
        }
        cout << "[ERROR] Forma de pago invalida. Seleccione una opcion entre 1 y "
             << mediosPago.size() << " (o 0 para cancelar).\n";
    }

    const MedioPago& estrategiaPago = *mediosPago[static_cast<size_t>(opcionPago - 1)];
    ventaActual.liquidarConMedioPago(estrategiaPago);

    for (const auto& item : ventaActual.items()) {
        Producto* p = inventario.buscarProducto(item.codigo);
        if (p != nullptr) {
            p->descontarStock(item.cantidad);
        }
    }

    inventario.registrarVentaConfirmada(ventaActual);

    if (persistenciaActiva && !rutaDatos.empty()) {
        if (!inventario.guardarEnArchivo(rutaDatos)) {
            cout << "[ADVERTENCIA] La venta se registro en memoria, pero fallo la actualizacion del archivo local.\n";
        }
    }

    Comprobante::mostrarFactura(ventaActual);
}

int main(int argc, char* argv[]) {
    // En la version online autocontenida, el modo por defecto es demostrativo en memoria
    // (persistenciaActiva = false) para que cada ejecucion en OnlineGDB inicie limpia con los
    // 5 productos semilla de V0 sin depender del sistema de archivos del contenedor remoto.
    // Si se desea probar persistencia en archivo CSV, se puede pasar: --con-persistencia
    string rutaDatos = "productos_online.csv";
    bool persistenciaActiva = false;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--con-persistencia") {
            persistenciaActiva = true;
        } else if (arg == "--sin-persistencia") {
            persistenciaActiva = false;
        } else if (arg == "--archivo-datos" && i + 1 < argc) {
            rutaDatos = argv[++i];
            persistenciaActiva = true;
        }
    }

    // 1. Registro polimorfico de fabricas (Factory Method)
    vector<unique_ptr<FabricaProducto>> fabricas;
    fabricas.push_back(make_unique<FabricaFisico>());
    fabricas.push_back(make_unique<FabricaDigital>());
    fabricas.push_back(make_unique<FabricaConsola>());
    fabricas.push_back(make_unique<FabricaAccesorio>());

    // 2. Registro polimorfico de estrategias de pago (Strategy)
    vector<unique_ptr<MedioPago>> mediosPago;
    mediosPago.push_back(make_unique<PagoEfectivo>());
    mediosPago.push_back(make_unique<PagoTransferencia>());
    mediosPago.push_back(make_unique<PagoTarjeta>());

    // 3. Inicializacion del inventario
    Inventario inventario;
    if (persistenciaActiva) {
        bool existia = false;
        if (!inventario.cargarDesdeArchivo(rutaDatos, fabricas, existia)) {
            if (existia) {
                cout << "[ADVERTENCIA] El archivo de datos '" << rutaDatos
                     << "' presenta formato invalido o corrupto. Se conserva el archivo original sin sobrescribir y se inicia con el catalogo base en memoria.\n";
                persistenciaActiva = false;
            } else {
                inventario.cargarSemillaInicial(fabricas);
                inventario.guardarEnArchivo(rutaDatos);
            }
        }
    } else {
        inventario.cargarSemillaInicial(fabricas);
    }

    // 4. Bucle principal con menu numerico corregido
    int opcion = -1;
    do {
        cout << "\n========== VIDEO SHOP ==========\n";
        cout << "1. Registrar producto\n";
        cout << "2. Consultar inventario\n";
        cout << "3. Registrar venta\n";
        cout << "4. Reporte de ventas\n";
        cout << "0. Salir\n\n";
        cout << "Seleccione una opcion: ";

        if (!leerEnteroSeguro(opcion)) {
            if (cin.eof()) {
                cout << "\nFin de entrada detectado. Cerrando sistema Video Shop.\n";
                break;
            }
            cout << "\n[ERROR] Entrada invalida. Ingrese un numero (1, 2, 3, 4 o 0).\n";
            opcion = -1;
            continue;
        }

        switch (opcion) {
            case 1:
                ejecutarRegistroProducto(inventario, fabricas, rutaDatos, persistenciaActiva);
                break;
            case 2:
                Comprobante::mostrarInventario(inventario);
                break;
            case 3:
                ejecutarRegistroVenta(inventario, mediosPago, rutaDatos, persistenciaActiva);
                break;
            case 4:
                Comprobante::mostrarReporteVentas(inventario);
                break;
            case 0:
                cout << "\nCerrando sistema Video Shop. Hasta luego!\n";
                break;
            default:
                cout << "\n[ERROR] Opcion no valida. Seleccione 1, 2, 3, 4 o 0.\n";
                break;
        }
    } while (opcion != 0);

    return 0;
}
