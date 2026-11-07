/**
 * ============================================================================
 * PROYECTO: Video Shop - Tienda de Videojuegos (Version 1 con Patrones GoF)
 * CURSO: Patrones de Software (Codigo 74362) - Cotecnova 2026-2
 * MODULO: v1/main.cpp
 * PATRONES INTEGRADOS:
 *   - Creacional: Factory Method (FabricaProducto)
 *   - Estructural: Decorator (CalculoPrecio / DecoradorPrecio)
 *   - Comportamiento: Strategy (MedioPago)
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <limits>
#include <cmath>
#include "FabricaProducto.h"
#include "MedioPago.h"
#include "Inventario.h"

using namespace std;

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

    // Invocacion polimorfica del Factory Method (sin switch por tipo)
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
                // Calcula precio de linea mediante la cadena Decorator del producto
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

    // Seleccion de estrategia de pago (Strategy)
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

    // Aplicar Strategy elegida
    const MedioPago& estrategiaPago = *mediosPago[static_cast<size_t>(opcionPago - 1)];
    ventaActual.liquidarConMedioPago(estrategiaPago);

    // Confirmada la venta y el medio de pago, descontar existencias reales (F5)
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
    // Ruta por defecto para persistencia local de productos (permite ruta personalizada en pruebas)
    string rutaDatos = "v1/data/productos.csv";
    bool persistenciaActiva = true;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--sin-persistencia") {
            persistenciaActiva = false;
        } else if (arg == "--archivo-datos" && i + 1 < argc) {
            rutaDatos = argv[++i];
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

    // 3. Inicializacion del inventario (desde archivo local o semilla inicial)
    Inventario inventario;
    if (persistenciaActiva) {
        bool existia = false;
        if (!inventario.cargarDesdeArchivo(rutaDatos, fabricas, existia)) {
            if (existia) {
                cout << "[ADVERTENCIA] El archivo de datos '" << rutaDatos
                     << "' presenta formato invalido o corrupto. Se conserva el archivo original sin sobrescribir y se inicia con el catalogo base en memoria.\n";
                persistenciaActiva = false; // Protege el archivo danado contra sobrescritura silenciosa
            } else {
                inventario.cargarSemillaInicial(fabricas);
                inventario.guardarEnArchivo(rutaDatos);
            }
        }
    } else {
        inventario.cargarSemillaInicial(fabricas);
    }

    // 4. Bucle principal con menu numerico corregido (Mejora A)
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
