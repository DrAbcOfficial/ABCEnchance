template<typename T>
inline T mem_get_le16(const void* vmem) {
	T val;
	const unsigned char* mem = (const unsigned char*)vmem;
	val = mem[1] << 8;
	val |= mem[0];
	return val;
}

template<typename T>
inline T mem_get_le32(const void* vmem) {
	T val;
	const unsigned char* mem = (const unsigned char*)vmem;
	val = ((T)mem[3]) << 24;
	val |= mem[2] << 16;
	val |= mem[1] << 8;
	val |= mem[0];
	return val;
}