#ifndef STREAM_H
#define STREAM_H

#include <cstring>
#define BUFFER_SIZE 10000

/**
 * @brief decode the response located in the buffer.
 */
class Response {
  public:
    Response(void* response_buffer) : mdata_(std::malloc(msize_)),
                                      curesult_buffer_(std::static_cast<CUresult*>(mdata_)),
                                      scalar_result_buffer_(std::reinterpret_cast<uint64_t*>(mdata_ + 1)){};
    ~Response(){ free(mdata_); };

    CUresult curesult() { return *curesult_buffer_; };
    uint64_t cuscalar() { return *scalar_result_buffer_; };
    void set_curesult(CUresult curesult) { *curesult_buffer_ = curesult; };
    void set_cuscalar(uint64_t cuscalar) { *scalar_result_buffer_ = cuscalar; };

    void* data() { return mdata_; };
    static size_t size() { return msize_; };

  private:
    CUresult *curesult_buffer_;
    uint64_t *scalar_result_buffer_;

    static size_t msize_ = sizeof(CUresult) + sizeof(uint64_t);
    void* mdata_;
};

/**
 * @class Serializer
 * @brief Prepares original cuda function information as a flattened message,
 *        which will then be transferred via vsock to the host.
 * @
 */
class Serializer {
  public:
    explicit Serializer() : mdata_(std::malloc(BUFFER_SIZE)), mcount_(0){};
    ~Serializer(){ free(mdata_); };

    Serializer& operator<<(size_t &size){
        std::memcpy(mdata_ + mcount_, &size, sizeof(size_t));
        mcount_ += sizeof(size_t);

        return *this;
    }

    Serializer& operator<<(const char* &str_content){
        size_t size = std::strlen(str_content);
        operator<<(size);

        std::memcpy(mdata_ + mcount_, str_content, size * sizeof(char));
        mcount_ += size * sizeof(char);

        std::memcpy(mdata_ + mcount_, '\0', sizeof(char));
        mcount_ += sizeof(char);

        return *this;
    }

    template <typename... Args>
    Serializer& operator<<(const std::tuple<Args...> &args){
        serialize_tuple(args, std::index_sequence_for<Args...>{});
        return *this;
    }

    template <typename T>
    Serializer& operator<<(T &content){
        size_t size = sizeof(T);
        operator<<(size);

        std::memcpy(mdata_ + mcount_, &content, size);
        mcount_ += size;

        return *this;
    }

    Serializer& operator<<(std::vector<uint64_t> &kernel_args){
        size_t args_count = kernel_args.size();
        operator<<(args_count);
        std::memcpy(mdata_ + mcount_, kernel_args.data(), args_count * sizeof(uint64_t));
        mcount_ += args_count * sizeof(uint64_t);

        return *this;
    }

    /** returns the size of the buffer that stores the flattened message. */
    size_t size() { return mcount_; };

    /** returns the starting of the buffer that stores the flattened message. */
    void* data() {return mdata_; };

    void clean() { mcount_ = 0; };

  private:
    void* mdata_;
    size_t mcount_;

    template <typename Tuple, std::size_t... I>
    void serialize_tuple(Tuple &tuple, std::index_sequence<I...>){
        (operator<<(std::get<I>(tuple)), ...);
    }
};

class Deserializer {
  public:
    explicit Deserializer() : mdata_(std::malloc(BUFFER_SIZE)), mcount_(0){};
    ~Deserializer(){ free(mdata_); };

    Deserializer& operator>>(size_t &size){
        std::memcpy(&size, mdata_ + mcount_, sizeof(size_t));
        mcount_ += sizeof(size_t);

        return *this;
    }

    //! for the server, allocate memory for func_name
    Deserializer& operator>>(char* &result){
        size_t size;
        operator>>(size);

        result = mdata_ + mcount_;
        mcount_ += (size + 1) * sizeof(char); //plus the terminator

        return *this;
    }

    template <typename... Args>
    Deserializer& operator>>(std::tuple<Args...> &args){
        deserialize_tuple(args, std::index_sequence_for<Args...>{});
        return *this;
    }

    template <typename T>
    Deserializer& operator>>(T &content){
        size_t size;
        operator>>(size);

        std::memcpy(&content, mdata_ + mcount_, size);
        mcount_ += size;

        return *this;
    }

    //! read through the doc to check if any other cuda func use this type of data.
    //! if so, have another func for getting the kernel!
    Deserializer& operator>>(std::vector<void*> &kernel_args){
        size_t size;
        operator>>(size);

        kernel_args.reserve(size);
        uint64_t* kernel_start = std::static_cast<uint64_t*>(mdata_ + mcount_);
        for(int i = 0; i < size; i++){
            kernel_args[i] = std::static_cast<void*>(kernel_start + i);
        }
        mcount_ += size * sizeof(uint64_t);

        return *this;
    }

    void clean() { mcount_ = 0; };
    size_t size() { return BUFFER_SIZE; };
  private:
    void* mdata_;
    size_t mcount_;

    template <typename Tuple, size_t... I>
    void deserialize_tuple(Tuple &tuple, std::index_sequence<I...>){
        (operator>>(std::get<I>(tuple)), ...);
    }
};

#endif