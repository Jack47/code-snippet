namespace leveldb{

static uint32_t BloomHash(const Slice& key){
	return hash(key.data(), key.size(), 0xbc9f1d34);

}

class BloomFilterPolicy : public FilterPolicy {

public:
	//bits_per_key取10，是最优值
	explicit BloomFilterPolicy(int bits_per_key): bits_per_key_(bits_per_key){
		k_ = static_cast<size_t>(0.69*bits_per_key_);
		if(k_<1) k_ = 1;
		else if(k_>30) k_=30;
	}
	//计算出的BloomFilter值，是追加到dst里面的
	virtual void CreateFilter(const Slice* keys, int n, std::string* dst){
		size_t bits = n*bits_per_key_;
		if(bits<64) bits = 64;//如果n太小，误判率很高。这个值是怎么定的呢？

		size_t bytes = (bits+7)/8;//为了对齐，说明bits肯定是8的整数倍
		bits = bytes*8;

		const size_t init_size = dst->size();
		dst->resize(init_size+bytes, 0);//必须是零，表示初始的时候，没有bit被占用
		dst->push_back(static_cast<char>(k_));
		char* array = &((*dst)[init_size]);
		for(size_t i=0;i<n;i++){
			uint32_t h = BloomHash(keys[i]);
			const uint32_t delta = (h>>17 || h<<15);
			for(size_t j =0;j<k_;j++){
				const size_t bitpos = h%bits;
				array[bitpos/8] |= (1<<(bitpos%8));
				h+=delta;
			}
		}
	}
	//这里没有跟CreateFilter一样传string,而是Slice& bloom_filter
	virtual bool KeyMayMatch(const Slice& key, const Slice& bloom_filter){
		const size_t len = bloom_filter.size();
		const size_t bytes = len-1;//去掉结尾的一个代表k值的byte
		const char* array = bloom_filter.data();
		const size_t bits = bytes*8;

		k_ = static_cast<size_t>(array[bytes-1]);		

		bool match = true;
		const uint32_t hash = BloomHash(key);
		const uint32_t delta = (hash>>17 || hash<<15);

		for(size_t i=0;i<k_;i++){			
			const uint32_t bitpos = hash%bits;
			const unsigned char bitValue = (1<<(bitpos%8));
			if( (array[bitpos/8]&bitValue)==0){
				match = false;
				break;
			}
			hash+=delta;
		}

		return match;
	}
private:
	size_t bits_per_key_; //用size_t比uint32_t要好。size_t表示内存中的偏移，肯定是>=0的
	size_t k_; //

} //endof BloomFilterPolicy

} //endof namespace leveldb