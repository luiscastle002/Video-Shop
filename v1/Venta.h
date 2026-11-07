#ifndef VENTA_H
#define VENTA_H

#include <string>
#include <vector>
#include "Producto.h"
#include "MedioPago.h"

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

    void agregarItem(const Producto& prod, int cantidad);
    int cantidadEnCarrito(const std::string& codigo) const;
    bool estaVacia() const { return items_.empty(); }
    void liquidarConMedioPago(const MedioPago& medioPago);

    int idVenta() const { return idVenta_; }
    const std::vector<ItemVenta>& items() const { return items_; }
    double subtotalConImpuestos() const { return subtotalConImpuestos_; }
    double totalFinal() const { return totalFinal_; }
    const std::string& nombreMedioPago() const { return nombreMedioPago_; }
    const std::string& etiquetaAjuste() const { return etiquetaAjuste_; }
    bool tieneAjustePago() const { return tieneAjustePago_; }
    int totalArticulos() const;
};

#endif // VENTA_H
