#include "Inventario.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>
#include <filesystem>

using namespace std;

// ============================================================================
// IMPLEMENTACION DE VENTA
// ============================================================================

void Venta::agregarItem(const Producto& prod, int cantidad) {
    // Integracion con Decorator: el producto delega en su cadena CalculoPrecio
    double precioLinea = prod.calcularPrecioLinea(cantidad);
    items_.push_back({prod.codigo(), prod.nombre(), cantidad, prod.precioBase(), precioLinea});
    subtotalConImpuestos_ += precioLinea;
}

int Venta::cantidadEnCarrito(const string& codigo) const {
    int acumulado = 0;
    for (const auto& item : items_) {
        if (item.codigo == codigo) {
            acumulado += item.cantidad;
        }
    }
    return acumulado;
}

void Venta::liquidarConMedioPago(const MedioPago& medioPago) {
    // Integracion con Strategy: delega el ajuste y descripcion al medio de pago
    totalFinal_ = medioPago.ajustar(subtotalConImpuestos_);
    nombreMedioPago_ = medioPago.nombre();
    etiquetaAjuste_ = medioPago.etiquetaComprobante();
    tieneAjustePago_ = medioPago.tieneAjuste();
}

int Venta::totalArticulos() const {
    int total = 0;
    for (const auto& item : items_) {
        total += item.cantidad;
    }
    return total;
}

// ============================================================================
// IMPLEMENTACION DE INVENTARIO Y PERSISTENCIA LOCAL
// ============================================================================

bool Inventario::existeCodigo(const string& codigo) const {
    return buscarProducto(codigo) != nullptr;
}

bool Inventario::agregarProducto(Producto&& prod) {
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

Producto* Inventario::buscarProducto(const string& codigo) {
    for (auto& p : productos_) {
        if (p.codigo() == codigo) {
            return &p;
        }
    }
    return nullptr;
}

const Producto* Inventario::buscarProducto(const string& codigo) const {
    for (const auto& p : productos_) {
        if (p.codigo() == codigo) {
            return &p;
        }
    }
    return nullptr;
}

void Inventario::registrarVentaConfirmada(const Venta& venta) {
    historialVentas_.push_back(venta);
    totalRecaudado_ += venta.totalFinal();
    consecutivoVentas_++;
}

static const FabricaProducto* buscarFabricaPorTipo(
    const vector<unique_ptr<FabricaProducto>>& fabricas, int idTipo) {
    for (const auto& f : fabricas) {
        if (f->idTipo() == idTipo) {
            return f.get();
        }
    }
    return nullptr;
}

void Inventario::cargarSemillaInicial(const vector<unique_ptr<FabricaProducto>>& fabricas) {
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

bool Inventario::cargarDesdeArchivo(const string& rutaArchivo,
                                    const vector<unique_ptr<FabricaProducto>>& fabricas,
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
            continue; // Encabezado CSV
        }

        stringstream ss(linea);
        string codigo, nombre, strTipo, strPrecio, strStock;

        if (!getline(ss, codigo, ';') ||
            !getline(ss, nombre, ';') ||
            !getline(ss, strTipo, ';') ||
            !getline(ss, strPrecio, ';') ||
            !getline(ss, strStock, ';')) {
            return false; // Registro corrupto o incompleto
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

        // Verificar que no haya codigos duplicados en el archivo
        for (const auto& existente : cargados) {
            if (existente.codigo() == codigo) {
                return false;
            }
        }

        const FabricaProducto* fab = buscarFabricaPorTipo(fabricas, idTipo);
        if (fab == nullptr) {
            return false;
        }

        // Reconstruir mediante Factory Method + Decorator
        cargados.push_back(fab->crearProducto(codigo, nombre, precio, stock));
    }

    if (cargados.empty()) {
        return false;
    }

    productos_ = std::move(cargados);
    return true;
}

bool Inventario::guardarEnArchivo(const string& rutaArchivo) const {
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

// ============================================================================
// IMPLEMENTACION DE COMPROBANTE (PRESENTACION DESACOPLADA)
// ============================================================================

void Comprobante::mostrarInventario(const Inventario& inv) {
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

void Comprobante::mostrarAlertaStock(const Producto& prod) {
    cout << ">>> [ALERTA DE STOCK] Stock critico para '" << prod.nombre()
         << "'! Quedan solo " << prod.stock() << " unidades en bodega.\n";
}

void Comprobante::mostrarFactura(const Venta& venta) {
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

void Comprobante::mostrarReporteVentas(const Inventario& inv) {
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
