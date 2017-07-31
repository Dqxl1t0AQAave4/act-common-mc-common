#pragma once



template
<
    typename _capacity_type,
    _capacity_type capacity,
    typename _storage_type
>
struct array
{

    typedef _storage_type  storage_type;
    typedef _capacity_type capacity_type;
    
    storage_type data[capacity];
    
    operator storage_type * ()
    {
        return data;
    }
};



template
<
    typename _capacity_type,
    typename _storage_type
>
struct size_aware_pointer
{

    typedef _storage_type  storage_type;
    typedef _capacity_type capacity_type;
    
    storage_type  * data;
    capacity_type   capacity;
    
    size_aware_pointer
    (
        storage_type  * data     = 0,
        capacity_type   capacity = 0
    )
    : data(data)
    , capacity(capacity)
    {
    }
    
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



template
<
    typename capacity_type,
    typename storage_type
>
inline capacity_type array_size(const size_aware_pointer<capacity_type, storage_type> & arr)
{
    return arr.capacity;
}