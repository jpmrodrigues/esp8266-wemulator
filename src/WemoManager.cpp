
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
    m_switch_cnt(0)
{
    for (uint32_t i = 0; i < MAX_SWITCHES; ++i)
    {
        m_switches[i] = NULL;
    }
}


WemoManager::~WemoManager()
{
    for (uint32_t i = 0; i < MAX_SWITCHES; ++i)
    {
        if ( NULL != m_switches[i] )
        {
            delete m_switches[i];
        }
    }
}


bool WemoManager::start()
{
    boolean state;

    Serial.println("Begin multicast ..");

    if( m_udp.beginMulticast(WiFi.localIP(), g_ip_multi, g_port_multi) )
    {
        Serial.print("Udp multicast server started at ");
        Serial.print(g_ip_multi);
        Serial.print(":");
        Serial.println(g_port_multi);

        state = true;
    }
    else
    {
        Serial.println("Connection failed");

        state = false;
    }

    return state;
}


bool WemoManager::addDevice(String alexaInvokeName, uint32_t port, WemoCallback cb)
{
    bool status = true;

    if ( MAX_SWITCHES > m_switch_cnt )
    {
        Serial.print("Adding switch : ");
        Serial.print(alexaInvokeName);
        Serial.print(" index : ");
        Serial.println(m_switch_cnt);

        m_switches[m_switch_cnt++] = new WemoSwitch(alexaInvokeName, port, cb);
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

                for(uint32_t n = 0; n < m_switch_cnt; n++)
                {
                    if ( NULL != m_switches[n] )
                    {
                        m_switches[n]->respondToSearch(senderIP, senderPort);
                    }
                }
            }
        }
    }

    for(uint32_t n = 0; n < m_switch_cnt; n++)
    {
        if ( NULL != m_switches[n] )
        {
            m_switches[n]->serverLoop();
        }
    }
}
