# TCPSocket

<h5>Original project:</h5>
https://github.com/stubma/cocos2dx-better

<h5> Updates:</h5>
- Port into cocos2dx v3
- Remove some dependencies (this part can work individually)
- thread safe

<h5> Example:</h5>

``` c++
   std::string s = "simple_string";
    
    auto mainHub = funny::network::TCPSocketHub::create();
    mainHub->setRawPolicy(true);
    mainHub->retain();
    
    auto socket = mainHub->createSocket("your ip here", 6869, 11);
    
    auto packet = new funny::network::Packet();
    packet->initWithRawBuf(s.c_str(), s.length(), -1);
    
    socket->sendPacket(packet);
    
    packet->release();
```

<h5> Test server </h5>
get server in /server/socketserver.py and run it in your local host to test connection.
``` shell
python socketserver.py
```

<h5> Dependencies </h5>
- JSONUtils: https://github.com/kudo108/CCJsonUtils

<h5> Contacts: </h5>
If you have any question, feel free to ask me, I will help you!
- Skype: kudo_108
- Email: quan.nguyenvan000@gmail.com
