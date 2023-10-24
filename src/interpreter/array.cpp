#include "pch.h"

#include "interpreter/array.h"

using namespace Interpreter;

ArrayDimension::ArrayDimension(int n, int_t lowerBound, int_t upperBound)
    : n(n), lowerBound(lowerBound), upperBound(upperBound)
{}

int_t ArrayDimension::getSize() const {
    return (upperBound - lowerBound) + 1;
}

bool ArrayDimension::isValidIndex(int_t idx) const {
    return idx >= lowerBound && idx <= upperBound;
}

bool Interpreter::operator==(const ArrayDimension &ad1, const ArrayDimension &ad2) {
    return ad1.n == ad2.n && ad1.lowerBound == ad2.lowerBound && ad1.upperBound == ad2.upperBound;
}


Array::Array(const std::string &name, DataType type, const std::vector<ArrayDimension> &dimensions)
    : DataHolder(name), type(type), dimensions(dimensions)
{}

Array::Array(const Array &other)
    : DataHolder(other.name),
    type(other.type),
    dimensions(Array::copyDimensions(other.dimensions))
{}

void Array::copyData(const Array &other) {
    for (size_t i = 0; i < data.size(); i++) {
        data[i] = std::make_unique<Interpreter::Variable>(*(other.data[i]), other.data[i]->parent);
    }
}

const std::vector<ArrayDimension> Array::copyDimensions(const std::vector<ArrayDimension> &source) {
    std::vector<ArrayDimension> copy;
    size_t size = source.size();
    copy.reserve(size);
    for (size_t i = 0; i < size; i++) {
        copy.emplace_back(source[i]);
    }
    return copy;
}

void Array::init(Context &ctx) {
    unsigned long size = 1;
    for (auto &dim : dimensions) {
        size *= dim.getSize();
    }

    data.reserve(size);

    size_t capacity = data.capacity();
    for (size_t i = 0; i < capacity; i++) {
        data.emplace_back(new Variable(name, type, false, &ctx));
    }
}

Variable &Array::getElement(const std::vector<int_t> &index) {
    if (index.size() != dimensions.size()) std::abort();

    size_t realIndex = 0;
    size_t prevSize = 1;

    for (size_t i = 0; i < index.size(); i++) {
        realIndex += (index[i] - dimensions[i].lowerBound) * prevSize;
        prevSize *= dimensions[i].getSize();
    }

    return *(data[realIndex]);
}
