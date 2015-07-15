/****************************************************************************
 Copyright (c) 2015 QuanNguyen
 
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

#ifndef __TCPSocket__EventCustomObject__
#define __TCPSocket__EventCustomObject__

#include "cocos2d.h"

class EventCustomObject : public cocos2d::EventCustom {
    
public:
    EventCustomObject(const std::string& name, cocos2d::Ref *obj) : EventCustom(name){
        _userObject = nullptr;
        setUserObject(obj);
    }
    
    virtual ~EventCustomObject(){
        CC_SAFE_RELEASE_NULL(_userObject);
    }
    
    CC_SYNTHESIZE_RETAIN(cocos2d::Ref *, _userObject, UserObject);
};

#endif /* defined(__TCPSocket__EventCustomObject__) */
