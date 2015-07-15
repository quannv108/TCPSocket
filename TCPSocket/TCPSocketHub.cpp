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

#include "TCPSocketHub.h"

USING_NS_CC;

namespace funny {
    namespace network {
        
        
        TCPSocketHub::TCPSocketHub() :
        m_rawPolicy(true) {
            pthread_mutex_init(&m_mutex, NULL);
            
            // start main loop
            auto s = Director::getInstance()->getScheduler();
            s->schedule(schedule_selector(TCPSocketHub::mainLoop), this, 0, false);
        }
        
        TCPSocketHub::~TCPSocketHub() {
            // release
            pthread_mutex_destroy(&m_mutex);
        }
        
        TCPSocketHub* TCPSocketHub::create() {
            TCPSocketHub* h = new TCPSocketHub();
            return (TCPSocketHub*)h->autorelease();
        }
        
        void TCPSocketHub::stopAll() {
            // release socket
            for(auto s : m_sockets){
                if(s->getConnected()) {
                    s->m_connected = false;
                    s->closeSocket();
                    
                    auto nc = Director::getInstance()->getEventDispatcher();
                    
                    EventCustomObject *e = new EventCustomObject(kCCNotificationTCPSocketDisconnected, s);
                    nc->dispatchEvent(e);
                    e->release();
                }
                
                s->setStop(true);
                s->setHub(NULL);
            }
            m_sockets.clear();
            
            // stop update
            auto s = Director::getInstance()->getScheduler();
            s->unschedule(schedule_selector(TCPSocketHub::mainLoop), this);
        }
        
        TCPSocket* TCPSocketHub::createSocket(const std::string& hostname, int port, int tag, int blockSec, bool keepAlive) {
            TCPSocket* s = TCPSocket::create(hostname, port, tag, blockSec, keepAlive);
            if(s){
                addSocket(s);
            }
            return s;
        }
        
        void TCPSocketHub::onSocketConnectedThreadSafe(TCPSocket* s) {
            pthread_mutex_lock(&m_mutex);
            m_connectedSockets.pushBack(s);
            pthread_mutex_unlock(&m_mutex);
        }
        
        void TCPSocketHub::onSocketDisconnectedThreadSafe(TCPSocket* s) {
            pthread_mutex_lock(&m_mutex);
            m_disconnectedSockets.pushBack(s);
            pthread_mutex_unlock(&m_mutex);
        }
        
        void TCPSocketHub::onPacketReceivedThreadSafe(Packet* packet) {
            pthread_mutex_lock(&m_mutex);
            m_packets.pushBack(packet);
            pthread_mutex_unlock(&m_mutex);
        }
        
        bool TCPSocketHub::addSocket(TCPSocket* socket) {
            for(auto s : m_sockets) {
                if(s->getSocket() == socket->getSocket()) {
                    return false;
                }
            }
            m_sockets.pushBack(socket);
            socket->setHub(this);
            return true;
        }
        
        TCPSocket* TCPSocketHub::getSocket(int tag) {
            for(auto s : m_sockets){
                if(s->getTag() == tag) {
                    return s;
                }
            }
            return NULL;
        }
        
        void TCPSocketHub::mainLoop(float delta) {
            pthread_mutex_lock(&m_mutex);
            
            // notification center
            auto nc = Director::getInstance()->getEventDispatcher();
            
            // connected events
            for(auto s : m_connectedSockets){
                EventCustomObject *e = new EventCustomObject(kCCNotificationTCPSocketConnected, s);
                nc->dispatchEvent(e);
                e->release();
            }
            m_connectedSockets.clear();
            
            // data event
            for (auto s : m_packets) {
                EventCustomObject *e = new EventCustomObject(kCCNotificationPacketReceived, s);
                nc->dispatchEvent(e);
                e->release();
            }
            m_packets.clear();
            
            // disconnected event
            for(auto s : m_disconnectedSockets){
                EventCustomObject *e = new EventCustomObject(kCCNotificationTCPSocketDisconnected, s);
                nc->dispatchEvent(e);
                e->release();
                
                m_sockets.eraseObject(s);
            }
            m_disconnectedSockets.clear();
            
            pthread_mutex_unlock(&m_mutex);
        }
        
        void TCPSocketHub::sendPacket(int tag, Packet* packet) {
            for (auto s : m_sockets) {
                if(s->getTag() == tag) {
                    s->sendPacket(packet);
                    break;
                }
            }
        }
        
        void TCPSocketHub::disconnect(int tag) {
            for (auto s : m_sockets) {
                if(s->getTag() == tag) {
                    s->setStop(true);
                    break;
                }
            }
        }
        
    }
}
