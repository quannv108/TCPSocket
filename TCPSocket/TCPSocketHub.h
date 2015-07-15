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

#ifndef __TCPSocketHub_h__
#define __TCPSocketHub_h__

#include "cocos2d.h"
#include "TCPSocket.h"
#include "ByteBuffer.h"
#include "Packet.h"
#include <pthread.h>
#include "EventCustomObject.h"

namespace funny {
    namespace network {
        
        
        /// object is a connected socket
#define kCCNotificationTCPSocketConnected "kCCNotificationTCPSocketConnected"
        
        /// object is disconnected socket
#define kCCNotificationTCPSocketDisconnected "kCCNotificationTCPSocketDisconnected"
        
        /// object is packet
#define kCCNotificationPacketReceived "kCCNotificationPacketReceived"
        
        /**
         * It manages a group of sockets and monitor them in every update. The update loop is started
         * after hub is created.
         */
        class CC_DLL TCPSocketHub : public cocos2d::Ref {
            friend class TCPSocket;
            
        private:
            /// pthread mutex
            pthread_mutex_t m_mutex;
            
            /// connected sockets
            cocos2d::Vector<TCPSocket *> m_connectedSockets;
            
            /// disconnected sockets
            cocos2d::Vector<TCPSocket *> m_disconnectedSockets;
            
            /// packet array
            cocos2d::Vector<Packet *> m_packets;
            
        protected:
            TCPSocketHub();
            
            /// listen on socket, read and write if necessary
            void mainLoop(float delta);
            
            /// add socket to hub
            bool addSocket(TCPSocket* socket);
            
            /// called by tcp socket when it is connected
            void onSocketConnectedThreadSafe(TCPSocket* s);
            
            /// called by tcp socket when it is disconnected
            void onSocketDisconnectedThreadSafe(TCPSocket* s);
            
            /// called when a socket want to deliver a packet
            void onPacketReceivedThreadSafe(Packet* packet);
            
        public:
            virtual ~TCPSocketHub();
            static TCPSocketHub* create();
            
            /**
             * create socket instance and auto add it to hub
             *
             * @param hostname host name or ip address, ipv4 only
             * @param port port
             * @param tag tag of socket
             * @param blockSec block time when create this socket, 0 means not block
             * @param keepAlive true means keep socket alive
             * @return instance or NULL if failed
             */
            TCPSocket* createSocket(const std::string& hostname, int port, int tag, int blockSec = kCCSocketDefaultTimeout, bool keepAlive = false);
            
            /// disconnect one socket
            void disconnect(int tag);
            
            // stop all
            void stopAll();
            
            /// get socket by tag
            TCPSocket* getSocket(int tag);
            
            /// send a packet
            void sendPacket(int tag, Packet* packet);
            
            /// socket array
            CC_SYNTHESIZE_PASS_BY_REF(cocos2d::Vector<TCPSocket *>, m_sockets, Sockets);
            
            /// raw policy means packet don't have header, just contains a piece of bytes
            /// so it will be developer's responsibility to parse the packet
            /// default value = true
            CC_SYNTHESIZE(bool, m_rawPolicy, RawPolicy);
        };
        
    }
}

#endif //__TCPSocketHub_h__