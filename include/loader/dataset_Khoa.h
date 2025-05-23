  #ifndef DATASET_H
  #define DATASET_H
  #include "tensor/xtensor_lib.h"

  using namespace std;

  template <typename DType, typename LType>
  class DataLabel {
  private:
    xt::xarray<DType> data;
    xt::xarray<LType> label;

  public:
    DataLabel(xt::xarray<DType> data, xt::xarray<LType> label)
        : data(data), label(label) {}
    xt::xarray<DType> getData() const { return data; }
    xt::xarray<LType> getLabel() const { return label; }
  };

  template <typename DType, typename LType>
  class Batch {
  private:
    xt::xarray<DType> data;
    xt::xarray<LType> label;
  public:
    Batch() {}
    Batch(xt::xarray<DType> data, xt::xarray<LType> label)
        : data(data), label(label) {}
    virtual ~Batch() {}
    bool operator==(const Batch<DType, LType> &other) const {
      if(data == other.data && label == other.label) {
        return true;
      }
      return false;
    }

    friend std::ostream& operator<<(std::ostream& os, const Batch<DType, LType>& batch) {
        os << "Data: " << batch.data << endl;
        os << "Label: " << batch.label << endl;
        return os;
    }

    xt::xarray<DType>& getData() { return data; }
    xt::xarray<LType>& getLabel() { return label; }
  };

  template <typename DType, typename LType>
  class Dataset {
  private:
  public:
    Dataset() {};
    virtual ~Dataset() {};

    virtual int len() = 0;
    virtual DataLabel<DType, LType> getitem(int index) = 0;
    virtual xt::svector<unsigned long> get_data_shape() = 0;
    virtual xt::svector<unsigned long> get_label_shape() = 0;
  };

  //////////////////////////////////////////////////////////////////////
  template <typename DType, typename LType>
  class TensorDataset : public Dataset<DType, LType> {
  private:
    xt::xarray<DType> data;
    xt::xarray<LType> label;
    xt::svector<unsigned long> data_shape, label_shape;

  public:
    TensorDataset(xt::xarray<DType> data, xt::xarray<LType> label) : data(data), label(label) {
        data_shape = data.shape();
        label_shape = label.shape();
    }

    int len() {
      return data.shape(0);
    }

    xt::xarray<DType> convertData(int ind) const { return xt::view(data, ind); }

    xt::xarray<LType> convertLabel(int ind) const { 
      if (label.dimension() == 0) {
        return label;
      }
      return xt::view(label, ind); 
    }

    DataLabel<DType, LType> getitem(int index) {
      if(index < 0 || index >= data.shape(0)) throw std::out_of_range("Index is out of range!");
      //get the data item at index
      xt::xarray<DType> data_item = convertData(index);
      xt::xarray<LType> label_item = convertLabel(index);
      //return the DataLabel object
      return DataLabel<DType, LType>(data_item,label_item);
    }

    xt::svector<unsigned long> get_data_shape() {
      return data_shape;
  }

    xt::svector<unsigned long> get_label_shape() {
      return label_shape;
    }  
    
  };

  #endif /* DATASET_H */
