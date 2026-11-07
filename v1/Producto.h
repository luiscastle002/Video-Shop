#ifndef PRODUCTO_H
#define PRODUCTO_H

#include <string>
#include <memory>
#include "CalculoPrecio.h"

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

#endif // PRODUCTO_H
