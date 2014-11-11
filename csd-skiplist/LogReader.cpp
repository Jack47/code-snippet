Log::Reader::Record {

}

_file;
_last_offset;//保存_file的当前offset
bool _eof;//指示_file.Read是否到达文件尾部
Reporter* _reporter;
Reader(SequentialFile* file, Reporter* reporter, bool checksum, uint64_t initial_offset){
	assert(file!=NULL);
	_file = file;
	_reporter = reporter;
	uint64_t blocks = initial_offset/kBlockSize;
	_eof = false;

	Status s = _file.Skip(blocks*kBlockSize);//跳到initial_offset所指示的块 TODO 异常情况
	if(s.ok){//到文件末尾了也会返回OK	 那怎么保证 确实是走到了要求的 initial_offset处呢？
		_last_offset = blocks*kBlockSize;
	}
	else ｛
		_eof = true;
		_reporter->();//如何在构造函数中处理错误？		
	｝

	String block_buffer(kBlockSize);
	Slice slice;

	Record record;//string 管理内存
	while(_last_offset<initial_offset){//跳过block中initial_off前的Record
		if(!ReadRecord(&record, &block_buffer)){//TODO 处理异常  返回值是当前偏移，如果为0，说明到达文件尾部？		
			Corruption；
		}
	}
	
}
//从db_impl看到Reader.ReadRecord()之后，直接就写入记录到WriteBatch了
bool ReadRecord(Slice* record, std::string* scratch);//读出来，就放到WriteBatch里面了：
{
	//遍历BlockRecord,读取到对外完整的一个“Record”
	bool needCheckSum = true;
	
	string blockRecord;
	
	Type t;
	Status s;

	do {
		s =	ReadBlockRecord(needCheckSum, &blockRecord,&t);
		scratch->append(s);
	} while(s.ok&&!(t==FULL || t== LAST));

	if(!s.ok){
		LogCorruption("");
		return false;
	}
	else return true;
}
private:
	char _block_buffer[kBlockSize];//不用考虑缓存的问题吧
	bool _bufferEmpty = true;
	int _bufferIndex =0;
	int _bufferLength = 0;
	//从当前的_file 里读取一个调用者之前写入的Record	
Status ReadBlockRecord(bool checksum, String* record, Type* t){
	Slice blockRecord;//一个block，32字节
	Slice block;

	Status s;
	Type t;
	int remainBuferSize = (_bufferLength-_bufferIndex);
	if(_eof)
		return Status(ERROR, "end of file");

	if(_bufferEmpty || remainBufferSize<=kHeaderSize ){//从文件读取. 跳过7个或者<=7个字节的trailer
		s = _file.Read(kBlockSize,&block, _block_buffer);//平常Read都是返回读了多少。从result.size()可以得知

		if(!s.ok()){
			return Status(error, s.errmsg());	//确实有问题
		}

		if(block.size()<log::kHeaderSize){		
			_eof = true;
			return Status(Error, read bytes <kHeaderSize);
		}	
		if(block.size<kBlockSize)
			_eof = true;

		_bufferLength = block.size();
		_bufferIndex = 0;
		_bufferEmpty = false;		
	}

	char* currentBlockBuffer = _block_buffer+_bufferIndex;

	int dataLength = *((uint16*)(currentBlockBuffer+kLengthIndex));//当前这一小块中，data长度
	int blockRecordSize = dataLength+kHeaderSize;//当前这一小块的总大小

	blockRecord->data=currentBlockBuffer;
	blockRecord->size=kHeaderSize+dataLength;
	_last_offset += blockRecord->size();
	
	if(checksum){
			bool ret = crc32(result->data+sizeof(uint32), result->size()-sizeof(uint32), reinterpreter_cast<uint32*>result->data());			if(!ret){
			return Status(Error, checkSum error);
		}
	}
	*t = *((*Type)(result->data()+kTypeIndex);//CheckSumLength
	const char* start = result->data()+kDataIndex //CheckSumLength+DataLength+TypeLength;
	uint16 length = *((uint16*)(result->data()+kLengthIndex));//CheckSumLength+DataLEgnth
	record->assign(start, length);
	return Status(OK);
}

const int kCheckSumSize = 4;
const int kLengthSize = 2;
const int kTypeSize = 1;
const int kTypeIndex = kCheckSumSize+kLengthSize;
const int kLengthIndex = kCheckSumSize;
const int kDataIndex = kCheckSumSize+kLengthSize+kTypeSize;
String block_buffer(kBlockSize);
Slice slice;
file.Read(kBlockSize,&slice, block_buffer->data());