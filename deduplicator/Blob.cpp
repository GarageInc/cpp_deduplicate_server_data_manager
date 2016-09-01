#include "Blob.h"

uint32_t blob_id_counter = 0;

Blob::Blob(void)
{
	init();
}

Blob::~Blob(void)
{
	delete[] file_name;

	fclose( pFile);

	printf("\nDeleted blob: %d", id);
}

void Blob::init() {

	file_name = new  char[max_file_name_length]();
	srand(time(0));

	char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	int string_length = sizeof(alphanum) - 1;

	for (unsigned int i = 0; i < max_file_name_length - 1; ++i)
	{
		file_name[i] = alphanum[rand() % string_length];
	}

	id = ++blob_id_counter;

	file_name[max_file_name_length - 1] = '\0';

	pFile = fopen(file_name, "a+");
}

uint32_t Blob::get_block_data(uint64_t id_block, uint64_t block_size, byte *data) {

	try {

		Block *block = NULL;

		for (int i = 0; i < blocks.size(); i++) {
			if (blocks[i]->id == id_block) {
				block = blocks[i];
				break;
			}
		}

		if (block != NULL) {
			
			fseek(pFile, block->offset, SEEK_SET);
			
			fread(data, 1, block_size, pFile);

			return 0;
		}
		else {

			return 1;
		}
		
	}
	catch (...) {
	
		return 1;
	}
}

uint32_t Blob::save_block_data(uint64_t block_id, const byte * block_data, uint64_t block_size) {

	try {

		uint64_t offset = block_size * blocks.size();

		fseek(pFile, offset, SEEK_SET);

		fwrite(block_data, sizeof(byte), sizeof(block_data), pFile);

		fflush(pFile);
		
		Block *block = new Block(block_id, offset);

		blocks.push_back(block);
		
		return block->id;

	} catch( ...){
	
		return 0;
	}

}

uint32_t Blob::get_blocks_count() {
	return blocks.size();
}