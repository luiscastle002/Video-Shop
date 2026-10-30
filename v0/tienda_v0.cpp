/**
 * ============================================================================
 * PROYECTO: Video Shop - Tienda de Videojuegos
 * CURSO: Patrones de Software (Codigo 74362)
 * INSTITUCION: Cotecnova - 2026-2
 * MODULO: Version 0 (v0/tienda_v0.cpp)
 * RESPONSABLE: Responsable de Codigo (Code Manager)
 *
 * DESCRIPCION:
 * Primera version funcional de la aplicacion de consola para gestion de tienda.
 * Implementada de forma directa y monolitica SIN patrones de diseno.
 * Cumple con las 6 funciones minimas (F1 a F6) y contiene deliberadamente
 * olores de codigo y antipatrones para el analisis del Seguimiento 2.
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

// Estructura basica de producto en inventario
struct Producto {
    string codigo;
    string nombre;
    double precio;
    int stock;
    int tipo; // 1: Juego Fisico, 2: Juego Digital/DLC, 3: Consola/Hardware, 4: Accesorio
};

// Item dentro de una venta
struct ItemVenta {
    string codigo;
    string nombre;
    int cantidad;
    double precioUnitarioBase;
    double precioFinalItem;
};

// Registro de una venta efectuada
struct Venta {
    int idVenta;
    vector<ItemVenta> items;
    int formaPago; // 1: Efectivo, 2: Transferencia, 3: Tarjeta de Credito
    double subtotalBase;
    double totalConImpuestosYDescuentos;
};

/**
 * ============================================================================
 * ANTIPATRON: CLASE DIOS (God Class) / Bloque monolitico
 * La clase TiendaVideojuegos centraliza inventario, logica de carrito,
 * calculo de impuestos, descuentos, control de stock, entrada/salida y reportes.
 * ============================================================================
 */
class TiendaVideojuegos {
public:
    vector<Producto> productos;
    vector<Venta> historialVentas;
    double totalVentas = 0.0;
    int contadorVentas = 1;

    TiendaVideojuegos() {
        // Carga de datos semilla (Semilla de inventario inicial para Video Shop)
        productos.push_back({"J01", "The Legend of Zelda: Tears", 280000.0, 12, 1}); // Fisico
        productos.push_back({"J02", "Elden Ring Shadow DLC (Code)", 150000.0, 25, 2}); // Digital
        productos.push_back({"C01", "Consola PlayStation 5 Slim", 2600000.0, 4, 3});  // Consola/Hardware
        productos.push_back({"C02", "Consola Nintendo Switch OLED", 1600000.0, 6, 3});  // Consola/Hardware
        productos.push_back({"A01", "Control Inalambrico Xbox", 290000.0, 8, 4});       // Accesorio
    }

    // F1: Registrar nuevo producto
    void registrarProducto() {
        cout << "\n--- REGISTRAR NUEVO PRODUCTO (F1) ---\n";
        Producto p;
        cout << "Ingrese codigo del producto (ej. J03): ";
        cin >> p.codigo;

        // Validar si el codigo ya existe
        for (const auto& item : productos) {
            if (item.codigo == p.codigo) {
                cout << "[ERROR] Ya existe un producto con el codigo " << p.codigo << ".\n";
                return;
            }
        }

        cout << "Ingrese nombre del producto: ";
        cin.ignore();
        getline(cin, p.nombre);

        cout << "Ingrese precio unitario (COP): ";
        cin >> p.precio;

        cout << "Ingrese stock inicial: ";
        cin >> p.stock;

        cout << "Seleccione tipo (1: Fisico, 2: Digital, 3: Consola, 4: Accesorio): ";
        cin >> p.tipo;

        if (p.tipo < 1 || p.tipo > 4) {
            cout << "[ERROR] Tipo de producto invalido.\n";
            return;
        }

        productos.push_back(p);
        cout << ">> Producto '" << p.nombre << "' registrado exitosamente en inventario.\n";
    }

    // F2: Consultar inventario
    void consultarInventario() {
        cout << "\n========================================================================================\n";
        cout << "                           INVENTARIO ACTUAL - VIDEO SHOP (F2)                          \n";
        cout << "========================================================================================\n";
        cout << left << setw(8) << "CODIGO"
             << setw(35) << "NOMBRE"
             << setw(16) << "PRECIO BASE"
             << setw(10) << "STOCK"
             << setw(18) << "TIPO" << "\n";
        cout << "----------------------------------------------------------------------------------------\n";

        for (const auto& p : productos) {
            string tipoStr = "";
            // ANTIPATRON: Switch/condicionales por tipo repetidos en la presentacion
            switch (p.tipo) {
                case 1: tipoStr = "Juego Fisico"; break;
                case 2: tipoStr = "Juego Digital"; break;
                case 3: tipoStr = "Consola/Hardware"; break;
                case 4: tipoStr = "Accesorio"; break;
                default: tipoStr = "Desconocido"; break;
            }

            cout << left << setw(8) << p.codigo
                 << setw(35) << p.nombre
                 << "$" << right << setw(13) << fixed << setprecision(2) << p.precio << "  "
                 << right << setw(6) << p.stock << "    "
                 << left << setw(18) << tipoStr << "\n";
        }
        cout << "========================================================================================\n";
    }

    // Buscar producto por codigo
    Producto* buscarProducto(const string& codigo) {
        for (auto& p : productos) {
            if (p.codigo == codigo) {
                return &p;
            }
        }
        return nullptr;
    }

    // F3, F4, F5: Registrar una venta completa (admite multiples items, aplica descuentos e impuestos y controla stock)
    void registrarVenta() {
        cout << "\n--- REGISTRAR NUEVA VENTA (F3, F4, F5) ---\n";
        vector<ItemVenta> itemsVenta;
        char agregarMas = 's';

        while (agregarMas == 's' || agregarMas == 'S') {
            consultarInventario();
            string cod;
            int cant = 0;

            cout << "\nIngrese codigo del producto a comprar: ";
            cin >> cod;

            Producto* prod = buscarProducto(cod);
            if (prod == nullptr) {
                cout << "[ERROR] Producto no encontrado.\n";
            } else {
                cout << "Producto seleccionado: " << prod->nombre << " (Stock disponible: " << prod->stock << ")\n";
                cout << "Ingrese cantidad a comprar: ";
                cin >> cant;

                if (cant <= 0) {
                    cout << "[ERROR] La cantidad debe ser mayor a 0.\n";
                } else if (cant > prod->stock) {
                    // Validacion de disponibilidad
                    cout << "[ERROR] Stock insuficiente. Solo hay " << prod->stock << " unidades disponibles.\n";
                } else {
                    // ANTIPATRON: Calculo con numeros magicos y switch por tipo quemado en el codigo
                    double precioBaseLinea = prod->precio * cant;
                    double precioConImpuesto = precioBaseLinea;

                    switch (prod->tipo) {
                        case 1: // Juego Fisico: IVA del 19%
                            precioConImpuesto = precioBaseLinea * 1.19; // NUMERO MAGICO 1.19
                            break;
                        case 2: // Digital: Servicio de plataforma 5%
                            precioConImpuesto = precioBaseLinea * 1.05; // NUMERO MAGICO 1.05
                            break;
                        case 3: // Consola: IVA 19% + Poliza obligatoria de garantia quemada ($5,000 COP)
                            precioConImpuesto = (precioBaseLinea * 1.19) + (5000.0 * cant); // NUMEROS MAGICOS
                            break;
                        case 4: // Accesorio: IVA del 19%
                            precioConImpuesto = precioBaseLinea * 1.19; // CODIGO DUPLICADO
                            break;
                        default:
                            precioConImpuesto = precioBaseLinea;
                            break;
                    }

                    ItemVenta item;
                    item.codigo = prod->codigo;
                    item.nombre = prod->nombre;
                    item.cantidad = cant;
                    item.precioUnitarioBase = prod->precio;
                    item.precioFinalItem = precioConImpuesto;

                    itemsVenta.push_back(item);

                    // F5: Control de stock inmediato
                    prod->stock -= cant;

                    // ANTIPATRON: Notificacion acoplada directamente a la consola
                    if (prod->stock < 5) {
                        cout << ">>> [ALERTA DE STOCK (F5)] Stock critico para '" << prod->nombre 
                             << "'! Quedan solo " << prod->stock << " unidades en bodega.\n";
                    }

                    cout << ">> Item agregado al carrito de venta.\n";
                }
            }

            cout << "\nDesea agregar otro producto a esta misma venta? (s/n): ";
            cin >> agregarMas;
        }

        if (itemsVenta.empty()) {
            cout << "[AVISO] No se seleccionaron productos. Venta cancelada.\n";
            return;
        }

        // Seleccion de forma de pago
        cout << "\nSeleccione la forma de pago (F4):\n";
        cout << "1. Efectivo (Sin recargos ni descuentos)\n";
        cout << "2. Transferencia Bancaria (Descuento del 3%)\n";
        cout << "3. Tarjeta de Credito (Descuento especial 3% pero recargo de pasarela $800 COP)\n";
        cout << "Opcion: ";
        int formaPago;
        cin >> formaPago;

        double subtotalAcumulado = 0.0;
        for (const auto& it : itemsVenta) {
            subtotalAcumulado += it.precioFinalItem;
        }

        double totalFinal = subtotalAcumulado;

        // ANTIPATRON: Logica de descuentos y recargos quemada con condicionales y codigo duplicado
        if (formaPago == 2) {
            totalFinal = totalFinal * 0.97; // NUMERO MAGICO: 3% descuento
        } else if (formaPago == 3) {
            totalFinal = (totalFinal * 0.97) + 800.0; // NUMERO MAGICO: recargo fijo quemado
        }

        // Registrar en historial
        Venta v;
        v.idVenta = contadorVentas++;
        v.items = itemsVenta;
        v.formaPago = formaPago;
        v.subtotalBase = subtotalAcumulado;
        v.totalConImpuestosYDescuentos = totalFinal;

        historialVentas.push_back(v);
        totalVentas += totalFinal;

        // Imprimir factura / comprobante
        cout << "\n========================================================================================\n";
        cout << "                          COMPROBANTE DE VENTA #" << v.idVenta << " - VIDEO SHOP                       \n";
        cout << "========================================================================================\n";
        cout << left << setw(10) << "CODIGO"
             << setw(35) << "DESCRIPCION"
             << setw(10) << "CANTIDAD"
             << setw(18) << "PRECIO LINEA" << "\n";
        cout << "----------------------------------------------------------------------------------------\n";
        for (const auto& it : v.items) {
            cout << left << setw(10) << it.codigo
                 << setw(35) << it.nombre
                 << right << setw(6) << it.cantidad << "    "
                 << "$" << right << setw(15) << fixed << setprecision(2) << it.precioFinalItem << "\n";
        }
        cout << "----------------------------------------------------------------------------------------\n";
        cout << left << setw(51) << "SUBTOTAL (CON IMPUESTOS POR TIPO):"
             << "$" << right << setw(15) << fixed << setprecision(2) << subtotalAcumulado << "\n";

        // ANTIPATRON: Duplicacion de la logica de forma de pago para impresion
        if (formaPago == 2) {
            cout << left << setw(51) << "DESCUENTO TRANSFERENCIA (3%):"
                 << "-$" << right << setw(14) << (subtotalAcumulado * 0.03) << "\n";
        } else if (formaPago == 3) {
            cout << left << setw(51) << "AJUSTE TARJETA (3% desc + $800 pasarela):"
                 << "$" << right << setw(15) << (totalFinal - subtotalAcumulado) << "\n";
        }

        cout << left << setw(51) << "TOTAL PAGADO:"
             << "$" << right << setw(15) << fixed << setprecision(2) << totalFinal << "\n";
        cout << "========================================================================================\n";
        cout << ">> Venta registrada con exito.\n";
    }

    // F6: Reporte general de ventas y recaudacion acumulada
    void reporteDeVentas() {
        cout << "\n========================================================================================\n";
        cout << "                         REPORTE HISTORICO DE VENTAS (F6)                               \n";
        cout << "========================================================================================\n";
        if (historialVentas.empty()) {
            cout << "No se han registrado ventas hasta el momento.\n";
        } else {
            cout << left << setw(10) << "VENTA #"
                 << setw(15) << "TOTAL ITEMS"
                 << setw(25) << "FORMA DE PAGO"
                 << setw(20) << "VALOR TOTAL" << "\n";
            cout << "----------------------------------------------------------------------------------------\n";

            for (const auto& v : historialVentas) {
                int totalArticulos = 0;
                for (const auto& item : v.items) {
                    totalArticulos += item.cantidad;
                }

                string fpStr = "";
                if (v.formaPago == 1) fpStr = "Efectivo";
                else if (v.formaPago == 2) fpStr = "Transferencia (-3%)";
                else if (v.formaPago == 3) fpStr = "Tarjeta de Credito";
                else fpStr = "Otro";

                cout << left << setw(10) << v.idVenta
                     << right << setw(7) << totalArticulos << "        "
                     << left << setw(25) << fpStr
                     << "$" << right << setw(16) << fixed << setprecision(2) << v.totalConImpuestosYDescuentos << "\n";
            }
        }
        cout << "----------------------------------------------------------------------------------------\n";
        cout << left << setw(50) << "RECAUDO TOTAL HISTORICO EN CAJA:"
             << "$" << right << setw(18) << fixed << setprecision(2) << totalVentas << "\n";
        cout << "========================================================================================\n";
    }
};

int main() {
    TiendaVideojuegos tienda;
    int opcion = 0;

    do {
        cout << "\n=========================================\n";
        cout << "      SISTEMA VIDEO SHOP - VERSION 0     \n";
        cout << "      (Patrones de Software Cotecnova)   \n";
        cout << "=========================================\n";
        cout << "1. Registrar producto (F1)\n";
        cout << "2. Consultar inventario (F2)\n";
        cout << "3. Registrar venta (F3, F4, F5)\n";
        cout << "4. Reporte general de ventas (F6)\n";
        cout << "0. Salir\n";
        cout << "Seleccione una opcion: ";

        if (!(cin >> opcion)) {
            cout << "\n[ERROR] Entrada invalida. Saliendo del programa...\n";
            break;
        }

        switch (opcion) {
            case 1:
                tienda.registrarProducto();
                break;
            case 2:
                tienda.consultarInventario();
                break;
            case 3:
                tienda.registrarVenta();
                break;
            case 4:
                tienda.reporteDeVentas();
                break;
            case 0:
                cout << "\nCerrando sistema Video Shop. Hasta luego!\n";
                break;
            default:
                cout << "\nOpcion no valida. Intente de nuevo.\n";
                break;
        }
    } while (opcion != 0);

    return 0;
}

