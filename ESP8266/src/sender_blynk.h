#ifndef _SENDERBLYNK_h
#define _SENDERBLYNK_h

// http://docs.blynk.cc/#widgets-notifications-email
// Максимальная длина тела электронного письма. М.б. до 1200
// email + subject + message length
#define BLYNK_MAX_SENDBYTES 512

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include "master_i2c.h"
#include "Logging.h"

#ifdef SEND_BLYNK
bool send_blynk(const Settings &sett, const SlaveData &data, const float &channel0, const float &channel1)
{
    Blynk.config(sett.key, sett.hostname, BLYNK_DEFAULT_PORT);
    if (Blynk.connect(SERVER_TIMEOUT)) {
        
        LOG_NOTICE( "BLK", "run");

        unsigned int delta0 = (channel0 - sett.channel0_previous)*1000;  // litres
        unsigned int delta1 = (channel1 - sett.channel1_previous)*1000;

        Blynk.virtualWrite(V0, channel0);
        Blynk.virtualWrite(V1, channel1);
        Blynk.virtualWrite(V2, (float)(data.voltage / 1000.0));
        Blynk.virtualWrite(V3, delta0);
        Blynk.virtualWrite(V4, delta1);
        Blynk.virtualWrite(V5, data.resets);

        LOG_NOTICE( "BLK", "virtualWrite OK");
        
        // Если заполнен параметр email отправим эл. письмо
        if (strlen(sett.email) > 4) {
            LOG_NOTICE( "BLK", "send email: " << sett.email);

            String msg = sett.email_template;
            String title = sett.email_title;
            String v0(channel0, 1);   //.1 для образца СМС сообщения
            String v1(channel1, 1);   //.1 для образца СМС сообщения
            String v2((float)(data.voltage / 1000.0), 3);
            String v3(delta0, DEC);
            String v4(delta1, DEC);
            String v5(data.resets, DEC);
            
            msg.replace("{V0}", v0);
            msg.replace("{V1}", v1);
            msg.replace("{V2}", v2);
            msg.replace("{V3}", v3);
            msg.replace("{V4}", v4);
            msg.replace("{V5}", v5);
            
            title.replace("{V0}", v0);
            title.replace("{V1}", v1);
            title.replace("{V2}", v2);
            title.replace("{V3}", v3);
            title.replace("{V4}", v4);
            title.replace("{V5}", v5);

            Blynk.email(sett.email, title, msg);

            LOG_NOTICE("BLK", "email was send");
            LOG_NOTICE("BLK", title);
            LOG_NOTICE("BLK", msg);
        }

        Blynk.disconnect();
        LOG_NOTICE("BLK", "disconnected");

        return true;
    } else {
        LOG_ERROR("BLK", "connect error");
    } 

    return false;
}        

#endif //#ifdef SEND_BLYNK

#endif
