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
#ifndef __Packet__
#define __Packet__

#include "cocos2d.h"

#define kPacketHeaderLength 24

namespace funny {
    namespace network {
        
        /**
         * A general packet definition, it will have a header. However, it provides
         * methods to create a raw packet which has no header.
         */
        class CC_DLL Packet : public cocos2d::Ref {
        public:
            typedef struct {
                char magic[4];
                int protocolVersion;
                int serverVersion;
                int command;
                int encryptAlgorithm;
                int length; // data length after header
            } Header;
            
        public:
            Packet();
            
            /**
             * Override autorelease method to avoid developers to call it.
             * If this function was called, it would trigger assert in debug mode
             *
             * @return Ref* always return nullptr.
             */
            Ref* autorelease(void)
            {
                CCASSERT(false, "Packet is used between network thread and ui thread \
                         therefore, autorelease is forbidden here");
                return NULL;
            }
            
        public:
            virtual bool initWithStandardBuf(const char* buf, size_t len);
            virtual bool initWithRawBuf(const char* buf, size_t len, int algorithm=-1);
            virtual bool initWithJson(const std::string& magic, int command, const cocos2d::Value& json, int protocolVersion, int serverVersion, int algorithm=-1);
            
        protected:
            // allocate buffer
            void allocate(size_t len);
            
            // write header
            void writeHeader();
            
        public:
            virtual ~Packet();
            
            // body length
            int getBodyLength() { return m_header.length; }
            
            // get body pointer
            const char* getBody();
            
            CC_SYNTHESIZE_PASS_BY_REF(Header, m_header, Header);
            CC_SYNTHESIZE(char*, m_buffer, Buffer);
            CC_SYNTHESIZE_READONLY(size_t, m_packetLength, PacketLength);
            CC_SYNTHESIZE_READONLY(bool, m_raw, Raw);
        };
    }
}

#endif /* defined(__Packet__) */
