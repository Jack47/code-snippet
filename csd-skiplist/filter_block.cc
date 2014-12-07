class FilterBlockBuilder {
public:
	const int kFilterLogBase = 11; 
	const int kFilterBlockSize = 1<<kFilterLogBase;//2KB
	FilterBlockBuilder(const FilterPolicy* filterPolicy):policy_(filterPolicy),
	current_offset_(0) {

	}
	void StartBlock(uint64_t block_offset){
		assert(block_offset>current_offset_);
		if(block_offset-current_offset_>=kFilterBlockSize){			
			vector<Slice> keys;
			for(size_t i=0;i<keysInFilter_.size();i++){
				keys.push_back(Slice(keysInFilter_[i]));
			}
			string filter;
			policy_.CreateFilter(keys, keys.size(), &filter);
			filters_.push_back(filter);

			keysInFilter_.clear();
			current_offset=block_offset;
		}
	}

	void AddKey(const Slice& key){
		keysInFilter_.push_back(key.ToString());//需要自己拥有一份存储，而非用调用者的
	}
	Slice Finish(){
		string filterBlock;
		//组装filter 0 ... filter n
		for(size_t i=0;i<filters_.size();i++){
			filterBlock.append(filters[i]);
		}

		char buffer[sizeof(int32_t)];
		size_t filtersSize = filterBlock.size();

		size_t offset = 0;		
		char buffer[sizeof(int)];
		for(size_t i=0;i<filters_.size();i++){	
			offset+=filters[i].size();							
			const char* filterOffset = EncodeFixed32(offset, buffer);
			filterBlock.append(filterOffset, sizeof(int));			
		}

		const char* beginOfFilterOffset = EncodeFixed32(filtersSize, buffer);
		filterBlock.append(beginOfOffset,4);
		filterBlock.push_back(static_cast<char>kFilterLogBase);
	}

private:
	void GenerateFilter(){

	}

	const FilterPolicy* policy_;
	uint64_t current_offset;
	//我在coding的过程中，发现filters_其实可以只用一个string来存储，这样更加节省空间，免得存储多个小的string，
	//同时那就需要把每个filter的长度记录下来了
	std::vector<std::string> filters_;//存储本FilterBlock中的所有的过滤器值: filter 0, filter1 .... filter N

	//在coding过程中，也发现可以用一个string来存储，原因同上
	std::vector<std::string> keysInFilter_;//存储filter i中的所有的key
};

class FilterBlockReader {

public:
	FilterBlockReader(const FilterPolicy* policy, const Slice& contents):policy_(policy){
		assert(contents.size()>1);

		const size_t kFilterBlockBase_OFFSET = contents.size()-1;
		const size_t kFilterOffsets_END_OFFSET = kFilterBlockBase_OFFSET-sizeof(int);

		filterBlockBase_ = static_cast<int>(contents.data()[kFilterBlockBase_OFFSET]);
		filterBlockSize_ = 1<<filterBlockBase;
	
		const size_t filterOffsets_start = static_cast<size_t>(DecodeFixed32(contents.data()+kFilterOffsets_END_OFFSET));
		const size_t filterOffsets_end = kFilterOffsets_END_OFFSET;

		size_t filterBeginOffset = 0;
		for(size_t offset = filterOffsets_start;offset<filterOffsets_end; offset+=sizeof(int)){
			//read offset
			const size_t filterOffset = static_cast<size_t>(DecodeFixed32(&contents.data()+offset));
			const size_t filterSize = filterOffset-filterBeginOffset;

			//read filter
			string filter(contents.data()+filterBeginOffset, filterSize);
			filters_.push_back(filter);
			filterBeginOffset = filterOffset;
		}
	}
	bool KeyMayMatch(uint64_t block_offset, const Slice& key){
		size_t filterOffset = block_offset/filterBlockSize;
		assert(filterOffset<filters_.size());
		return policy_->KeyMayMatch(Slice(filters_[filterOffset]),key);
	}

	//联系google leveldb的思路
	void GenerateFilter(){
		vector<Slice> keys;
		size_t numOfKeys = start_.size();
		if(numOfKeys==0){
			filter_offsets_.push_back(result_.size());
			return;
		}
		start_.push_back(keys_.size());//
		for(size_t i=0;i<numOfKeys;i++){
			const size_t start = start_[i];
			const size_t length = start_[i+1]-start[i];
			keys.push_back( Slice(keys_.data()+start, length) );
		}	
		filter_offsets_.push_back(result_.size());
		policy_->CreateFilter(&keys[0], keys.size(), &result_);		
		keys_.clear();
		start_.clear();
	}
	Slice Finish(){
		size_t filter_offsets_begin = result_.size();

		for(size_t n=0;n<filter_offsets.size();n++){
			PutFixed32(&result_, filter_offsets[i]);
		}
		PutFixed32(&result_, filter_offsets_begin);
		result_.push_back(kFilterBaseLog);

	}
private:
	vector<string> filters_;
	int filterBlockBase_;
	int filterBlockSize_;
};