#pragma once



template
<
    typename capacity_type,
    capacity_type capacity,
    typename storage_type
>
struct array
{
    typedef storage_type type;
    
    storage_type data[capacity];
    
    operator storage_type * ()
    {
        return data;
    }
};



template
<
    typename capacity_type,
    capacity_type capacity,
    typename storage_type
>
inline capacity_type array_size(const array<capacity_type, capacity, storage_type> &)
{
    return capacity;
}