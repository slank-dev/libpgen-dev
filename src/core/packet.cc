


#include <assert.h>
#include <pgen2/data_container.h>
#include <pgen2/core/packet.h>
#include <pgen2/util.h>



namespace pgen {


void packet::set_contents(const void* buffer, size_t buflen) {
    _raw.set_content(buffer, buflen);       
}
const uint8_t* packet::contents() const {
    assert(_raw.size() >= pgen::packet::max_header_len);
    return _raw.data() + pgen::packet::max_header_len;  
}


const uint8_t* packet::raw() const {
    size_t headerlen = header_length();
    assert(headerlen <= pgen::packet::max_header_len);
    return contents() - headerlen;  
}


size_t packet::length() const {
    assert(_raw.size() >= pgen::packet::max_header_len);
    return _raw.size() - pgen::packet::max_header_len + header_length();
}


packet_type packet::type() const {
    return _type;   
}
void packet::hex() const {
    pgen::hex(raw(), length());
}



} /* namespace pgen */
