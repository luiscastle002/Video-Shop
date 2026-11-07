#ifndef MEDIO_PAGO_H
#define MEDIO_PAGO_H

#include <string>

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

#endif // MEDIO_PAGO_H
