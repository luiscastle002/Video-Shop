#ifndef CALCULO_PRECIO_H
#define CALCULO_PRECIO_H

#include <memory>

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

#endif // CALCULO_PRECIO_H
