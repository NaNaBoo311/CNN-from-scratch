
#ifndef DATALOADER_H
#define DATALOADER_H
#include "loader/dataset.h"
#include "tensor/xtensor_lib.h"
#include "list/XArrayList.h"

using namespace std;

template <typename DType, typename LType>
class DataLoader {
 public:
  // DataLoader(Dataset<DType, LType>* ptr_dataset, int batch_size, bool shuffle = true, bool drop_last = false, int seed = -1)
  // : ptr_dataset(ptr_dataset), batch_size(batch_size), shuffle(shuffle), drop_last(drop_last), m_seed(seed) {
  //     randomInd();
  // }

  DataLoader(Dataset<DType, LType>* ptr_dataset, int batch_size, bool shuffle=true, bool drop_last=false, int seed=-1)
    : ptr_dataset(ptr_dataset), batch_size(batch_size), shuffle(shuffle), m_seed(seed){
        nbatch = ptr_dataset->len()/batch_size;
        item_indices = xt::arange(0, ptr_dataset->len());
        randomInd();
    }

  virtual ~DataLoader() {
  }

  int get_batch_size(){ return batch_size; }
  int get_sample_count(){ return ptr_dataset->len(); }
  int get_total_batch(){return int(ptr_dataset->len()/batch_size); }


  class Iterator {
  friend class DataLoader;
   public:
    Iterator(DataLoader<DType, LType>* loader, int pos)
    : loader(loader), pos(pos) {}

    Iterator& operator=(const Iterator& iterator) {
      if(this != &iterator) {
        loader = iterator.loader;
        pos = iterator.pos;
      }
      return *this;
    }

    Iterator& operator++() {
      ++pos;
      return *this;
    }

    Iterator operator++(int) {
      Iterator temp = *this;
      ++*this;
      return temp;
    }

    bool operator!=(const Iterator& other) const {
      return pos != other.pos;
    }

    Batch<DType, LType> operator*() const {
      return loader->getBatch(pos);
    }

   private:
    DataLoader<DType, LType>* loader;
    int pos;
  };

  Iterator begin() {
        return Iterator(this, 0);
  }

  Iterator end() {
    int num_batches = batch.size();
    return Iterator(this, num_batches);
  }

  int getNumBatches() {
    return batch.size();
  }

  Batch<DType, LType> getBatch(int ind) {
    if (ind < 0) {
      int num_batches = getNumBatches();
      if(ind >= num_batches || ind < 0) throw out_of_range("Index is out of range!");
    }

    return batch.get(ind);
  }

  private:
    Dataset<DType, LType>* ptr_dataset;
    int batch_size;
    bool shuffle;
    bool drop_last;
    xt::xarray<size_t> ind;
    XArrayList<Batch<DType, LType>> batch;
    bool check = false;
    int nbatch;
    ulong_tensor item_indices;
    int m_seed;

  int NumBatches(int samples) {
    int num = samples / batch_size;
    int rem = samples % batch_size;
    if(rem != 0) {
      if(!drop_last) num += 0;
    }
    return num;
  }

  xt::xarray<size_t> getIndices(int startIndex, int endIndex) const {
    return xt::view(ind, xt::range(startIndex, endIndex));
  }

  xt::xarray<DType> initializeBatchData(const xt::svector<size_t>& shape) const {
    return xt::empty<DType>(shape);
  }
  xt::xarray<LType> initializeBatchLabels(const xt::svector<size_t>& shape) const {
    return xt::empty<LType>(shape);
  }

  void fillBatchDataAndLabels(xt::xarray<DType>& batch_data, xt::xarray<LType>& batch_labels, const xt::xarray<size_t>& currInd) const {
        for (size_t j = 0; j < currInd.size(); j++) {
            DataLabel<DType, LType> currSample = ptr_dataset->getitem(currInd(j));
            xt::view(batch_data, j) = currSample.getData();
            if (!ptr_dataset->get_label_shape().empty()) {
                xt::view(batch_labels, j) = currSample.getLabel();
            }
        }
    }

  Batch<DType, LType> createBatch(int i, int num_batches, int samples) {
    int start = i * batch_size;
    int end = std::min(start + batch_size, samples);

    if(i == num_batches - 1 && !drop_last) {
      end = samples;
    }

    xt::xarray<size_t> currInd = getIndices(start, end);
    DataLabel<DType, LType> sample = ptr_dataset->getitem(currInd(0));

    auto data_shape = sample.getData().shape();
    auto label_shape = sample.getLabel().shape();

    xt::svector<size_t> shape_data;
    shape_data.push_back(end - start);
    shape_data.insert(shape_data.end(), data_shape.begin(), data_shape.end());
    xt::xarray<DType> batch_data = initializeBatchData(shape_data);

    xt::xarray<LType> batch_labels;
    if(!ptr_dataset->get_label_shape().empty()) {
      xt::svector<size_t> shape_label;
      shape_label.push_back(end - start);
      shape_label.insert(shape_label.end(), label_shape.begin(), label_shape.end());
      batch_labels = initializeBatchLabels(shape_label);
    }

    fillBatchDataAndLabels(batch_data, batch_labels, currInd);
    return Batch<DType, LType>(batch_data, batch_labels);
  }

void randomInd() {
  ind = xt::arange<size_t>(0, ptr_dataset->len());
  check = true;

  if(shuffle) {
    int m_seed = m_seed;
    if(!check) {
      throw runtime_error("Error: Indices have not been initialized!");
    }
    if(m_seed >= 0) {
      // auto rng = xt::random::default_engine_type(m_seed);
      xt::random::seed(m_seed);
    }

    // auto rng = xt::random::default_engine_type(0);
    xt::random::shuffle(ind);
  }

  int samples = ptr_dataset->len();
  int num_batches = NumBatches(samples);
  batch = XArrayList<Batch<DType, LType>>(nullptr, 0, num_batches);
  for(int i = 0; i < num_batches; i++) {
    Batch<DType, LType> currBatch = createBatch(i, num_batches, samples);
    batch.add(currBatch);
  }
}

};

#endif /* DATALOADER_H */
