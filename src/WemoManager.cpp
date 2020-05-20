
#include "WemoManager.h"
#include "WemoSwitch.h"
#include <functional>

// ----------------------------------------------------------------------------
//  Local variables
// ----------------------------------------------------------------------------

// Multicast declarations
static const IPAddress g_ip_multi(239, 255, 255, 250);
static const uint32_t  g_port_multi = 1900;



// ----------------------------------------------------------------------------
//  Main code
// ----------------------------------------------------------------------------

WemoManager::WemoManager() :
    m_switch_cnt(0),
    m_running(false)
{
}


WemoManager::~WemoManager()
{
    // Gracefuly, stop all network services...
    stop();

    // ...then, delete all stored instances
    while ( !m_switches.empty() )
    {
        auto it = m_switches.begin();

        delete it->second;

        m_switches.erase(it);
    }
}


bool WemoManager::start()
{
    boolean state;

    Serial.println("Begin multicast ..");

    if( m_udp.beginMulticast(WiFi.localIP(), g_ip_multi, g_port_multi) )
    {
        auto it = m_switches.begin();

        Serial.print("Udp multicast server started at ");
        Serial.print(g_ip_multi);
        Serial.print(":");
        Serial.println(g_port_multi);

        for (; it != m_switches.end(); ++it)
        {
            it->second->start();
        }

        state = true;
    }
    else
    {
        Serial.println("Connection failed");

        state = false;
    }

    // Save state for later
    m_running = state;

    return state;
}


void WemoManager::stop()
{
    auto it = m_switches.begin();

    m_udp.stop();

    for (; it != m_switches.end(); ++it)
    {
        it->second->stop();
    }

    m_running = false;
}


bool WemoManager::addDevice(String name, uint32_t port, WemoCallback cb)
{
    bool status = true;

    // Only allow one instance per port
    if ( m_switches.end() == m_switches.find(port) )
    {
        Serial.print("Adding switch : ");
        Serial.print(name);
        Serial.print(" index : ");
        Serial.println(m_switch_cnt);

        m_switches[port] = new WemoSwitch(name, port, cb);
    }
    else
    {
        status = false;
    }

    return status;
}

void WemoManager::serverLoop()
{
    const uint32_t size = m_udp.parsePacket();

    auto it = m_switches.begin();

    if ( 0 < size )
    {
        String    request;
        IPAddress senderIP   = m_udp.remoteIP();
        uint32_t  senderPort = m_udp.remotePort();

        // Read the packet into the buffer
        m_udp.read(m_buffer, size);

        // Check if this is a M-SEARCH for WeMo device
        request = String((char *)m_buffer);

        if ( 0 < request.indexOf('M-SEARCH') )
        {
            if( (0 < request.indexOf("urn:Belkin:device:**")) || ( 0 < request.indexOf("ssdp:all")) || (0 < request.indexOf("upnp:rootdevice")) )
            {
                Serial.println("Got UDP Belkin Request..");

                for (; it != m_switches.end(); ++it)
                {
                    it->second->respondToSearch(senderIP, senderPort);
                }
            }
        }
    }

    for(it = m_switches.begin(); it != m_switches.end(); ++it)
    {
        it->second->serverLoop();
    }
}

bool WemoManager::isRunning()
{
    return m_running;
}