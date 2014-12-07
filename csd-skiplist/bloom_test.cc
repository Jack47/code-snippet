
Slice Key(int n, char* const buffer){
	EncodeInt32(n, buffer);
	return Slice(buffer, sizeof(int));
}
BloomTest{
private:
	const FilterPolicy * filterPolicy_;
	std::string filter_;
	std::vector<std::string> keys_;

public:
	BloomTest(): filterPolicy_(CreateBloomFilter(10)){
		Clear();
	}
	~BloomTest(){
		delete filterPolicy_;
	}
	void Reset(){
		filter_.clear();
		keys_.clear();
	}
	void Add(int key){
		char buffer[sizeof(int)];		
		keys_.push_back(Key(key, buffer).ToString());
	}
	void Build(){
		filter_.clear();
		std::vector<Slice> keys;
		for(size_t i=0;i<keys_.size();i++){
			keys.push_back(Slice(keys_[i]));
		}

		filterPolicy_->CreateFilter(&keys[0], keys_.size(), &filter_);
	}
	bool Match(int key){
		if(filter_.empty()){
			Build();
		}
		char buffer[sizeof(int)];
		return filterPolicy_->KeyMayMatch(Key(key, buffer), Slice(filter_));
	}
	size_t FilterSize() const{
		return filter_.size();
	}
	double FalsePositiveRate(){		
		int badFitler = 0;
		double rate;

		for(int i=0;i<=10000;i++){
			if(Match(10000+i)){
				badFitler++;
			}			
		}
		rate = badFitler/10000;
	}
	void DumpFilter(){
		fprintf(stderr, "F(");
		for(size_t i=0;i<filter_.size();i++){
			const byte = static_cast<char>(filter_[i]);
			for(int j=0;j<8;j++){
				fprintf(stderr,"%c", byte&(1<<j)?'1':'.');
			}
		}

	}

};
int NextLength(int& len){
	if(len<10){
		len+=1;
	}
	else if(len<100){
		len+=10;
	}
	else if(len<1000){
		len+=100;
	}
	else if(len<10000){
		len+=1000;
	}
	else if(len<100000){
		len+=10000;
	}
}
TEST(Bloom, VarintLength){
	BloomTest test;
	for(int length=0; length<=10000;NextLength()){
		test.Clear();
		for(int i=0;i<=10000;i++){
			test.Add(i);
		}
		for(int i=0;i<=10000;i++){
			ASSERT_TRUE(test.Match(i));
		}
		test.FitlerSize();
	}
}