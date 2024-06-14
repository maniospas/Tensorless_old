#ifndef SIGNED_H
#define SIGNED_H

#include <iostream>
#include <vector>
#include <bitset>
#include <cstdlib>
#include <random>
#include "types/vecutils.cpp"


template <typename Number>
class Signed {
private:
    VECTOR isNegative;
    Number value;
    Signed(Number value, VECTOR isNegative) : value(value), isNegative(isNegative) {}
public:
    Signed<Number> zerolike() {
        return Signed();
    }

    Signed(): isNegative(0) {}

    Signed(const std::vector<double>& vec) {
        value = Number();
        for (int i = 0; i < vec.size(); ++i) 
            if (vec[i]) {
                if(vec[i]>0)
                    value.set(i, vec[i]);
                else {
                    value.set(i, -vec[i]);
                    isNegative |= ((VECTOR)1 << i);
                }
            }
        value = value.twosComplement(isNegative);
    }

    const double get(int i) {
        if((isNegative >> i) & 1)
            return -value.twosComplement(isNegative).get(i);
        return value.get(i);
    }

    const double get(int i) const {
        if((isNegative >> i) & 1)
            return -value.get(i);
        return value.get(i);
    }

    Signed<Number> set(int i, double val) {
        VECTOR applyNegative = ((VECTOR)1 << i);
        if(val<0) {
            value.set(i, -val);
            value = value.twosComplement(applyNegative);
            isNegative |= applyNegative;
        }
        else {
            isNegative &= ~applyNegative;
            value.set(i, val);
        }
        return *this;
    }

    double operator[](int i) {
        return get(i);
    }

    double operator[](int i) const {
        return get(i);
    }
    
    Signed<Number>& operator[](std::pair<int, double> p) {
        set(p.first, p.second);
        return *this;
    }

    int size() const {
        return value.size();
    }

    friend std::ostream& operator<<(std::ostream &os, const Signed<Number> &si) {
        os << "[" << si.get(0);
        for(int i=1;i<si.size();i++)
            os << "," << si.get(i);
        os << "]";
        return os;
    }

    Signed<Number> operator+(const Signed<Number> &other) const {
        VECTOR neg;
        Number result = value.addWithCarry(other.value, neg);
        neg = isNegative^other.isNegative^neg;

        // detect overflow
        VECTOR negCarry = (isNegative & other.isNegative) | (neg & (isNegative ^ other.isNegative));
        if(negCarry ^ neg)
            throw std::logic_error("arithmetic overflow");

        return Signed(result, neg);
    }

    
    Signed<Number> operator-(const Signed<Number> &other) const {
        auto negatedOther = Signed(other.value.twosComplement(other.isNegative), ~other.isNegative);
        return *this+negatedOther;
    }

    
    Signed<Number> operator*(const Signed<Number> &other) const {
    }

};


#endif  // SIGNED_H
