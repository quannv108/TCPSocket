/****************************************************************************
 Author: Luma (stubma@gmail.com)
 
 https://github.com/stubma/cocos2dx-better
  @ 2015 QuanNguyen
 
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
#include "ByteBuffer.h"

USING_NS_CC;

#define DEFAULT_SIZE 0x1000

namespace funny {
    namespace network {
        
        ByteBuffer::ByteBuffer() :
        m_buffer(NULL),
        m_readPos(0),
        m_writePos(0),
        m_external(false) {
            reserve(DEFAULT_SIZE);
        }
        
        ByteBuffer::ByteBuffer(size_t res) :
        m_buffer(NULL),
        m_readPos(0),
        m_writePos(0),
        m_external(false) {
            reserve(res);
        }
        
        ByteBuffer::ByteBuffer(const ByteBuffer& b) :
        m_buffer(NULL),
        m_readPos(0),
        m_writePos(0),
        m_external(false) {
            reserve(b.m_bufferSize);
            memcpy(b.m_buffer, m_buffer, b.m_writePos);
            m_readPos = b.m_readPos;
            m_writePos = b.m_writePos;
        }
        
        ByteBuffer::ByteBuffer(const char* buf, size_t bufSize, size_t dataLen) :
        m_buffer((uint8_t*)buf),
        m_readPos(0),
        m_writePos(dataLen),
        m_external(true),
        m_bufferSize(bufSize) {
            
        }
        
        ByteBuffer::~ByteBuffer() {
            if(!m_external) {
                CC_SAFE_FREE(m_buffer);
            }
        }
        
        ByteBuffer* ByteBuffer::create() {
            ByteBuffer* b = new ByteBuffer();
            return (ByteBuffer*)b->autorelease();
        }
        
        ByteBuffer* ByteBuffer::create(size_t res) {
            ByteBuffer* b = new ByteBuffer(res);
            return (ByteBuffer*)b->autorelease();
        }
        
        void ByteBuffer::reserve(size_t res) {
            if(m_external)
                return;
            
            if(m_buffer)
                m_buffer = (uint8_t*)realloc(m_buffer, res);
            else
                m_buffer = (uint8_t*)malloc(res);
            
            m_bufferSize = res;
        }
        
        void ByteBuffer::compact() {
            if(m_readPos > 0) {
                memmove(m_buffer, m_buffer + m_readPos, available());
                m_writePos -= m_readPos;
                m_readPos = 0;
            }
        }
        
        size_t ByteBuffer::read(uint8_t * buffer, size_t len) {
            if(m_readPos + len > m_writePos)
                len = (m_writePos - m_readPos);
            
            memcpy(buffer, &m_buffer[m_readPos], len);
            m_readPos += len;
            return len;
        }
        
        void ByteBuffer::read(std::string& dest) {
            readCString(dest);
        }
        
        void ByteBuffer::readCString(std::string& dest) {
            dest.clear();
            char c;
            while(true)	{
                c = read<char>();
                if(c == 0)
                    break;
                dest += c;
            }
        }
        
        void ByteBuffer::readPascalString(std::string& dest) {
            dest.clear();
            uint16_t len = read<uint16_t>();
            while(len-- > 0) {
                dest += read<char>();
            }
        }
        
        void ByteBuffer::readLine(std::string& dest) {
            dest.clear();
            char c;
            while(m_readPos < m_bufferSize)	{
                c = read<char>();
                if(c == '\r')
                    continue;
                else if(c == 0 || c == '\n')
                    break;
                dest += c;
            }
        }
        
        void ByteBuffer::write(const uint8_t* data, size_t size) {
            size_t new_size = m_writePos + size;
            if(new_size > m_bufferSize) {
                if(m_external) {
                    CCLOGWARN("external mode: buffer size is not enough to write");
                    return;
                }
                new_size = (new_size / DEFAULT_INCREASE_SIZE + 1) * DEFAULT_INCREASE_SIZE;
                reserve(new_size);
            }
            
            memcpy(&m_buffer[m_writePos], data, size);
            m_writePos += size;
        }
        
        void ByteBuffer::write(const std::string& value) {
            writeCString(value);
        }
        
        void ByteBuffer::writeCString(const std::string& value) {
            if(m_writePos + value.length() + 1 > m_bufferSize) {
                if(m_external) {
                    CCLOGWARN("external mode: buffer size is not enough to write");
                    return;
                }
                ensureCanWrite(value.length() + 1);
            }
            
            memcpy(&m_buffer[m_writePos], value.c_str(), value.length() + 1);
            m_writePos += (value.length() + 1);
        }
        
        void ByteBuffer::writePascalString(const std::string& value) {
            if(m_writePos + value.length() + sizeof(uint16_t) > m_bufferSize) {
                if(m_external) {
                    CCLOGWARN("external mode: buffer size is not enough to write");
                    return;
                }
                ensureCanWrite(value.length() + sizeof(uint16_t));
            }
            
            write<uint16_t>(value.length());
            memcpy(&m_buffer[m_writePos], value.c_str(), value.length());
            m_writePos += value.length();
        }
        
        void ByteBuffer::writeLine(const std::string& value) {
            if(m_writePos + value.length() + 2 * sizeof(char) > m_bufferSize) {
                if(m_external) {
                    CCLOGWARN("external mode: buffer size is not enough to write");
                    return;
                }
                ensureCanWrite(value.length() + 2 * sizeof(char));
            }
            
            memcpy(&m_buffer[m_writePos], value.c_str(), value.length());
            m_writePos += value.length();
            write<char>('\r');
            write<char>('\n');
        }
        
        void ByteBuffer::skip(size_t len) {
            if(m_readPos + len > m_writePos)
                len = (m_writePos - m_readPos);
            m_readPos += len;
        }
        
        void ByteBuffer::revoke(size_t len) {
            m_readPos -= len;
            m_readPos = MAX(0, m_readPos);
        }
        
        void ByteBuffer::ensureCanWrite(size_t size) {
            size_t new_size = m_writePos + size;
            if(new_size > m_bufferSize) {
                new_size = (new_size / DEFAULT_INCREASE_SIZE + 1) * DEFAULT_INCREASE_SIZE;
                reserve(new_size);
            }
        }
        
    }
};