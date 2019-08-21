#include "Value.hpp"

Value::Value() 
{
    svalue = "";
    ivalue = 0;
    rvalue = 0.0;
    bvalue = false;
    m_type = vt_null;
}

Value::Value(std::string s)
{
    svalue = s;
    ivalue = 0;
    rvalue = 0.0;
    bvalue = false;
    m_type = vt_string;
}

Value::Value(int i) 
{
    svalue = "";
    ivalue = i;
    rvalue = 0.0;
    bvalue = false;
    m_type = vt_integer;
}

Value::Value(float f)
{
    double fractpart, intpart;
    fractpart = modf(f, &intpart);
    if (fractpart == 0)
    {
        svalue = "";
        ivalue = int(intpart);
        rvalue = 0.0;
        bvalue = false;
        m_type = vt_integer;
    } else 
    {
        svalue = "";
        ivalue = 0;
        rvalue = f;
        bvalue = false;
        m_type = vt_real;
    }
}

Value::Value(double f) : Value(float(f)) {};

Value::Value(bool b)
{
    svalue = "";
    ivalue = 0;
    rvalue = 0.0;
    bvalue = b;
    m_type = vt_bool;
}

bool Value::equals(const Value &v)
{
    if (isNull() || v.isNull()) return false;
    else if (isNumeric() && v.isNumeric()) return real() == v.real();
    else if (isString() && v.isString()) return string() == v.string();
    else return boolean() == v.boolean();
 }

bool Value::isGreaterThan(const Value &v)
{
    if (isNumeric() && v.isNumeric()) return real() > v.real();
    else if (isString() && v.isString()) return string() > v.string();
    else return false;
}

bool Value::isLessThan(const Value &v)
{
    if (isNumeric() && v.isNumeric()) return real() < v.real();
    else if (isString() && v.isString()) return string() < v.string();
    else return false;
}

string trimTrailingZeroes(float n)
{
    std::string result = to_string(n);
    char c = result.back();
    while (c == '0')
    {
        result.pop_back();
        c = result.back();
    }
    return result; 
}

string Value::string() const
{
    switch (m_type) {
        case vt_string:
            return svalue;
        case vt_integer:
            return to_string(ivalue);
        case vt_real:
            return trimTrailingZeroes(rvalue);
        case vt_bool:
            return (bvalue ? "1" : "0");
        default:
            return "";
    }
}

bool Value::boolean() const
{
    switch (m_type) {
        case vt_string:
            return (svalue != "");
        case vt_integer:
            return (ivalue != 0);
        case vt_real:
            return (rvalue != 0.0);
        case vt_bool:
            return bvalue;
        default:
            return false;
    }
}

int Value::integer() const
{
    switch (m_type) {
        case vt_integer:
            return ivalue;
        case vt_real:
            return int(rvalue);
        case vt_bool:
            return (bvalue ? 1 : 0);
        default:
            return 0;
    }
}

float Value::real() const
{
    switch (m_type) {
        case vt_integer:
            return float(ivalue);
        case vt_real:
            return rvalue;
        case vt_bool:
            return (bvalue ? 1.0 : 0.0);
        default:
            return 0.0;
    }
}
