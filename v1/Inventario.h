#ifndef INVENTARIO_H
#define INVENTARIO_H

#include <vector>
#include <string>
#include <memory>
#include "Producto.h"
#include "FabricaProducto.h"
#include "Venta.h"

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

    bool existeCodigo(const std::string& codigo) const;
    bool agregarProducto(Producto&& prod);
    Producto* buscarProducto(const std::string& codigo);
    const Producto* buscarProducto(const std::string& codigo) const;

    int siguienteIdVenta() const { return consecutivoVentas_; }
    void registrarVentaConfirmada(const Venta& venta);

    const std::vector<Producto>& productos() const { return productos_; }
    const std::vector<Venta>& historialVentas() const { return historialVentas_; }
    double totalRecaudado() const { return totalRecaudado_; }

    // Persistencia local en archivo de texto CSV (biblioteca estandar C++)
    bool cargarDesdeArchivo(const std::string& rutaArchivo,
                            const std::vector<std::unique_ptr<FabricaProducto>>& fabricas,
                            bool& archivoExistia);
    bool guardarEnArchivo(const std::string& rutaArchivo) const;
    void cargarSemillaInicial(const std::vector<std::unique_ptr<FabricaProducto>>& fabricas);
};

class Comprobante {
public:
    static void mostrarInventario(const Inventario& inv);
    static void mostrarAlertaStock(const Producto& prod);
    static void mostrarFactura(const Venta& venta);
    static void mostrarReporteVentas(const Inventario& inv);
};

#endif // INVENTARIO_H
