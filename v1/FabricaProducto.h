#ifndef FABRICA_PRODUCTO_H
#define FABRICA_PRODUCTO_H

#include <string>
#include <memory>
#include "Producto.h"
#include "CalculoPrecio.h"

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

#endif // FABRICA_PRODUCTO_H
