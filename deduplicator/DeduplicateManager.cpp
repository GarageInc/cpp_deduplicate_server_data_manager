#include "DeduplicateManager.h"


DeduplicateManager::DeduplicateManager(void)
{
}

DeduplicateManager::~DeduplicateManager(void)
{
}

int DeduplicateManager::init(uint64_t i_block_size, uint32_t i_blob_size) {
	block_size = i_block_size;
	blob_size = i_blob_size;

	return 0;
}

int DeduplicateManager::put_block(uint64_t block_id, const byte* block_data) {
	
	uint64_t data_hash = hash(block_data);

	auto mapped_block_for_data = blocks_hash_map.find(data_hash);
	auto mapped_id_for_block = blocks_ids_map.find(block_id);

	// can't rewraite added block!
	if (mapped_id_for_block != blocks_ids_map.end()) {
		return 1;
	}// pass


	if (mapped_block_for_data != blocks_hash_map.end()) {
		
		blocks_ids_map[block_id] = mapped_block_for_data->second;
		return 0;// success;
	}
	else {

		uint64_t saved_block_id;
		Blob* saved_blob = NULL;
		
		for ( auto blob : blobs) {
			
			auto count = blob->get_blocks_count();

			if (blob->get_blocks_count() < blob_size) {

				saved_block_id = blob->save_block_data(block_id, block_data, block_size);

				if (saved_block_id != 0) {

					saved_blob = blob;
					break;
				} // pass
			}// pass
		}

		if (saved_blob != NULL) {

			blocks_hash_map[data_hash] = saved_blob;
		} else {

			Blob *new_blob = new Blob();

			blobs.push_back(new_blob);

			blocks_hash_map[data_hash] = new_blob;

			saved_block_id = new_blob->save_block_data(block_id, block_data, block_size);

			saved_blob = new_blob;
		}

		blocks_ids_map[saved_block_id] = saved_blob;

		return 0;
	}

	return 2;
}

int  DeduplicateManager::get_block( uint64_t block_id, byte* block_data) {

	auto data = blocks_ids_map.find( block_id);

	if (data != blocks_ids_map.end()) {

		auto blob_in_map = data->second;

		for (auto blob : blobs) {
			
			if (blob->id == blob_in_map->id) {

				blob->get_block_data(block_id, block_size, block_data);

				return 0;
			}
		}

		return 1;
	}
	else {
		return 2;
	}
}

// this algorithm (k=33) was first reported by dan bernstein many years ago in comp.lang.c. 
// another version of this algorithm (now favored by bernstein) uses xor: hash(i) = hash(i - 1) * 33 ^ str[i];
// the magic of number 33 (why it works better than many other constants, prime or not) has never been adequately explained.
uint64_t DeduplicateManager::hash(const byte *block_data) {
	
	uint64_t hash = 5381;
	
	int c;

	while (c = *block_data++) {
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}

	return hash;
}