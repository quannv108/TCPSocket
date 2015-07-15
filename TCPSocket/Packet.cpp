/****************************************************************************
 Author: Luma (stubma@gmail.com)
 
 https://github.com/stubma/cocos2dx-better
 @2015 QuanNguyen
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "Packet.h"
#include "ByteBuffer.h"
#include "JSONUtils.h"

USING_NS_CC;

namespace funny {
    namespace network {
        
        
        Packet::Packet() :
        m_buffer(NULL),
        m_packetLength(0),
        m_raw(false) {
            memset(&m_header, 0, sizeof(Header));
        }
        
        Packet::~Packet() {
            CC_SAFE_FREE(m_buffer);
        }
        
        bool Packet::initWithJson(const std::string& magic, int command, const cocos2d::Value& json, int protocolVersion, int serverVersion, int algorithm) {
            // check magic
            if(magic.length() < 4)
                return false;
            
            // magic
            m_header.magic[0] = magic.at(0);
            m_header.magic[1] = magic.at(1);
            m_header.magic[2] = magic.at(2);
            m_header.magic[3] = magic.at(3);
            
            // protocol version
            m_header.protocolVersion = protocolVersion;
            
            // server version
            m_header.serverVersion = serverVersion;
            
            // command id
            m_header.command = command;
            
            // no encrypt
            m_header.encryptAlgorithm = algorithm;
            
            // body
            std::string bodyStr = JSONUtils::getInstance()->JSONStringFromValue(json);
            char* plain = (char*)bodyStr.c_str();
            
            m_header.length = (int)bodyStr.length();
            allocate(bodyStr.length() + kPacketHeaderLength + 1);
            memcpy(m_buffer + kPacketHeaderLength, plain, bodyStr.length());
            m_packetLength = bodyStr.length() + kPacketHeaderLength;
            
            // write header
            writeHeader();
            
            // init
            m_raw = false;
            
            return true;
        }
        
        bool Packet::initWithStandardBuf(const char* buf, size_t len) {
            // quick check
            if(len < kPacketHeaderLength) {
                return false;
            }
            
            // header
            ByteBuffer bb(buf, len, len);
            m_header.magic[0] = bb.read<char>();
            m_header.magic[1] = bb.read<char>();
            m_header.magic[2] = bb.read<char>();
            m_header.magic[3] = bb.read<char>();
            m_header.protocolVersion = (int32_t)(bb.read<int>());
            m_header.serverVersion = (int32_t)(bb.read<int>());
            m_header.command = (int32_t)(bb.read<int>());
            m_header.encryptAlgorithm = (int32_t)(bb.read<int>());
            m_header.length = (int32_t)(bb.read<int>());
            
            // body
            if(bb.available() >= m_header.length) {
                allocate(m_header.length + kPacketHeaderLength + 1);
                bb.read((uint8_t*)m_buffer + kPacketHeaderLength, m_header.length);
            } else {
                return false;
            }
            
            // init other
            m_raw = false;
            m_packetLength = m_header.length + kPacketHeaderLength;
            
            // write header
            writeHeader();
            
            return true;
        }
        
        bool Packet::initWithRawBuf(const char* buf, size_t len, int algorithm) {
            
            m_header.length = (int)len;
            allocate(len + 1);
            memcpy(m_buffer, buf, len);
            
            // other
            m_raw = true;
            m_packetLength = m_header.length;
            
            return true;
        }
        
        void Packet::writeHeader() {
            ByteBuffer bb(m_buffer, m_packetLength, 0);
            
            // magic
            bb.write<char>(m_header.magic[0]);
            bb.write<char>(m_header.magic[1]);
            bb.write<char>(m_header.magic[2]);
            bb.write<char>(m_header.magic[3]);
            
            // protocol version
            bb.write<int>(int32_t(m_header.protocolVersion));
            
            // server version
            bb.write<int>(int32_t(m_header.serverVersion));
            
            // command id
            bb.write<int>(int32_t(m_header.command));
            
            // no encrypt
            bb.write<int>(int32_t(m_header.encryptAlgorithm));
            
            // body length
            bb.write<int>(int32_t(m_header.length));
        }
        
        const char* Packet::getBody() {
            if(m_raw)
                return m_buffer;
            else
                return m_buffer + kPacketHeaderLength;
        }
        
        void Packet::allocate(size_t len) {
            if(!m_buffer)
                m_buffer = (char*)calloc(len, sizeof(char));
        }
    }
}
