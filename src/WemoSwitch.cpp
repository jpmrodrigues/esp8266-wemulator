
#include "WemoSwitch.h"


WemoSwitch::WemoSwitch(String name, unsigned int port, WemoCallback cb) :
m_server(NULL),
m_device_name(name),
m_local_port(port),
m_cb(cb)
{
    char uuid[64];

    uint32_t uniqueSwitchId = ESP.getChipId() + port;

    sprintf_P(uuid, PSTR("38323636-4558-4dda-9188-cda0e6%02x%02x%02x"),
          (uint16_t) ((uniqueSwitchId >> 16) & 0xff),
          (uint16_t) ((uniqueSwitchId >>  8) & 0xff),
          (uint16_t)   uniqueSwitchId        & 0xff);

    m_serial = String(uuid);
    m_persistent_uuid = "Socket-1_0-" + m_serial+"-"+ String(port);
}


WemoSwitch::~WemoSwitch()
{
}


void WemoSwitch::serverLoop()
{
    if ( NULL != m_server)
    {
        m_server->handleClient();
        delay(1);
    }
}

void WemoSwitch::start()
{
    if ( NULL == m_server )
    {
        m_server = new ESP8266WebServer(m_local_port);

        m_server->on("/", [&]() {
        handleRoot();
        });


        m_server->on("/setup.xml", [&]() {
        handleSetupXml();
        });

        m_server->on("/upnp/control/basicevent1", [&]() {
        handleUpnpControl();
        });

        m_server->on("/eventservice.xml", [&]() {
        handleEventservice();
        });


        m_server->begin();

        Serial.println("WebServer started on port: ");
        Serial.println(m_local_port);
    }
}

void WemoSwitch::stop()
{
    if ( NULL != m_server )
    {
        m_server->stop();

        delete m_server;

        m_server = NULL;
    }
}

void WemoSwitch::handleEventservice()
{
    Serial.println(" ########## Responding to eventservice.xml ... ########\n");

    String eventservice_xml = "<scpd xmlns=\"urn:Belkin:service-1-0\">"
        "<actionList>"
          "<action>"
            "<name>SetBinaryState</name>"
            "<argumentList>"
              "<argument>"
                "<retval/>"
                "<name>BinaryState</name>"
                "<relatedStateVariable>BinaryState</relatedStateVariable>"
                "<direction>in</direction>"
              "</argument>"
            "</argumentList>"
          "</action>"
          "</actionList>"
             "<serviceStateTable>"
              "<stateVariable sendEvents=\"yes\">"
                "<name>BinaryState</name>"
                "<dataType>Boolean</dataType>"
                "<defaultValue>0</defaultValue>"
              "</stateVariable>"
              "<stateVariable sendEvents=\"yes\">"
                "<name>level</name>"
                "<dataType>string</dataType>"
                "<defaultValue>0</defaultValue>"
              "</stateVariable>"
            "</serviceStateTable>"
        "</scpd>\r\n"
        "\r\n";

    m_server->send(200, "text/plain", eventservice_xml.c_str());
}

void WemoSwitch::handleUpnpControl()
{
    Serial.println("########## Responding to  /upnp/control/basicevent1 ... ##########");

    String response_xml = "";
    String request      = m_server->arg(0);

    Serial.print("request:");
    Serial.println(request);

    Serial.println("Responding to Control request");


    if( 0 < request.indexOf("<BinaryState>1</BinaryState>") )
    {
        Serial.println("Got Turn on request");

        handleRequestState(true);
    }
    else if ( 0 < request.indexOf("<BinaryState>0</BinaryState>") )
    {
        Serial.println("Got Turn off request");

        handleRequestState(false);
    }
    else
    {
        // Do nothing
    }
}


void WemoSwitch::handleRequestState(bool state)
{
    // Toggle relay
    m_cb(true);

    // Reply back with new state
    String body = 
        "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body>\r\n"
        "<u:GetBinaryStateResponse xmlns:u=\"urn:Belkin:service:basicevent:1\">\r\n"
        "<BinaryState>";

    body += (state ? "1" : "0");

    body += "</BinaryState>\r\n"
            "</u:GetBinaryStateResponse>\r\n"
            "</s:Body> </s:Envelope>\r\n";

    m_server->send(200, "text/xml", body.c_str());

    Serial.print("Sending :");
    Serial.println(body);
}


void WemoSwitch::handleRoot()
{
    m_server->send(200, "text/plain", "You should tell Alexa to discover devices");
}


void WemoSwitch::handleSetupXml()
{
    IPAddress localIP = WiFi.localIP();
    char s[16];
    sprintf(s, "%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);

    Serial.println(" ########## Responding to setup.xml ... ########\n");

    String setup_xml = "<?xml version=\"1.0\"?>"
         "<root xmlns=\"urn:Belkin:device-1-0\">"
           "<specVersion>"
           "<major>1</major>"
           "<minor>0</minor>"
           "</specVersion>"
           "<device>"
             "<deviceType>urn:Belkin:device:controllee:1</deviceType>"
             "<friendlyName>"+ m_device_name +"</friendlyName>"
             "<manufacturer>Belkin International Inc.</manufacturer>"
             "<modelName>Emulated Socket</modelName>"
             "<modelNumber>3.1415</modelNumber>"
             "<manufacturerURL>http://www.belkin.com</manufacturerURL>"
             "<modelDescription>Belkin Plugin Socket 1.0</modelDescription>"
             "<modelURL>http://www.belkin.com/plugin/</modelURL>"
             "<UDN>uuid:"+ m_persistent_uuid +"</UDN>"
             "<serialNumber>"+ m_serial +"</serialNumber>"
             "<binaryState>0</binaryState>"
             "<serviceList>"
               "<service>"
                 "<serviceType>urn:Belkin:service:basicevent:1</serviceType>"
                 "<serviceId>urn:Belkin:serviceId:basicevent1</serviceId>"
                 "<controlURL>/upnp/control/basicevent1</controlURL>"
                 "<eventSubURL>/upnp/event/basicevent1</eventSubURL>"
                 "<SCPDURL>/eventservice.xml</SCPDURL>"
               "</service>"
             "</serviceList>"
           "</device>"
         "</root>\r\n"
         "\r\n";

    m_server->send(200, "text/xml", setup_xml.c_str());

    Serial.print("Sending :");
    Serial.println(setup_xml);
}


String WemoSwitch::getName()
{
    return m_device_name;
}


void WemoSwitch::respondToSearch(IPAddress& senderIP, unsigned int senderPort)
{
    Serial.println("");
    Serial.print("Sending response to ");
    Serial.println(senderIP);
    Serial.print("Port : ");
    Serial.println(senderPort);

    IPAddress localIP = WiFi.localIP();
    char s[16];
    sprintf(s, "%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);

    String response =   "HTTP/1.1 200 OK\r\n"
                        "CACHE-CONTROL: max-age=86400\r\n"
                        "DATE: Sat, 26 Nov 2016 04:56:29 GMT\r\n"
                        "EXT:\r\n"
                        "LOCATION: http://" + String(s) + ":" + String(m_local_port) + "/setup.xml\r\n"
                        "OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n"
                        "01-NLS: b9200ebb-736d-4b93-bf03-835149d13983\r\n"
                        "SERVER: Unspecified, UPnP/1.0, Unspecified\r\n"
                        "ST: urn:Belkin:device:**\r\n"
                        "USN: uuid:" + m_persistent_uuid + "::urn:Belkin:device:**\r\n"
                        "X-User-Agent: redsonic\r\n\r\n";

    m_udp.beginPacket(senderIP, senderPort);
    m_udp.write(response.c_str());
    m_udp.endPacket();

    Serial.println("Response sent !");
}
