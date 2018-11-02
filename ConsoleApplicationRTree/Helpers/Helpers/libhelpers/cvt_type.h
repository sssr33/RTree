#pragma once
#include "TypeConverterCP.h"

template<class D, class S>
static D cvt_type(const S &v) {
    return TypeConverter<D, S>::Convert(v);
}