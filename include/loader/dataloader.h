/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/*
 * File:   dataloader.h
 * Author: ltsach
 *
 * Created on September 2, 2024, 4:01 PM
 */

#ifndef DATALOADER_H
#define DATALOADER_H
#include "loader/dataset.h"
#include "tensor/xtensor_lib.h"

using namespace std;

template <typename DType, typename LType>
class DataLoader
{
public:
    class Iterator; //forward declare
private:
    Dataset<DType, LType> *ptr_dataset;
    int batch_size;
    bool shuffle;
    bool drop_last;
    int m_seed;
    ulong_tensor item_indices;

    /*TODO: add more member variables to support the iteration*/
    xt::xarray<unsigned long> array_index;
    int data_len = 0;
    int num_batch = 0;
    int num_remain_data = 0;
    bool no_label = false;



public:

    //New method: from V2: begin
    int get_batch_size(){ return batch_size; }
    int get_sample_count(){ return ptr_dataset->len(); }
    int get_total_batch(){return int(ptr_dataset->len()/batch_size); }


    DataLoader(Dataset<DType, LType> *ptr_dataset,
               int batch_size,
               bool shuffle = true,
               bool drop_last = false,
               int seed = -1) : shuffle(shuffle), drop_last(drop_last), ptr_dataset(ptr_dataset)
    {
        /*TODO: Add your code to do the initialization */
        this->m_seed = seed;
        data_len = ptr_dataset->len();

        if (ptr_dataset->get_label_shape()[0] != ptr_dataset->get_data_shape()[0])
        {
            
            no_label = true;
        }

        if (drop_last == true && data_len < batch_size)
        {
            num_batch = 0;
        }

        else
        {

            // Shuffle array_index
            array_index = xt::arange<int>(ptr_dataset->len());
            if (shuffle == true && this->m_seed >= 0)
            {   
                xt::random::seed(this->m_seed);
                xt::random::shuffle(array_index);
            }

            else if (shuffle == true && this->m_seed < 0) {
                xt::random::shuffle(array_index);
            }


            this->batch_size = batch_size;
            // if (data_len < batch_size && data_len > 0)
            // {
            //     num_batch = 1;
            //     num_remain_data = 0;
            //     this->batch_size = data_len;
            // }
            // else
            // {
            if (batch_size > 0)
            {

                num_batch = data_len / batch_size;
                num_remain_data = data_len % batch_size;
            }
            // }
        }
    }
    virtual ~DataLoader() {
    }

    /////////////////////////////////////////////////////////////////////////
    // The section for supporting the iteration and for-each to DataLoader //
    /// START: Section                                                     //
    /////////////////////////////////////////////////////////////////////////

    /*TODO: Add your code here to support iteration on batch*/
    Iterator begin()
    {
        return Iterator(this, true);
    }

    Iterator end()
    {
        return Iterator(this, false);
    }
    class Iterator
    {
    public:
        // TODO implement contructor
        Iterator(DataLoader *data_loader, bool begin = true)
        {
            if (begin)
            {
                batch_index = 0;
            }

            else
            {
                batch_index = data_loader->num_batch;
            }
            this->data_loader = data_loader;
        }

        Iterator &operator=(const Iterator &iterator)
        {
            // TODO implement
        }

        Iterator &operator++()
        {
            // TODO implement
            batch_index++;
            return *this;
        }

        Iterator operator++(int)
        {
            // TODO implement
            Iterator iterator = *this;
            ++*this;
            return iterator;
        }

        bool operator!=(const Iterator &other) const
        {
            // TODO implement
            return this->batch_index != other.batch_index;
        }

        Batch<DType, LType> operator*() const
        {
            // TODO implement
            int start_index = batch_index * data_loader->batch_size;
            int end_index = start_index + data_loader->batch_size - 1;

            xt::xarray<DType> my_data = expand_dims(data_loader->ptr_dataset->getitem(data_loader->array_index[start_index]).getData(), 0);
            xt::xarray<LType> my_label = expand_dims(data_loader->ptr_dataset->getitem(data_loader->array_index[start_index]).getLabel(), 0);

            for (int i = start_index + 1; i <= end_index; i++)
            {
                xt::xarray<DType> temp_data = xt::concatenate(xt::xtuple(my_data, expand_dims(data_loader->ptr_dataset->getitem(data_loader->array_index[i]).getData(), 0)));
                xt::xarray<LType> temp_label;

                temp_label = xt::concatenate(xt::xtuple(my_label, expand_dims(data_loader->ptr_dataset->getitem(data_loader->array_index[i]).getLabel(), 0)));

                my_data = temp_data;
                my_label = temp_label;
            }

            if (batch_index == data_loader->num_batch - 1)
            {
                if (data_loader->drop_last == false)
                {
                    for (int i = end_index + 1; i <= end_index + data_loader->num_remain_data; i++)
                    {
                        xt::xarray<DType> temp_data = xt::concatenate(xt::xtuple(my_data, expand_dims(data_loader->ptr_dataset->getitem(data_loader->array_index[i]).getData(), 0)));
                        xt::xarray<LType> temp_label;
                        temp_label = xt::concatenate(xt::xtuple(my_label, expand_dims(data_loader->ptr_dataset->getitem(data_loader->array_index[i]).getLabel(), 0)));

                        my_data = temp_data;
                        my_label = temp_label;
                    }
                }
            }

            if (data_loader->no_label == true)
            {
                xt::xarray<LType> empty_arr;
                my_label = empty_arr;
            }

            return Batch<DType, LType>(my_data, my_label);
        }

    private:
        // TODO implement
        int batch_index;
        DataLoader *data_loader;
    };

    /////////////////////////////////////////////////////////////////////////
    // The section for supporting the iteration and for-each to DataLoader //
    /// END: Section                                                       //
    /////////////////////////////////////////////////////////////////////////
};

#endif /* DATALOADER_H */
