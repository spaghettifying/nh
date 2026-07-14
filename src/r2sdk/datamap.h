struct datamap_t {
    void* dataDesc;           // pointer to array of DataDesc_t
    int32_t dataFieldsCount;  // number of fields
    const char* dataClassName; // class name string
    uint64_t paddedSize;      // whatever this is (padding/size)
    datamap_t* baseMap;         // pointer to parent/base datamap
};