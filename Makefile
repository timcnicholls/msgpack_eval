all: variant_map 

variant_map: variant_map.cpp
	g++ -o variant_map -DMSGPACK_USE_BOOST variant_map.cpp